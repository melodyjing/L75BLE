;/**************************************************************************//**
; * @file     sdk_bin.s
; * @brief    
; * @Auth     Liujian         
; * @version  V5.00
; * @date     2020/12/31
; ******************************************************************************/
;*/
     
     AREA  SDK_BIN_FILE, CODE, READONLY, PREINIT_ARRAY
     IF      :DEF:SDK_BIN_ENABLE		 
	     incbin  ..\lib\ble_firmware.bin
     ENDIF
     END