/******************************************************************************
	File Name	  :	  ota_shell_cmd.h
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/28

	Description :  
     通过命令, 来查询周边设备的状态

	Changed Log	:
		Liujian  2020/08/28	   - Creation

*****************************************************************************/

#ifndef __OTA_SHELL_CMD_H__
#define __OTA_SHELL_CMD_H__


void ota_shell_list_ota_device(unsigned int argc, unsigned char * argv[]);

void ota_shell_connect_ota_device(unsigned int argc, char * argv[]);


void ota_shell_disconnect_ota_device(unsigned int argc, char * argv[]);




#endif
