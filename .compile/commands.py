#!/usr/bin/env python3
"""Build local solutions and compare them against a reference solution."""

from __future__ import annotations

import argparse
from datetime import datetime
import importlib.util
import json
import math
import os
from pathlib import Path
import py_compile
import shutil
import subprocess
import sys
import tempfile
import time
import uuid


LIBRARY_ROOT = Path(__file__).resolve().parent.parent / "CP"
INCLUDE_DIRECTORY = LIBRARY_ROOT / "include"


class CommandError(RuntimeError):
    pass


def source_file(value: str, extension: str) -> Path:
    source = Path(value).resolve()
    if not source.is_file() or source.suffix.lower() != extension:
        raise CommandError(f"Expected an existing {extension} source: {source}")
    return source


def output_file(source: Path, value: str | None) -> Path:
    output = Path(value if value is not None else source.stem).absolute()
    if output.suffix.lower() != ".exe":
        output = output.with_name(output.name + ".exe")
    output = output.resolve()
    if output == source:
        raise CommandError("The output must not overwrite the source.")
    output.parent.mkdir(parents=True, exist_ok=True)
    return output


def compile_cpp(source: Path, output: Path) -> None:
    compiler = os.environ.get("CXX") or "g++"
    if not shutil.which(compiler):
        raise CommandError(f"C++ compiler not found: {compiler}")
    # Build beside the destination so a failed build leaves the previous file intact.
    with tempfile.TemporaryDirectory(prefix=".compilecpp-", dir=output.parent) as temporary:
        executable = Path(temporary) / "program.exe"
        command = [
            compiler, "-std=c++23", "-O2", "-Wall", "-Wextra",
            "-I", str(INCLUDE_DIRECTORY), str(source), "-o", str(executable),
        ]
        result = subprocess.run(command)
        if result.returncode:
            raise CommandError(f"C++ compilation failed: {source}")
        publish(executable, output)


def publish(executable: Path, output: Path) -> None:
    # A moved file would retain TemporaryDirectory's owner-only Windows ACL.
    # A new file inherits the destination directory's normal permissions.
    staged = output.parent / f".{output.name}.{uuid.uuid4().hex}.tmp"
    try:
        with executable.open("rb") as source, staged.open("xb") as target:
            shutil.copyfileobj(source, target)
        os.replace(staged, output)
    finally:
        staged.unlink(missing_ok=True)


def compile_python(source: Path, output: Path) -> None:
    if importlib.util.find_spec("PyInstaller") is None:
        raise CommandError("PyInstaller is missing. Install it with: python -m pip install pyinstaller")
    with tempfile.TemporaryDirectory(prefix=".compilepy-", dir=output.parent) as temporary:
        directory = Path(temporary)
        py_compile.compile(str(source), cfile=str(directory / "syntax.pyc"), doraise=True)
        command = [
            sys.executable, "-m", "PyInstaller", "--onefile", "--noconfirm",
            "--log-level", "WARN", "--name", output.stem,
            "--distpath", str(directory / "dist"),
            "--workpath", str(directory / "build"),
            "--specpath", str(directory), str(source),
        ]
        result = subprocess.run(command)
        if result.returncode:
            raise CommandError(f"Python packaging failed: {source}")
        publish(directory / "dist" / (output.stem + ".exe"), output)


def run_program(
    executable: Path, input_path: Path | None, output_path: Path,
    error_path: Path, timeout: float, arguments: list[str] | None = None,
) -> tuple[str, float]:
    started = time.perf_counter()
    with output_path.open("wb") as output, error_path.open("wb") as error:
        with (input_path.open("rb") if input_path else open(os.devnull, "rb")) as data:
            try:
                result = subprocess.run(
                    [str(executable), *(arguments or [])], stdin=data,
                    stdout=output, stderr=error, timeout=timeout,
                )
                status = "OK" if result.returncode == 0 else f"EXIT {result.returncode}"
            except subprocess.TimeoutExpired:
                status = "TIMEOUT"
            except OSError as exception:
                status = f"ERROR: {exception}"
    return status, time.perf_counter() - started


def save_failure(directory: Path, destination: Path, report: dict) -> Path:
    destination.mkdir(parents=True, exist_ok=True)
    prefix = "failure-" + datetime.now().strftime("%Y%m%d-%H%M%S-%f") + "-"
    saved = Path(tempfile.mkdtemp(prefix=prefix, dir=destination))
    for name in ("input.txt", "actual.txt", "expected.txt", "candidate.stderr.txt",
                 "reference.stderr.txt", "generator.stderr.txt"):
        path = directory / name
        if path.exists():
            shutil.copy2(path, saved / name)
    (saved / "report.json").write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
    return saved


def brutecheck(args: argparse.Namespace) -> int:
    candidate = source_file(args.candidate, ".cpp")
    reference = source_file(args.reference, ".cpp")
    generator = source_file(args.generator, ".cpp")
    if args.count < 0 or args.seed < 0 or not math.isfinite(args.timeout) or args.timeout <= 0:
        raise CommandError("Count and seed must be nonnegative; timeout must be finite and positive.")
    destination = Path(args.output).resolve() if args.output else Path.cwd() / ".output_cph" / "brutecheck"
    with tempfile.TemporaryDirectory(prefix="brutecheck-") as temporary:
        directory = Path(temporary)
        programs = [directory / name for name in ("candidate.exe", "reference.exe", "generator.exe")]
        for source, executable in zip((candidate, reference, generator), programs):
            print(f"Compiling: {source}", flush=True)
            compile_cpp(source, executable)
        print(f"Checking {'until failure' if args.count == 0 else str(args.count) + ' cases'}; "
              f"first seed {args.seed}, timeout {args.timeout:g}s per program.", flush=True)
        index = 0
        while args.count == 0 or index < args.count:
            seed = args.seed + index
            # Truncate every artifact so a generator failure cannot retain a previous case.
            for name in ("input.txt", "actual.txt", "expected.txt", "candidate.stderr.txt",
                         "reference.stderr.txt", "generator.stderr.txt"):
                (directory / name).write_bytes(b"")
            report = {"case": index + 1, "seed": seed, "timeout": args.timeout,
                      "sources": [str(candidate), str(reference), str(generator)],
                      "cwd": str(Path.cwd()), "comparison": "exact" if args.exact else "tokens"}
            status, elapsed = run_program(programs[2], None, directory / "input.txt",
                                          directory / "generator.stderr.txt", args.timeout, [str(seed)])
            report["generator"] = {"status": status, "seconds": elapsed}
            reason = f"Generator {status}" if status != "OK" else ""
            if not reason:
                for role, executable, output in (("reference", programs[1], "expected.txt"),
                                                 ("candidate", programs[0], "actual.txt")):
                    status, elapsed = run_program(executable, directory / "input.txt", directory / output,
                                                  directory / (role + ".stderr.txt"), args.timeout)
                    report[role] = {"status": status, "seconds": elapsed}
                    if status != "OK":
                        reason = f"{role.capitalize()} {status}"
                        break
            if not reason:
                actual = (directory / "actual.txt").read_bytes()
                expected = (directory / "expected.txt").read_bytes()
                matches = actual == expected if args.exact else actual.split() == expected.split()
                if not matches:
                    reason = "Wrong answer"
            if reason:
                report["reason"] = reason
                saved = save_failure(directory, destination, report)
                print(f"{reason} at case {index + 1}, seed {seed}.\nSaved: {saved}", flush=True)
                return 1
            index += 1
            if index % 100 == 0:
                print(f"Passed {index} cases (last seed {seed}).", flush=True)
        print(f"All {index} cases passed.", flush=True)
    return 0


def make_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    for name in ("compilecpp", "compilepy"):
        build = commands.add_parser(name)
        build.add_argument("source")
        build.add_argument("-o", "--output")
    check = commands.add_parser("brutecheck")
    check.add_argument("candidate")
    check.add_argument("reference")
    check.add_argument("generator")
    check.add_argument("-n", "--count", "-Count", dest="count", type=int, default=1000,
                       help="number of cases (default: 1000; 0: until failure or Ctrl+C)")
    check.add_argument("--seed", "-Seed", dest="seed", type=int, default=1)
    check.add_argument("--timeout", "-Timeout", dest="timeout", type=float, default=3,
                       help="timeout in seconds per program (default: 3)")
    check.add_argument("--exact", "-Exact", dest="exact", action="store_true",
                       help="compare output bytes instead of whitespace-separated tokens")
    check.add_argument("-o", "--output", help="failure artifact directory")
    return parser


def main() -> int:
    args = make_parser().parse_args()
    try:
        if args.command == "brutecheck":
            return brutecheck(args)
        source = source_file(args.source, ".cpp" if args.command == "compilecpp" else ".py")
        output = output_file(source, args.output)
        if args.command == "compilecpp":
            compile_cpp(source, output)
        else:
            compile_python(source, output)
        print(f"Built: {output}", flush=True)
        return 0
    except (CommandError, OSError, py_compile.PyCompileError) as error:
        print(f"{args.command}: {error}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("\nStopped.", file=sys.stderr)
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
