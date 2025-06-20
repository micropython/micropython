set BOARD=Tiger_Plus
"C:\Program Files\SEGGER\JLink\JLink.exe" -device STM32F746NG -If SWD -speed 4000 -autoconnect 1 -CommandFile %BOARD%/cmd.jlink 
PAUSE