/*
liujian add
2018/7/19
通过串口发送命令, 相当于 shell
*/

#if CFG_APP_CONSOLE_CMD

#include "cobra_system.h"
#include "app_network.h"
#include "rwip_config.h"
#include "rdw_queue.h"
#include "rdw_console.h"
#include "rdw_command.h"
#include "ke_event.h"
#include <string.h>
#include <stdio.h>
#include "co_bt_defines.h"
#include "cobra_uart.h"
#include "rdw_mcu.h"
#include "smpc.h"
#include "nvds.h"
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"
#include "app_host_uart.h"
#include "app_host2_uart.h"
#include "app.h"
#include "cobra_timer.h"
#include "app_iris_console.h"

#if(UART_MODEL_TIMEOUT_ENABLE == 1) 
#include "app_uart_module.h"
#endif


QUEUE64_TYPE   CMD_Uart_buffer;

static const char cEraseSeq[] = "\b \b";		/* erase sequence */
static char cConsoleBuffer[consoleBUF_LEN] = {0};

static char HistoryBuffer[consoleBUF_LEN] = {0};
static uint32_t  history_cmd = 0;

static char echo;
static char *pcNow;
static 	uint32_t ulBufCnt = 0;			/* receive char cnt */
static	uint32_t ulProLen = 0;			/* pcPrompt length */
static	uint32_t ulOutColCnt = 0;		/* output column cnt */
static  uint32_t console_init = 0;

void console_task_callback(void);

void iris_uart_task_callback(void);

#if(UART_MODEL_TIMEOUT_ENABLE == 1) 
void xuju_uart_task_callback(void);
#endif

//unsigned char blue_name[13];
static void rdw_show_promt(void)
{
			printf("RDW:\\>");
}

static void rdw_get_line_init(void)
{
		pcNow = cConsoleBuffer;
		ulBufCnt = 0;
		ulProLen = 0;
		ulOutColCnt = 0;		

}

/*******************************************************************************
*@function  shellBackspace
*@brief     命令行退格操作
*@param     length      退格长度
*@retval    None
*@author    Letter
*******************************************************************************/
void shellBackspace(uint8_t length)
{
    while (length--)
    {
        printf(cEraseSeq);
    }
}

void cobra_set_uart_event_callback(void)
{
		ke_event_callback_set(KE_EVENT_UART, &console_task_callback);
}

void rdw_console_task_init(void)
{
	// Create uart receive kernel event
	  cobra4_uart_set_receive_callback(rdw_console_msg);
		memset((uint8_t *)&CMD_Uart_buffer, 0, sizeof(CMD_Uart_buffer));
		QueueInit(CMD_Uart_buffer);
		
	
#if(DIANBIAO_MODULE_ENABLE == 1)	
		ke_event_callback_set(KE_EVENT_UART, &iris_uart_task_callback);
#elif(UART_MODEL_TIMEOUT_ENABLE == 1)
    ke_event_callback_set(KE_EVENT_UART, &xuju_uart_task_callback);	
#else
	  ke_event_callback_set(KE_EVENT_UART, &console_task_callback);
#endif	
	
	
		echo = 1;
		rdw_get_line_init();
		console_init =  1;
}

static void rdw_cmd_run(uint32_t cmd_length)
{
		static int argc;
		static char *argv[consoleARG_NUM];
		static const xCommand_t *xCmdtoProcess;
		argc = lconsoleArgAnalyze(argv);
		if (argc > 0)
		{
				xCmdtoProcess = xconsoleCmdAnalyze(argv);
				if(xCmdtoProcess)
				{
					
					if(xCmdtoProcess->handle != NULL)
					{
							(xCmdtoProcess->handle)(argc,argv);
					}
					else
					{
							printf("NULL funtion pointer: %s %d\n",__FILE__, __LINE__);
					}
				}
				else
				{
						printf("There is no such command! \n");
				}
		}
	
}



static void console_task_callback(void)
{
		uint8_t cChar;		
		uint32_t  cmd_length;
		while(1)
		{
			if (true == QueueOut(CMD_Uart_buffer, cChar)) 
			{
					/**
					* Special character handling
					*/
					switch (cChar)
					{
					case 0x0D:						/* Enter */
					
					case 0x0A:
						if ( (pcNow >= &cConsoleBuffer[0]) && (pcNow <= (&cConsoleBuffer[consoleBUF_LEN]+1)) )
						{
							*pcNow = '\0';
							if (echo &&(cChar == 0x0D))
							{
								printf("\n");
							}
							cmd_length = (pcNow- cConsoleBuffer);
							if(cmd_length > 0)
							{
									history_cmd = 1;
								  strcpy(HistoryBuffer, cConsoleBuffer);
							}

							rdw_cmd_run(cmd_length);
							rdw_get_line_init();
							if(cChar == 0x0D)
							{
									rdw_show_promt();
							}
							break;
						}
						else
						{
							memset(cConsoleBuffer,0,sizeof(cConsoleBuffer));
							rdw_get_line_init();
							rdw_show_promt();
							break;
						}
						
					case 0x03:						/* ^C - break */
						memset(cConsoleBuffer,0,sizeof(cConsoleBuffer));
					  printf("\n");
						rdw_get_line_init();
						rdw_show_promt();
						break;
					case 0x15:						/* ^U - erase line */
						while (ulOutColCnt > ulProLen)
						{
							if (echo)
							{
								printf(cEraseSeq);
							}
							ulOutColCnt--;
						}
						pcNow = cConsoleBuffer;
						ulBufCnt = 0x00;
						break;
						
					case 0x08:						/* ^H  - backspace	*/
					case 0x7F:						/* DEL - backspace	*/
						if( (ulOutColCnt > ulProLen) && (pcNow > &cConsoleBuffer[0]) && (ulBufCnt > 0x00) )
						{
							if(echo)
							{
								printf(cEraseSeq);
							}
							ulOutColCnt--;
							pcNow--;
							ulBufCnt--;
						}
						break;
				  case '\t':
						
						if(history_cmd)
						{
								cmd_length = (pcNow- cConsoleBuffer);
								
								shellBackspace(cmd_length);
								strcpy(cConsoleBuffer, HistoryBuffer);
								ulOutColCnt = strlen(cConsoleBuffer);
								ulBufCnt = strlen(cConsoleBuffer);
							
								pcNow = &cConsoleBuffer[strlen(cConsoleBuffer)];
								printf("%s", cConsoleBuffer);
						}
						break;
					
						
					default:
						/**
						* Receive buffer is not full and must be a normal character
						* Increase the defense function, to prevent the pointer cross the border,
						* modify the memory
						*/
						if ( ulBufCnt < sizeof(cConsoleBuffer) && (pcNow >= &cConsoleBuffer[0]) && (pcNow <= &cConsoleBuffer[consoleBUF_LEN]) && (cChar > 0x19 && cChar < 0x7F) )
						{
							ulOutColCnt++;
							if(echo)
							{
								/* echo input */
								putchar(cChar);
							}
							*pcNow = cChar;
							pcNow++;
							ulBufCnt++;
						}
						else						/* Buffer full */
						{
							putchar ('\a');
						}
					}
			}
			else
			{
				break;
			}
		}
		
		ke_event_clear(KE_EVENT_UART);
	
}

#if(UART_MODEL_TIMEOUT_ENABLE == 1) 
void xuju_uart_task_callback(void)
{
	  uint8_t cChar;		
		while(1)
		{
			if (true == QueueOut(CMD_Uart_buffer, cChar)) 
			{
					 app_uart_module_process_uart_rx_data(cChar);
			}
			else
			{
				 break;
			}
		}
		ke_event_clear(KE_EVENT_UART);
	
}
#endif

void rdw_console_msg(uint8_t ch)
{
		if(console_init)
		{
			
			QueueIn(CMD_Uart_buffer, ch);
#if CFG_NRF24L01_DEMO
			ke_event_set(KE_EVENT_24L01);
#else
			ke_event_set(KE_EVENT_UART);
#endif
		}
}



/**
* 命令参数分析函数,以空格作为一个参数结束,支持输入十六进制数(如:0x15),支持输入负数(如-15)
* @param argv: 参数字符串指针数组
* @return:     参数个数
* @return -1 : 参数超过限制
*/
int32_t lconsoleArgAnalyze(char *argv[])
{
	uint32_t i = 0;
	int32_t argc = 0;

	while(argc < consoleARG_NUM)
	{
		/* skip any separator */
		while ( (cConsoleBuffer[i] == ' ') || \
				(cConsoleBuffer[i] == ',') || \
				(cConsoleBuffer[i] == ';') )
		{
			i++;
		}
		/* end of line, no more args	*/
		if (cConsoleBuffer[i] == '\0')
		{
			argv[argc] = NULL;
			return argc;
		}
		/* begin of argument string */
		argv[argc] = &cConsoleBuffer[i];
		argc++;

		/* find end of argument string */
		while ( cConsoleBuffer[i] && \
				(cConsoleBuffer[i] != ' ') && \
				(cConsoleBuffer[i] != ',') && \
				(cConsoleBuffer[i] != ';') )
		{
			i++;
		}
		/* end of line, no more args */
		if (cConsoleBuffer[i] == '\0')
		{
			argv[argc] = NULL;
			return argc;
		}
		cConsoleBuffer[i] = '\0';
		i++;
	}
	return -1;
}


/**
* 在命令结构体数组中查找相应的命令
* @param argv: 参数字符串指针数组
* @return normal: 命令结构体指针
* @return NULL: 没有找到该命令
*/
const xCommand_t * xconsoleCmdAnalyze(char *argv[])
{
	const xCommand_t *pxCmdTemp;
	
	/* Search command table - Use linear search - it's a small table */
	for (pxCmdTemp = &xCmdArray[0]; pxCmdTemp->name; pxCmdTemp++)
	{
		if (strcmp(argv[0], pxCmdTemp->name) == 0x00)
		{
			return pxCmdTemp;
		}
	}
	return NULL;/* not found */
}


#if CFG_NRF24L01_DEMO
unsigned char nrf24l01_cmd_state = 0;
unsigned char nrf24l01_cmd_checksum = 0;
unsigned char nrf24l01_cmd_value;
void app_state_grid_gateway_callback(void)
{
		uint8_t cChar;		
		while(1)
		{
			if (true == QueueOut(CMD_Uart_buffer, cChar)) 
			{
					if(nrf24l01_cmd_state == 0)
					{
							if(cChar == 0xAA)
							{
									nrf24l01_cmd_state = 1; //continte
									nrf24l01_cmd_checksum = cChar;  //0xAA
							}
							continue;							
					}
					else if(nrf24l01_cmd_state == 1)              //0x55
					{
							if(cChar == 0x05)
							{
									nrf24l01_cmd_state = 2;
									nrf24l01_cmd_checksum += cChar;
							}
							continue;
					}
					else if(nrf24l01_cmd_state == 2)					//cmd
					{
							nrf24l01_cmd_value = cChar;
							nrf24l01_cmd_checksum += cChar;
							nrf24l01_cmd_state = 3;
							continue;
					}
					else if(nrf24l01_cmd_state == 3)           ///checksum
					{
							if(nrf24l01_cmd_checksum == cChar)
							{
									nrf24l01_cmd_state = 4;
									continue;
							}
							else
							{
									nrf24l01_cmd_state = 0;
							}
					}
					else if(nrf24l01_cmd_state == 4)    //0x55
					{
							if(cChar == 0x55)
							{
									app_network_command_callback(nrf24l01_cmd_value);
							}
							nrf24l01_cmd_state = 0;
							continue;
					}
					nrf24l01_cmd_state = 0;
					continue;
			}
			else
			{
					break;
			}
			
		}
		
		ke_event_clear(KE_EVENT_24L01);
}
#endif

#endif



