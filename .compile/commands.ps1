# Shared by Windows PowerShell and PowerShell 7 profiles.
$script:CppCommandTools = Join-Path $PSScriptRoot 'commands.py'

function compilecpp {
    & python -X utf8 $script:CppCommandTools compilecpp @args
    if ($LASTEXITCODE -ne 0) {
        throw 'compilecpp failed. See the diagnostics above.'
    }
}

function compilepy {
    & python -X utf8 $script:CppCommandTools compilepy @args
    if ($LASTEXITCODE -ne 0) {
        throw 'compilepy failed. See the diagnostics above.'
    }
}

function brutecheck {
    & python -X utf8 $script:CppCommandTools brutecheck @args
    if ($LASTEXITCODE -ne 0) {
        throw 'brutecheck stopped with a failure. See the saved case or diagnostics above.'
    }
}
