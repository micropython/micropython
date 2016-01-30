cd /D %2
set tooldir=%2\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools

del Debug/Exe/target.map Debug/Exe/target.asm *.bin
cmd /c "%tooldir%\nm Debug/Exe/target.axf | %tooldir%\sort > Debug/Exe/target.map"
cmd /c "%tooldir%\objdump -d Debug/Exe/target.axf > Debug/Exe/target.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/target.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram1_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 Debug/Exe/target.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep SDRAM  Debug/Exe/target.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram3_start=0x%%i
::for /f "delims=" %%i in ('cmd /c "%tooldir%\grep .ram_image4 Debug/Exe/target.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram4_start=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/target.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram1_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 Debug/Exe/target.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep SDRAM  Debug/Exe/target.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram3_end=0x%%i
::for /f "delims=" %%i in ('cmd /c "%tooldir%\grep .ram_image4 Debug/Exe/target.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram4_end=0x%%i

::echo %ram1_start% > tmp.txt
::echo %ram2_start% >> tmp.txt
::echo %ram3_start% >> tmp.txt
::echo %ram1_end% >> tmp.txt
::echo %ram2_end% >> tmp.txt
::echo %ram3_end% >> tmp.txt

%tooldir%\objcopy -j "A2 rw" -Obinary Debug/Exe/target.axf Debug/Exe/ram_1.bin
%tooldir%\objcopy -j "A3 rw" -Obinary Debug/Exe/target.axf Debug/Exe/sdram.bin

%tooldir%\pick %ram1_start% %ram1_end% Debug\Exe\ram_1.bin Debug\Exe\ram_1.p.bin head
%tooldir%\pick %ram2_start% %ram2_end% Debug\Exe\ram_1.bin Debug\Exe\ram_2.p.bin body
if defined %ram3_start (
%tooldir%\pick %ram3_start% %ram3_end% Debug\Exe\sdram.bin Debug\Exe\ram_3.p.bin body
)

:: SDRAM case
if defined %ram3_start (
copy /b Debug\Exe\ram_1.p.bin+Debug\Exe\ram_2.p.bin+Debug\Exe\ram_3.p.bin Debug\Exe\ram_all.bin
)

:: NO SDRAM case
if not defined %ram3_start (
copy /b Debug\Exe\ram_1.p.bin+Debug\Exe\ram_2.p.bin Debug\Exe\ram_all.bin
)

:: board generator
%tooldir%\..\gen_board.bat %ram2_start% %ram2_end% %ram1_start%

exit