/*
liujian add
2018/7/19

*/

#if CFG_APP_CONSOLE_CMD
#include "rdw_command.h"
//#include "app_rdw_voice.h"
#include "cobra_uart.h"
#include "ke_event.h"
//#include "cobra_cortex_m3_nvic.h"
//#include "cobra_cortex_m4_nvic.h"
//#include "ll.h"
#include <stdio.h>
#include "nvds.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "app_system_trim.h"
//#include "LCD_Z240IT010.h"   
#include "app_nrf24l01.h"
#include "cobra_mmu.h"

extern void cobra_jtag_enable(void);

static DOWNLOAD_TypeDef cobra_download;
DOWNLOAD_STATE  download_state;

static void rdw_console_help(int argc, char *argv[])
{
	const xCommand_t *pxCmdTemp;
	
	for (pxCmdTemp = &xCmdArray[0]; pxCmdTemp->handle != rdw_console_help; pxCmdTemp++)
	{
		if(pxCmdTemp->handle == NULL)
		{
			return;
		}
	}
	if (argc == 1)
	{
		printf("user command: \n");
		printf("nvds_read  tag   : read the nvds tag    \n");
		printf("reboot           : reboot cpu cortex M3 \n");
		printf("Download         : Download M0 program to Eflash 0x800000 \n");
		printf("jtag             : make the m0, m3 jtag good \n");
		printf("trim             : trim the system 8M RC clock \n");
		printf("mem              : list information \n");
		printf("you can also use TAB key show history command\n");
	}
	else
	{
			printf("%s\n",pxCmdTemp->help);
	}
}

static void  rdw_nvds_read(int argc, char *argv[])
{
		const xCommand_t *pxCmdTemp;
	
		for (pxCmdTemp = &xCmdArray[0]; pxCmdTemp->handle != rdw_nvds_read; pxCmdTemp++)
		{
			if(pxCmdTemp->handle == NULL)
			{
				return;
			}
		}
		if(argc == 2)
		{
				uint8_t len;
				uint8_t i;
			  uint8_t  temp_buf[64];
			  uint8_t  magic = strtol(argv[1], NULL, 16);
			
			  len = 64;
				if (nvds_get(magic, &len, temp_buf) == NVDS_OK)
				{
						for(i = 0; i < len; i++)
						{
							  printf("0x%02x ", temp_buf[i]);
						}
						printf("\n");
				}
				else
				{
					 printf("NVDS does not have that tag 0x%02x \n", magic);
				}
			
		}
		else
		{
			printf("%s\n",pxCmdTemp->help);
		}
	
}

static void rdw_reboot(int argc, char *argv[])
{
	const xCommand_t *pxCmdTemp;
	
	for (pxCmdTemp = &xCmdArray[0]; pxCmdTemp->handle != rdw_reboot; pxCmdTemp++)
	{
		if(pxCmdTemp->handle == NULL)
		{
			return;
		}
	}
	
		platform_reset(0x00);
	

}

static void rdw_voice(int argc, char *argv[])
{
	const xCommand_t *pxCmdTemp;
	
	for (pxCmdTemp = &xCmdArray[0]; pxCmdTemp->handle != rdw_voice; pxCmdTemp++)
	{
		if(pxCmdTemp->handle == NULL)
		{
			return;
		}
	}
	if (argc == 1)
	{
#if BLE_APP_RDW_VOICE
		 app_audio_record_begin();
#endif 
	}
	else
	{
			printf("%s\n",pxCmdTemp->help);
	}
}


void cobra_m0_download_task_callback(void)
{
		unsigned char  receive_data;
		while(1)
		{
			
			if(download_state == LOOK_KEY_STATE)      //找关键字 0xAA
			{
				if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
				{
					 if(receive_data != 0xAA) continue;
					 download_state = CMD_STATE;
					 continue;
				}
				else
				{
						break;
				}
			}
			else if(download_state == CMD_STATE)             //接受 command
			{

					if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
					{
						 cobra_download.download_cmd = receive_data;
						 download_state = LOOK_LENGTH_STATE1;
						 continue;
					}
					else
					{
							break;
					}
			}
			else if(download_state == LOOK_LENGTH_STATE1)          //接收数据长度1
			{

					if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
					{
							cobra_download.length1 = receive_data;
							download_state = LOOK_LENGTH_STATE2;
							continue;
					}
					else
					{
							break;
					}
			}
			else if(download_state == LOOK_LENGTH_STATE2)     //接收数据长度2
			{

					if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
					{
							cobra_download.length2 = receive_data;
							download_state = GET_DATA_STATE;
							cobra_download.length = cobra_download.length2;
							cobra_download.length <<= 8;
							cobra_download.length += cobra_download.length1;
							cobra_download.receive_length = 0;
							cobra_download.check_sum = 0;
							continue;
					}
					else
					{
							break;
					}
			}
			else if(download_state == GET_DATA_STATE)            //取得总的数据,放到download_buffer 中
			{
			
					if(cobra_download.receive_length < cobra_download.length)
					{
							if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
							{
									cobra_download.check_sum += receive_data;
									cobra_download.download_buffer[cobra_download.receive_length++] = receive_data;
									continue;
							}
							else
							{
									break;
							}
					}
					else
					{
							download_state = CHECKSUM_STATE;
					}
			}
			else if(download_state == CHECKSUM_STATE)
			{
				
					if (true == QueueOut(CMD_Uart_buffer, receive_data)) 
					{
								if(cobra_download.check_sum != receive_data)
								{
										cobra_uart_write(M3_UART0, 0x55);
										cobra_uart_write(M3_UART0, 0x02);
								}
								else
								{
										cobra_download_deal_with_data();
								}
								download_state = LOOK_KEY_STATE;
								continue;
					}
					else
					{
							break;
					}
			}
		}
		ke_event_clear(KE_EVENT_UART);
}

unsigned int download_buffer_remain = 0;


void cobra_download_write_data_to_eflash(void)
{
	    cobra_pcrm_eflash_ctrl_active_mode_clock_enable(1);
			if(download_buffer_remain == 0)
			{
					GLOBAL_INT_DISABLE();
					cobra_flash_erase(cobra_download.download_flash_address, COBRA_EFLASH_BOUNDARY4K);
				  GLOBAL_INT_RESTORE();
					download_buffer_remain = COBRA_EFLASH_BOUNDARY4K;
			}
			cobra_flash_write(cobra_download.download_flash_address, cobra_download.length, cobra_download.download_buffer);
			download_buffer_remain -= cobra_download.length;			
}

void cobra_download_deal_with_data(void)
{
			if(cobra_download.download_cmd == 0x01)  //Data write
			{
				if(cobra_download.length)
				{
						cobra_download_write_data_to_eflash();
				}

				cobra_download.download_flash_address += cobra_download.length;
				cobra_uart_write(M3_UART0, 0x55);
				cobra_uart_write(M3_UART0, 0x01);

			}

			if(cobra_download.download_cmd == 0x02)  //COMMAND
			{
				cobra_uart_write(M3_UART0, 0x55);
				cobra_uart_write(M3_UART0, 0x01);
				if(cobra_download.download_buffer[0] == 0x02)
				{
						cobra_set_uart_event_callback();               //结束烧写状态
						return;
				}
				else if(cobra_download.download_buffer[0] == 0x01)
				{
						cobra_download.download_flash_address = cobra_download.download_buffer[4];
						cobra_download.download_flash_address <<= 8;
						cobra_download.download_flash_address += cobra_download.download_buffer[3];
						cobra_download.download_flash_address <<= 8;
						cobra_download.download_flash_address += cobra_download.download_buffer[2];
						cobra_download.download_flash_address <<= 8;
						cobra_download.download_flash_address += cobra_download.download_buffer[1];
				}

			}
	
}

static void rdw_download_program(int argc, char *argv[])
{
		download_state = LOOK_KEY_STATE;
		cobra_download.download_flash_address = 0;
		ke_event_callback_set(KE_EVENT_UART, &cobra_m0_download_task_callback);
	  cobra_pcrm_eflash_ctrl_active_mode_clock_enable(1);
		cobra_eflash_parameter_init();
}


static void rdw_jtag(int argc, char *argv[])
{
//		cobra_jtag_enable();
}

static void rdw_trim(int argc, char *argv[])
{
#if (COBRA_TRIM_EN)
	
		 app_trim_init();
	
#endif
}

static void rdw_lcd(int argc, char *argv[])
{
#if CFG_LCD_DEMO
		 app_cobra_lcd_z240it010_init();
#endif
	
}

static void rdw_lcd1(int argc, char *argv[])
{
#if CFG_LCD_DEMO
		 app_cobra_lcd_display_1();
#endif
	
}

static void rdw_lcd2(int argc, char *argv[])
{
#if CFG_LCD_DEMO
		 app_cobra_lcd_display_2();
#endif
	
}

//static void rdw_nrf24L01(int argc, char *argv[])
//{
//#if CFG_NRF24L01_DEMO
//		app_cobra_nRF24L01_demo();
//#endif	
//}

//static void rdw_nrf24L01_init(int argc, char *argv[])
//{
//#if CFG_NRF24L01_DEMO
//		app_cobra_nRF24L01_init();
//#endif	
//}

static void rdw_iris_send_to_peer(int argc, char *argv[])
{
//#if DIANBIAO_ENABLE
//			app_device1_send_data_to_uart_server();
//#else
//		  app_uart_temp_send();
//#endif
	
}

/*命令表*/
const xCommand_t xCmdArray[] =
{
	/* 命令		处理函数			帮助信息																									*/
	{ "help",				rdw_console_help,				"help    \n" },
	{ "nvds_read",	rdw_nvds_read,		  		"nvds_read tag    \n" },
	{ "reboot", 		rdw_reboot,  						"reboot \n"},
	{ "voice", 			rdw_voice, 							"voice key input \n" },
	{ "Download",   rdw_download_program, 	"Download M0 program to Eflash 0x800000 \n" }, 
	{ "jtag", 			rdw_jtag, 							"make the m0, m3 jtag good \n" },
	{ "trim", 			rdw_trim,								"trim the system clock \n" },
#if (COBRA4_MMU_ENABLE == 1)
	{ "mem" ,       rdw_memory_information,         "list memory information" },
#endif	
//	{ "rf_init",    rdw_nrf24L01_init,       "nRF24L01 init \n" },  
//	{ "rf",         rdw_nrf24L01,           "nRF24L01 begin work \n"},
	{ "lcd1", 				rdw_lcd,								"lcd demo\n" },
	{ "lcd2", 				rdw_lcd1,								"lcd demo\n" },
	{ "lcd3", 				rdw_lcd2,								"lcd demo\n" },
	{ "send",         rdw_iris_send_to_peer,    "send data to other \n"},
	{ 0 }		//You must add this to indicate that the array is over
};

#endif


