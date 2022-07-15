/*	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_iris_console.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/19

	Description :   
	  ��������, ����ʵ�� 68 Э��		

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
�����ж���������ϵ����ݲ����� 2s ��֮���յ�ȫ������, �Ϳ�ʼ������

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
									uart_cmd_state = UART_CMD_LOOK_PREMBLE;  //����Ѱ�� 0xFE
							}
							else if(cChar == 0x68)
							{
									uart_cmd_state = UART_CMD_LOOK_LENGTH_L;    //Ѱ������
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
									uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;   //Ѱ�ҿ�����
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
//�ش�������ģ��
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

////���õ����Ϣ�ظ�
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

//���ص��ǰ���ӵĴ��豸�ĵ�ַ
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
			
			if(length < 10)  //˵��û�������κζ���, ���ش���
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
   ���ض�ȡ�������������

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
			
			cobra_uart_write(M3_UART0, 0x00);  //����״̬
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

//�������������õĴӻ���Ϣ
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
			
		//��·��1 ��Ϣ
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
		//��·��2
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
		//��·��3
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
  ����������Ӳ��� (��)

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
			temp = app_master_device_get_connection_interval();   //���Ӽ��
			
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
      temp = 15*5*1.25;   //���Ӽ��
			
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


//���洮�ڲ�����Ϣ
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
			
      cobra_uart_write(M3_UART0, 0x08);   //8bit ֹͣλ
			
			cobra_uart_write(M3_UART0, 0x02);    //żУ��
			
			cobra_uart_write(M3_UART0, 0x00);     // 1 λֹͣλ

      checksum += 0x08;    
			checksum += 0x02;    
			checksum += 0x00;    
 
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

/*
���书����Ϣ
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
����SMP �ȼ���Ϣ
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
��ȡ�Լ����õĹ㲥�����ݵ���Ϣ
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
�õ���������ģʽ����Ϣ
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
��ȡ͸����Ϣ
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
     
		  
      temp = 0x02;            // 698 Э��
			cobra_uart_write(M3_UART0, temp);
			checksum += temp; 
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
	
}

/*
  ��ȡ ��Ϊ���豸�����Ӳ���
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
			temp = app_slave_device_get_connection_interval();   //���Ӽ��
			
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
      temp = APP_ADV_INT_MIN*1.25;   //���Ӽ��
			
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
//��ȡ���ǰ���ӵ����豸 ��ַ
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
			
			if(length < 10)  //û�������κ�����, Ҳ�������
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
	  if(uart_frame.control_cmd == 0x01)  //������Ϊ 0x01 ,��ô ���� Ҫ ���� ���� 
		{
					 switch(cmd_data_flag)
					 {
						 case 0x01000001:   //���ñ��豸MAC��ַ+���豸��������+���豸�����
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
										  /*  ������ַ�����5 Byte ���ǵ���ͨѶ��ַ */
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
											
											appm_send_gapm_reset_cmd();   //һ��Ҫ�յ����ڹ���, Ҳ���ǹ���о���͵����õ�ַ,�ſ�����������
											
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
					 
					 case 0x02000001:  //(Ŀ���豸 1 MAC ��ַ + Ŀ���豸1 �������� + Ŀ���豸1 �����) X 3
					 {
								 //���õ����� ���ӵ� ��·�� �� ������ַ 
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
					 case 0x04000001:   //���û������Ӳ���(��)
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
					 case 0x05000001:   //���ô��ڲ���
					 {
						  iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x06000001:   //�޸ķ��书��
					 {
						  if(uart_frame.user_data[4] < 5)
							{
						     app_set_ble_power_level(uart_frame.user_data[4]);
							}
						  iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x07000001:  //����SMP �ȼ�
					 {
						  if(uart_frame.user_data[4] < 4)
							{
						     app_set_ble_smp_level(uart_frame.user_data[4]);
							}
							
						  iris_dianbiao_meter_set_switch_info_rsp(); 
							
							printf("SMP  %d \n", 	uart_frame.user_data[4]);
							
							
					 }
					 break;
					 
					 case 0x08000001:  //���ù㲥����ΪMAC ��ʽ
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
					 
					 case 0x09000001:  // ����
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
					 
					 case 0x0A010001:  //���ù㲥��1 �㲥����(���㲥)
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
					 case 0x0B000001:  //������������ģʽ
					 {
						 #if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
						  app_set_ble_work_mode(uart_frame.user_data[4]);	
						 #endif	
						 
						 iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x0D000001:  // ����͸��Э��
					 {
						 iris_dianbiao_meter_set_switch_info_rsp();
					 }
					 break;
					 case 0x0E000001:  //���û������Ӳ���(��)
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
					 case 0x01000002:  //ֹͣ���������չ㲥����
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
					 
					 case 0x02000002:  //����Уʱģʽ
					 {
#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)						 
                unsigned char mode = 2; 
			          ble_meter_struct * p_meter_param = 	(ble_meter_struct *)&uart_frame.user_data[4];	
						    nvds_put(CALIBRATION_METER_PARAM_FLAG, CALIBRATION_METER_PARAM_LENGTH, (unsigned char *)p_meter_param);
               	
                nvds_put(IRIS_CALCULATE_SETFLAG, IRIS_CALCULATE_LENGTH, &mode);
						 
								iris_dianbiao_device_reset_rsp();
						 
						    //�õ�̨�����ڵ�������ַ
						    iris_dianbiao_save_taiti_address();
						 
						    iris_auto_reply_ack_command();
#endif
						 
					 }
					 break;
					 
					 
					 case 0x03000002:  //����/�Ͽ�ָ��MAC ��ַ�豸(����)
					 
					 case 0x04000002: // ����/�Ͽ�ָ��MAC ��ַ�豸(���)
					 {

		#if BLE_DEVICE1_UART_CLIENT_ENABLE				 
									if(app_device_compare_duanluqi1_address(&uart_frame.user_data[5]))
									{
											if(uart_frame.user_data[4])
											{
													app_device_connect_to_duanluqi_1_enable(0); //   �Ͽ�
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
													app_device_connect_to_duanluqi_2_enable(0); //   �Ͽ�
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
													app_device_connect_to_duanluqi_3_enable(0);//   �Ͽ�
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
					 {//��λ�����
								iris_dianbiao_device_reset_rsp();
								cobra_soft_delay_ms(100);
								platform_reset(0);
								break;
					 }
					 
					 case 0x07000002:   //����ӻ����Ӱ���Ϣ
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
//2020/3/2 ��� �Զ�Ѱ�Ҷ�·��			 
			 if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x01))
			 {
				 //�����Զ�Ѱ�Ҷ�·������
				 app_iris_autoconnect_process(uart_frame.user_length - 4, uart_frame.user_data + 4);
			 }
			 else if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x02))
			 {
				 //����������ָ�����ݲɼ�
				 app_iris_autoconnect_process_sample_data(uart_frame.user_length, uart_frame.user_data);
			 }	
			 
			else if((uart_frame.user_data[0] == 0x04)&&(uart_frame.user_data[1] == 0x00) \
				 &&(uart_frame.user_data[2] == 0x00)&&(uart_frame.user_data[3] == 0x08))
			 {
//           ��ȡ�㲥���еĵ���ָ��������
//				   Ӧ��֡
//           68H	L1L2	898989898989	00	00	68H	84 	04 00 00 08	D0-Dn	CS	16H
//           ��ȡ�㲥�����е�16λ�����롣
//           ����ģ�鷵�ض�·����MAC��ַ�������롣
				     app_iris_autoconnect_get_adv_record();  //��������ɨ��, 10s �к󷵻ؽ��
				     // app_iris_autoconnect_process_sample_data(uart_frame.user_length, uart_frame.user_data);
			 }		
		}
#endif
		
		else if(uart_frame.control_cmd == 0x00)   //���Ҫ���� ͸�����ݸ���·��
		{
				//iris_uart_send_to_meter(uart_frame.length_l, uart_frame.user_data);
				iris_uart_send_to_duanluqi(uart_frame.mac_address, uart_frame.length_l, uart_frame.user_data);
		}
		else if(uart_frame.control_cmd == 0x02)   //��ȡģ�����
		{
			     switch(cmd_data_flag)
					 {
						  case 0x01000000: //��ȡ��ǰ���ӵ� ���豸
						  {
							    iris_dianbiao_slave_number_rsp();
						  }
							break;
							
							case 0x02000000:  //��ȡ��ǰ���ӵ����豸
							{
									iris_dianbiao_master_number_rsp();
							}
							break;
							
							case 0x03000000: //��ȡ �̼��汾��
							{
									iris_dianbiao_module_version_rsp();
							}
							break;
							
							case 0x04000000: //��ȡ���յ��Ĺ㲥��
							{
								  app_adv_report_to_mcu();
							}
							break;
							
						  case 0x01000001: //��ȡ ���豸�� mac ��ַ
						  {
								  iris_dianbiao_mac_address_rsp();
							}
							break;
							
							case 0x02000001:  //Ŀ���豸MAC ��ַ + Ŀ���豸�������� + Ŀ���豸�����
							{
								 iris_dianbiao_slave_information_rsp();
							}
							break;
							
							case 0x04000001:  //�������Ӳ��� (��) ���Ӽ�� + ʱ�� + ��ʱʱ��
							{
						
					       iris_dianbiao_master_information_rsp();		
								
							}
							break;
							case 0x05000001:  //���ڲ�������
							{
								 iris_dianbiao_rs232_information_rsp();			
							}
							break;
							
							case 0x06000001:  //���书��
							{
								 iris_dianbiao_rf_power_information_rsp();		
							}
							break;
							
							case 0x07000001:  //SMP �ȼ�
							{
								 iris_dianbiao_smp_information_rsp();		
							}
							break;
							
							case 0x0A010001:  //�㲥��1 ���㲥����
							{
							  #if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)
						     iris_dianbiao_adv_content_information_rsp();
						    #endif
							}
							break;
							
				      case 0x0B000001:  //��������ģʽ
							{
								#if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
								 iris_dianbiao_work_mode_information_rsp();
								#endif
								
							}
							break;
							
							case 0x0D000001:  //͸��Э��
							{
								 iris_dianbiao_protocol_information_rsp();
							}
							break;
							
							case 0x0E000001:  //�ӻ� �������Ӳ��� (���Ӽ��+ʱ��+��ʱʱ��)
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
2020/6/5 ���ע��
������Ҫ���͵ĵ�ַ, �Ӵ��ڽ�������, ͨ��
�������͵���Ӧ�Ķ�·��
�����Ӧ����·û�н���,���ܷ���
*/
void iris_uart_send_to_duanluqi(unsigned char * mac_address, unsigned char length, unsigned char *p_buffer)
{
#if APP_SCAN_DEVICE_ENABLE	
//2020/4/24 Liujian ����
//����о��ȡ����ģ��ġ�����ģ���Զ���Թ��ܱ�״̬�֡���������ģ��������ݣ�����ֱ�ӷ��ڹ���о����)
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
//����յ���·����͸�����ݣ� Ȼ�����������
//index �� ���ӵ� ��·���� ���
void app_iris_dianbiao_uart_output(uint8_t index, uint8_t length,  uint8_t *p_buffer)
{
			unsigned int i;
			unsigned short toatl_length;
			unsigned short checksum = 0;
	
			unsigned char str_buffer[7];
			
#if DIANBIAO_AUTO_ENABLE 	
//2020/3/2  ��·���Ĵ���
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
	
			if(index == 1) //��·��1
			{
					app_get_duanluqi1_address(str_buffer);  //�õ���·��1�ĵ�ַ
				
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
					app_get_duanluqi2_address(str_buffer);  ////�õ���·��2�ĵ�ַ
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
					app_get_duanluqi3_address(str_buffer);  ////�õ���·��3�ĵ�ַ
				
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

//�����յ��ɼ����� ���� �� �ϱ�����
//index �� ���ӵ��� �ɼ����� ������ַ
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
	
			//���ӵĲɼ����� ��ַ
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
		if(uart_frame.control_cmd == 0x01)  //������Ϊ 0x01 ,��ô ���� Ҫ ���� ���� 
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
		else if(uart_frame.control_cmd == 0x00)   //���Ҫ���� ͸�����ݸ�̨��
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
									uart_cmd_state = UART_CMD_LOOK_PREMBLE;  //����Ѱ�� 0xFE
							}
							else if(cChar == 0x68)
							{
									uart_cmd_state = UART_CMD_LOOK_LENGTH_L;    //Ѱ������
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
									uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;   //Ѱ�ҿ�����
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
					
					//�����Ҫ���ڶ�һֱ����������ѯģ��, �������ݷ��Ͳ���ȥ��̨��
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
  �õ���ʱ����ʱ̨��������ַ
  ԭ���ϲ�Ӧ�õ�ʱ���������豸����������
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
