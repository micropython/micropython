function Test-LastExitCode {
    # Turn non-zero exitcode into a hard error.
    if ($LASTEXITCODE -ne 0) {
        throw "$Args exited with code $LASTEXITCODE"
    }
}

function Watch-MicroPythonTestRun {
    # Run tests, guarded by failure printing if needed.
    $upyDir, $script = $Args
    & $env:MICROPY_CPYTHON3 "$upyDir/tests/run-tests.py" --clean-failures
    & $script
    if ($LASTEXITCODE -ne 0) {
        & $env:MICROPY_CPYTHON3 "$upyDir/tests/run-tests.py" --print-failures
        throw "run-tests failed"
    }
}

function Get-MsvcBuildOutput {
    $projectFile, $buildOpts = $Args
    (msbuild $projectFile /nologo /v:m /t:ShowTargetPath $buildOpts).Trim()
    Test-LastExitCode "msbuild"
}

function Invoke-MsvcBuild {
    $upyDir, $buildOpts = $Args
    $projectFile = "$upyDir/build.proj"
    Write-Verbose "Building for msvc: $projectFile with $buildOpts"

    # Build multiple projects in parallel by using a separate project file as driver.
    @"
<Project>
<Target Name="Build">
  <MsBuild BuildInParallel="True" Projects="$upyDir/mpy-cross/mpy-cross.vcxproj;$upyDir/ports/windows/micropython.vcxproj"/>
</Target>
</Project>
"@ | Set-Content $projectFile

    msbuild /m $projectFile /t:Build $buildOpts
    Test-LastExitCode "msbuild"
}

function Invoke-MsvcTests {
    $upyDir, $buildOpts = $Args
    $env:MICROPY_MICROPYTHON = Get-MsvcBuildOutput "$upyDir/ports/windows/micropython.vcxproj" $buildOpts
    Write-Verbose "Testing with msvc: $env:MICROPY_MICROPYTHON"
    Set-Location "$upyDir/tests"
    Watch-MicroPythonTestRun $upyDir {& $env:MICROPY_CPYTHON3 run-tests.py}
    Watch-MicroPythonTestRun $upyDir {& $env:MICROPY_CPYTHON3 run-tests.py --via-mpy -d basics float micropython}
}

function Invoke-MingwBuild {
    $upyDir, $env:MSYSTEM, $bash, $crossMakeOpts, $upyMakeOpts = $Args
    $env:CHERE_INVOKING = "enabled_from_arguments"
    Write-Verbose "Building for $($env:MSYSTEM)"
    Write-Verbose "mpy-cross: $crossMakeOpts"
    Write-Verbose "micropython: $upyMakeOpts"
    Set-Location "$upyDir"
    # Treat passing BINDIR as trigger for the install target.
    $crossTargets = "all"
    if ($crossMakeOpts -match "BINDIR=") {
        $crossTargets += " install"
    }
    $upyTargets = "all"
    if ($upyMakeOpts -match "BINDIR=") {
        $upyTargets += " install"
    }
    # Clear LIB because when we get invoked from an environment which has it already, as is the case when
    # the msvc build tools are configured, it might include bogus content like parentheses in paths.
    & $bash -l -c "LIB='' make -B -C mpy-cross $crossMakeOpts $crossTargets"
    Test-LastExitCode "make"
    & $bash -l -c "LIB='' make -B -C ports/windows MICROPY_MPYCROSS=../../mpy-cross/mpy-cross.exe $upyMakeOpts $upyTargets"
    Test-LastExitCode "make"
}

function Invoke-MingwTests {
    $upyDir, $env:MSYSTEM, $bash, $makeOpts = $Args
    $env:CHERE_INVOKING = "enabled_from_arguments"
    Write-Verbose "Testing with $($env:MSYSTEM): $makeOpts"
    Set-Location "$upyDir"
    Watch-MicroPythonTestRun $upyDir {& $bash -l -c "make -C ports/windows test_full $makeOpts"}
}
