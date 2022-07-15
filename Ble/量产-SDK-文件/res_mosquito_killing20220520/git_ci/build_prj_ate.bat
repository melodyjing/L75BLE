set uv_builder=d:\Keil_v5\UV4\UV4.exe

set result=0

%uv_builder% -cr ..\project\ate\cobra4.uvprojx -t cobra4 -o build_log_prj_ate.txt
set /a result=%result%+%errorlevel%

echo %result% > build_result_prj_ate.txt
