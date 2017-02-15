cd /D %2
set tooldir=%2\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set libdir=%2\..\..\..\component\soc\realtek\8195a\misc\bsp

del Debug/Exe/bootloader.map Debug/Exe/bootloader.asm *.bin
cmd /c "%tooldir%\nm Debug/Exe/bootloader.axf | %tooldir%\sort > Debug/Exe/bootloader.map"
cmd /c "%tooldir%\objdump -d Debug/Exe/bootloader.axf > Debug/Exe/bootloader.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/bootloader.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram1_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/bootloader.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram1_end=0x%%i


::echo %ram1_start% > tmp.txt
::echo %ram2_start% >> tmp.txt
::echo %ram3_start% >> tmp.txt
::echo %ram1_end% >> tmp.txt
::echo %ram2_end% >> tmp.txt
::echo %ram3_end% >> tmp.txt

%tooldir%\objcopy -j "A2 rw" -Obinary Debug/Exe/bootloader.axf Debug/Exe/ram_1.bin
::%tooldir%\objcopy -j "A3 rw" -Obinary Debug/Exe/bootloader.axf Debug/Exe/sdram.bin

%tooldir%\pick %ram1_start% %ram1_end% Debug\Exe\ram_1.bin Debug\Exe\ram_1.p.bin head 0xb000
%tooldir%\pick %ram1_start% %ram1_end% Debug\Exe\ram_1.bin Debug\Exe\ram_1.r.bin raw

:: update ram_1.p.bin, raw file for application
copy Debug\Exe\ram_1.p.bin %libdir%\image\ram_1.p.bin
copy Debug\Exe\ram_1.r.bin %libdir%\image\ram_1.r.bin

:: delete hal_spi_flash_ram.o object after image built
del Debug\Obj\hal_spi_flash_ram.*

exit