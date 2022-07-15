#ifndef __RDW_CONSOLE_H__
#define __RDW_CONSOLE_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "rdw_command.h"

#define consolePRO_LEN			(0x20U)			//提示信息最大字节数
#define consoleARG_NUM			(0x0FU)			//最多有几个参数
#define consoleBUF_LEN			(0xFFU)			//接受缓存最大字节数


extern void rdw_console_task_init(void);
extern void rdw_console_msg(uint8_t ch);


int32_t lconsoleArgAnalyze(char *argv[]);

const xCommand_t * xconsoleCmdAnalyze(char *argv[]);



#endif

