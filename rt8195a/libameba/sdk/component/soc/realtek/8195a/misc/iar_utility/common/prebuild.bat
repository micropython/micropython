cd /D %1
:: Generate build_info.h
echo off
::echo %date:~0,10%-%time:~0,8%
::echo %USERNAME%
for /f "usebackq" %%i in (`hostname`) do set hostname=%%i
::echo %hostname%

echo #define UTS_VERSION "%date:~0,10%-%time:~0,8%" > ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_TIME "%date:~0,10%-%time:~0,8%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_BY "%USERNAME%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_HOST "%hostname%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_DOMAIN >> ..\inc\build_info.h
echo #define RTL195AFW_COMPILER "IAR compiler" >> ..\inc\build_info.h

exit

