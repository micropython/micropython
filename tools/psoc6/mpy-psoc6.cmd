@echo off
setlocal

if "%1"=="quick-start" goto cmd_quick_start
if "%1"=="device-setup" goto cmd_device_setup
if "%1"=="firmware-deploy" goto cmd_firmware_deploy
if "%1"=="help" goto help

goto help

exit /b %ERRORLEVEL%

:cmd_quick_start
    call :mpy_quick_start %2 %3
exit /b 0

:cmd_device_setup
    call :mpy_device_setup %2 %3 %4
exit /b 0

:cmd_firmware_deploy
    call :mpy_firmware_deploy %2 %3
exit /b 0


rem ~~~~~~~~~~~~~~~~
rem Script functions
rem ~~~~~~~~~~~~~~~~

:help

    echo Micropython PSoC6 utility script
    echo usage: mpy-psoc6.cmd ^<command^> 
    echo The available commands are:
    echo: 
    echo   quick-start          Setup of MicroPython IDE and PSoC6 board.
    echo                        Use this command for a guided installation and 
    echo                        quick start using MicroPython PSoC6. 
    echo                        usage: mpy-psoc6.cmd quick-start [board [version]]
    echo:
    echo                        board       PSoC6 prototyping kit name
    echo                        version     MicroPython PSoC6 firmware version
    echo:
    echo   device-setup         Setup of MicroPython PSoC6 board.
    echo                        Use this command to install the deployment tools 
    echo                        and MicroPython firmware binary, and deploy the
    echo                        firmware on the PSoC6 device.
    echo                        usage: mpy-psoc6.cmd device-setup [board [[version [\q]]]
    echo:
    echo                        board       PSoC6 prototyping kit name
    echo                        version     MicroPython PSoC6 firmware version
    echo                        \q          Quiet. Do not prompt any user confirmation request
    echo:
    echo   firmware-deploy      Firmware deployment on MicroPython board.
    echo                        Use this command to deploy an existing .hex file 
    echo                        on a PSoC6 board.
    echo                        Requires openocd available on the system path.
    echo                        usage: mpy-psoc6.cmd firmware-deploy ^<board^> ^<hex_file^>
    echo:
    echo                        board       PSoC6 prototyping kit name
    echo                        hex_file    MicroPython PSoC6 firmware .hex file
    echo:

exit /b 0

:mpy_firmware_deploy

    Rem Board is yet unused. In future the suitable target .cfg selection needs to be based on the board.
    set board=%~1
    set hex_file=%~2

    echo Deploying Micropython...
    openocd.exe -s openocd\scripts -c "source [find interface/kitprog3.cfg]; ; source [find target/psoc6_2m.cfg]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program %hex_file% verify reset exit;"

exit /b 0

:hw_firmware_download:

    set board=%~1
    curl.exe -s -L https://github.com/infineon/micropython/releases/v0.3.0/hello-world_${board}.hex > hello-world_${board}.hex

exit /b 0

:hw_firmware_clean

    set board=%~1
    del hello-world_%board%.hex

exit /b 0

:mpy_firmware_download:

    set board=%~1
    set version=%~2

    if "%version%" == "latest" (
        call set sub_url=latest/download
    ) else (
        call set sub_url=download/%version%
    )

    echo Downloading MicroPython PSoC6 port %version% for %board% board...
    curl.exe -s -L https://github.com/infineon/micropython/releases/%sub_url%/mpy-psoc6_%board%.hex > mpy-psoc6_%board%.hex

exit /b 0

:mpy_firmware_clean

    set board=%~1
    echo Cleaning up micropython hex files...
    del mpy-psoc6_%board%.hex

exit /b 0

:openocd_download_install

    echo Downloading openocd...
    curl.exe -s -L https://github.com/Infineon/openocd/releases/download/release-v4.4.0/openocd-4.4.0.2134-windows.zip > openocd.zip
    echo Extracting openocd...
    powershell -command "Expand-Archive -Force 'openocd.zip' -DestinationPath '.'"
    set PATH=openocd\bin;%PATH%

exit /b 0

:openocd_uninstall_clean

    echo Cleaning up openOCD installation package...
    del openocd.zip 
    rmdir /s /q openocd

exit /b 0

:mpy_device_setup

    setlocal enabledelayedexpansion

    Rem Board selection
    set board=%~1
    set board_list[0]=CY8CPROTO-062-4343W
    if [%board%]==[] (
        echo:
        echo       Supported MicroPython PSoC6 boards
        echo +---------+-----------------------------------+
        echo ^|   ID    ^|              Board                ^|
        echo +---------+-----------------------------------+
        echo ^|   0     ^|  CY8CPROTO-062-4343W (default^)    ^|
        echo +---------+-----------------------------------+
        echo:
        echo No user selection required. Only one choice.
        set /a board_index=0
        echo:
        Rem set  /p/( "board_index=Please type the desired board ID. " --> Uncomment and remove preselection above when more options are available
        call set board=%%board_list[!board_index!]%%
    )
    echo MicroPython PSoC6 Board  :: %board%

    Rem Version selection
    set mpy_firmware_version=%~2
    if [%mpy_firmware_version%]==[] set mpy_firmware_version=latest
    echo MicroPython PSoC6 Version :: %mpy_firmware_version% 
    echo:

    Rem Download flashing tool and firmware
    call :openocd_download_install
    call :hw_firmware_download %board%
    call :mpy_firmware_download %board% %mpy_firmware_version%

    if not [%~3]==[\q] (
        echo:
        echo Please CONNECT THE BOARD and PRESS ANY KEY to start the firmware deployment...
        pause >nul
        echo:
    )

    Rem Deploy on board
    call :mpy_firmware_deploy %board% hello-world_%board%.hex
    call :mpy_firmware_deploy %board% mpy-psoc6_%board%.hex
    echo Device firmware deployment completed.   

    call :openocd_uninstall_clean
    call :hw_firmware_clean %board%
    call :mpy_firmware_clean %board%

    if not [%~3]==[\q] (
        echo:
        echo Press any key to continue...
        pause >nul
        echo:
    )

exit /b 0

:arduino_lab_download_and_launch

    echo Downloading Arduino Lab for Micropython...
    curl.exe -s -L https://github.com/arduino/lab-micropython-editor/releases/download/0.5.0-alpha/Arduino.Lab.for.Micropython-win_x64.zip > arduino-for-micropython.zip
    echo Extracting Arduino Lab for Micropython...
    mkdir arduino-lab-mpy
	powershell -command "Expand-Archive -Force 'arduino-for-micropython.zip' -DestinationPath 'arduino-lab-mpy'"
    cd arduino-lab-mpy
    echo Launching Arduino Lab for Micropython...
    start "" "Arduino Lab for Micropython"
    cd ..

exit /b 0

:arduino_lab_install_package_clean

    echo Cleaning up Arduino Lab for Micropython installation package...
    del arduino-for-micropython.zip

exit /b 0

:mpy_quick_start

    setlocal enabledelayedexpansion

    echo ################################################
    echo                 Welcome to the 
    echo  __  __ _            ___      _   _             
    echo ^|  \/  (_)__ _ _ ___^| _ \_  _^| ^|_^| ^|_  ___ _ _ 
    echo ^| ^|\/^| ^| / _^| '_/ _ \  _/ ^|^| ^|  _^| ' \/ _ \ ' \ 
    echo ^|_^|_ ^|_^|_\__^|_^| \___/_^|  \_, ^|\__^|_^|^|_\___/_^|^|_^|
    echo ^| _ \/ __^| ___ / __^|/ /  ^|__/                    
    echo ^|  _/\__ \/ _ \ (__/ _ \                         
    echo ^|_^|  ^|___/\___/\___\___/ 
    echo:
    echo                  Quick Start
    echo ################################################

    call :mpy_device_setup %~1 %~2

    call :arduino_lab_download_and_launch

    call :arduino_lab_install_package_clean

    echo:   
    echo ################################################
    echo    The installation is completed. Have fun :)                 
    echo ################################################  

    echo:
    echo Press any key to exit...
    pause >nul

exit /b 0
