#ifndef __RDW_COMMAND_H__
#define __RDW_COMMAND_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "rdw_queue.h"


/*命令结构体类型*/
typedef struct
{
	char *name;									//命令字符串
	void (*handle)(int argc, char *argv[]);		//命令回调函数
	char *help;									//帮助信息
} xCommand_t;

extern void cobra_set_uart_event_callback(void);
extern const xCommand_t xCmdArray[];
extern QUEUE64_TYPE   CMD_Uart_buffer;

#define MAX_LENGTH 128

//Download program
typedef struct Download_Type
{
	 unsigned char  download_cmd;
	 unsigned char  length1;
	 unsigned char  length2;
	 unsigned int   length;
	 unsigned int   receive_length;
	 unsigned char  check_sum;
	
	 unsigned int 	download_flash_address;
	 unsigned char  download_buffer[MAX_LENGTH];	
}DOWNLOAD_TypeDef;


typedef enum{
		LOOK_KEY_STATE,
		CMD_STATE,
		LOOK_LENGTH_STATE1,
		LOOK_LENGTH_STATE2,
		GET_DATA_STATE,
		CHECKSUM_STATE,	
}DOWNLOAD_STATE;


void cobra_download_write_data_to_eflash(void);
void cobra_download_deal_with_data(void);
#endif
