/*	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_iris_console.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/19

	Description :   
	  串口命令, 用于实现 68 协议		

	Changed Log	:

		Liujian  2019/12/19			- Creation


*****************************************************************************/
#include "app_iris_console.h"
#include "app_iris_device.h"
#include "app_calculate_meter.h"
#include "cobra4_ble_common.h"
#if (CFG_APP_CONSOLE_CMD	 && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)

static struct bd_addr  rdw_set_bdaddr;
static unsigned char   rdw_bluetooth_name[12];
static unsigned char   rdw_pin_name[6]; 


void convert_address_to_ascii_hex(long long address, unsigned char *p_buffer);

void iris_uart_commmand_process(void);
void iris_uart_send_to_duanluqi(unsigned char * mac_address, unsigned char length, unsigned char *p_buffer);
void iris_dianbiao_rsp_read_param_error(unsigned int cmd, unsigned char error_number);
void iris_dianbiao_device_set_rsp(unsigned int kind, unsigned char result);

void iris_dianbiao_rsp_read_param_error(unsigned int cmd, unsigned char error_number);
void iris_dianbiao_master_information_rsp(void);
void iris_dianbiao_rs232_information_rsp(void);
void iris_dianbiao_rf_power_information_rsp(void);
void iris_dianbiao_smp_information_rsp(void);
void iris_dianbiao_adv_content_information_rsp(void);
void iris_dianbiao_protocol_information_rsp(void);
void iris_dianbiao_slave_param_rsp(void);

static UART_CMD_FRAME uart_frame;

static unsigned int temp_index;
static unsigned int uart_cmd_state = UART_CMD_IDLE;

void iris_auto_reply_ack_command(void);
void iris_dianbiao_save_taiti_address(void);
/*
用于判断如果串口上的数据不能在 2s 中之内收到全部数据, 就开始报错了

*/
static uint32_t dianbiao_console_timeout;

void app_dianbiao_console_timeout_function(unsigned int temp)
{
	   uart_cmd_state = UART_CMD_IDLE;
}

void  iris_dianbiao_console_init(void)
{
	 dianbiao_console_timeout = cobra_soft_timer_create(app_dianbiao_console_timeout_function);
}

void iris_dianbiao_begin_timeout(void)
{
	 cobra_soft_timer_start(dianbiao_console_timeout, 200, 0);
}

void iris_dianbiao_stop_timeout(void)
{
	 cobra_soft_timer_stop(dianbiao_console_timeout);
}

void iris_uart_task_callback(void)
{
		uint8_t cChar;		
		uint32_t  cmd_length;
		while(1)
		{
			if (true == QueueOut(CMD_Uart_buffer, cChar)) 
			{

					if((uart_cmd_state == UART_CMD_IDLE)||(uart_cmd_state == UART_CMD_LOOK_PREMBLE))
					{
							if(cChar == 0xFE)
							{	
									uart_cmd_state = UART_CMD_LOOK_PREMBLE;  //接着寻找 0xFE
							}
							else if(cChar == 0x68)
							{
									uart_cmd_state = UART_CMD_LOOK_LENGTH_L;    //寻找命令
									uart_frame.checksum = 0x68;
								  iris_dianbiao_stop_timeout();
								  iris_dianbiao_begin_timeout();
								
							}
							else
							{
									uart_cmd_state = UART_CMD_IDLE;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_LENGTH_L)
					{
						  uart_frame.length_l = cChar;
							uart_cmd_state = UART_CMD_LOOK_LENGTH_H;
							uart_frame.checksum += cChar;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_LENGTH_H)
					{
							uart_frame.length_h = cChar;
							uart_cmd_state = UART_CMD_LOOK_ADDRESS;
							uart_frame.checksum += cChar;
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_ADDRESS)
					{
							
							uart_frame.mac_address[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 6)
							{
									uart_cmd_state = UART_CMD_LOOK_STA1;
							}			
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_STA1)
					{
							uart_frame.sta = cChar;
							uart_cmd_state = UART_CMD_LOOK_RESERVE;
							uart_frame.checksum += cChar;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_RESERVE)
					{
							uart_frame.reserve = cChar;
							uart_cmd_state = UART_CMD_LOOK_SECOND_BEGIN;
							uart_frame.checksum += cChar;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_SECOND_BEGIN)
					{
							if(cChar != 0x68)
							{
								 uart_cmd_state = UART_CMD_IDLE;   //error command
								 continue;
							}
							else
							{
									uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;   //寻找控制字
									uart_frame.checksum += cChar;
									continue;
							}
					}
					else if(uart_cmd_state == UART_CMD_LOOK_CONTROL_CMD)
					{
							uart_frame.control_cmd = cChar;
							uart_cmd_state = UART_CMD_LOOK_USER_DATA;   //
						  uart_frame.checksum += cChar;							
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_USER_DATA)
					{
							cmd_length = uart_frame.length_h;
							cmd_length <<= 8;
							cmd_length += uart_frame.length_l;
							uart_frame.user_data[temp_index++] = cChar;
						
						  uart_frame.checksum += cChar;
							if(temp_index == cmd_length)
							{
									uart_cmd_state = UART_CMD_LOOK_CHECKSUM;
									uart_frame.checksum &= 0xff;
							}
							
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_CHECKSUM)
					{
							if(cChar != uart_frame.checksum)
							{
									uart_cmd_state = UART_CMD_LOOK_END;  //checksum error
							}
							else
							{
								  uart_cmd_state = UART_CMD_LOOK_END;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_END)
					{
						  iris_dianbiao_stop_timeout();
							if(cChar != 0x16)
							{
									uart_cmd_state = UART_CMD_IDLE;
									continue;
							}
							else
							{
										//
										//printf("OK>>>>>>>>>>>>>\n");
										uart_frame.user_length = uart_frame.length_h;
										uart_frame.user_length <<= 8;
										uart_frame.user_length += uart_frame.length_l;
										iris_uart_commmand_process();
										uart_cmd_state = UART_CMD_IDLE;
										
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




unsigned char * app_get_meter_master_pincode(void)
{
	  return rdw_pin_name;
}

#if(DIANBIAO_MODULE_ENABLE == 1)
//回答电表设置模块
void iris_dianbiao_meter_set_info_rsp(void)
{
			unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x00;
			checksum += 0x01;
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x81);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x68;
			checksum += 0x81;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}

////设置电表信息回复
void iris_dianbiao_meter_set_switch_info_rsp(void)
{
			unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x00;
			checksum += 0x01;
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x81);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x68;
			checksum += 0x81;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}

void iris_dianbiao_meter_connect_switch_rsp(void)
{
			unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x00;
			checksum += 0x01;
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x81);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x68;
			checksum += 0x81;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}


void iris_dianbiao_device_reset_rsp(void)
{
			unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x00;
			checksum += 0x01;
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x81);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x68;
			checksum += 0x81;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}

//返回电表当前连接的从设备的地址
void iris_dianbiao_slave_number_rsp(void)
{
			unsigned short length = 4;
		  unsigned short checksum = 0;
			unsigned char  temp_str[7];
#if DIANBIAO_AUTO_ENABLE 	
		  if(app_iris_auto_connect_state())
			{
					printf("app_iris_auto_connect_state .......\n");
					return;
			}
#endif
#if APP_SCAN_DEVICE_ENABLE	
			if(app_get_duanluqi_1_connected_flag())
			{
					length += 6;
			}
			
			if(app_get_duanluqi_2_connected_flag())
			{
					length += 6;
			}
			
			if(app_get_duanluqi_3_connected_flag())
			{
					length += 6;
			}
			
			if(length < 10)  //说明没有连接任何东西, 返回错误
			{
				  iris_dianbiao_rsp_read_param_error(0x00000001, 0xff);
				  return;
			}
		
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x01);
				
			checksum += 0x68;
			checksum += 0x82;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x01;
			
			if(app_get_duanluqi_1_connected_flag())
			{
					app_get_duanluqi1_address(temp_str);
				
					cobra_uart_write(M3_UART0, temp_str[0]);
					cobra_uart_write(M3_UART0, temp_str[1]);
					cobra_uart_write(M3_UART0, temp_str[2]);
					cobra_uart_write(M3_UART0, temp_str[3]);
					cobra_uart_write(M3_UART0, temp_str[4]);
					cobra_uart_write(M3_UART0, temp_str[5]) ;
				
					checksum += temp_str[0];
					checksum += temp_str[1];
					checksum += temp_str[2];
					checksum += temp_str[3];
					checksum += temp_str[4];
					checksum += temp_str[5];			
			}
			
			if(app_get_duanluqi_2_connected_flag())
			{
					app_get_duanluqi2_address(temp_str);
				
					cobra_uart_write(M3_UART0, temp_str[0]);
					cobra_uart_write(M3_UART0, temp_str[1]);
					cobra_uart_write(M3_UART0, temp_str[2]);
					cobra_uart_write(M3_UART0, temp_str[3]);
					cobra_uart_write(M3_UART0, temp_str[4]);
					cobra_uart_write(M3_UART0, temp_str[5]) ;
				
					checksum += temp_str[0];
					checksum += temp_str[1];
					checksum += temp_str[2];
					checksum += temp_str[3];
					checksum += temp_str[4];
					checksum += temp_str[5];			
			}
			
			if(app_get_duanluqi_3_connected_flag())
			{
					app_get_duanluqi3_address(temp_str);
				
					cobra_uart_write(M3_UART0, temp_str[0]);
					cobra_uart_write(M3_UART0, temp_str[1]);
					cobra_uart_write(M3_UART0, temp_str[2]);
					cobra_uart_write(M3_UART0, temp_str[3]);
					cobra_uart_write(M3_UART0, temp_str[4]);
					cobra_uart_write(M3_UART0, temp_str[5]) ;
				
					checksum += temp_str[0];
					checksum += temp_str[1];
					checksum += temp_str[2];
					checksum += temp_str[3];
					checksum += temp_str[4];
					checksum += temp_str[5];			
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
#endif				
	
}

/*
   返回读取参数的命令错误

*/
void iris_dianbiao_rsp_read_param_error(unsigned int cmd, unsigned char error_number)
{
		  unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x05);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x00;
			checksum += 0x05;
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			cobra_uart_write(M3_UART0, (cmd>>24)&0xff);
			cobra_uart_write(M3_UART0, (cmd>>16)&0xff);
			cobra_uart_write(M3_UART0, (cmd>>8)&0xff);
			cobra_uart_write(M3_UART0, (cmd)&0xff);
			cobra_uart_write(M3_UART0, error_number);
			
			checksum += 0x68;
			checksum += 0x82;
			checksum += ((cmd>>24)&0xff);
			checksum += ((cmd>>16)&0xff);
			checksum += ((cmd>>8)&0xff);
			checksum += ((cmd)&0xff);
			checksum += error_number;
		
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

void iris_dianbiao_module_state_rsp(void)
{
			unsigned short length = 5;
		  unsigned short checksum = 0;
			unsigned char  temp_str[7];
	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
				
			checksum += 0x68;
			checksum += 0x82;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, 0x00);  //空闲状态
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}


void iris_dianbiao_module_version_rsp(void)
{
			unsigned short length = 4+32;
		  unsigned short checksum = 0;

			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x03);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x03;
//00 01 00 00 
//B0 43 81 00 B4 58 21 00 
//00 00 00 00 00 00 00 00 
//09 00 00 00 B4 58 21 00 
//50 5D 21 00		

			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x01);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);

			checksum += 0x00;
			checksum += 0x01;
			checksum += 0x00;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, 0xB0);  
			cobra_uart_write(M3_UART0, 0x43);  
			cobra_uart_write(M3_UART0, 0x81);  
			cobra_uart_write(M3_UART0, 0x00); 
			cobra_uart_write(M3_UART0, 0xB4);  
			cobra_uart_write(M3_UART0, 0x58);  
			cobra_uart_write(M3_UART0, 0x21);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0xB0;
			checksum += 0x43;
			checksum += 0x81;
			checksum += 0x00;
			checksum += 0xB4;
			checksum += 0x58;
			checksum += 0x21;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, 0x09);
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);  
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0xB4);  
			cobra_uart_write(M3_UART0, 0x58);  
			cobra_uart_write(M3_UART0, 0x21);
			cobra_uart_write(M3_UART0, 0x00);  
			checksum += 0x09;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0xB4;
			checksum += 0x58;
			checksum += 0x21;
			checksum += 0x00;
			
			cobra_uart_write(M3_UART0, 0x50);  
			cobra_uart_write(M3_UART0, 0x5D);  
			cobra_uart_write(M3_UART0, 0x21);
			cobra_uart_write(M3_UART0, 0x00);  
			checksum += 0x50;
			checksum += 0x5D;
			checksum += 0x21;
			checksum += 0x00;
			


			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

void iris_dianbiao_mac_address_rsp(void)
{
				uint8_t length, i;
        uint8_t set_flag = 0;
				unsigned short checksum = 0;
				length = IRIS_MAC_SETFLAG_LENGTH;
				if(rwip_param.get(IRIS_MAC_SETFLAG, &length, &set_flag) != PARAM_OK)
				{
						set_flag = 0;		
				}

		  
			length = 4 + 24;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x01);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x01;
			
						
			if(set_flag == 0)
			{
				for(i = 0; i < 24; i++)
				{
						cobra_uart_write(M3_UART0, 0x00);
				}
				
			}
			else
			{
			
					for(i = 0; i < 6; i++)
					{
							cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[i]);
							checksum += rdw_set_bdaddr.addr[i];
					}
//					cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[1]);
//					cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[2]);
//					cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[3]);
//					cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[4]);
//					cobra_uart_write(M3_UART0, rdw_set_bdaddr.addr[5]);
				
					for(i = 0; i < 12; i++)
					{
						cobra_uart_write(M3_UART0, rdw_bluetooth_name[i]);
						checksum += rdw_bluetooth_name[i];
					}
				
					for(i = 0; i < 6; i++)
					{
						cobra_uart_write(M3_UART0, rdw_pin_name[i]);
						checksum += rdw_pin_name[i];
					}
					

				
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
	
}

//报告蓝牙上设置的从机信息
void iris_dianbiao_slave_information_rsp(void)
{
	    uint8_t length, i;

			unsigned short checksum = 0;
	
      unsigned char mac_address[6];
	    unsigned char *p_buffer;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 24*3;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x02);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x02;    //D0
			
		//断路器1 信息
		  app_get_duanluqi1_address(mac_address);
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, mac_address[i]);
				 checksum += mac_address[i];
			}
			p_buffer = app_get_duanluqi1_name();
			for(i = 0; i < 12; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}
			p_buffer = app_get_new_duanluqi1_pincode();
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}
		//断路器2
		  app_get_duanluqi2_address(mac_address);
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, mac_address[i]);
				 checksum += mac_address[i];
			}
			p_buffer = app_get_duanluqi2_name();
			for(i = 0; i < 12; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}
			p_buffer = app_get_new_duanluqi2_pincode();
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}	
		//断路器3
		  app_get_duanluqi3_address(mac_address);
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, mac_address[i]);
				 checksum += mac_address[i];
			}
			p_buffer = app_get_duanluqi3_name();
			for(i = 0; i < 12; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}
			p_buffer = app_get_new_duanluqi3_pincode();
			for(i = 0; i < 6; i++)
			{
				 cobra_uart_write(M3_UART0, p_buffer[i]);
				 checksum += p_buffer[i];
			}		 
		
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}


/*
  报告基本连接参数 (主)

*/


void iris_dianbiao_master_information_rsp(void)
{
		  uint8_t length;
	    uint16_t  temp;
   
			unsigned short checksum = 0;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 5;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x04);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x04;    //D0
#if(DIANBIAO_MASTER_PARAM_SET_ENABE == 1)			
			temp = app_master_device_get_connection_interval();   //连接间隔
			
	    cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
			
			temp = app_master_device_get_latency();
			cobra_uart_write(M3_UART0, temp&0xFF);
		  checksum += (temp&0xFF);
			
			temp = app_master_device_get_supervision_timeout(); 
			cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
#else
      temp = 15*5*1.25;   //连接间隔
			
	    cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
			
			temp = 3;
			cobra_uart_write(M3_UART0, temp&0xFF);
		  checksum += (temp&0xFF);
			
			temp = 10000; 
			cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);

#endif
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}


//报告串口参数信息
void iris_dianbiao_rs232_information_rsp(void)
{
	    uint8_t length;
	    uint16_t  temp;
   
			unsigned short checksum = 0;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 7;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x05);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x05;    //D0
	//115200
      cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0xC2);
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;    
			checksum += 0xC2;    
			checksum += 0x01;    
			checksum += 0x00;    
			
      cobra_uart_write(M3_UART0, 0x08);   //8bit 停止位
			
			cobra_uart_write(M3_UART0, 0x02);    //偶校验
			
			cobra_uart_write(M3_UART0, 0x00);     // 1 位停止位

      checksum += 0x08;    
			checksum += 0x02;    
			checksum += 0x00;    
 
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

/*
发射功率信息
*/
void iris_dianbiao_rf_power_information_rsp(void)
{
	    uint8_t length;
	    uint16_t  temp;
   
			unsigned short checksum = 0;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 1;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x06);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x06;    //D0

      temp = app_get_ble_power_level();
			cobra_uart_write(M3_UART0, temp&0xff);
			checksum += (temp&0xff);
			
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

/*
设置SMP 等级信息
*/
void iris_dianbiao_smp_information_rsp(void)
{
		  uint8_t length;
	    uint16_t  temp;
   
			unsigned short checksum = 0;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 1;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x06);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x06;    //D0

      temp = app_get_ble_smp_level();
			cobra_uart_write(M3_UART0, temp&0xff);
			checksum += (temp&0xff);
			
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);

	
}


/*
读取自己设置的广播包内容的信息
*/
#if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)
void iris_dianbiao_adv_content_information_rsp(void)
{
	    uint8_t length;
	    uint8_t  i;
	    uint8_t  adv_length;
   
			unsigned short checksum = 0;
			unsigned char *p_adv_buffer = app_get_ble_adv_content();
	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    adv_length = app_get_ble_adv_length();
	    length = 4 + adv_length;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x0A);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x01;    //D1
			checksum += 0x0A;    //D0
     
		 
       for(i = 0; i < adv_length; i++)
			 {
				  cobra_uart_write(M3_UART0, p_adv_buffer[i]);
				  checksum += p_adv_buffer[i];
			 }
			
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}
#endif

/*
得到蓝牙工作模式的信息
*/
#if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
void iris_dianbiao_work_mode_information_rsp(void)
{
		  uint8_t length;
	    uint8_t  temp;
   
			unsigned short checksum = 0;

	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 1;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x0B);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x0B;    //D0
     
		  
      temp = app_get_ble_work_mode();
			cobra_uart_write(M3_UART0, temp);
			checksum += temp; 
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}
#endif

/*
读取透传信息
*/
void iris_dianbiao_protocol_information_rsp(void)
{
		  uint8_t length;
	    uint8_t  temp;
   
			unsigned short checksum = 0;

	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 1;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x01);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x0D);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x01;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x0D;    //D0
     
		  
      temp = 0x02;            // 698 协议
			cobra_uart_write(M3_UART0, temp);
			checksum += temp; 
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
	
}

/*
  读取 作为从设备的连接参数
*/
void iris_dianbiao_slave_param_rsp(void)
{
			  uint8_t length;
	    uint16_t  temp;
   
			unsigned short checksum = 0;
				
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	    length = 4 + 5;
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x02);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x06);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x02;    //D3
			checksum += 0x00;    //D2
			checksum += 0x00;    //D1
			checksum += 0x06;    //D0
			
#if(DIANBIAO_SLAVE_PARAM_SET_ENABE == 1)			
			temp = app_slave_device_get_connection_interval();   //连接间隔
			
	    cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
			
			temp = app_slave_device_get_latency();
			cobra_uart_write(M3_UART0, temp&0xFF);
		  checksum += (temp&0xFF);
			
			temp = app_slave_device_get_supervision_timeout(); 
			cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
#else
      temp = APP_ADV_INT_MIN*1.25;   //连接间隔
			
	    cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);
			
			temp = 3;
			cobra_uart_write(M3_UART0, temp&0xFF);
		  checksum += (temp&0xFF);
			
			temp = 10000; 
			cobra_uart_write(M3_UART0, temp&0xFF);
			cobra_uart_write(M3_UART0, (temp>>8)&0xFF);
			checksum += (temp&0xFF);
			checksum += ((temp>>8)&0xFF);

#endif
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
	
}
//读取电表当前连接的主设备 地址
void iris_dianbiao_master_number_rsp(void)
{
			unsigned short length = 4;
		  unsigned short checksum = 0;
	
			unsigned char str_temp[7];
	
#if (APP_HOST2_ENABLE)	
			if(app_collection_1_is_ready())
			{
					length += 6;
			}
#endif			
			if(app_collection_2_is_ready())
			{
					length += 6;
			}
			
			if(length < 10)  //没有连接任何主机, 也报告错误
			{
				  iris_dianbiao_rsp_read_param_error(0x00000002, 0xff);
				  return;
			}
			
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
	
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x02);
				
			checksum += 0x68;
			checksum += 0x82;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x02;

#if (APP_HOST2_ENABLE)
			if(app_collection_1_is_ready())
			{
					app_get_collection1_address(str_temp);
				
					cobra_uart_write(M3_UART0, str_temp[0]);
					cobra_uart_write(M3_UART0, str_temp[1]);
					cobra_uart_write(M3_UART0, str_temp[2]);
					cobra_uart_write(M3_UART0, str_temp[3]);
					cobra_uart_write(M3_UART0, str_temp[4]);
					cobra_uart_write(M3_UART0, str_temp[5]) ;
				
					checksum += str_temp[0];
					checksum += str_temp[1];
					checksum += str_temp[2];
					checksum += str_temp[3];
					checksum += str_temp[4];
					checksum += str_temp[5];
				
			}
#endif			
			if(app_collection_2_is_ready())
			{				
					app_get_collection2_address(str_temp);
								
					cobra_uart_write(M3_UART0, str_temp[0]);
					cobra_uart_write(M3_UART0, str_temp[1]);
					cobra_uart_write(M3_UART0, str_temp[2]);
					cobra_uart_write(M3_UART0, str_temp[3]);
					cobra_uart_write(M3_UART0, str_temp[4]);
					cobra_uart_write(M3_UART0, str_temp[5]);
				
					checksum += str_temp[0];
					checksum += str_temp[1];
					checksum += str_temp[2];
					checksum += str_temp[3];
					checksum += str_temp[4];
					checksum += str_temp[5];
				
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

static long long calculate_comm_address;

static unsigned char comm_address[4];

unsigned char * iris_uart_get_address(void)
{
     return 	comm_address;
}


#endif


void iris_uart_commmand_process(void)
{
	  unsigned int  cmd_data_flag;
		cmd_data_flag = (unsigned int )(*(unsigned int *)uart_frame.user_data);
	  uint8_t i;
#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)	
	  if(uart_frame.control_cmd == 0x01)  //控制码为 0x01 ,那么 数据 要 单独 分析 
		{
					 switch(cmd_data_flag)
					 {
						 case 0x01000001:   //设置本设备MAC地址+本设备蓝牙名称+本设备配对码
						 {
									uint8_t length;
									uint8_t set_flag = 0;
									length = IRIS_MAC_SETFLAG_LENGTH;
									if(rwip_param.get(IRIS_MAC_SETFLAG, &length, &set_flag) != PARAM_OK)
									{
											set_flag = 0;		
									}
									
									if(set_flag == 0)
									{
											set_flag = 1;
											nvds_put(IRIS_MAC_SETFLAG, IRIS_MAC_SETFLAG_LENGTH, &set_flag);
											rdw_set_bdaddr.addr[0] = uart_frame.user_data[4]; 
											rdw_set_bdaddr.addr[1] = uart_frame.user_data[5]; 
											rdw_set_bdaddr.addr[2] = uart_frame.user_data[6]; 
											rdw_set_bdaddr.addr[3] = uart_frame.user_data[7]; 
											rdw_set_bdaddr.addr[4] = uart_frame.user_data[8]; 
											rdw_set_bdaddr.addr[5] = uart_frame.user_data[9]; 
										  /*  蓝牙地址的最低5 Byte 就是电表的通讯地址 */
									    calculate_comm_address = uart_frame.user_data[8];
										  calculate_comm_address <<= 8;
										  calculate_comm_address += uart_frame.user_data[7];
										  calculate_comm_address <<= 8;
										  calculate_comm_address += uart_frame.user_data[6];
										  calculate_comm_address <<= 8;
										  calculate_comm_address += uart_frame.user_data[5];
										  calculate_comm_address <<= 8;
										  calculate_comm_address += uart_frame.user_data[4];
										
										  convert_address_to_ascii_hex(calculate_comm_address, comm_address);
										
										  nvds_put(CALIBRATION_MODE_ADDRESS_FLAG, CALIBRATION_MODE_ADDRESS_LENGTH, (uint8_t *)comm_address);
											
											memcpy(rdw_bluetooth_name, &uart_frame.user_data[10], 12);
										
											memcpy(rdw_pin_name, &uart_frame.user_data[10+12], 6);
											
											nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&rdw_set_bdaddr);
										
											iris_dianbiao_meter_set_info_rsp();
											
											appm_send_gapm_reset_cmd();   //一定要收到串口过来, 也就是管理芯发送的设置地址,才可以正常工作
											
											if((rdw_pin_name[0] != 0xFF) || (rdw_pin_name[1] != 0xFF) ||(rdw_pin_name[2] != 0xFF) || \
												  (rdw_pin_name[3] != 0xFF) || (rdw_pin_name[4] != 0xFF) || (rdw_pin_name[5] != 0xFF))
											{
												 app_set_ble_smp_level(SMP_PASSKEY_ENTRY);
											}
											
											printf("pin 0x%02x 0x%02x  0x%02x 0x%02x 0x%02x 0x%02x \n", rdw_pin_name[0], rdw_pin_name[1], rdw_pin_name[2], \
											      rdw_pin_name[3], rdw_pin_name[4], rdw_pin_name[5]);
																		
									}
									else
									{
									
											iris_dianbiao_meter_set_info_rsp();
									}
								
							break;
					
					 }
					 
					 case 0x02000001:  //(目标设备 1 MAC 地址 + 目标设备1 蓝牙名称 + 目标设备1 配对码) X 3
					 {
								 //设置电表可以 连接的 断路器 的 蓝牙地址 
								 //memcpy(&duanluqi_address1.addr, &uart_frame.user_data[4], BD_ADDR_LEN);
		#if APP_SCAN_DEVICE_ENABLE						  
								 app_set_new_duanluqi1_address(&uart_frame.user_data[4]);
						     app_set_new_duanluqi1_pincode(&uart_frame.user_data[4+18]);
						     app_set_duanluqi1_name(&uart_frame.user_data[4+6]);
								 if(uart_frame.length_l > 27)
								 {
										 app_set_new_duanluqi2_address(&uart_frame.user_data[4+24]);
									   app_set_new_duanluqi2_pincode(&uart_frame.user_data[4+24+18]);
									   app_set_duanluqi2_name(&uart_frame.user_data[4+24+6]);
								 }
								 if(uart_frame.length_l > 52)
								 {
										 app_set_new_duanluqi3_address(&uart_frame.user_data[4+24+24]);
									   app_set_new_duanluqi3_pincode(&uart_frame.user_data[4+24+24+18]);
									   app_set_duanluqi3_name(&uart_frame.user_data[4+24+24+6]);

								 }
								 iris_dianbiao_meter_set_switch_info_rsp();
		#endif						 
								 break;
					 }
					 case 0x04000001:   //设置基本连接参数(主)
					 {
#if(DIANBIAO_MASTER_PARAM_SET_ENABE == 1)						 
						  unsigned short temp_short;
						  temp_short = uart_frame.user_data[5];
						  temp_short <<= 8;
						  temp_short += uart_frame.user_data[4];
						 
						  app_master_device_set_connection_interval(temp_short);
						  app_master_device_set_latency(uart_frame.user_data[6]);
						 
						  temp_short = uart_frame.user_data[8];
						  temp_short <<= 8;
						  temp_short += uart_frame.user_data[7];
						  app_master_device_set_supervision_timeout(temp_short);
#endif
						 
						  iris_dianbiao_meter_set_switch_info_rsp();
						 
					 }
					 break;
					 case 0x05000001:   //设置串口参数
					 {
						  iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x06000001:   //修改发射功率
					 {
						  if(uart_frame.user_data[4] < 5)
							{
						     app_set_ble_power_level(uart_frame.user_data[4]);
							}
						  iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x07000001:  //设置SMP 等级
					 {
						  if(uart_frame.user_data[4] < 4)
							{
						     app_set_ble_smp_level(uart_frame.user_data[4]);
							}
							
						  iris_dianbiao_meter_set_switch_info_rsp(); 
							
							printf("SMP  %d \n", 	uart_frame.user_data[4]);
							
							
					 }
					 break;
					 
					 case 0x08000001:  //设置广播过滤为MAC 方式
					 {
						  unsigned char total_item;
						   
						  total_item = (uart_frame.user_data[4] > MAX_FILTER_NUMBER)? MAX_FILTER_NUMBER:uart_frame.user_data[4];
						  app_adv_set_filter_number(total_item);
						  
						  for(i = 0; i < total_item; i++)
						  {
							  app_adv_set_filter_mac_address(i,  &uart_frame.user_data[5+i*6]);
						  }
						  iris_dianbiao_meter_set_info_rsp();	
              if(uart_frame.user_data[4] == 0)
							{
                  app_adv_set_filter_rule(FILTER_NULL);
							}
              else
              {								
							    app_adv_set_filter_rule(FILTER_BY_MAC);
				      }
							app_adv_reset_database();
					 }
					 break;
					 
					 case 0x09000001:  // 设置
					 {
						  unsigned char total_item;
						   
						  total_item = (uart_frame.user_data[4] > MAX_FILTER_NUMBER)? MAX_FILTER_NUMBER:uart_frame.user_data[4];
						  app_adv_set_filter_number(total_item);
						  for(i = 0; i < total_item; i++)
						  {
							  app_adv_set_filter_mac_kind(i,  uart_frame.user_data[5+i]);
						  }
							
						  iris_dianbiao_meter_set_info_rsp();	
              
							if(uart_frame.user_data[4] == 0)
							{
                  app_adv_set_filter_rule(FILTER_NULL);
							}
              else
							{								
							    app_adv_set_filter_rule(FILTER_BY_KIND);
							}
							app_adv_reset_database();
							
					 }
					 break;
					 
					 case 0x0A010001:  //设置广播包1 广播内容(主广播)
					 {
						  #if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)
						  app_set_ble_adv_content(&uart_frame.user_data[4], uart_frame.user_length - 4);
						  #endif
						  iris_dianbiao_meter_set_switch_info_rsp();
						 
						  printf("\n -----------------------------------------------------------------\n");
						  for(int i = 0; i < uart_frame.user_length - 4; i++)
              {
                  printf("0x%02x ", uart_frame.user_data[4 + i]);
              }	
						  printf("\n -----------------------------------------------------------------\n");
							
						  if((uart_frame.user_length - 4) < 31)
							{
						     ble_update_adv_data(&uart_frame.user_data[4], uart_frame.user_length - 4, TASK_APP);
								
								 ble_update_adv_data(&uart_frame.user_data[4], uart_frame.user_length - 4, TASK_ADV_DEVICE1);
								
							}
					 }
					 break;
					 case 0x0B000001:  //设置蓝牙工作模式
					 {
						 #if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
						  app_set_ble_work_mode(uart_frame.user_data[4]);	
						 #endif	
						 
						 iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x0D000001:  // 设置透传协议
					 {
						 iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x0E000001:  //设置基本连接参数(从)
					 {
#if(DIANBIAO_SLAVE_PARAM_SET_ENABE == 1)
              unsigned short temp_short;
						  temp_short = uart_frame.user_data[5];
						  temp_short <<= 8;
						  temp_short += uart_frame.user_data[4];
						 
						  app_slave_device_set_connection_interval(temp_short);
						  app_slave_device_set_latency(uart_frame.user_data[6]);
						 
						  temp_short = uart_frame.user_data[8];
						  temp_short <<= 8;
						  temp_short += uart_frame.user_data[7];
						  app_slave_device_set_supervision_timeout(temp_short);
#endif						 
						  iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x01000002:  //停止或启动接收广播内容
					 {
						   if(uart_frame.user_data[4] == 0x01) 
							 {
										app_adv_report_begin();
							 }
							 else if(uart_frame.user_data[4] == 0x00)
							 {
                    app_adv_report_stop();
							 }
               iris_dianbiao_meter_set_switch_info_rsp();							 
					 }
					 break;
					 
					 case 0x02000002:  //进入校时模式
					 {
#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)						 
                unsigned char mode = 2; 
			          ble_meter_struct * p_meter_param = 	(ble_meter_struct *)&uart_frame.user_data[4];	
						    nvds_put(CALIBRATION_METER_PARAM_FLAG, CALIBRATION_METER_PARAM_LENGTH, (unsigned char *)p_meter_param);
               	
                nvds_put(IRIS_CALCULATE_SETFLAG, IRIS_CALCULATE_LENGTH, &mode);
						 
								iris_dianbiao_device_reset_rsp();
						 
						    //得到台体现在的蓝牙地址
						    iris_dianbiao_save_taiti_address();
						 
						    iris_auto_reply_ack_command();
#endif
						 
					 }
					 break;
					 
					 
					 case 0x03000002:  //连接/断开指定MAC 地址设备(连接)
					 
					 case 0x04000002: // 连接/断开指定MAC 地址设备(配对)
					 {

		#if BLE_DEVICE1_UART_CLIENT_ENABLE				 
									if(app_device_compare_duanluqi1_address(&uart_frame.user_data[5]))
									{
											if(uart_frame.user_data[4])
											{
													app_device_connect_to_duanluqi_1_enable(0); //   断开
													if(app_get_duanluqi_1_connected_flag())
													{
															app_device1_disconnect();
													}
											}
											else
											{
													app_device_connect_to_duanluqi_1_enable(1);
											}
											iris_dianbiao_meter_connect_switch_rsp();
									}
		#endif

		#if BLE_DEVICE2_UART_CLIENT_ENABLE								
									if(app_device_compare_duanluqi2_address(&uart_frame.user_data[5]))
									{
											if(uart_frame.user_data[4])
											{
													app_device_connect_to_duanluqi_2_enable(0); //   断开
													if(app_get_duanluqi_2_connected_flag())
													{
															app_device2_disconnect();
													}
											}
											else
											{
													app_device_connect_to_duanluqi_2_enable(1);
											}
											iris_dianbiao_meter_connect_switch_rsp();
									}
		#endif

		#if BLE_DEVICE3_UART_CLIENT_ENABLE								
									if(app_device_compare_duanluqi3_address(&uart_frame.user_data[5]))
									{
											if(uart_frame.user_data[4])
											{
													app_device_connect_to_duanluqi_3_enable(0);//   断开
													if(app_get_duanluqi_3_connected_flag())
													{
															app_device3_disconnect();
													}
											}
											else
											{
													app_device_connect_to_duanluqi_3_enable(1);
											}
											iris_dianbiao_meter_connect_switch_rsp();
									}
		#endif							
						break; 
					 }
					 case 0x05000002:
					 {//复位命令返回
								iris_dianbiao_device_reset_rsp();
								cobra_soft_delay_ms(100);
								platform_reset(0);
								break;
					 }
					 
					 case 0x07000002:   //清除从机连接绑定信息
					 {
						   iris_dianbiao_meter_set_switch_info_rsp();
						   break;
					 }
					 default:
					 {
							 iris_dianbiao_meter_set_switch_info_rsp();
					 }
							break;
				 }
			 
		}
		
#if DIANBIAO_AUTO_ENABLE 	
		else if(uart_frame.control_cmd == 0x04)
		{
//2020/3/2 电表 自动寻找断路器			 
			 if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x01))
			 {
				 //启动自动寻找断路器流程
				 app_iris_autoconnect_process(uart_frame.user_length - 4, uart_frame.user_data + 4);
			 }
			 else if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x02))
			 {
				 //启动电表电流指纹数据采集
				 app_iris_autoconnect_process_sample_data(uart_frame.user_length, uart_frame.user_data);
			 }	
			 
			else if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x08))
			 {
//           读取广播包中的电流指纹特征码
//				   应答帧
//           68H	L1L2	898989898989	00	00	68H	84 	04 00 00 08	D0-Dn	CS	16H
//           读取广播包当中的16位特征码。
//           蓝牙模块返回断路器的MAC地址和特征码。
				     app_iris_autoconnect_get_adv_record();  //启动快速扫描, 10s 中后返回结果
				     // app_iris_autoconnect_process_sample_data(uart_frame.user_length, uart_frame.user_data);
			 }		
		}
#endif
		
		else if(uart_frame.control_cmd == 0x00)   //电表要求发送 透传数据给断路器
		{
				//iris_uart_send_to_meter(uart_frame.length_l, uart_frame.user_data);
				iris_uart_send_to_duanluqi(uart_frame.mac_address, uart_frame.length_l, uart_frame.user_data);
		}
		else if(uart_frame.control_cmd == 0x02)   //读取模块参数
		{
			     switch(cmd_data_flag)
					 {
						  case 0x01000000: //读取当前连接的 从设备
						  {
							    iris_dianbiao_slave_number_rsp();
						  }
							break;
							
							case 0x02000000:  //读取当前连接的主设备
							{
									iris_dianbiao_master_number_rsp();
							}
							break;
							
							case 0x03000000: //读取 固件版本号
							{
									iris_dianbiao_module_version_rsp();
							}
							break;
							
							case 0x04000000: //读取接收到的广播包
							{
								  app_adv_report_to_mcu();
							}
							break;
							
						  case 0x01000001: //读取 本设备的 mac 地址
						  {
								  iris_dianbiao_mac_address_rsp();
							}
							break;
							
							case 0x02000001:  //目标设备MAC 地址 + 目标设备蓝牙名称 + 目标设备配对码
							{
								 iris_dianbiao_slave_information_rsp();
							}
							break;
							
							case 0x04000001:  //基本连接参数 (主) 连接间隔 + 时延 + 超时时间
							{
						
					       iris_dianbiao_master_information_rsp();		
								
							}
							break;
							case 0x05000001:  //串口参数配置
							{
								 iris_dianbiao_rs232_information_rsp();			
							}
							break;
							
							case 0x06000001:  //发射功率
							{
								 iris_dianbiao_rf_power_information_rsp();		
							}
							break;
							
							case 0x07000001:  //SMP 等级
							{
								 iris_dianbiao_smp_information_rsp();		
							}
							break;
							
							case 0x0A010001:  //广播包1 主广播内容
							{
							  #if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)
						     iris_dianbiao_adv_content_information_rsp();
						    #endif
							}
							break;
							
				      case 0x0B000001:  //蓝牙工作模式
							{
								#if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
								 iris_dianbiao_work_mode_information_rsp();
								#endif
								
							}
							break;
							
							case 0x0D000001:  //透传协议
							{
								 iris_dianbiao_protocol_information_rsp();
							}
							break;
							
							case 0x0E000001:  //从机 基本连接参数 (连接间隔+时延+超时时间)
							{
								 iris_dianbiao_slave_param_rsp();
							}
							break;
							
							default:
								break;
					 }
 
		}
#endif
		
	
}

/*
2020/6/5 添加注释
电表根据要发送的地址, 从串口接受数据, 通过
蓝牙发送到对应的断路器
如果对应的链路没有建立,不能发送
*/
void iris_uart_send_to_duanluqi(unsigned char * mac_address, unsigned char length, unsigned char *p_buffer)
{
#if APP_SCAN_DEVICE_ENABLE	
//2020/4/24 Liujian 解析
//管理芯读取蓝牙模块的“蓝牙模块自动配对功能表状态字”。（蓝牙模块解析数据？还是直接放在管理芯解析)
// 68 17 00 43 05 83 00 00 00 18 01 ff f6 02 05 01 00 20 42 02 00 00 24 91 16 
	
//	printf("msg number = %d \n", app_iris_get_msg());
	
	  if(app_iris_get_msg() > 40) return;
	
#if BLE_DEVICE1_UART_CLIENT_ENABLE	
		if(app_get_duanluqi_1_connected_flag())
		{
				if(app_device_compare_duanluqi1_address(mac_address))
				{
							app_device1_send_data_to_uart_server(length, p_buffer);
				}
		}
#endif

#if BLE_DEVICE2_UART_CLIENT_ENABLE			
		if(app_get_duanluqi_2_connected_flag())
		{
				if(app_device_compare_duanluqi2_address(mac_address))
				{
							app_device2_send_data_to_uart_server(length, p_buffer);
				}
		}
#endif

#if BLE_DEVICE3_UART_CLIENT_ENABLE			
		if(app_get_duanluqi_3_connected_flag())
		{
				if(app_device_compare_duanluqi3_address(mac_address))
				{
							app_device3_send_data_to_uart_server(length, p_buffer);
				}
		}
#endif
#endif

#if (BLE_HOST_UART)		
		if(app_collection_2_is_ready())
		{
				if(app_device_compare_collection2_address(mac_address))
				{
						app_meter_send_data_to_collection2(length, p_buffer);
				}
		}
#endif
		
		
#if (APP_HOST2_ENABLE)
		if(app_collection_1_is_ready())
		{
				if(app_device_compare_collection1_address(mac_address))
				{
						app_meter_send_data_to_collection1(length, p_buffer);
				}
		}
#endif

		
}

#if APP_SCAN_DEVICE_ENABLE
//电表收到断路器的透传数据， 然后输出到串口
//index 是 连接的 断路器的 序号
void app_iris_dianbiao_uart_output(uint8_t index, uint8_t length,  uint8_t *p_buffer)
{
			unsigned int i;
			unsigned short toatl_length;
			unsigned short checksum = 0;
	
			unsigned char str_buffer[7];
			
#if DIANBIAO_AUTO_ENABLE 	
//2020/3/2  断路器的处理
			if(app_iris_is_modified_value(length, p_buffer))
				return ;
#endif
			
			toatl_length = length;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, toatl_length&0xFF);
			cobra_uart_write(M3_UART0, (toatl_length>>8)&0xFF);
			checksum += toatl_length&0xFF;
			checksum += ((toatl_length>>8)&0xFF);
	
			if(index == 1) //断路器1
			{
					app_get_duanluqi1_address(str_buffer);  //得到断路器1的地址
				
					cobra_uart_write(M3_UART0, str_buffer[0]);
					cobra_uart_write(M3_UART0, str_buffer[1]);
					cobra_uart_write(M3_UART0, str_buffer[2]);
					cobra_uart_write(M3_UART0, str_buffer[3]);
					cobra_uart_write(M3_UART0, str_buffer[4]);
					cobra_uart_write(M3_UART0, str_buffer[5]);
			
					checksum += str_buffer[0];
					checksum += str_buffer[1];
					checksum += str_buffer[2];
					checksum += str_buffer[3];
					checksum += str_buffer[4];
					checksum += str_buffer[5];
		  }
			else if(index == 2)
			{
					app_get_duanluqi2_address(str_buffer);  ////得到断路器2的地址
					cobra_uart_write(M3_UART0, str_buffer[0]);
					cobra_uart_write(M3_UART0, str_buffer[1]);
					cobra_uart_write(M3_UART0, str_buffer[2]);
					cobra_uart_write(M3_UART0, str_buffer[3]);
					cobra_uart_write(M3_UART0, str_buffer[4]);
					cobra_uart_write(M3_UART0, str_buffer[5]);
			
					checksum += str_buffer[0];
					checksum += str_buffer[1];
					checksum += str_buffer[2];
					checksum += str_buffer[3];
					checksum += str_buffer[4];
					checksum += str_buffer[5];
				
			}
			else if(index == 3)
			{
					app_get_duanluqi3_address(str_buffer);  ////得到断路器3的地址
				
					cobra_uart_write(M3_UART0, str_buffer[0]);
					cobra_uart_write(M3_UART0, str_buffer[1]);
					cobra_uart_write(M3_UART0, str_buffer[2]);
					cobra_uart_write(M3_UART0, str_buffer[3]);
					cobra_uart_write(M3_UART0, str_buffer[4]);
					cobra_uart_write(M3_UART0, str_buffer[5]);
			
					checksum += str_buffer[0];
					checksum += str_buffer[1];
					checksum += str_buffer[2];
					checksum += str_buffer[3];
					checksum += str_buffer[4];
					checksum += str_buffer[5];
			}
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;
			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x80);   //ACK OK
			
			checksum += 0x68;
			checksum += 0x80;
			
			for(i = 0; i < length; i++)
			{
					cobra_uart_write(M3_UART0, p_buffer[i]);
					checksum += p_buffer[i];
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
	
}

#endif

//电表接收到采集器的 数据 ， 上报出来
//index 是 连接电表的 采集器的 蓝牙地址
void app_iris_up_collection_data(unsigned char connection_index, unsigned char length, unsigned char *p_buffer)
{
			unsigned int i;
			unsigned short toatl_length;
			unsigned short checksum = 0;

			toatl_length = length;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, toatl_length&0xFF);
			cobra_uart_write(M3_UART0, (toatl_length>>8)&0xFF);
			checksum += toatl_length&0xFF;
			checksum += ((toatl_length>>8)&0xFF);
	
			//连接的采集器的 地址
			{
					struct gap_bdaddr* p_addr = gapc_get_bdaddr(connection_index, SMPC_INFO_PEER);
				
					cobra_uart_write(M3_UART0, p_addr->addr.addr[0]);
					cobra_uart_write(M3_UART0, p_addr->addr.addr[1]);
					cobra_uart_write(M3_UART0, p_addr->addr.addr[2]);
					cobra_uart_write(M3_UART0, p_addr->addr.addr[3]);
					cobra_uart_write(M3_UART0, p_addr->addr.addr[4]);
					cobra_uart_write(M3_UART0, p_addr->addr.addr[5]);
			
					checksum += p_addr->addr.addr[0];
					checksum += p_addr->addr.addr[1];
					checksum += p_addr->addr.addr[2];
					checksum += p_addr->addr.addr[3];
					checksum += p_addr->addr.addr[4];
					checksum += p_addr->addr.addr[5];
		  }
		
			
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;
			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x80);   //ACK OK
			
			checksum += 0x68;
			checksum += 0x80;
			
			for(i = 0; i < length; i++)
			{
					cobra_uart_write(M3_UART0, p_buffer[i]);
					checksum += p_buffer[i];
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}



#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)	

void iris_uart_calibration_ommmand_process(void)
{
	  volatile unsigned int i;
	  unsigned int  cmd_data_flag;
		cmd_data_flag = (unsigned int )(*(unsigned int *)uart_frame.user_data);
	
	  uint8_t mode;
		if(uart_frame.control_cmd == 0x01)  //控制码为 0x01 ,那么 数据 要 单独 分析 
		{
         switch(cmd_data_flag)
				 {
					 case 0x02000002:
					 {
						   ble_meter_struct * p_meter_param = 	(ble_meter_struct *)&uart_frame.user_data[4];	
						 
						//   printf("22*******\n");
						 
						   process_command_in_24g(p_meter_param);
						 
	//					 app_meter_param_process_uart_command(p_meter_param);
						 
						   iris_dianbiao_device_reset_rsp();
					 }
					 break;
					 
					 default:
					 {
						  iris_dianbiao_meter_set_info_rsp();
						  break;
					 }
				 }				 
	  }
		else if(uart_frame.control_cmd == 0x02)  
		{  
			 unsigned int temp_cmd;
			 temp_cmd = uart_frame.user_data[3];
			 temp_cmd <<= 8;
			 temp_cmd += uart_frame.user_data[2];
			 temp_cmd <<= 8;
			 temp_cmd += uart_frame.user_data[1];
			 temp_cmd <<= 8;
			 temp_cmd += uart_frame.user_data[0];	
			 iris_dianbiao_rsp_read_param_error(temp_cmd, 0xff);
		}
		else if(uart_frame.control_cmd == 0x00)   //电表要求发送 透传数据给台体
		{
				//iris_uart_send_to_meter(uart_frame.length_l, uart_frame.user_data);
			//   printf("11---------\n");
			   app_meter_ack_data_to_taiti(uart_frame.mac_address, uart_frame.length_l, uart_frame.user_data);
		
				//iris_uart_send_to_duanluqi(uart_frame.mac_address, uart_frame.length_l, uart_frame.user_data);
		}
	
}

void iris_calibration_loop(void)
{
		uint8_t cChar;		
		uint32_t  cmd_length;
		while(1)
		{
					cChar = cobra_uart_read(M3_UART0);

					if((uart_cmd_state == UART_CMD_IDLE)||(uart_cmd_state == UART_CMD_LOOK_PREMBLE))
					{
							if(cChar == 0xFE)
							{	
									uart_cmd_state = UART_CMD_LOOK_PREMBLE;  //接着寻找 0xFE
							}
							else if(cChar == 0x68)
							{
									uart_cmd_state = UART_CMD_LOOK_LENGTH_L;    //寻找命令
									uart_frame.checksum = 0x68;
							}
							else
							{
									uart_cmd_state = UART_CMD_IDLE;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_LENGTH_L)
					{
						  uart_frame.length_l = cChar;
							uart_cmd_state = UART_CMD_LOOK_LENGTH_H;
							uart_frame.checksum += cChar;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_LENGTH_H)
					{
							uart_frame.length_h = cChar;
							uart_cmd_state = UART_CMD_LOOK_ADDRESS;
							uart_frame.checksum += cChar;
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_ADDRESS)
					{
							
							uart_frame.mac_address[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 6)
							{
									uart_cmd_state = UART_CMD_LOOK_STA1;
							}			
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_STA1)
					{
							uart_frame.sta = cChar;
							uart_cmd_state = UART_CMD_LOOK_RESERVE;
							uart_frame.checksum += cChar;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_RESERVE)
					{
							uart_frame.reserve = cChar;
							uart_cmd_state = UART_CMD_LOOK_SECOND_BEGIN;
							uart_frame.checksum += cChar;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_SECOND_BEGIN)
					{
							if(cChar != 0x68)
							{
								 uart_cmd_state = UART_CMD_IDLE;   //error command
								 continue;
							}
							else
							{
									uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;   //寻找控制字
									uart_frame.checksum += cChar;
									continue;
							}
					}
					else if(uart_cmd_state == UART_CMD_LOOK_CONTROL_CMD)
					{
							uart_frame.control_cmd = cChar;
							uart_cmd_state = UART_CMD_LOOK_USER_DATA;   //
						  uart_frame.checksum += cChar;							
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_USER_DATA)
					{
							cmd_length = uart_frame.length_h;
							cmd_length <<= 8;
							cmd_length += uart_frame.length_l;
							uart_frame.user_data[temp_index++] = cChar;
						
						  uart_frame.checksum += cChar;
							if(temp_index == cmd_length)
							{
									uart_cmd_state = UART_CMD_LOOK_CHECKSUM;
									uart_frame.checksum &= 0xff;
							}
							
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_CHECKSUM)
					{
							if(cChar != uart_frame.checksum)
							{
									uart_cmd_state = UART_CMD_LOOK_END;  //checksum error
							}
							else
							{
								  uart_cmd_state = UART_CMD_LOOK_END;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_END)
					{
							if(cChar != 0x16)
							{
									uart_cmd_state = UART_CMD_IDLE;
									continue;
							}
							else
							{
										//
										//printf("OK>>>>>>>>>>>>>\n");
										iris_uart_calibration_ommmand_process();
										uart_cmd_state = UART_CMD_IDLE;
										
							}
					}
					
					//这个需要串口端一直有数据来查询模组, 否则数据发送不过去到台体
					app_metetr_process_commucate_data();
					
		}
	

}

#endif

static uint32_t timer_reply_timeout_index;

void app_iris_reply_timeout_function(unsigned int param)
{
	   //app_cobra_soft_timer_uninit();
	  // cobra_soft_delay_ms(200);
	 //  platform_reset(0);
	    cobra_app_reboot_task();
}


void iris_auto_reply_ack_command(void)
{
    timer_reply_timeout_index = cobra_soft_timer_create(app_iris_reply_timeout_function);
	
		cobra_soft_timer_start(timer_reply_timeout_index, COBRA_SOFT_TIMER_TICK*3, 0);   //
	
}


void convert_address_to_ascii_hex(long long address, unsigned char *p_buffer)
{
		uint32_t temp;
	  uint32_t i;
	  
	  unsigned char ch_high;
	  unsigned char ch_low;
		
	  if(p_buffer == NULL)  return ;
	
	  for(i = 0; i < 4; i++)
	  {
		   ch_low =  address%10;
		 
	     address /= 10;
	
		   ch_high= address%10;
		
		   address /= 10;
	
	     p_buffer[i] = (ch_high << 4) + ch_low;
	 }
	
}

static unsigned char taiti_mac_address[BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/*
  得到当时连接时台体的物理地址
  原则上不应该当时有两个主设备联机在上面
*/
void iris_dianbiao_save_taiti_address(void)
{
	 if(app_collection_2_is_ready())
	 {
		  app_get_collection2_address(taiti_mac_address);
	 }
	 
#if(APP_HOST2_ENABLE)		 
	 else if(app_collection_1_is_ready())
	 { 
		   app_get_collection1_address(taiti_mac_address);
	 }
#endif
	 
}

unsigned char * iris_dianbiao_get_taiti_address(void)
{
	  return taiti_mac_address;
}

#endif
