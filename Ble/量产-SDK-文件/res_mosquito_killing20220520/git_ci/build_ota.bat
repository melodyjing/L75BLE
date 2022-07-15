set uv_builder=c:\Keil_v5\UV4\UV4.exe

set result=0

%uv_builder% -cr ..\ota_build\ota_cobra4.uvprojx -t ota_cobra4 -o ..\ota_build\build_log_ota.txt
set /a result=%result%+%errorlevel%
if %errorlevel%==0 (
  .\BinMergeConsole.exe ..\ota_build\bin\cobra4.bin\cobra4.bin_0 ..\ota_build\bin\cobra4.bin\APP_IROM1 0x2A000 >> ..\ota_build\build_log_ota.txt
)

echo %result% > build_result.txt
