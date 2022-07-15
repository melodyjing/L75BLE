/*	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Lufeng
	
	Cobra Drivers

	File Name	:	app_realtek_console.c
	Version		:	0.1
	Created	By	:	Lufeng
	Date		:	2021/1/7

	Description :   
	  串口命令, 用于实现 68 协议		

	Changed Log	:

		Lufeng  2021/1/7			- Creation

*****************************************************************************/
#if (CFG_APP_CONSOLE_CMD && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "app_realtek_console.h"
#include "app_realtek_device.h"
#include "app_realtek_uart_cmd_handler_export.h"

LOCAL void handleUartCommmand(void);
void iris_uart_send_to_duanluqi(UCHAR * mac_address, UCHAR length, UCHAR *p_buffer);
void iris_dianbiao_rsp_read_param_error(UINT cmd, UCHAR error_number);
void iris_dianbiao_device_set_rsp(UINT kind, UCHAR result);
void iris_auto_reply_ack_command(void);
void iris_dianbiao_save_taiti_address(void);
/*
用于判断如果串口上的数据不能在 2s 中之内收到全部数据, 就开始报错了

*/
LOCAL UINT32 dianbiao_console_timeout;
LOCAL UART_CMD_FRAME uart_frame;
LOCAL UINT temp_index;
LOCAL UINT uart_cmd_state = UART_CMD_IDLE;
LOCAL UCHAR   rdw_pin_name[6];
LOCAL UINT32 realtek_timeout;
LOCAL UINT32 realtek_fadv_timer;
LOCAL UINT8 realtek_fadv_intv;
LOCAL bool realtek_fadv_enable;
#if CMD_DEV_ADDR_FUNCTION
bool readcommand_with_para(void);
#endif
#if BLE_DATA_RECEIVE_MODE
enum BLE_DATA_RECE_MODE ble_data_rece_mode=DIRECT_DAT_RECE_MODE;
void app_set_ble_data_rece_mode(enum BLE_DATA_RECE_MODE mode)
{
	ble_data_rece_mode = mode;
}
enum BLE_DATA_RECE_MODE app_get_ble_data_rece_mode(void)
{
	return ble_data_rece_mode;
}
#endif
#if CMD_DEV_NAME_FUNCTION
void app_set_device_name(USHORT length,UCHAR *name)
{
    nvds_put(NVDS_TAG_DEVICE_NAME, length, name);
		memcpy(app_env.dev_name, name, length);
		app_env.dev_name_len = length;
}
USHORT app_get_device_name(UCHAR *name)
{
		memcpy(name, app_env.dev_name, app_env.dev_name_len);
		return app_env.dev_name_len;
}
#endif
void set_appm_adv_started_state(void)
{
	 app_env.adv_state = APP_ADV_STATE_STARTED;
}
void app_dianbiao_console_timeout_function(UINT temp)
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

void app_realtek_timeout_callfunction(UINT temp)
{
   LOCAL  UINT8 cs_timeout=0;
	 cs_timeout++;
	 if(get_state_report()== 1 || cs_timeout > 2)
	 {
		 cobra_soft_timer_stop(realtek_timeout);
		 cs_timeout = 0;
	 }
	 else
		 cobra_soft_timer_start(realtek_timeout, 1000, 0);
	 cobra_app_set_reset_report_task();
}
void  realtek_dianbiao_report_init(void)
{
	 realtek_timeout = cobra_soft_timer_create(app_realtek_timeout_callfunction);
	 cobra_soft_timer_start(realtek_timeout, 1000, 0);
}

void app_realtek_db_filter_adv_timeout_callfunction(UINT temp)
{

	 if(realtek_fadv_enable == 0)
		 cobra_soft_timer_stop(realtek_fadv_timer);
	 else
	 {
		 if(realtek_fadv_intv == 0)
			 cobra_soft_timer_start(realtek_fadv_timer, 200, 0);
		 else
		   cobra_soft_timer_start(realtek_fadv_timer, 100*realtek_fadv_intv, 0);		
     
     cmd_get_filter_info_read_handler();	
     printf("app_realtek_db_filter_adv_timeout_callfunction\n");		 
	 }
}
void  realtek_db_filter_adv_report_init(UINT8 enable,UINT8 intv_time)
{
	 realtek_fadv_enable = enable;
	 realtek_fadv_intv   = intv_time;
	 if(realtek_fadv_intv>0x14)
	   realtek_fadv_intv = 0;         //如果超范围，默认为0，2秒
	 if(realtek_fadv_enable == false)
	 {
		 if(realtek_fadv_timer != 0)
		 cobra_soft_timer_stop(realtek_fadv_timer);
		 return;
	 }
	 if(realtek_fadv_timer == 0)
	   realtek_fadv_timer = cobra_soft_timer_create(app_realtek_db_filter_adv_timeout_callfunction);
	 else
		 cobra_soft_timer_stop(realtek_fadv_timer);
	 
	 if(realtek_fadv_intv == 0)
		 cobra_soft_timer_start(realtek_fadv_timer, 200, 0);
	 else	 
	   cobra_soft_timer_start(realtek_fadv_timer, 100*realtek_fadv_intv, 0);
}
void convert_address_to_ascii_hex(long long address, UCHAR *p_buffer)
{
		UINT32 temp;
	  UINT32 i;
	  
	  UCHAR ch_high;
	  UCHAR ch_low;
		
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

void iris_uart_task_callback(void)
{
		UINT8 cChar;		
		UINT32  cmd_length;
		while(1)
		{
			if (true == QueueOut(CMD_Uart_buffer, cChar)) 
			{
//				printf("=%x\r\n",uart_cmd_state);
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
							uart_cmd_state = UART_CMD_LOOK_TYPE;
							uart_frame.checksum += cChar;
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_TYPE)
					{
							uart_frame.type = cChar;
							uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;
							uart_frame.checksum += cChar;
						  temp_index = 0;
						  continue;
					}				
					else if(uart_cmd_state == UART_CMD_LOOK_CONTROL_CMD)
					{
							uart_frame.control_cmd[temp_index++] = cChar;
						  uart_frame.checksum += cChar;
							if(temp_index == 4)
							{
									uart_cmd_state = UART_CMD_LOOK_ADDRESS;
									uart_frame.checksum &= 0xff;
								  temp_index = 0;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_ADDRESS)
					{
							
							uart_frame.mac_address[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 6)
							{
									uart_cmd_state = UART_CMD_LOOK_RESERVE;
								  temp_index = 0;
							}		
               continue;							
					}
					else if(uart_cmd_state == UART_CMD_LOOK_RESERVE)
					{
							uart_frame.reserve[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 4)
							{
									uart_cmd_state = UART_CMD_LOOK_SECOND_BEGIN;
								  temp_index = 0;
							}		
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
								#if CMD_DEV_ADDR_FUNCTION
								  if(uart_frame.type == 3 && !readcommand_with_para())
								#else
									if(uart_frame.type == 3)
								#endif
									{
									uart_cmd_state = UART_CMD_LOOK_CHECKSUM;   //寻找控制字
									uart_frame.checksum += cChar;
									uart_frame.checksum &= 0xff;
								  temp_index = 0;
									continue;										
									}
									else
									{
									uart_cmd_state = UART_CMD_LOOK_USER_DATA;   //寻找控制字
									uart_frame.checksum += cChar;
								  temp_index = 0;
									continue;
									}
							}
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
//							printf("111 %x %x\n",cChar,uart_frame.checksum);
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
//										printf("OK>>>>>>>>>>>>>\n");
										uart_frame.user_length = uart_frame.length_h;
										uart_frame.user_length <<= 8;
										uart_frame.user_length += uart_frame.length_l;
										handleUartCommmand();
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

UCHAR   rdw_bluetooth_name[12];

LOCAL volatile long long calculate_comm_address;

LOCAL volatile UCHAR comm_address[4];

UCHAR * iris_uart_get_address(void)
{
     return 	comm_address;
}

void handleUartCommmand(void)
{
	  UINT  uartCmdFlag;
	  UINT8 i;

	  uartCmdFlag = (uart_frame.control_cmd[0]&0xff) | ((uart_frame.control_cmd[1]<<8)&0xff00)\
	                | ((uart_frame.control_cmd[2]<<16)&0xff0000) | ((uart_frame.control_cmd[3]<<24)&0xff000000);
	  
	  if(uart_frame.type == UART_CMD_WRITE)  //报文类型为配置蓝牙模块参数 02H
		{
					 switch(uartCmdFlag)
					 {
					 case CMD_DEV_PAIR_MODE:   //设置设备配对码
							cmd_dev_pair_mode_handler(uart_frame.user_length,uart_frame.user_data);									
							break;
					 case CMD_DEV_MAC_ADDR:
					 {
							UINT8 length;
							UINT8 set_flag = 0;
							length = IRIS_MAC_SETFLAG_LENGTH;
							if(rwip_param.get(IRIS_MAC_SETFLAG, &length, &set_flag) != PARAM_OK)
									set_flag = 0;		

//							if(set_flag == 0)
							{
									set_flag = 1;
									nvds_put(IRIS_MAC_SETFLAG, IRIS_MAC_SETFLAG_LENGTH, &set_flag);
									cmd_dev_mac_addr_handler(uart_frame.user_length,uart_frame.user_data);											
							}
//							else						
//									realtek_dianBiao_set_rsp(CMD_DEV_MAC_ADDR,CMD_ERROR_NO);
						 break;
					 }
					 case CMD_MODULE_INFO:
					 {
						 cmd_module_info_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }		
					 #if BLE_DATA_RECEIVE_MODE
					 case CMD_BLE_DATA_MODE:
					 {
             cmd_ble_data_receive_mode_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }
					 #endif	
					 #if SCAN_ENABLE_FUCTION
					 case CMD_SCAN:
					 {
             cmd_scan_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }				
           #endif					 
					 case CMD_BLE_PARAMS:
					 {
						 cmd_ble_params_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }	
					 case CMD_FILTER_ADDR:
					 {
             cmd_filter_addr_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }	
					 case CMD_FILTER_TYPE:
					 {
             cmd_filter_type_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }
					 case CMD_ADV_INFO_REPORT:
					 {
             cmd_adv_info_report_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }		
					 case CMD_UART_PARAMS:
					 {
             cmd_uart_params_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }		
					 case CMD_ADV_DATA:
					 {
             cmd_adv_data_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }		
					 case CMD_RESP_DATA:
					 {
             cmd_resq_data_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }			
					 #if CMD_DEV_NAME_FUNCTION
					 case CMD_DEV_NAME:
					 {
             cmd_device_name_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }
					 #endif	
					 #if CMD_TX_POWER_FUNCTION
					 case CMD_TX_POWER:
					 {
             cmd_tx_power_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif
					 #if CMD_ADV_INTERVAL_FUNCTION
					 case CMD_ADV_INTERVAL:
					 {
             cmd_adv_interval_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif	
					 #if CMD_DEV_ADDR_FUNCTION
					 case CMD_DEV_ADDR:
					 {
             cmd_dev_add_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif	
					 #if CMD_DEV_PAIR_MODE_FUNCTION
					 case CMD_DEV_PAIR_MM:
					 {
             cmd_dev_pair_mm_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif		
					 #if CMD_DEV_PAIR_VALUE_FUNCTION
					 case CMD_DEV_PAIR_VALUE:
					 {
             cmd_dev_pair_value_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif		
					 #if CMD_LINK_CMD_FUNCTION
					 case CMD_LINK_CMD:
					 {
             cmd_link_cmd_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif		
					 #if CMD_ADV_FUNCTION
					 case CMD_ADV:
					 {
             cmd_adv_handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }					 
					 #endif			
					 case CMD_PD:
					 {
             cmd_pd_Handler(uart_frame.user_length,uart_frame.user_data);
						 break;
					 }						 					 					 					 				 
						 default:
							 break;
				 }
			 
		}
		else if(uart_frame.type == UART_CMD_READ)
		{
			 switch(uartCmdFlag)
					 {
						 case CMD_DEV_PAIR_MODE:
						 {
								  cmd_dev_pair_mode_read_handler();
								  break;
						 }		
						 case CMD_DEV_MAC_ADDR:
						 {
								  cmd_dev_mac_addr_read_handler();
								  break;
						 }					
						 case CMD_BLE_PARAMS:
						 {
								  cmd_ble_params_read_handler();
								  break;
						 }				
						 #if BLE_DATA_RECEIVE_MODE
						 case CMD_BLE_DATA_MODE:
						 {
                  cmd_ble_data_rece_mode_read_handler();
							    break;
						 }
						 #endif	
						 #if CMD_DEV_NAME_FUNCTION
						 case CMD_DEV_NAME:
						 {
                  cmd_device_name_read_handler();
							    break;
						 }						 
						 #endif
						 #if CMD_TX_POWER_FUNCTION
						 case CMD_TX_POWER:
						 {
                  cmd_tx_power_read_handler();
							    break;
						 }
						 #endif					 
						 #if CMD_ADV_INTERVAL_FUNCTION
						 case CMD_ADV_INTERVAL:
						 {
									 cmd_adv_interval_read_handler();
									 break;
						 }					 
						 #endif	
						 #if CMD_DEV_ADDR_FUNCTION
						 case CMD_DEV_ADDR:
						 {
                   cmd_dev_addr_read_handler(uart_frame.user_length,uart_frame.user_data);
							     break;
						 }					 
						 #endif			
						 #if CMD_DEV_PAIR_MODE_FUNCTION
						 case CMD_DEV_PAIR_MM:
						 {
									 cmd_dev_pair_mm_read_handler(uart_frame.user_length,uart_frame.user_data);
									 break;
						 }					 
						 #endif			
						 #if CMD_DEV_PAIR_VALUE_FUNCTION
						 case CMD_DEV_PAIR_VALUE:
						 {
							     cmd_dev_pair_value_read_handler(uart_frame.user_length,uart_frame.user_data);
							     break;
						 }					 
						 #endif	
						 #if CMD_FIRMWARE_BUILD_TIME_FUNCTION
						 case CMD_FIRMWARE_BUILD_TIME:
						 {
							     cmd_fw_build_time_read_handler();
							 break;
						 }					 
						 #endif		
						 #if CMD_LINK_RSSI_FUNCTION
						 case CMD_LINK_RSSI:
						 {
							     cmd_link_rssi_read_handler(uart_frame.user_length,uart_frame.user_data);
							 break;
						 }					 
						 #endif						 
						 case CMD_BLE_CONN_INFO:   //
						 {
                  cmd_ble_conn_info_handler(UART_CMD_READ);
							    break;
						 }
						 case CMD_FIRMWARE_VERSION:   //
						 {
                  cmd_firmware_version_read_handler();
							    break;
						 }		
						 case CMD_GET_FILTER_INFO:   //
						 {
                  cmd_get_filter_info_read_handler();
							    break;
						 }
					 }
		}			
		else if(uart_frame.type == UART_ANSWER)
		{
			 switch(uartCmdFlag)
					 {
						 case CMD_MODULE_INFO:   //
						 {
								  report_up_rsp(MODUL_RESET,uart_frame.user_data);
							    break;
						 }
					 }
		}		
		else if(uart_frame.type == UART_CMD_TRANS)
		{
			 switch(uartCmdFlag)
					 {
						 case 0x00:   //
						 {
								  iris_uart_send_to_duanluqi(uart_frame.mac_address, uart_frame.length_l, uart_frame.user_data);
							    break;
						 }
					 }
		}		
}

/*
2020/6/5 添加注释
电表根据要发送的地址, 从串口接受数据, 通过
蓝牙发送到对应的断路器
如果对应的链路没有建立,不能发送
*/
void iris_uart_send_to_duanluqi(UCHAR * mac_address, UCHAR length, UCHAR *p_buffer)
{
#if APP_SCAN_DEVICE_ENABLE	
//2020/4/24 Liujian 解析
//管理芯读取蓝牙模块的“蓝牙模块自动配对功能表状态字”。（蓝牙模块解析数据？还是直接放在管理芯解析)
// 68 17 00 43 05 83 00 00 00 18 01 ff f6 02 05 01 00 20 42 02 00 00 24 91 16 
	
//	printf("msg number = %d \n", app_iris_get_msg());
	
	  if(app_iris_get_msg() > 40) return;
	
#if BLE_DEVICE1_UART_CLIENT_ENABLE	
//	printf("333 = %d %d %x %x %x %x\n", app_get_duanluqi_1_connected_flag(),app_get_duanluqi_2_connected_flag(),p_buffer[0],p_buffer[1],p_buffer[2],p_buffer[3]);
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
		
		if(app_collection_2_is_ready())
		{
				if(app_device_compare_collection2_address(mac_address))
				{
						app_meter_send_data_to_collection2(length, p_buffer);
				}
		}

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

void iris_uart_calibration_ommmand_process(void)
{
	  volatile UINT i;
	  UINT8 mode;
	  UINT  cmd_data_flag;
	  cmd_data_flag = (uart_frame.control_cmd[0]&0xff) | ((uart_frame.control_cmd[1]<<8)&0xff00)\
	                | ((uart_frame.control_cmd[2]<<16)&0xff0000) | ((uart_frame.control_cmd[3]<<24)&0xff000000);
	  printf("type=%x %x ",uart_frame.type, cmd_data_flag);
	  if(uart_frame.type == UART_CMD_WRITE)  //报文类型为配置蓝牙模块参数 02H
		{
					 switch(cmd_data_flag)
					 {
//						 case 0x88000002:   //
//						if(uart_frame.user_data[0] == 0x01)  //收模式
//						{
//									mode = 1;
//						}
//						else if(uart_frame.user_data[4] == 0x02)  //发送模式
//						{
//								mode = 2;
//							
//						}
//						else              //退出校时模式
//						{
//								mode = 0;
//						}
//						
//						nvds_put(IRIS_CALCULATE_SETFLAG, IRIS_CALCULATE_LENGTH, &mode);
//						iris_dianbiao_device_reset_rsp();
//						for(i = 0; i < 0x1000; i++);
//						platform_reset(0);
//						break;
						case CMD_PD:   //
							{
								ble_meter_struct *p_meter_param = (ble_meter_struct *)(&uart_frame.user_data[0]);
								app_meter_param_process(p_meter_param);
								realtek_dianBiao_set_rsp(CMD_PD,CMD_ERROR_NO);
								break;							
							}
					  default:
						 {
								realtek_dianBiao_set_rsp(cmd_data_flag,CMD_ERROR_UNDEF);
								break;
						 }
					 }
		}	
}

void iris_calibration_loop(void)
{
		UINT8 cChar;		
		UINT32  cmd_length;
		while(1)
		{
					cChar = cobra_uart_read(M3_UART0);
//			    printf("uart_cmd_state %x %x\n",uart_cmd_state,cChar);
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
//						printf("length_l %x",cChar);
						  uart_frame.length_l = cChar;
							uart_cmd_state = UART_CMD_LOOK_LENGTH_H;
							uart_frame.checksum += cChar;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_LENGTH_H)
					{
//						printf("length_h %x",cChar);
							uart_frame.length_h = cChar;
							uart_cmd_state = UART_CMD_LOOK_TYPE;
							uart_frame.checksum += cChar;
							temp_index = 0;
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_TYPE)
					{
							uart_frame.type = cChar;
							uart_cmd_state = UART_CMD_LOOK_CONTROL_CMD;
							uart_frame.checksum += cChar;
						  temp_index = 0;
						  continue;
					}				
					else if(uart_cmd_state == UART_CMD_LOOK_CONTROL_CMD)
					{
							uart_frame.control_cmd[temp_index++] = cChar;
						  uart_frame.checksum += cChar;
							if(temp_index == 4)
							{
									uart_cmd_state = UART_CMD_LOOK_ADDRESS;
									uart_frame.checksum &= 0xff;
								  temp_index = 0;
							}
							continue;
					}
					else if(uart_cmd_state == UART_CMD_LOOK_ADDRESS)
					{
							
							uart_frame.mac_address[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 6)
							{
									uart_cmd_state = UART_CMD_LOOK_RESERVE;
								  temp_index = 0;
							}		
              continue;							
					}
					else if(uart_cmd_state == UART_CMD_LOOK_RESERVE)
					{
							uart_frame.reserve[temp_index++] = cChar;
							uart_frame.checksum += cChar;
							if(temp_index == 4)
							{
									uart_cmd_state = UART_CMD_LOOK_SECOND_BEGIN;
								  temp_index = 0;
							}		
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
								  if(uart_frame.type == 3)
									{
									uart_cmd_state = UART_CMD_LOOK_CHECKSUM;   //寻找控制字
									uart_frame.checksum += cChar;
								  temp_index = 0;
									continue;										
									}
									else
									{
									uart_cmd_state = UART_CMD_LOOK_USER_DATA;   //寻找控制字
									uart_frame.checksum += cChar;
								  temp_index = 0;
									continue;
									}
							}
					}
					else if(uart_cmd_state == UART_CMD_LOOK_USER_DATA)
					{
							cmd_length = uart_frame.length_h;
							cmd_length <<= 8;
							cmd_length += uart_frame.length_l;
//							printf("222 %x %x  %x %x\n",cmd_length,temp_index,uart_frame.length_h,uart_frame.length_l);
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
							printf("111 %x %x",cChar,uart_frame.checksum);
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
//										printf("OK>>>>>>>>>>>>>\n");
										uart_frame.user_length = uart_frame.length_h;
										uart_frame.user_length <<= 8;
										uart_frame.user_length += uart_frame.length_l;
										iris_uart_calibration_ommmand_process();
										uart_cmd_state = UART_CMD_IDLE;
										
							}
					}
					
		}
	

}

LOCAL UINT32 timer_reply_timeout_index;

void app_iris_reply_timeout_function(UINT param)
{
	    cobra_app_reboot_task();
}

void iris_auto_reply_ack_command(void)
{
    timer_reply_timeout_index = cobra_soft_timer_create(app_iris_reply_timeout_function);
	
		cobra_soft_timer_start(timer_reply_timeout_index, COBRA_SOFT_TIMER_TICK*3, 0);   //
	
}

LOCAL UCHAR taiti_mac_address[BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
	 
	 else if(app_collection_1_is_ready())
	 { 
		   app_get_collection1_address(taiti_mac_address);
	 }	
}

UCHAR * iris_dianbiao_get_taiti_address(void)
{
	  return taiti_mac_address;
}
#if CMD_LINK_RSSI_FUNCTION
void get_connect_rssi(uint8_t conidx,uint8_t task_index)
{
	  struct gapc_get_info_cmd *p_cmd;
	  if(task_index ==0)
		{
	  p_cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,\
																									 gapc_get_info_cmd);
	  }
		else if(task_index ==1)
		{
	  p_cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,KE_BUILD_ID(TASK_GAPC, conidx), TASK_ADV_DEVICE1,\
																									 gapc_get_info_cmd);
	  }		
		else if(task_index ==2)
		{
		printf("get_connect_rssi %x\n",task_index);
	  p_cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,KE_BUILD_ID(TASK_GAPC, conidx), TASK_CONNECTION_DEVICE1,\
																									 gapc_get_info_cmd);
	  }	
		else if(task_index ==3)
		{
	  p_cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,KE_BUILD_ID(TASK_GAPC, conidx), TASK_CONNECTION_DEVICE2,\
																									 gapc_get_info_cmd);
	  }
		else if(task_index ==4)
		{
	  p_cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,KE_BUILD_ID(TASK_GAPC, conidx), TASK_CONNECTION_DEVICE3,\
																									 gapc_get_info_cmd);
	  }
		// request peer device name.
		p_cmd->operation = GAPC_GET_CON_RSSI;

		// send command
		ke_msg_send(p_cmd);
}
#endif
#if CMD_DEV_ADDR_FUNCTION
bool readcommand_with_para(void)
{ 
	uint32_t cmd;
	bool result=true;

	do
	{
	cmd = CMD_DEV_ADDR;
	if(memcmp(uart_frame.control_cmd,&cmd,4)==0)
		break;
	cmd = CMD_DEV_PAIR_MM;
	if(memcmp(uart_frame.control_cmd,&cmd,4)==0)
		break;
	cmd = CMD_DEV_PAIR_VALUE;
		if(memcmp(uart_frame.control_cmd,&cmd,4)==0)
		break;
	cmd = CMD_LINK_RSSI;
	  if(memcmp(uart_frame.control_cmd,&cmd,4)==0)
		break;
	result=false;
  }while(0);
	return result;
}
#endif
#endif
