set uv_builder=c:\Keil_v5\UV4\UV4.exe

set result=0

%uv_builder% -cr ..\build\cobra4.uvprojx -t cobra4 -o ..\build\build_log_ble.txt
set /a result=%result%+%errorlevel%

echo %result% > build_result_ble.txt
