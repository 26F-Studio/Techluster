Function Pause ($Message = "Press any key to continue . . . ") {
    if ((Test-Path variable:psISE) -and $psISE) {
        $Shell = New-Object -ComObject "WScript.Shell"
        $Button = $Shell.Popup("Click OK to continue.", 0, "Script Paused", 0)
    }
    else {     
        Write-Host -NoNewline $Message
        [void][System.Console]::ReadKey($true)
        Write-Host
    }
}
Function CheckCommandResult () {
    if ($LastExitCode -ne 0) {
        Write-Host "------- Initialization Failed -------" -ForegroundColor 'Red'
        Pause
        exit 1
    }
}

Set-Location $PSScriptRoot 

Write-Host "--------- Initializing Vcpkg --------" -ForegroundColor 'Cyan'

git submodule update --init --recursive
CheckCommandResult

Push-Location vcpkg
./bootstrap-vcpkg.bat -disableMetrics
CheckCommandResult

# Write-Host "------ Installing Dependencies ------" -ForegroundColor 'Cyan'
#
# ./vcpkg.exe --triplet="x64-windows-static" install
# CheckCommandResult

Write-Host "------ Initialization Succeeded -----" -ForegroundColor 'Green'

Pop-Location

Pause
exit 0