@echo off
setlocal

if "%1"=="quick-start" goto cmd_quick_start
if "%1"=="device-setup" goto cmd_device_setup
if "%1"=="firmware-deploy" goto cmd_firmware_deploy
if "%1"=="device-erase" goto cmd_device_erase
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

:cmd_device_erase
    call :mpy_device_erase %2 %3
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
    echo   device-erase         Erase the external memory of the device.
    echo                        Use this command to erase the external memory if available
    echo                        for the selected board.
    echo                        Requires openocd available on the system path.
    echo                        usage: mpy-psoc6.cmd device-erase [board [[\q]]
    echo:
    echo                        board       PSoC6 prototyping kit name
    echo                        \q          Quiet. Do not prompt any user confirmation request
    echo:

exit /b 0

:mpy_firmware_deploy

    set board=%~1
    set hex_file=%~2
    set serial_adapter_sn=%~3

    if "%board%" == "CY8CPROTO-062-4343W" (
        call set target_cfg=psoc6_2m.cfg
    ) else if "%board%" == "CY8CPROTO-063-BLE" (
        call set target_cfg=psoc6.cfg
    )

    if [%serial_adapter_sn%] == [] (
        call set serial_adapter_opt=
    ) else (
        call set serial_adapter_opt="adapter serial %serial_adapter_sn%"
    )

    echo Deploying Micropython...
    openocd.exe -s openocd\scripts -s openocd\board -c "source [find interface/kitprog3.cfg]; %serial_adapter_opt% ; source [find target/%target_cfg%]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program %hex_file% verify reset exit;"

exit /b 0

:mpy_firmware_download:

    set fw_name=%~1
    set board=%~2
    set version=%~3

    if "%version%" == "latest" (
        call set sub_url=latest/download
    ) else (
        call set sub_url=download/%version%
    )

    echo Downloading %fw_name% %version% for %board% board...
    curl.exe -s -L https://github.com/infineon/micropython/releases/%sub_url%/%fw_name%_%board%.hex > %fw_name%_%board%.hex

exit /b 0

:mpy_firmware_clean

    set fw_name=%~1
    set board=%~2
    echo Cleaning up %fw_name% hex files...
    del %fw_name%_%board%.hex

exit /b 0

:openocd_download_install

    echo Downloading openocd...
    curl.exe -s -L https://github.com/Infineon/micropython/releases/download/v0.3.0/openocd-4.4.0.2134-windows.zip > openocd.zip
    echo Extracting openocd...
    powershell -command "Expand-Archive -Force 'openocd.zip' -DestinationPath '.'"
    set PATH=openocd\bin;%PATH%

exit /b 0

:openocd_board_conf_download

    set board=%~1
    echo Downloading openocd %board% configuration...
    cd openocd
    mkdir board 
    cd board
    curl.exe -s -L https://github.com/infineon/micropython/releases/download/v0.3.0/qspi_config_%board%.cfg > qspi_config.cfg
    cd ../..

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
    set board_list[1]=CY8CPROTO-063-BLE
    if [%board%]==[] (
        echo:
        echo       Supported MicroPython PSoC6 boards
        echo +---------+-----------------------------------+
        echo ^|   ID    ^|              Board                ^|
        echo +---------+-----------------------------------+
        echo ^|   0     ^|  CY8CPROTO-062-4343W              ^|
        echo +---------+-----------------------------------+
        echo ^|   1     ^|  CY8CPROTO-063-BLE                ^|
        echo +---------+-----------------------------------+
        echo:
        @REM echo No user selection required. Only one choice.
        @REM set /a board_index=0
        echo:
        set  /p/( board_index="Please type the desired board ID.")
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
    call :openocd_board_conf_download %board%
    call :mpy_firmware_download hello-world %board% v0.3.0
    call :mpy_firmware_download mpy-psoc6 %board% %mpy_firmware_version%

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
    call :mpy_firmware_clean hello-world %board%
    call :mpy_firmware_clean mpy-psoc6 %board%

    if not [%~3]==[\q] (
        echo:
        echo Press any key to continue...
        pause >nul
        echo:
    )

exit /b 0

:mpy_device_erase

    setlocal enabledelayedexpansion

    Rem Board selection
    set board=%~1
    set board_list[0]=CY8CPROTO-062-4343W
    set board_list[1]=CY8CPROTO-063-BLE
    if [%board%]==[] (
        echo:
        echo       Supported MicroPython PSoC6 boards
        echo +---------+-----------------------------------+
        echo ^|   ID    ^|              Board                ^|
        echo +---------+-----------------------------------+
        echo ^|   0     ^|  CY8CPROTO-062-4343W              ^|
        echo +---------+-----------------------------------+
        echo ^|   1     ^|  CY8CPROTO-063-BLE                ^|
        echo +---------+-----------------------------------+
        echo:
        @REM echo No user selection required. Only one choice.
        @REM set /a board_index=0
        echo:
        set  /p/( board_index="Please type the desired board ID.")
        call set board=%%board_list[!board_index!]%%
    )
    echo MicroPython PSoC6 Board  :: %board%

    Rem Download flashing tool and firmware
    call :openocd_download_install
    call :openocd_board_conf_download %board%
    call :mpy_firmware_download device-erase %board% v0.3.0

    if not [%~2]==[\q] (
        echo:
        echo Please CONNECT THE BOARD and PRESS ANY KEY to start the firmware deployment...
        pause >nul
        echo:
    )

    Rem Deploy on board
    call :mpy_firmware_deploy %board% device-erase_%board%.hex
    echo Device firmware deployment completed.   

    call :openocd_uninstall_clean
    call :mpy_firmware_clean device-erase %board%

    if not [%~2]==[\q] (
        echo:
        echo Press any key to continue...
        pause >nul
        echo:
    )
    
exit /b 0


:arduino_lab_download_and_launch

    echo Downloading Arduino Lab for Micropython...
    curl.exe -s -L https://github.com/arduino/lab-micropython-editor/releases/download/0.8.0/Arduino.Lab.for.Micropython-win_x64.zip > arduino-for-micropython.zip
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
