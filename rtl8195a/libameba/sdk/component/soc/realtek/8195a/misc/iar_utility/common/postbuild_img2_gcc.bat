cd /D %2
set bindir=application/Debug/bin
set bindirw=application\Debug\bin
set tooldir=%2\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set libdir=%2\..\..\..\component\soc\realtek\8195a\misc\bsp

echo %tooldir%
echo %libdir%

::del Debug/Exe/target.map Debug/Exe/application.asm *.bin
cmd /c "%tooldir%\nm %bindir%/application.elf | %tooldir%\sort > %bindir%/application.nm.map"
cmd /c "%tooldir%\objdump -d %bindir%/application.elf > %bindir%/application.asm"


for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text_start__ %bindir%/application.nm.map | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __sdram_data_start__ %bindir%/application.nm.map | %tooldir%\gawk '{print $1}'"') do set ram3_start=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text_end__ %bindir%/application.nm.map  | %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __sdram_data_end__ %bindir%/application.nm.map  | %tooldir%\gawk '{print $1}'"') do set ram3_end=0x%%i

::echo %ram1_start% > tmp.txt
echo %ram2_start%
echo %ram3_start%
::echo %ram1_end% >> tmp.txt
echo %ram2_end%
echo %ram3_end%


%tooldir%\objcopy -j .image2.start.table -j .ram_image2.text -j .ram.data -Obinary %bindir%/application.elf %bindir%/ram_2.bin
if NOT %ram3_start% == %ram3_end% (
	%tooldir%\objcopy -j .sdr_data -Obinary %bindir%/application.elf %bindir%/sdram.bin
)

%tooldir%\pick %ram2_start% %ram2_end% %bindirw%\ram_2.bin %bindirw%\ram_2.p.bin body+reset_offset+sig
if defined %ram3_start (
%tooldir%\pick %ram3_start% %ram3_end% %bindirw%\sdram.bin %bindirw%\ram_3.p.bin body+reset_offset
)

:: check ram_1.p.bin exist, copy default
if not exist %bindirw%\ram_1.p.bin (
	copy %libdir%\image\ram_1.p.bin %bindirw%\ram_1.p.bin
)

::padding ram_1.p.bin to 32K+4K+4K+4K, LOADER/RSVD/SYSTEM/CALIBRATION
%tooldir%\padding 44k 0xFF %bindirw%\ram_1.p.bin

:: SDRAM case
if defined %ram3_start (
copy /b %bindirw%\ram_1.p.bin+%bindirw%\ram_2.p.bin+%bindirw%\ram_3.p.bin %bindirw%\ram_all.bin
copy /b %bindirw%\ram_2.p.bin+%bindirw%\ram_3.p.bin %bindirw%\ota.bin
)

%tooldir%\checksum Debug\Exe\ota.bin

:: NO SDRAM case
if not defined %ram3_start (
copy /b %bindirw%\ram_1.p.bin+%bindirw%\ram_2.p.bin %bindirw%\ram_all.bin
copy /b %bindirw%\ram_2.p.bin %bindirw%\ota.bin
)

exit
