$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$workspaceRoot = Split-Path -Parent (Split-Path -Parent $root)
$executableDirectory = Join-Path $workspaceRoot ".output_exe"
$executable = Join-Path $executableDirectory "CP-library-test.exe"
New-Item -ItemType Directory -Force -Path $executableDirectory | Out-Null

Push-Location $root
try {
    & g++ -std=c++23 -O2 -Wall -Wextra -I include tests\test_library.cpp -o $executable
    if ($LASTEXITCODE -ne 0) {
        throw "C++ test compilation failed."
    }
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw "C++ tests failed."
    }
    $env:PYTHONDONTWRITEBYTECODE = "1"
    & python -m unittest discover -s tests -v
    if ($LASTEXITCODE -ne 0) {
        throw "Bundler tests failed."
    }
    Write-Host "All tests passed." -ForegroundColor Green
} finally {
    Pop-Location
}
