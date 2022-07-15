set uv_builder=c:\Keil_v5\UV4\UV4.exe

set result=0

%uv_builder% -cr ..\project\bqb\cobra4.uvprojx -t cobra4 -o ..\bqb\build_log_bqb.txt
set /a result=%result%+%errorlevel%

echo %result% > build_result_bqb.txt
