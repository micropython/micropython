cd /D %2
set tooldir=%2\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set libdir=%2\..\..\..\component\soc\realtek\8195a\misc\bsp
set cfgdir=%3

echo config %3

del %cfgdir%/Exe/target.map %cfgdir%/Exe/application.asm *.bin
cmd /c "%tooldir%\nm %cfgdir%/Exe/application.axf | %tooldir%\sort > %cfgdir%/Exe/application.map"
cmd /c "%tooldir%\objdump -d %cfgdir%/Exe/application.axf > %cfgdir%/Exe/application.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 %cfgdir%/Exe/application.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep SDRAM  %cfgdir%/Exe/application.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram3_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep FLASH_RUN  %cfgdir%/Exe/application.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set flash_run_start=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 %cfgdir%/Exe/application.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep SDRAM  %cfgdir%/Exe/application.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram3_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep FLASH_RUN  %cfgdir%/Exe/application.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set flash_run_end=0x%%i

::echo %ram1_start% > tmp.txt
::echo %ram2_start% >> tmp.txt
::echo %ram3_start% >> tmp.txt
::echo %ram1_end% >> tmp.txt
::echo %ram2_end% >> tmp.txt
::echo %ram3_end% >> tmp.txt

%tooldir%\objcopy -j "A3 rw" -Obinary %cfgdir%/Exe/application.axf %cfgdir%/Exe/ram_2.bin
if defined %ram3_start (
	%tooldir%\objcopy -j "A5 rw" -Obinary %cfgdir%/Exe/application.axf %cfgdir%/Exe/sdram.bin
)
if defined %flash_run_start (
	%tooldir%\objcopy -j "A7 rw" -Obinary %cfgdir%/Exe/application.axf %cfgdir%/Exe/flash_run.bin
) else (
	set flash_run_start=0xFFFFFFFF
	set flash_run_end=0xFFFFFFFF
)

%tooldir%\pick %ram2_start% %ram2_end% %cfgdir%\Exe\ram_2.bin %cfgdir%\Exe\ram_2.p.bin body+reset_offset+sig
%tooldir%\pick %ram2_start% %ram2_end% %cfgdir%\Exe\ram_2.bin %cfgdir%\Exe\ram_2.ns.bin body+reset_offset
if defined %ram3_start (
%tooldir%\pick %ram3_start% %ram3_end% %cfgdir%\Exe\sdram.bin %cfgdir%\Exe\ram_3.p.bin body+reset_offset
)

:: force update ram_1.p.bin
del %cfgdir%\Exe\ram_1.p.bin

:: check ram_1.p.bin exist, copy default
if not exist %cfgdir%\Exe\ram_1.p.bin (
	copy %libdir%\image\ram_1.p.bin %cfgdir%\Exe\ram_1.p.bin
)

::if not exist %cfgdir%\Exe\data.p.bin (
::	copy %tooldir%\..\image\data.p.bin %cfgdir%\Exe\data.p.bin
::)

::padding ram_1.p.bin to 32K+4K+4K+4K, LOADER/RSVD/SYSTEM/CALIBRATION
%tooldir%\padding 44k 0xFF %cfgdir%\Exe\ram_1.p.bin

:: SDRAM case
if defined %ram3_start (
copy /b %cfgdir%\Exe\ram_1.p.bin+%cfgdir%\Exe\ram_2.p.bin+%cfgdir%\Exe\ram_3.p.bin %cfgdir%\Exe\ram_all.bin
copy /b %cfgdir%\Exe\ram_2.ns.bin+%cfgdir%\Exe\ram_3.p.bin %cfgdir%\Exe\ota.bin
)

:: NO SDRAM case
if not defined %ram3_start (
copy /b %cfgdir%\Exe\ram_1.p.bin+%cfgdir%\Exe\ram_2.p.bin %cfgdir%\Exe\ram_all.bin
copy /b %cfgdir%\Exe\ram_2.ns.bin %cfgdir%\Exe\ota.bin
set ram3_end=0xFFFFFFFF
)

%tooldir%\checksum %cfgdir%\Exe\ota.bin

:: board generator
%tooldir%\..\gen_board_img2.bat %ram2_start% %ram2_end% %ram3_end% %flash_run_start% %flash_run_end%

exit
