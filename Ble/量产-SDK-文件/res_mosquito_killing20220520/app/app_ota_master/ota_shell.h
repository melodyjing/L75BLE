/******************************************************************************
	File Name	  :	  ota_shell.h
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/27

	Description :  
     ͨ������ķ�ʽ, �� xmode Э��ķ�ʽ��д�ļ�

	Changed Log	:
		Liujian  2020/08/27	   - Creation

*****************************************************************************/
#ifndef __OTA_SHELL_H__
#define __OTA_SHELL_H__

#include "app_rdw_shell.h"

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))
#define SOH 0x01  
#define STX 0x02  
#define EOT 0x04  
#define ACK 0x06  
#define NAK 0x15  
#define CAN 0x18  
#define CTRLZ 0x1A  
#define DLY_1S 1000  
#define MAXRETRANS 25 

void ota_shell_main(unsigned int argc, unsigned char * argv[]);

void app_ota_shell_task_loop(void);

#endif

#endif
