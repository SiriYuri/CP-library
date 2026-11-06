param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Source,

    [string]$Output,

    [switch]$NoCompile
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$bundler = Join-Path $PSScriptRoot "bundle.py"
$sourcePath = (Resolve-Path -LiteralPath $Source).Path
$workspaceRoot = Split-Path -Parent (Split-Path -Parent $root)
$executableDirectory = Join-Path $workspaceRoot ".output_exe"
New-Item -ItemType Directory -Force -Path $executableDirectory | Out-Null

if ([string]::IsNullOrWhiteSpace($Output)) {
    $outputPath = Join-Path $env:TEMP "CP-submission.cpp"
} else {
    $outputPath = [IO.Path]::GetFullPath($Output)
}

& python $bundler $sourcePath -o $outputPath --explain
if ($LASTEXITCODE -ne 0) {
    throw "Bundle failed. Clipboard was not changed."
}

if (-not $NoCompile) {
    $compiler = Get-Command g++ -ErrorAction Stop
    $executable = Join-Path $executableDirectory "CP-submission.exe"
    & $compiler.Source -std=c++23 -O2 -Wall -Wextra $outputPath -o $executable
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed. Clipboard was not changed."
    }
}

$content = Get-Content -LiteralPath $outputPath -Raw -Encoding UTF8
Set-Clipboard -Value $content

$lineCount = ($content -split "`n").Count
$byteCount = [Text.Encoding]::UTF8.GetByteCount($content)
Write-Host "Bundled successfully: $outputPath" -ForegroundColor Green
if (-not $NoCompile) {
    Write-Host "Compilation check passed." -ForegroundColor Green
}
Write-Host "Copied $lineCount lines ($byteCount UTF-8 bytes) to the clipboard."
