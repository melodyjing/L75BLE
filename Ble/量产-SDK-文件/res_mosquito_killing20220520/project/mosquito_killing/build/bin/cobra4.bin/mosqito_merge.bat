
copy .\APP_IROM1  .\app_rom1.bin  /y
ChecksumWrite.exe   .\app_rom1.bin  0x1A000

copy .\cobra4.bin_0  .\cobra4_lib.bin  /y
BinMergeConsole.exe   .\cobra4_lib.bin  .\app_rom1_tmp.bin 0x30000

ota_format.exe app_rom1.bin cobra4_app_ota.bin

pause