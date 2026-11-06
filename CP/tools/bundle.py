#!/usr/bin/env python3
"""Turn a modular CP solution into one self-contained C++ source file."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path


INCLUDE_RE = re.compile(
    r"^[ \t]*#[ \t]*include[ \t]*[<\"](?P<header>[^>\"]+)[>\"][^\r\n]*(?:\r?\n|$)",
    re.MULTILINE,
)
IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
REQUIRE_RE = re.compile(
    r"^[ \t]*//[ \t]*CP[ \t]*:[ \t]*require[ \t]+(?P<names>[^\r\n]+)",
    re.MULTILINE | re.IGNORECASE,
)
RAW_LITERAL_RE = re.compile(r'(?:u8|u|U|L)?R"([^ ()\\\t\r\n]{0,16})\(')
QUOTED_LITERAL_RE = re.compile(r'(?:u8|u|U|L)?(["\'])')


class BundleError(RuntimeError):
    pass


@dataclass(frozen=True)
class Module:
    name: str
    path: Path
    display_path: str
    symbols: tuple[str, ...]
    dependencies: tuple[str, ...]


@dataclass(frozen=True)
class Library:
    root: Path
    umbrella_headers: frozenset[str]
    modules: dict[str, Module]
    header_to_module: dict[str, str]


def mask_non_code(source: str) -> str:
    """Replace comments and literals with spaces while preserving line positions."""
    result = list(source)
    n = len(source)
    i = 0

    def erase(left: int, right: int) -> None:
        for j in range(left, right):
            if result[j] not in "\r\n":
                result[j] = " "

    while i < n:
        if source.startswith("//", i):
            end = source.find("\n", i + 2)
            end = n if end == -1 else end
            erase(i, end)
            i = end
            continue
        if source.startswith("/*", i):
            closing = source.find("*/", i + 2)
            end = n if closing == -1 else closing + 2
            erase(i, end)
            i = end
            continue
        raw = RAW_LITERAL_RE.match(source, i)
        if raw:
            terminator = ")" + raw.group(1) + '"'
            closing = source.find(terminator, raw.end())
            end = n if closing == -1 else closing + len(terminator)
            erase(i, end)
            i = end
            continue
        if (
            source[i] == "'"
            and i > 0
            and i + 1 < n
            and source[i - 1].isalnum()
            and source[i + 1].isalnum()
        ):
            i += 1
            continue
        quoted = QUOTED_LITERAL_RE.match(source, i)
        if quoted:
            quote = quoted.group(1)
            cursor = quoted.end()
            while cursor < n:
                if source[cursor] == "\\":
                    cursor += 2
                    continue
                cursor += 1
                if source[cursor - 1] == quote:
                    break
            erase(i, min(cursor, n))
            i = cursor
            continue
        i += 1
    return "".join(result)


def load_library(manifest: Path) -> Library:
    try:
        raw = json.loads(manifest.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise BundleError(f"cannot load manifest {manifest}: {error}") from error
    if raw.get("version") != 1:
        raise BundleError("unsupported manifest version")
    raw_modules = raw.get("modules")
    if not isinstance(raw_modules, dict) or not raw_modules:
        raise BundleError("manifest must contain modules")

    root = manifest.resolve().parent
    modules: dict[str, Module] = {}
    header_to_module: dict[str, str] = {}
    for name, config in raw_modules.items():
        if not isinstance(config, dict):
            raise BundleError(f"module {name!r} must be an object")
        header = config.get("header")
        symbols = config.get("symbols", [])
        dependencies = config.get("depends", [])
        if not isinstance(header, str):
            raise BundleError(f"module {name!r} has no header")
        if not isinstance(symbols, list) or not all(isinstance(x, str) for x in symbols):
            raise BundleError(f"module {name!r} has invalid symbols")
        if not isinstance(dependencies, list) or not all(isinstance(x, str) for x in dependencies):
            raise BundleError(f"module {name!r} has invalid dependencies")
        path = root / header
        if not path.is_file():
            raise BundleError(f"module header does not exist: {path}")
        normalized = Path(header).as_posix()
        modules[name] = Module(
            name, path, normalized, tuple(symbols), tuple(dependencies)
        )
        header_to_module[normalized] = name
        if normalized.startswith("include/"):
            header_to_module[normalized.removeprefix("include/")] = name

    for module in modules.values():
        unknown = [name for name in module.dependencies if name not in modules]
        if unknown:
            raise BundleError(
                f"module {module.name!r} has unknown dependencies: {', '.join(unknown)}"
            )
    umbrellas = raw.get("umbrella_headers", [])
    if not isinstance(umbrellas, list) or not all(isinstance(x, str) for x in umbrellas):
        raise BundleError("umbrella_headers must be a list of strings")
    return Library(root, frozenset(umbrellas), modules, header_to_module)


def active_includes(source: str, masked: str) -> list[re.Match[str]]:
    result: list[re.Match[str]] = []
    for match in INCLUDE_RE.finditer(source):
        prefix = masked[match.start() : match.start("header")]
        if "#" in prefix:
            result.append(match)
    return result


def split_names(values: list[str]) -> set[str]:
    result: set[str] = set()
    for value in values:
        result.update(x for x in re.split(r"[\s,]+", value.strip()) if x)
    return result


def call_arities(source: str, name: str) -> set[int]:
    arities: set[int] = set()
    for match in re.finditer(rf"\b{re.escape(name)}\b", source):
        cursor = match.end()
        while cursor < len(source) and source[cursor].isspace():
            cursor += 1
        if cursor < len(source) and source[cursor] == "<":
            depth = 1
            cursor += 1
            while cursor < len(source) and depth:
                if source[cursor] == "<":
                    depth += 1
                elif source[cursor] == ">":
                    depth -= 1
                cursor += 1
            while cursor < len(source) and source[cursor].isspace():
                cursor += 1
        if cursor >= len(source) or source[cursor] != "(":
            continue
        cursor += 1
        parentheses = 1
        brackets = 0
        braces = 0
        commas = 0
        has_argument = False
        while cursor < len(source) and parentheses:
            character = source[cursor]
            if character == "(":
                parentheses += 1
            elif character == ")":
                parentheses -= 1
                if parentheses == 0:
                    break
            elif character == "[":
                brackets += 1
            elif character == "]":
                brackets -= 1
            elif character == "{":
                braces += 1
            elif character == "}":
                braces -= 1
            elif character == "," and parentheses == 1 and brackets == 0 and braces == 0:
                commas += 1
            if not character.isspace():
                has_argument = True
            cursor += 1
        if parentheses == 0:
            arities.add(commas + 1 if has_argument else 0)
    return arities


def choose_modules(
    source: str, masked: str, library: Library, cli_required: list[str]
) -> tuple[list[Module], dict[str, set[str]], bool]:
    includes = active_includes(source, masked)
    managed = [
        match
        for match in includes
        if match.group("header") in library.umbrella_headers
        or match.group("header") in library.header_to_module
    ]
    if not managed:
        return [], {}, False
    reasons: dict[str, set[str]] = {}

    def add(name: str, reason: str) -> None:
        if name not in library.modules:
            raise BundleError(f"unknown module {name!r}")
        reasons.setdefault(name, set()).add(reason)

    has_umbrella = False
    for match in managed:
        header = match.group("header")
        if header in library.umbrella_headers:
            has_umbrella = True
        else:
            add(library.header_to_module[header], f"direct include {header}")
    directives = [match.group("names") for match in REQUIRE_RE.finditer(source)]
    for name in sorted(split_names(directives)):
        add(name, "source directive")
    for name in sorted(split_names(cli_required)):
        add(name, "command line")
    if has_umbrella:
        identifiers = set(IDENT_RE.findall(masked))
        for module in library.modules.values():
            found = identifiers.intersection(symbol for symbol in module.symbols if symbol != "power")
            if found:
                add(module.name, "symbol " + ", ".join(sorted(found)))
        if "power" in identifiers:
            arities = call_arities(masked, "power")
            if 2 in arities:
                add("modint", "2-argument call power")
            if 3 in arities:
                add("math_base", "3-argument call power")
            if not arities or arities.difference({2, 3}):
                add("modint", "unclassified symbol power")
                add("math_base", "unclassified symbol power")

    ordered: list[Module] = []
    done: set[str] = set()
    active: set[str] = set()

    def visit(name: str, parent: str | None = None) -> None:
        if parent is not None:
            reasons.setdefault(name, set()).add(f"dependency of {parent}")
        if name in done:
            return
        if name in active:
            raise BundleError(f"dependency cycle at module {name!r}")
        active.add(name)
        module = library.modules[name]
        for dependency in module.dependencies:
            visit(dependency, name)
        active.remove(name)
        done.add(name)
        ordered.append(module)

    for name in library.modules:
        if name in reasons:
            visit(name)
    return ordered, reasons, True


def strip_header(module: Module, library: Library) -> tuple[str, list[str]]:
    source = module.path.read_text(encoding="utf-8")
    standard_headers: list[str] = []
    output: list[str] = []
    for line in source.splitlines(keepends=True):
        if re.fullmatch(r"[ \t]*#[ \t]*pragma[ \t]+once[ \t]*(?:\r?\n)?", line):
            continue
        match = INCLUDE_RE.fullmatch(line)
        if match:
            header = match.group("header")
            if header in library.umbrella_headers or header in library.header_to_module:
                continue
            if line.lstrip().startswith("#include <"):
                standard_headers.append(header.strip())
                continue
        output.append(line)
    return "".join(output).strip(), standard_headers


def render(source: str, masked: str, modules: list[Module], library: Library) -> str:
    sections: list[str] = []
    module_headers: list[str] = []
    for module in modules:
        body, headers = strip_header(module, library)
        module_headers.extend(headers)
        label = module.display_path.removeprefix("include/CP/")
        sections.append(f"// CP-lib/{label}\n{body}\n")
    bundle = "\n".join(sections)
    if bundle:
        bundle += "\n// CP-lib ends\n"

    active = active_includes(source, masked)
    managed_starts = {
        match.start()
        for match in active
        if match.group("header") in library.umbrella_headers
        or match.group("header") in library.header_to_module
    }
    active_starts = {match.start() for match in active}
    inserted = False
    source_headers: list[str] = []

    def replace(match: re.Match[str]) -> str:
        nonlocal inserted
        if match.start() in managed_starts:
            if inserted:
                return ""
            inserted = True
            return bundle
        if match.start() in active_starts and match.group(0).lstrip().startswith("#include <"):
            source_headers.append(match.group("header").strip())
            return ""
        return match.group(0)

    body = INCLUDE_RE.sub(replace, source).lstrip("\r\n")
    # bits/stdc++.h already provides the ordinary C and C++ standard headers.
    # Keep explicitly written source includes, but avoid re-emitting redundant
    # module headers such as <iostream> or <vector>. Headers with a path (for
    # example ext/pb_ds/...) are retained because bits does not guarantee them.
    if "bits/stdc++.h" in source_headers:
        module_headers = [
            header
            for header in module_headers
            if "/" in header and not header.startswith("bits/")
        ]

    unique_headers: list[str] = []
    seen: set[str] = set()
    for header in source_headers + module_headers:
        if header not in seen:
            seen.add(header)
            unique_headers.append(header)
    prefix = "".join(f"#include <{header}>\n" for header in unique_headers)
    return f"{prefix}\n{body}" if prefix else body


def make_parser() -> argparse.ArgumentParser:
    default_manifest = Path(__file__).resolve().parents[1] / "library.json"
    parser = argparse.ArgumentParser(description="bundle a modular CP solution")
    parser.add_argument("source", nargs="?", type=Path)
    parser.add_argument("-o", "--output", type=Path)
    parser.add_argument("--manifest", type=Path, default=default_manifest)
    parser.add_argument("--require", action="append", default=[])
    parser.add_argument("--list", action="store_true")
    parser.add_argument("--explain", action="store_true")
    return parser


def main() -> int:
    args = make_parser().parse_args()
    try:
        library = load_library(args.manifest)
        if args.list:
            for module in library.modules.values():
                print(f"{module.name}: {', '.join(module.symbols)}")
            return 0
        if args.source is None:
            raise BundleError("source file is required")
        source_path = args.source.resolve()
        if args.output is not None and args.output.resolve() == source_path:
            raise BundleError("refusing to overwrite the input file")
        source = source_path.read_text(encoding="utf-8")
        masked = mask_non_code(source)
        modules, reasons, has_library_include = choose_modules(
            source, masked, library, args.require
        )
        output = render(source, masked, modules, library) if has_library_include else source
        if args.explain:
            for module in modules:
                why = "; ".join(sorted(reasons[module.name]))
                print(f"selected {module.name}: {why}", file=sys.stderr)
        if args.output is None:
            sys.stdout.write(output)
        else:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(output, encoding="utf-8", newline="")
        return 0
    except (BundleError, OSError) as error:
        print(f"bundle: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
