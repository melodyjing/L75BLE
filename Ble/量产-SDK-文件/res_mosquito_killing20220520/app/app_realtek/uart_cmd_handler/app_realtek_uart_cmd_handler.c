/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description: 串口命令处理, 用于实现 68 协议		

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/
#include "app_realtek_console.h"
#include "app_realtek_device.h"
#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE)

RDW_PIN_CODE realtek_pin[4];
UCHAR realtek_device;
UINT16 adv_inter;
UINT16 scan_inter;
LOCAL BOOL  scan_enable=true;
LOCAL BOOL  adv_enable=true;
UINT8 state_report;
uint8_t data_width,stop_bit;
uint8_t adv_restart_enable[2];
LOCAL CONST UINT8 pre_rsp[8]={0xFE,0xFE,0xFE,0xFE,0x68,0x01,0x00,0x04};
LOCAL CONST UINT8 suf_rsp[12]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x68,0x00};
LOCAL CONST UINT8 pre_ver[27]={0xFE,0xFE,0xFE,0xFE,0x68,0x04,0x00,0x83,0x08,0x00,0x00,0x00,0xFF,\
	0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x68,0x00,0x00,0x00,0x01};
typedef struct rdw_adv_data{
	
	  UINT8 adv_data[31];  
			
    UINT8 adv_data_length;
}RDW_ADV_DATA;
RDW_ADV_DATA def_adv_data;
typedef struct rdw_resq_data{
	
	  UINT8 resq_data[31];  
			
    UINT8 resq_data_length;
}RDW_RESQ_DATA;
RDW_RESQ_DATA def_resq_data;
#if CMD_ADV_FUNCTION
extern struct adv_env_tag  adv_device1_env;
void appm_stop_advertising(void);
void app_device1_stop_advertising(void);
#endif
#if CMD_LINK_RSSI_FUNCTION
typedef struct rdw_connect_condiex{
	
	  UINT8 slave1_condiex; 
    UINT8 slave2_condiex;
	  UINT8 master1_condiex; 
    UINT8 master2_condiex;	
    UINT8 master3_condiex;

    int16_t slave1_rssi;	
    int16_t slave2_rssi;	
	  int16_t master1_rssi;	
    int16_t master2_rssi;	
    int16_t master3_rssi;		
}RDW_CONN_CONDIEX;
UINT8 mask_rssi;
UINT8 active_obj_sum;
UINT8 rssi_mac_addr[30];
LOCAL UINT32 timer_rssi_timeout;
struct rdw_connect_condiex  conn_condiex;
extern void get_connect_rssi(uint8_t conidx,uint8_t task_index);
#endif
void cmd_report_up(enum UART_COMMAND_INDEX cmd,UINT8 para)
{
	UINT8 i;
	USHORT checksum=0;
	
	for(i=0;i<8;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	cobra_uart_write(M3_UART0, cmd & 0xFF);
	cobra_uart_write(M3_UART0, (cmd>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (cmd>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (cmd>>24) & 0xFF);
	checksum += cmd & 0xFF;
	checksum += (cmd>>8) & 0xFF;
	checksum += (cmd>>16) & 0xFF;
	checksum += (cmd>>24) & 0xFF;
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	if(para>0)
	cobra_uart_write(M3_UART0, para);
	checksum += para;
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
UINT8 get_state_report(void)
{
	return state_report;  //  0 -> 需要上报 1-> 不需要上报了
}
void set_state_report(UINT8 para)
{
	 state_report = para;
}
void report_up_reset(void)
{
	if(state_report == 0)  // 还没有上报复位 
	cmd_report_up(CMD_MODULE_INFO,1);
}
void report_up_rsp(enum MODUL_INFO info_index,UINT8 *data)
{
	if(info_index == MODUL_RESET && *(data+1) == 0) // 收到复位成功
		set_state_report(1);
	else
		set_state_report(0);		
}
void realtek_start(void)
{
	report_up_reset();
}
UINT16 app_device_get_adv_interval(void)
{
	    return adv_inter ;
}

void app_device_set_adv_interval(UINT16 value)
{
	   	adv_inter = value;
}

UINT16 app_device_get_scan_interval(void)
{
	    return scan_inter ;
}
UCHAR * app_get_meter_master_pincode(void)
{
	  return  realtek_pin[0].pin_name;
}
void app_device_set_can_interval(UINT16 value)
{
	   	scan_inter = value;
}
BOOL is_host_device(void)
{
	if((realtek_device & HOST_DEV) == true)
		return true;
}
BOOL is_slave_device1(void)
{
	if((realtek_device & SLAVE_DEV1) == true)
		return true;
}
BOOL is_slave_device2(void)
{
	if((realtek_device & SLAVE_DEV2) == true)
		return true;
}
BOOL is_slave_device3(void)
{
	if((realtek_device & SLAVE_DEV3) == true)
		return true;
}
//回答电表设置模块
void realtek_dianBiao_set_rsp(enum UART_COMMAND_INDEX index,enum UART_CMD_ERROR_CODE error_id)
{
	UINT8 i;
	USHORT checksum=0;
	for(i=0;i<7;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	
	checksum += 0xC2;
	cobra_uart_write(M3_UART0, 0xC2);
	
	cobra_uart_write(M3_UART0, index & 0xFF);
	cobra_uart_write(M3_UART0, (index>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (index>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (index>>24) & 0xFF);
	checksum += index & 0xFF;
	checksum += (index>>8) & 0xFF;
	checksum += (index>>16) & 0xFF;
	checksum += (index>>24) & 0xFF;
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	cobra_uart_write(M3_UART0, error_id & 0xFF);
	checksum += error_id;
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
void load_dev_pair_param(void)
{
	UINT8 temp=4*18+1,index=0;
	UINT8 para[4*18+1];
  if(nvds_get(NVDS_TAG_PAIR_PARAM_FLAG, (uint8_t *)&temp, para) != NVDS_OK)
	{
		memset(para, 0, temp);
		nvds_put(NVDS_TAG_PAIR_PARAM_FLAG, temp, (UINT8 *)para);
	}
	realtek_device = para[0];  //mask
	index++;
		
  if(is_host_device() == true)  // 对本机配置
	 {
	 if(para[index]<5)
//		 App_Set_Smp_Level(*(user_data + 1),0);
	 app_set_ble_smp_level(para[index]);
	 index++;
	 realtek_pin[0].pin_length = para[index];
	 index++;
   memcpy(realtek_pin[0].pin_name, &para[index], realtek_pin[0].pin_length);
	 index = index + realtek_pin[0].pin_length;
   }	
	 
	 if(is_slave_device1() == true)  // 对从机1配置
	 {
	 if(para[index]<5)
		 App_Set_Smp_Level(para[index],1);
	 index++;
	 realtek_pin[1].pin_length = para[index];
	 index++;
	 app_set_new_duanluqi1_pincode(&para[index]);	 
   memcpy(realtek_pin[1].pin_name, &para[index], realtek_pin[1].pin_length);
	 index += realtek_pin[1].pin_length;
   }

	 if(is_slave_device2() == true)  // 对从机2配置
	 {
	 if(para[index]<5)
		 App_Set_Smp_Level(para[index],2);
	 index++;
	 realtek_pin[2].pin_length = para[index];
	 index++;
	 app_set_new_duanluqi2_pincode(&para[index]);	 
   memcpy(realtek_pin[2].pin_name, &para[index], realtek_pin[2].pin_length);
	 index += realtek_pin[2].pin_length;
   }
	 
	 if(is_slave_device3() == true)  // 对从机3配置
	 {
	 if(para[index]<5)
		 App_Set_Smp_Level(para[index],3);
	 index++;
	 realtek_pin[3].pin_length = para[index];
	 index++;
	 app_set_new_duanluqi3_pincode(&para[index]);	 
   memcpy(realtek_pin[3].pin_name, &para[index], realtek_pin[3].pin_length);
	 index += realtek_pin[3].pin_length;
   }	 
}
void save_dev_pair_param(void)
{
	UINT8 temp=4*18+1,index=0;
	UINT8 para[4*18+1];
  if(nvds_get(NVDS_TAG_PAIR_PARAM_FLAG, (uint8_t *)&temp, para) != NVDS_OK)
		 memset(para, 0, temp);
	para[0] = realtek_device;  //mask
	index++;
		
  if(is_host_device() == true)  // 对本机配置
	 {
	 para[index] = app_get_ble_smp_level();
	 index++;
	 para[index] = realtek_pin[0].pin_length;
	 index++;
   memcpy(&para[index],realtek_pin[0].pin_name , realtek_pin[0].pin_length);
	 index = index + realtek_pin[0].pin_length;
   }	
	 
	 if(is_slave_device1() == true)  // 对从机1配置
	 {
	 para[index] = App_Get_Smp_Level(1);
	 index++;
	 para[index] = realtek_pin[1].pin_length;
	 index++;
   memcpy(&para[index],realtek_pin[1].pin_name , realtek_pin[1].pin_length);
	 index = index + realtek_pin[1].pin_length;
   }

	 if(is_slave_device2() == true)  // 对从机2配置
	 {
	 para[index] = App_Get_Smp_Level(2);
	 index++;
	 para[index] = realtek_pin[2].pin_length;
	 index++;
   memcpy(&para[index],realtek_pin[2].pin_name , realtek_pin[2].pin_length);
	 index = index + realtek_pin[2].pin_length;
   }
	 
	 if(is_slave_device3() == true)  // 对从机3配置
	 {
	 para[index] = App_Get_Smp_Level(3);
	 index++;
	 para[index] = realtek_pin[3].pin_length;
	 index++;
   memcpy(&para[index],realtek_pin[3].pin_name , realtek_pin[3].pin_length);
	 index = index + realtek_pin[3].pin_length;
   }	 
	 
	 nvds_put(NVDS_TAG_PAIR_PARAM_FLAG, index, (UINT8 *)para);
}
void cmd_dev_pair_mode_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index=0;
	
	 load_dev_pair_param();
	 realtek_device |= *user_data;
	 if(is_host_device() == true)  // 对本机配置
	 {
	 if(*(user_data + 1)<5)
//		 App_Set_Smp_Level(*(user_data + 1),0);
	 app_set_ble_smp_level(*(user_data + 1));
	 realtek_pin[0].pin_length = *(user_data + 2);
   memcpy(realtek_pin[0].pin_name, user_data + 3, realtek_pin[0].pin_length);
	 index += 2 + realtek_pin[0].pin_length;
   }
	 
	 if(is_slave_device1() == true)  // 对从机1配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),1);
	 index++;
	 realtek_pin[1].pin_length = *(user_data + index);
	 index++;
	 app_set_new_duanluqi1_pincode(user_data + index);	 
   memcpy(realtek_pin[1].pin_name, user_data + index, realtek_pin[1].pin_length);
	 index += realtek_pin[1].pin_length - 1;
   }
	 
	 if(is_slave_device2() == true)  // 对从机2配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),2);
	 index++;
	 realtek_pin[2].pin_length = *(user_data + index);
	 index++;
	 app_set_new_duanluqi2_pincode(user_data + index);	 
   memcpy(realtek_pin[2].pin_name, user_data + index, realtek_pin[2].pin_length);
	 index += realtek_pin[2].pin_length - 1;
   }	 
	 
	 if(is_slave_device3() == true)  // 对从机3配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),3);
//	 app_set_new_duanluqi3_pincode(user_data + index);
	 index++;
	 realtek_pin[3].pin_length = *(user_data + index);
	 index++;
	 app_set_new_duanluqi3_pincode(user_data + index);	 
   memcpy(realtek_pin[3].pin_name, user_data + index, realtek_pin[3].pin_length);
	 index += realtek_pin[3].pin_length - 1;
   }		 
	 	 
	 save_dev_pair_param();
	 
	 realtek_dianBiao_set_rsp(CMD_DEV_PAIR_MODE,CMD_ERROR_NO);
}	
void bdadrr_reverse(UCHAR *end_data,UCHAR *ori_data)
{
	 UINT8 i;
	 for(i=0;i<6;i++)
	    *(end_data + i) = *(ori_data + 5 - i);
}
void cmd_dev_mac_addr_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index[6];
	 UINT8 reboot_flag = 0;
	 UINT8 bd_length=NVDS_LEN_BD_ADDRESS;
   struct bd_addr  rdw_set_bdaddr[4];	
	
	 if(length>=6)
	 {
		 nvds_get(NVDS_TAG_BD_ADDRESS, &bd_length, rdw_set_bdaddr[1].addr);
		 bdadrr_reverse(rdw_set_bdaddr[0].addr,user_data);
		 if(memcmp(rdw_set_bdaddr[0].addr, rdw_set_bdaddr[1].addr, 6) != 0)
		 {
		 reboot_flag = 1;		 
		 nvds_put(NVDS_TAG_BD_ADDRESS, NVDS_LEN_BD_ADDRESS, (UINT8 *)rdw_set_bdaddr[0].addr);	
		 gapm_set_bdaddr(rdw_set_bdaddr[0].addr);
     llm_set_local_address(rdw_set_bdaddr[0].addr);			 
		 }
	 }
	 if(length>=12)
	 {
		 bdadrr_reverse(rdw_set_bdaddr[1].addr,user_data+6);
		 app_set_new_duanluqi1_address(rdw_set_bdaddr[1].addr);
		 app_device_connect_to_duanluqi_1_enable(1);
		 nvds_put(BREAKER_ADDR1_FLAG, BREAKER_ADDR1_LEN, (UINT8 *)rdw_set_bdaddr[1].addr);
	 }
	 if(length>=18)
	 {
		 bdadrr_reverse(rdw_set_bdaddr[2].addr,user_data+12);		 
		 app_set_new_duanluqi2_address(rdw_set_bdaddr[2].addr);
		 app_device_connect_to_duanluqi_2_enable(1);		 
		 nvds_put(BREAKER_ADDR2_FLAG, BREAKER_ADDR2_LEN, (UINT8 *)rdw_set_bdaddr[2].addr);
	 }
	 if(length==24)
	 {
		 bdadrr_reverse(rdw_set_bdaddr[3].addr,user_data+18);	
		 app_set_new_duanluqi3_address(rdw_set_bdaddr[3].addr);  
		 app_device_connect_to_duanluqi_3_enable(1);
		 nvds_put(BREAKER_ADDR3_FLAG, BREAKER_ADDR3_LEN, (UINT8 *)rdw_set_bdaddr[3].addr);
	 } 
	 
	 realtek_dianBiao_set_rsp(CMD_DEV_MAC_ADDR,CMD_ERROR_NO);
//	 

//	 appm_send_gapm_reset_cmd();
  if(reboot_flag)
	{
			//platform_reset(0);
//		    app_adv_report_begin();
				if(app_collection_2_is_ready() == 1)
				{
					appm_disconnect();               //断开连接
				}
				if(app_collection_1_is_ready() == 1)
				{
					app_collection1_disconnect();
				}

					set_device1_adv_started_state();
					app_device1_adv_fsm_next();	
					set_appm_adv_started_state();														
					appm_adv_fsm_next();
				
					if((get_devicel_adv() == 0) && (app_env.adv_ok == 0))
					{
							app_env.adv_ok = 1;
					}

					adv_restart_enable[0]=1;
					adv_restart_enable[1]=1;

					// appm_send_gapm_reset_cmd();
	}	 
}
void cmd_module_info_handler(USHORT length,UCHAR *user_data)
{
		realtek_dianBiao_set_rsp(CMD_MODULE_INFO,CMD_ERROR_NO);
		cobra_soft_delay_ms(100);
		platform_reset(0);
}
#if BLE_DATA_RECEIVE_MODE
void cmd_ble_data_receive_mode_handler(USHORT length,UCHAR *user_data)
{
	  if(*user_data != 1 && *user_data != 2) // 
		{
			realtek_dianBiao_set_rsp(CMD_BLE_DATA_MODE,CMD_ERROR_REPORT_PARA_FAIL);
			return;
		}	
	  app_set_ble_data_rece_mode(*user_data);
		realtek_dianBiao_set_rsp(CMD_BLE_DATA_MODE,CMD_ERROR_NO);
}
#endif
#if CMD_DEV_NAME_FUNCTION
void cmd_device_name_handler(USHORT length,UCHAR *user_data)
{
	  if(length > 5) // 设备名称字符小于等于5
		{
			realtek_dianBiao_set_rsp(CMD_DEV_NAME,CMD_ERROR_REPORT_PARA_FAIL);
			return;
		}
	  app_set_device_name(length,user_data);
		realtek_dianBiao_set_rsp(CMD_DEV_NAME,CMD_ERROR_NO);
}
#endif
#if CMD_TX_POWER_FUNCTION
void cmd_tx_power_handler(USHORT length,UCHAR *user_data)
{
	  if(*user_data >= LEVEL_MAX_DB) // 目前共5档，超出报错
		{
			realtek_dianBiao_set_rsp(CMD_TX_POWER,CMD_ERROR_REPORT_PARA_FAIL);
			return;			
		}
	  app_set_ble_power_level(*user_data);
		realtek_dianBiao_set_rsp(CMD_TX_POWER,CMD_ERROR_NO);
}
#endif
#if CMD_ADV_INTERVAL_FUNCTION
void cmd_adv_interval_handler(USHORT length,UCHAR *user_data)
{
	  uint32_t adv_inter_value = (*(user_data))<<8 | (*(user_data+1));
	  if(adv_inter_value<0x20 || adv_inter_value>0x4000)
		{
			realtek_dianBiao_set_rsp(CMD_ADV_INTERVAL,CMD_ERROR_REPORT_PARA_FAIL);
			return;				
		}
    app_device_set_adv_interval(adv_inter_value);
		realtek_dianBiao_set_rsp(CMD_ADV_INTERVAL,CMD_ERROR_NO);
}
#endif
#if CMD_DEV_ADDR_FUNCTION
void cmd_dev_add_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 i,mask,index[6];
   struct bd_addr  rdw_set_bdaddr[4];	
	
	 mask = *user_data;
	 user_data++;
	 for(i=0;i<4;i++)
	 {
		 if(mask>>i && 0x01)
		 {
			 switch(i)
			 {
				 case 0:
					 nvds_put(NVDS_TAG_BD_ADDRESS, NVDS_LEN_BD_ADDRESS, (UINT8 *)user_data);
				   break;
				 case 1:
					 app_set_new_duanluqi1_address(user_data);
					 app_device_connect_to_duanluqi_1_enable(1);
					 nvds_put(BREAKER_ADDR1_FLAG, BREAKER_ADDR1_LEN, (UINT8 *)user_data);
				   break;
				 case 2:
					 app_set_new_duanluqi2_address(user_data);
					 app_device_connect_to_duanluqi_2_enable(1);		 
					 nvds_put(BREAKER_ADDR2_FLAG, BREAKER_ADDR2_LEN, (UINT8 *)user_data);
				 	 break;
				 case 3:
					 app_set_new_duanluqi3_address(user_data);
					 app_device_connect_to_duanluqi_3_enable(1);		 
					 nvds_put(BREAKER_ADDR3_FLAG, BREAKER_ADDR3_LEN, (UINT8 *)user_data);
				 	 break;				 
			 }
			 user_data = user_data +6;
		 }
	 }
	 
	 realtek_dianBiao_set_rsp(CMD_DEV_ADDR,CMD_ERROR_NO);
	 if(mask && 0x01)
	 {
	 app_adv_report_begin();

	 appm_send_gapm_reset_cmd();
	 }
}
#endif
#if CMD_DEV_PAIR_MODE_FUNCTION
void cmd_dev_pair_mm_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index=0;
	 UINT8 mask;
	
	 load_dev_pair_param();	
	 mask = realtek_device;
	 realtek_device = *user_data;
		
	 if(is_host_device() == true)  // 对本机配置
	 {
	 index++;		 
	 if(*(user_data + 1)<5)
	 app_set_ble_smp_level(*(user_data + index));
   }
	 
	 if(is_slave_device1() == true)  // 对从机1配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),1);
   }
	 
	 if(is_slave_device2() == true)  // 对从机2配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),2);
   }	 
	 
	 if(is_slave_device3() == true)  // 对从机3配置
	 {
	 index++;
	 if(*(user_data + index)<5)
		 App_Set_Smp_Level(*(user_data + index),3);
   }		 
	 	 
	 realtek_device |= mask;
	 
	 save_dev_pair_param();
	 realtek_dianBiao_set_rsp(CMD_DEV_PAIR_MODE,CMD_ERROR_NO);
}
#endif
#if CMD_DEV_PAIR_VALUE_FUNCTION
void cmd_dev_pair_value_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index=0,sum=0;
	 UINT8 i,mask;
	
	 load_dev_pair_param();
	 mask = *user_data;
	 index++;
	
	 for(i=0;i<4;i++)
		 {
			 if(mask>>i && 0x01)
			 {
				 switch(i)
				 {
					 case 0:
							 realtek_pin[0].pin_length = *(user_data + index);
					     index++;
							 memcpy(realtek_pin[0].pin_name, user_data + index, realtek_pin[0].pin_length);
							 index += realtek_pin[0].pin_length;
					 break;
					 case 1:
							 realtek_pin[1].pin_length = *(user_data + index);
					     index++;
							 memcpy(realtek_pin[1].pin_name, user_data + index, realtek_pin[1].pin_length);
							 index += realtek_pin[1].pin_length;
					 break;
					 case 2:
							 realtek_pin[2].pin_length = *(user_data + index);
					     index++;
							 memcpy(realtek_pin[2].pin_name, user_data + index, realtek_pin[2].pin_length);
							 index += realtek_pin[2].pin_length;
					 break;
					 case 3:
							 realtek_pin[3].pin_length = *(user_data + index);
					     index++;
							 memcpy(realtek_pin[3].pin_name, user_data + index, realtek_pin[3].pin_length);
							 index += realtek_pin[3].pin_length;
					 break;	
				 }					 				   
			 }

		 }
				 
	 	 realtek_device |= mask;
	 	 save_dev_pair_param();
	   realtek_dianBiao_set_rsp(CMD_DEV_PAIR_VALUE,CMD_ERROR_NO);
}
#endif
#if CMD_LINK_CMD_FUNCTION
void cmd_link_cmd_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 link=0,i=0,j,mask,index=0;
	 mask = *user_data;
	 index++;	   
	
	 for(i=0;i<5;i++)
		 {
			 if(mask>>i && 0x01)
			 {
				  switch(i)
				 {
					 case 0:
					 case 1:
						 link = *(user_data+index); 
					   index++;
					   if(link)  //希望连接
						 {
//							 if(!app_collection_2_is_ready())
//							 {
//								 	if(app_device_compare_duanluqi1_address(user_data+index))
//									{
//												
//									}
//							 }
						 }
						 else
						 {
							 if(app_collection_2_is_ready())
							 {
								 	if(app_device_compare_collection2_address(user_data+index))
									{
										    appm_disconnect();
									}								 
							 }	
							 if(app_collection_1_is_ready())
							 {
								 	if(app_device_compare_collection1_address(user_data+index))
									{
												app_collection1_disconnect();
									}								 
							 }								 
						 }
						 index = index + 6;
					 break;
//					 case 1:
//						 link = *(user_data+index); 
//					   index++;
//					   if(link)  //希望连接
//						 {
////							 if(!app_collection_1_is_ready())
////							 {
////								 	if(app_device_compare_collection1_address(user_data+index))
////									{
////												
////									}
////							 }
//						 }
//						 else
//						 {
//							 if(app_collection_1_is_ready())
//							 {
//								 	if(app_device_compare_collection1_address(user_data+index))
//									{
//												app_collection1_disconnect();
//									}								 
//							 }							 
//						 }
//						 index = index + 6;						 
//					 break;	
					 case 2:
					 case 3:
					 case 4:
						 link = *(user_data+index);
					   index++;
					   if(link)  //希望连接
						 {
							 if(!app_device1_is_connected()) // 准备连接 
							 {
									if(app_device_compare_duanluqi1_address(user_data+index))
									{
												app_device_connect_to_duanluqi_1_enable(1);
									}
							 }
							 else if(!app_device2_is_connected()) // 准备连接 
							 {
									if(app_device_compare_duanluqi2_address(user_data+index))
									{
												app_device_connect_to_duanluqi_2_enable(1);
									}
							 }
							 else if(!app_device3_is_connected()) // 准备连接 
							 {
									if(app_device_compare_duanluqi3_address(user_data+index))
									{
												app_device_connect_to_duanluqi_3_enable(1);
									}
							 }							 
						 }
						 else
						 {
							 if(app_device1_is_connected()) // 准备断开 
							 {
									if(app_device_compare_duanluqi1_address(user_data+index))
									{
										 app_device_connect_to_duanluqi_1_enable(0); //   断开
								     app_device1_disconnect();
									}								 
							 }
							 else if(app_device2_is_connected()) // 准备断开 
							 {
									if(app_device_compare_duanluqi2_address(user_data+index))
									{
										 app_device_connect_to_duanluqi_2_enable(0); //   断开
								     app_device2_disconnect();
									}								 
							 } 	
							 else if(app_device3_is_connected()) // 准备断开 
							 {
									if(app_device_compare_duanluqi3_address(user_data+index))
									{
										 app_device_connect_to_duanluqi_3_enable(0); //   断开
								     app_device3_disconnect();
									}								 
							 }
						 }						 
						 index = index + 6;
					 break;
//					 case 3:
//						 link = *(user_data+index);
//					   index++;
//					   if(link)  //希望连接
//						 {
//							 if(!app_device2_is_connected()) // 准备连接 
//							 {
//									if(app_device_compare_duanluqi2_address(user_data+index))
//									{
//												app_device_connect_to_duanluqi_2_enable(1);
//									}
//							 }
//						 }
//						 else
//						 {
//							 if(app_device2_is_connected()) // 准备断开 
//							 {
//									if(app_device_compare_duanluqi2_address(user_data+index))
//									{
//								     app_device2_disconnect();
//									}								 
//							 }
//						 }						 
//						 index = index + 6;						 
//					 break;
//					 case 4:
//						 link = *(user_data+index);
//					   index++;
//					   if(link)  //希望连接
//						 {
//							 if(!app_device3_is_connected()) // 准备连接 
//							 {
//									if(app_device_compare_duanluqi3_address(user_data+index))
//									{
//												app_device_connect_to_duanluqi_3_enable(1);
//									}
//							 }
//						 }
//						 else
//						 {
//							 if(app_device3_is_connected()) // 准备断开 
//							 {
//									if(app_device_compare_duanluqi1_address(user_data+index))
//									{
//								     app_device3_disconnect();
//									}								 
//							 }
//						 }						 
//						 index = index + 6;						 
//					 break;					 
				 }
			 }
			 
		 }
	 realtek_dianBiao_set_rsp(CMD_LINK_CMD,CMD_ERROR_NO);
}
#endif
#if CMD_ADV_FUNCTION
BOOL get_adv_enable(void)
{
	return adv_enable;
}
void set_adv_enable(BOOL is_enable)
{
	 adv_enable = is_enable;
}
void cmd_adv_handler(USHORT length,UCHAR *user_data)
{
    set_adv_enable(*user_data);
	  if(get_adv_enable() == false) // 停止广播
		{
	  if(adv_device1_env.adv_state == APP_DEVICE1_ADV_STATE_STARTED)
			app_device1_stop_advertising();
		if(app_env.adv_state == APP_ADV_STATE_STARTED)
			appm_stop_advertising();
	  }
		else
		{
			if(adv_device1_env.adv_state == APP_DEVICE1_ADV_STATE_CREATED)
				app_host2_start_advertising();
			if(app_env.adv_state == APP_ADV_STATE_CREATED)
				appm_start_advertising();
		}
		realtek_dianBiao_set_rsp(CMD_ADV,CMD_ERROR_NO);
}
#endif
#if CMD_FIRMWARE_BUILD_TIME_FUNCTION
void cmd_fw_build_time_read_handler(void)
{
	char timer_buffer[30]="Sun Aug 1 10:36:12 2021"; //Mon Tues Wed Thur Fri SAT SUN 
	
  UINT8 i;
	USHORT checksum=0;
	
	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}

	checksum += strlen(timer_buffer);
	cobra_uart_write(M3_UART0, strlen(timer_buffer) & 0xFF);  // 报文长度
	cobra_uart_write(M3_UART0, 0);
	
	cobra_uart_write(M3_UART0, 0x83);  //报文类型 83H
	checksum += 0x83;
	
	cobra_uart_write(M3_UART0, CMD_FIRMWARE_BUILD_TIME & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_FIRMWARE_BUILD_TIME>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_FIRMWARE_BUILD_TIME>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_FIRMWARE_BUILD_TIME>>24) & 0xFF);
	checksum += CMD_FIRMWARE_BUILD_TIME & 0xFF;
	checksum += (CMD_FIRMWARE_BUILD_TIME>>8) & 0xFF;
	checksum += (CMD_FIRMWARE_BUILD_TIME>>16) & 0xFF;
	checksum += (CMD_FIRMWARE_BUILD_TIME>>24) & 0xFF;
	
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	
  for(i=0;i<strlen(timer_buffer);i++)
	{
	cobra_uart_write(M3_UART0, timer_buffer[i]);
	checksum += timer_buffer[i];
	}
	
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
#endif
#if CMD_LINK_RSSI_FUNCTION
void printout_rssi_value(void)
{
  UINT8 i,length=0;
	USHORT checksum=0;	
	
	 for(i=0;i<5;i++)
	 {
		 if(mask_rssi>>i && 0x01)
		 {
			 length = length+6;
			 switch(i)
			 {
				 case 0:
           rssi_mac_addr[length++] =(uint8_t)(conn_condiex.slave1_rssi>>8) & 0xFF;
				   rssi_mac_addr[length++] =(uint8_t)(conn_condiex.slave1_rssi & 0xFF);
				 break;
				 case 1:
           rssi_mac_addr[length++] =(uint8_t)(conn_condiex.slave2_rssi>>8) & 0xFF;
				   rssi_mac_addr[length++] =(uint8_t)(conn_condiex.slave2_rssi & 0xFF);
				 break;			
				 case 2:
           rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master1_rssi>>8) & 0xFF;
				   rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master1_rssi & 0xFF);
				 break;	
				 case 3:
           rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master2_rssi>>8) & 0xFF;
				   rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master2_rssi & 0xFF);
				 break;	
				 case 4:
           rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master3_rssi>>8) & 0xFF;
				   rssi_mac_addr[length++] =(uint8_t)(conn_condiex.master3_rssi & 0xFF);
				 break;	
			 }			 
		 }			 
	 }
	 
	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	checksum += length;
	cobra_uart_write(M3_UART0, length & 0xFF);  // 报文长度
	cobra_uart_write(M3_UART0, 0);
	
	cobra_uart_write(M3_UART0, 0x83);  //报文类型 83H
	checksum += 0x83;
	
	cobra_uart_write(M3_UART0, CMD_LINK_RSSI & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_LINK_RSSI>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_LINK_RSSI>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_LINK_RSSI>>24) & 0xFF);
	checksum += CMD_LINK_RSSI & 0xFF;
	checksum += (CMD_LINK_RSSI>>8) & 0xFF;
	checksum += (CMD_LINK_RSSI>>16) & 0xFF;
	checksum += (CMD_LINK_RSSI>>24) & 0xFF;
	
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	
  for(i=0;i<length;i++)
	{
	cobra_uart_write(M3_UART0, rssi_mac_addr[i]);
	checksum += rssi_mac_addr[i];
	}
	
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);			 
}
void cmd_link_rssi_read_handler(USHORT length,UCHAR *user_data)
{	
  UINT8 i,j,index=0;	
	UINT8 mask,*ptr_addr;
	ptr_addr = rssi_mac_addr;
	 mask_rssi = *user_data; 
	 index++;
	
	 for(i=0;i<5;i++)
		 {
			 if(mask_rssi>>i && 0x01)
			 {
				 memcpy(ptr_addr,user_data+index,6);
				 *(ptr_addr+6)=0;
				 *(ptr_addr+7)=0;
				 ptr_addr = ptr_addr+8;	
         index    = index+6;	 
				  switch(i)
				 {
					 case 0:
						 if(app_collection_2_is_ready())
						 {
							 active_obj_sum++;
							 get_connect_rssi(conn_condiex.slave1_condiex,0); //0->app_task 1->TASK_ADV_DEVICE1
						 }
					 break;
					 case 1:
						 if(app_collection_1_is_ready())
						 {
							 active_obj_sum++;							 
							 get_connect_rssi(conn_condiex.slave2_condiex,1); //0->app_task 1->TASK_ADV_DEVICE1
						 }
					 break;			
					 case 2:
						 if(app_device1_is_connected())
						 {
							 active_obj_sum++;							 
							 get_connect_rssi(conn_condiex.master1_condiex,2); //0->app_task 1->TASK_ADV_DEVICE1
						 }
					 break;	
					 case 3:
						 if(app_device2_is_connected())
						 {
							 active_obj_sum++;							 
							 get_connect_rssi(conn_condiex.master2_condiex,3); //0->app_task 1->TASK_ADV_DEVICE1
						 }
					 break;	
					 case 4:
						 if(app_device3_is_connected())
						 {
							 active_obj_sum++;							 
							 get_connect_rssi(conn_condiex.master3_condiex,4); //0->app_task 1->TASK_ADV_DEVICE1
						 }
					 break;	
				 }
			 }
			 
		 }

//    timer_rssi_timeout = cobra_soft_timer_create(app_get_rssi_timeout_function);
//	
//		cobra_soft_timer_start(timer_rssi_timeout, COBRA_SOFT_TIMER_TICK, 0);
}
#endif
#if SCAN_ENABLE_FUCTION
BOOL get_scan_enable(void)
{
	return scan_enable;
}
void cmd_scan_handler(USHORT length,UCHAR *user_data)
{
	 if(*user_data == false && scan_enable == true)
   {
		 scan_enable = false;
		 if(get_reset_state())
		 app_device1_stop_scan();
	 }
	 else if(*user_data == true && scan_enable == false)
	 {
		 scan_enable = true;
		 if(get_reset_state())
		 app_device1_start_scan();
	 } 
	 realtek_dianBiao_set_rsp(CMD_SCAN,CMD_ERROR_NO);
}
#endif
void cmd_ble_params_handler(USHORT length,UCHAR *user_data)
{
	 app_set_ble_power_level(*user_data);
	
	 app_device_set_adv_interval((*(user_data+1))<<8 | (*(user_data+2)));
	
	 app_device_set_can_interval((*(user_data+3))<<8 | (*(user_data+4)));
	
	 nvds_put(NVDS_TAG_BLE_PARAM_FLAG, NVDS_TAG_BLE_PARAM_LENGTH, (UINT8 *)user_data);
	
	 realtek_dianBiao_set_rsp(CMD_BLE_PARAMS,CMD_ERROR_NO);
}
void print_mac_addr(UCHAR * p,USHORT  *checksum)
{
	UINT8 i;
	for(i=0;i<6;i++)
	{
		*checksum += p[i];
		cobra_uart_write(M3_UART0, p[i]);
	}		
}
void cmd_ble_conn_info_handler(enum UART_COMMAND_TYPE type)
{
	UINT8 i,length;
	USHORT checksum=0;
	UCHAR mac_address[6];	
	UINT8 conn_count=0,conn_flag=0;
	struct bd_addr  rdw_set_bdaddr;	
	
	if(app_collection_1_is_ready())
	{
		conn_count++;
		conn_flag |= 1<<0;
	}
	if(app_collection_2_is_ready())
	{
		conn_count++;
		conn_flag |= 1<<1;
	}
	if(app_get_duanluqi_1_connected_flag())
	{
		conn_count++;
		conn_flag |= 1<<2;
	}	
	if(app_get_duanluqi_2_connected_flag())
	{
		conn_count++;
		conn_flag |= 1<<3;
	}	
		if(app_get_duanluqi_3_connected_flag())
	{
		conn_count++;
		conn_flag |= 1<<4;
	}	
	
	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	length = conn_count * 6 + 1;
	checksum += length;
	cobra_uart_write(M3_UART0, length & 0xFF);  // 报文长度
	cobra_uart_write(M3_UART0, 0);
	
	if(type == UART_CMD_READ)
	{
	cobra_uart_write(M3_UART0, 0x83);  //报文类型 83H
	checksum += 0x83;
	}
	else if(type == UART_CMD_REPORT)
	{
	cobra_uart_write(M3_UART0, 0x04);  //报文类型 04H
	checksum += 0x04;
	}
	
	cobra_uart_write(M3_UART0, CMD_BLE_CONN_INFO & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_BLE_CONN_INFO>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_BLE_CONN_INFO>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_BLE_CONN_INFO>>24) & 0xFF);
	checksum += CMD_BLE_CONN_INFO & 0xFF;
	checksum += (CMD_BLE_CONN_INFO>>8) & 0xFF;
	checksum += (CMD_BLE_CONN_INFO>>16) & 0xFF;
	checksum += (CMD_BLE_CONN_INFO>>24) & 0xFF;
	
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	
	cobra_uart_write(M3_UART0, conn_flag & 0xFF);
	checksum += conn_flag;
	
	if(app_collection_1_is_ready())
	{	
	app_get_collection1_address(mac_address);
	bdadrr_reverse(rdw_set_bdaddr.addr,mac_address);
	print_mac_addr(rdw_set_bdaddr.addr,&checksum);
	}

	if(app_collection_2_is_ready())
	{		
	app_get_collection2_address(mac_address);
	bdadrr_reverse(rdw_set_bdaddr.addr,mac_address);
	print_mac_addr(rdw_set_bdaddr.addr,&checksum);
	}
	
	if(app_get_duanluqi_1_connected_flag())
	{
  app_get_duanluqi1_address(mac_address);
	bdadrr_reverse(rdw_set_bdaddr.addr,mac_address);
	print_mac_addr(rdw_set_bdaddr.addr,&checksum);	
	}		
	
	if(app_get_duanluqi_2_connected_flag())
	{
  app_get_duanluqi2_address(mac_address);
	bdadrr_reverse(rdw_set_bdaddr.addr,mac_address);
	print_mac_addr(rdw_set_bdaddr.addr,&checksum);	
	}

	if(app_get_duanluqi_3_connected_flag())
	{
  app_get_duanluqi3_address(mac_address);
	bdadrr_reverse(rdw_set_bdaddr.addr,mac_address);
	print_mac_addr(rdw_set_bdaddr.addr,&checksum);	
	}
	
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
void cmd_firmware_version_read_handler(void)
{
	UINT8 i,length;
	USHORT checksum=0;
	UCHAR mac_address[6];

	for(i=0;i<27;i++)
	{
		checksum += pre_ver[i];
		cobra_uart_write(M3_UART0, pre_ver[i]);
	}
	
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
void cmd_filter_addr_handler(USHORT length,UCHAR *user_data)
{
	UINT8 i;
	UCHAR total_item;
	
	total_item = (*user_data > MAX_FILTER_NUMBER)? MAX_FILTER_NUMBER:*user_data;
	app_adv_set_filter_number(total_item);
	
	for(i = 0; i < total_item; i++)
	{
		app_adv_set_filter_mac_address(i,&user_data[1+i*6]);
	}

	realtek_dianBiao_set_rsp(CMD_FILTER_ADDR,CMD_ERROR_NO);
	
	if(*user_data == 0)
	{
			app_adv_set_filter_rule(FILTER_NULL);
	}
	else
	{								
			app_adv_set_filter_rule(FILTER_BY_MAC);
	}
	app_adv_reset_database();
	
}
void cmd_filter_type_handler(USHORT length,UCHAR *user_data)
{
	UINT8 i;
	UCHAR total_item;
	 
	total_item = (*user_data > MAX_FILTER_NUMBER)? MAX_FILTER_NUMBER:*user_data;
	app_adv_set_filter_number(total_item);
	for(i = 0; i < total_item; i++)
	{
		app_adv_set_filter_mac_kind(i,  *(user_data+1+i));
	}
	
	realtek_dianBiao_set_rsp(CMD_FILTER_TYPE,CMD_ERROR_NO);	
	
	if(*user_data == 0)
	{
			app_adv_set_filter_rule(FILTER_NULL);
	}
	else
	{								
			app_adv_set_filter_rule(FILTER_BY_KIND);
	}
	app_adv_reset_database();
}

void cmd_get_filter_info_read_handler(void)
{
	UINT16 i,j,length;
	UINT adv_number;
	ADV_STRUCT adv_database[MAX_ADV_NUMBER];
	USHORT checksum=0;
	UCHAR mac_address[6];
	UINT8 conn_count=0,conn_flag=0;
	
	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	
	length = Get_Filter_Adv_Info_length();
	cobra_uart_write(M3_UART0, length&0xff);
	cobra_uart_write(M3_UART0, (length>>8)&0xff);
	checksum += length;
	
	cobra_uart_write(M3_UART0, 0x83);
	checksum += 0x83;
	
	cobra_uart_write(M3_UART0, CMD_GET_FILTER_INFO & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_GET_FILTER_INFO>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_GET_FILTER_INFO>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD_GET_FILTER_INFO>>24) & 0xFF);
	checksum += CMD_GET_FILTER_INFO & 0xFF;
	checksum += (CMD_GET_FILTER_INFO>>8) & 0xFF;
	checksum += (CMD_GET_FILTER_INFO>>16) & 0xFF;
	checksum += (CMD_GET_FILTER_INFO>>24) & 0xFF;
	
	for(i=0;i<11;i++)
	{
		checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
	
	//广播数据
    Print_Filter_Adv_Info(&checksum);	
	
	cobra_uart_write(M3_UART0, checksum & 0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
void cmd_adv_info_report_handler(USHORT length,UCHAR *user_data)
{
	realtek_db_filter_adv_report_init(*user_data,*(user_data+1));
	realtek_dianBiao_set_rsp(CMD_ADV_INFO_REPORT,CMD_ERROR_NO);
}
void cmd_uart_params_handler(USHORT length,UCHAR *user_data)
{
  uint32_t baud_rate;
	uint8_t parity;
	baud_rate  = *(user_data+3)<< 24 |*(user_data+2)<< 16 |*(user_data+1)<< 8 | *user_data;
	data_width = *(user_data+4);
	parity     = *(user_data+5);
	stop_bit   = *(user_data+6);
	if(parity == 2)
	{
		cobra_uart_init_rate(M3_UART0, baud_rate, PARITY_EVEN);
	}else if(parity == 1)
	{
		cobra_uart_init_rate(M3_UART0, baud_rate, PARITY_ODD);
	}else if(parity == 0)
	{
		cobra_uart_init_rate(M3_UART0, baud_rate, PARITY_NULL);
	}
	realtek_dianBiao_set_rsp(CMD_UART_PARAMS,CMD_ERROR_NO);
}
#if ADV_DATA_SELF_DEFINE
void cmd_adv_data_handler(USHORT length,UCHAR *user_data)
{
  def_adv_data.adv_data_length = length;
	memcpy(def_adv_data.adv_data,user_data,length);
	realtek_dianBiao_set_rsp(CMD_ADV_DATA,CMD_ERROR_NO);
}
BOOL Is_def_adv_data(void)
{
	if(def_adv_data.adv_data_length > 0)
		return true;
	else
		return false;
}
BOOL copy_adv_data(UINT8 *data,UINT16 *length)
{
	if(Is_def_adv_data())
	{
     memcpy(data, def_adv_data.adv_data, def_adv_data.adv_data_length);		
		 *length = def_adv_data.adv_data_length;
		 return true;
	}		
	else 
		return false;
}
#endif
#if ADV_RESQ_SELF_DEFINE
void cmd_resq_data_handler(USHORT length,UCHAR *user_data)
{
  def_resq_data.resq_data_length = length;
	memcpy(def_resq_data.resq_data,user_data,length);
	realtek_dianBiao_set_rsp(CMD_RESP_DATA,CMD_ERROR_NO);
}

BOOL is_def_resq_data(void)
{
	if(def_resq_data.resq_data_length > 0)
		return true;
	else
		return false;
}

BOOL Copy_resq_data(UINT8 *data,UINT16 *length)
{
	if(is_def_resq_data())
	{
     memcpy(data, def_resq_data.resq_data, def_resq_data.resq_data_length);		
		 *length = def_resq_data.resq_data_length;
		 return true;
	}
	else 
		return false;
}
#endif
#if ADV_DATA_SELF_DEFINE || ADV_RESQ_SELF_DEFINE
void selfdef_adv_and_resq_data_init(void)
{
	def_adv_data.adv_data_length = 0;  //   0-> 没有自定义广播数据
	def_resq_data.resq_data_length = 0;  // 0-> 没有自定义广播响应数据
}
#endif
void cmd_pd_Handler(USHORT length,UCHAR *user_data)
{
		UCHAR mode = 2; 
		ble_meter_struct * p_meter_param = 	(ble_meter_struct *)user_data;	
		nvds_put(CALIBRATION_METER_PARAM_FLAG, CALIBRATION_METER_PARAM_LENGTH, (UCHAR *)p_meter_param);
		
		nvds_put(IRIS_CALCULATE_SETFLAG, IRIS_CALCULATE_LENGTH, &mode);
		
		realtek_dianBiao_set_rsp(CMD_PD,CMD_ERROR_NO);

		//得到台体现在的蓝牙地址
		iris_dianbiao_save_taiti_address();
 
		iris_auto_reply_ack_command();
}

//电表接收到采集器的 数据 ， 上报出来
//index 是 连接电表的 采集器的 蓝牙地址
void app_realtek_up_collection_data(UCHAR connection_index, UCHAR length, UCHAR *p_buffer)
{
			UINT i;
			USHORT toatl_length;
			USHORT checksum = 0;

	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	
	cobra_uart_write(M3_UART0, length&0xff);
	cobra_uart_write(M3_UART0, (length>>8)&0xff);
	checksum += length&0xff;
	checksum += (length>>8)&0xff;
	
	cobra_uart_write(M3_UART0, 0x01);  //透传报文
	checksum += 0x01;
	
	for(i=0;i<4;i++)
	  cobra_uart_write(M3_UART0, suf_rsp[6+i]); // 00 00 00 00
	
	print_mac_addr(gapc_get_bdaddr(connection_index, SMPC_INFO_PEER)->addr.addr,&checksum); //连接的采集器的 地址
	
	for(i=0;i<5;i++)
	  cobra_uart_write(M3_UART0, suf_rsp[6+i]); // 00 00 00 00 68	
	checksum += 0x68;
		
	for(i = 0; i < length; i++)
	{
		cobra_uart_write(M3_UART0, p_buffer[i]);
		checksum += p_buffer[i];
	}
	
	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}

void app_iris_dianbiao_uart_output(UINT8 index, UINT8 length,  UINT8 *p_buffer)
{
			UINT i;
			USHORT checksum = 0;
	
			UCHAR str_buffer[7];

#if DIANBIAO_AUTO_ENABLE 	
//2020/3/2  断路器的处理
			if(app_iris_is_modified_value(length, p_buffer))
				return ;
#endif
			
	for(i=0;i<5;i++)
	{
		if(i>3)
		checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}
	
	
	cobra_uart_write(M3_UART0, length&0xff);
	cobra_uart_write(M3_UART0, (length>>8)&0xff);
	checksum += length&0xff;
	checksum += (length>>8)&0xff;

	cobra_uart_write(M3_UART0, 0x01);  //透传报文
	checksum += 0x01;
	
	for(i=0;i<4;i++)
	  cobra_uart_write(M3_UART0, suf_rsp[6+i]); // 00 00 00 00
	
	if(index == 1) //断路器1
	app_get_duanluqi1_address(str_buffer);  //得到断路器1的地址
	else if(index == 2) //断路器2
	app_get_duanluqi2_address(str_buffer);  //得到断路器2的地址
	else if(index == 3) //断路器3
	app_get_duanluqi3_address(str_buffer);  //得到断路器3的地址	
	
	print_mac_addr(str_buffer,&checksum);	
	
	for(i=0;i<5;i++)
	  cobra_uart_write(M3_UART0, suf_rsp[6+i]); // 00 00 00 00 68	
	checksum += 0x68;
		
	for(i = 0; i < length; i++)
	{
		cobra_uart_write(M3_UART0, p_buffer[i]);
		checksum += p_buffer[i];
	}
	
	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);		
}
void print_read_rsq_public_part(UINT  CMD,UINT8 length,USHORT *checksum,enum UART_COMMAND_TYPE type)
{
	UINT i;
	for(i=0;i<5;i++)
	{
		if(i>3)
		*checksum += pre_rsp[i];
		cobra_uart_write(M3_UART0, pre_rsp[i]);
	}

	*checksum += length;
	cobra_uart_write(M3_UART0, length & 0xFF);  // 报文长度
	cobra_uart_write(M3_UART0, (length >> 8) & 0xFF);
	
	if(type == UART_CMD_READ)
	{
	cobra_uart_write(M3_UART0, 0x83);  //报文类型 83H
	*checksum += 0x83;
	}
	else if(type == UART_CMD_REPORT)
	{
	cobra_uart_write(M3_UART0, 0x04);  //报文类型 04H
	*checksum += 0x04;
	}
	
	cobra_uart_write(M3_UART0, CMD & 0xFF);
	cobra_uart_write(M3_UART0, (CMD>>8) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD>>16) & 0xFF);
	cobra_uart_write(M3_UART0, (CMD>>24) & 0xFF);
	*checksum += CMD & 0xFF;
	*checksum += (CMD>>8) & 0xFF;
	*checksum += (CMD>>16) & 0xFF;
	*checksum += (CMD>>24) & 0xFF;
	
	for(i=0;i<11;i++)
	{
		*checksum += suf_rsp[i];
		cobra_uart_write(M3_UART0, suf_rsp[i]);
	}	
}
void cmd_dev_mac_addr_read_handler(void)
{
	 struct bd_addr  rdw_set_bdaddr[4];
   USHORT checksum = 0;	
	 UINT8 i,len=6;
	 int8_t j;
	 nvds_get(NVDS_TAG_BD_ADDRESS, &len, rdw_set_bdaddr[0].addr);
	
	 nvds_get(BREAKER_ADDR1_FLAG, &len, rdw_set_bdaddr[1].addr);
	
	 nvds_get(BREAKER_ADDR2_FLAG, &len, rdw_set_bdaddr[2].addr);
	
	 nvds_get(BREAKER_ADDR3_FLAG, &len, rdw_set_bdaddr[3].addr);

   len *= 4;
	
	 print_read_rsq_public_part(CMD_DEV_MAC_ADDR,len,&checksum,UART_CMD_READ);
	
	 for(i=0;i<4;i++)
		 {
			 for(j=5;j>=0;j--)
					{
						cobra_uart_write(M3_UART0, rdw_set_bdaddr[i].addr[j]);
						checksum += rdw_set_bdaddr[i].addr[j];
					}	
		 }
	 cobra_uart_write(M3_UART0, checksum&0xFF);
	 cobra_uart_write(M3_UART0, 0x16);
}
void cmd_dev_pair_mode_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j;
	print_read_rsq_public_part(CMD_DEV_PAIR_MODE,4*8+1,&checksum,UART_CMD_READ);
	
	cobra_uart_write(M3_UART0, realtek_device);
	
	 for(i=0;i<4;i++)
		 {
				cobra_uart_write(M3_UART0, app_get_ble_smp_level());
			  checksum += app_get_ble_smp_level();
				cobra_uart_write(M3_UART0, realtek_pin[i].pin_length);	
        checksum += realtek_pin[i].pin_length;			 
			  for(j=0;j<realtek_pin[i].pin_length;j++)
					{
           cobra_uart_write(M3_UART0, realtek_pin[i].pin_name[j]);	
					 checksum += realtek_pin[i].pin_name[j];
					}
			}
	 cobra_uart_write(M3_UART0, checksum&0xFF);
	 cobra_uart_write(M3_UART0, 0x16);
}
void cmd_ble_params_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j;
	print_read_rsq_public_part(CMD_BLE_PARAMS,5,&checksum,UART_CMD_READ);
	
	cobra_uart_write(M3_UART0, app_get_ble_power_level());
	checksum += app_get_ble_power_level();
	
	cobra_uart_write(M3_UART0, (app_device_get_adv_interval()>>8)&0xFF);
	checksum += (app_device_get_adv_interval()>>8)&0xFF;	

	cobra_uart_write(M3_UART0, app_device_get_adv_interval()&0xFF);
	checksum += app_device_get_adv_interval()&0xFF;	
	
	cobra_uart_write(M3_UART0, (app_device_get_scan_interval()>>8)&0xFF);
	checksum += (app_device_get_scan_interval()>>8)&0xFF;	

	cobra_uart_write(M3_UART0, app_device_get_scan_interval()&0xFF);
	checksum += app_device_get_scan_interval()&0xFF;		
	
	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
#if BLE_DATA_RECEIVE_MODE
void cmd_ble_data_rece_mode_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j;
	print_read_rsq_public_part(CMD_BLE_DATA_MODE,1,&checksum,UART_CMD_READ);
	
	cobra_uart_write(M3_UART0, app_get_ble_data_rece_mode());
	checksum += app_get_ble_data_rece_mode();	
	
	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
#endif
#if CMD_DEV_NAME_FUNCTION
void cmd_device_name_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j,length;
	UCHAR dev_name[APP_DEVICE_NAME_MAX_LEN];
	
	length = app_get_device_name(dev_name);
	print_read_rsq_public_part(CMD_DEV_NAME,length,&checksum,UART_CMD_READ);
	
	for(i=0;i<length;i++)
	{
	cobra_uart_write(M3_UART0, dev_name[i]);
	checksum += dev_name[i];	
	}
	
	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
#endif
#if CMD_TX_POWER_FUNCTION
void cmd_tx_power_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j,length;
	
	print_read_rsq_public_part(CMD_TX_POWER,1,&checksum,UART_CMD_READ);
	
	cobra_uart_write(M3_UART0, app_get_ble_power_level());
	checksum += app_get_ble_power_level();	

	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
#endif
#if CMD_ADV_INTERVAL_FUNCTION
void cmd_adv_interval_read_handler(void)
{
	USHORT checksum = 0;
	UINT8 i,j,length;
	
	print_read_rsq_public_part(CMD_ADV_INTERVAL,2,&checksum,UART_CMD_READ);
	
	cobra_uart_write(M3_UART0, (app_device_get_adv_interval()>>8)&0xFF);
	checksum += (app_device_get_adv_interval()>>8)&0xFF;	

	cobra_uart_write(M3_UART0, app_device_get_adv_interval()&0xFF);
	checksum += app_device_get_adv_interval()&0xFF;		

	cobra_uart_write(M3_UART0, checksum&0xFF);
	cobra_uart_write(M3_UART0, 0x16);	
}
#endif
#if CMD_DEV_ADDR_FUNCTION
void cmd_dev_addr_read_handler(USHORT length,UCHAR *user_data)
{
	 struct bd_addr  rdw_set_bdaddr[4];
   USHORT checksum = 0;	
	 UINT8 i,len=6,mask;
	 UINT8 j,sum=0;
	
	 mask = *user_data;

	 for(i=0;i<4;i++)
	 {
		 if(mask>>i && 0x01)
		 {
			 sum++;
		 }
		 
	 }
	 print_read_rsq_public_part(CMD_DEV_ADDR,sum*6+1,&checksum,UART_CMD_READ);
	
		cobra_uart_write(M3_UART0, mask);
		checksum += mask;
	 
	 for(i=0;i<4;i++)
		 {
			 if(mask>>i && 0x01)
			 {
				 switch(i)
				 {
					 case 0:
						 nvds_get(NVDS_TAG_BD_ADDRESS, &len, rdw_set_bdaddr[0].addr);
					 break;
					 case 1:
						 nvds_get(BREAKER_ADDR1_FLAG, &len, rdw_set_bdaddr[0].addr);
					 break;
					 case 2:
						 nvds_get(BREAKER_ADDR2_FLAG, &len, rdw_set_bdaddr[0].addr);
					 break;
					 case 3:
						 nvds_get(BREAKER_ADDR3_FLAG, &len, rdw_set_bdaddr[0].addr);
					 break;	
				 }					 
				 for(j=0;j<=5;j++)
						{
							cobra_uart_write(M3_UART0, rdw_set_bdaddr[0].addr[j]);
							checksum += rdw_set_bdaddr[0].addr[j];
						}						   
			 }

		 }
	 cobra_uart_write(M3_UART0, checksum&0xFF);
	 cobra_uart_write(M3_UART0, 0x16);
}
#endif
#if CMD_DEV_PAIR_MODE_FUNCTION
void cmd_dev_pair_mm_read_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index=0,sum=0;
	 UINT8 i,j,mask;
	 USHORT checksum = 0;
	
	 mask = *user_data;

	 for(i=0;i<4;i++)
	 {
		 if(mask>>i && 0x01)
		 {
			 sum++;
		 }
		 
	 }
	 print_read_rsq_public_part(CMD_DEV_ADDR,sum+1,&checksum,UART_CMD_READ);
	
		cobra_uart_write(M3_UART0, mask);
		checksum += mask;
	 
	 for(i=0;i<4;i++)
		 {
			 if(mask>>i && 0x01)
			 {
				 switch(i)
				 {
					 case 0:
							cobra_uart_write(M3_UART0, app_get_ble_smp_level());
							checksum += app_get_ble_smp_level();
					 break;
					 case 1:
						 	cobra_uart_write(M3_UART0, App_Get_Smp_Level(1));
							checksum += App_Get_Smp_Level(1);
					 break;
					 case 2:
						 	cobra_uart_write(M3_UART0, App_Get_Smp_Level(2));
							checksum += App_Get_Smp_Level(2);
					 break;
					 case 3:
						 	cobra_uart_write(M3_UART0, App_Get_Smp_Level(3));
							checksum += App_Get_Smp_Level(3);
					 break;	
				 }					 				   
			 }

		 }
	 cobra_uart_write(M3_UART0, checksum&0xFF);
	 cobra_uart_write(M3_UART0, 0x16);
}
#endif
#if CMD_DEV_PAIR_VALUE_FUNCTION
void cmd_dev_pair_value_read_handler(USHORT length,UCHAR *user_data)
{
	 UINT8 index=0,sum=0;
	 UINT8 i,j,mask;
	 USHORT checksum = 0;
	
	 mask = *user_data;

	 for(i=0;i<4;i++)
	 {
		 if(mask>>i && 0x01)
		 {
			 sum += realtek_pin[i].pin_length;
		 }		 
	 }
	 print_read_rsq_public_part(CMD_DEV_PAIR_VALUE,sum+1,&checksum,UART_CMD_READ);
	
		cobra_uart_write(M3_UART0, mask);
		checksum += mask;
	 
	 for(i=0;i<4;i++)
		 {
			 if(mask>>i && 0x01)
			 {
				 switch(i)
				 {
					 case 0:
							cobra_uart_write(M3_UART0, realtek_pin[0].pin_length);
							checksum += realtek_pin[0].pin_length;
					    for(j=0;j<realtek_pin[0].pin_length;j++)
							{
							cobra_uart_write(M3_UART0, realtek_pin[0].pin_name[j]);
							checksum += realtek_pin[0].pin_name[j];								
							}
					 break;
					 case 1:
							cobra_uart_write(M3_UART0, realtek_pin[1].pin_length);
							checksum += realtek_pin[1].pin_length;
					    for(j=0;j<realtek_pin[1].pin_length;j++)
							{
							cobra_uart_write(M3_UART0, realtek_pin[1].pin_name[j]);
							checksum += realtek_pin[1].pin_name[j];								
							}
					 break;
					 case 2:
							cobra_uart_write(M3_UART0, realtek_pin[2].pin_length);
							checksum += realtek_pin[2].pin_length;
					    for(j=0;j<realtek_pin[2].pin_length;j++)
							{
							cobra_uart_write(M3_UART0, realtek_pin[2].pin_name[j]);
							checksum += realtek_pin[2].pin_name[j];								
							}
					 break;
					 case 3:
							cobra_uart_write(M3_UART0, realtek_pin[3].pin_length);
							checksum += realtek_pin[3].pin_length;
					    for(j=0;j<realtek_pin[3].pin_length;j++)
							{
							cobra_uart_write(M3_UART0, realtek_pin[3].pin_name[j]);
							checksum += realtek_pin[3].pin_name[j];								
							}
					 break;	
				 }					 				   
			 }

		 }
	 cobra_uart_write(M3_UART0, checksum&0xFF);
	 cobra_uart_write(M3_UART0, 0x16);
}
#endif
#if CMD_LINK_RSSI_FUNCTION
void app_slave1_condix(uint8_t conidex)
{
	conn_condiex.slave1_condiex = conidex;
}
void app_slave2_condix(uint8_t conidex)
{
	conn_condiex.slave2_condiex = conidex;
}
void app_master1_condix(uint8_t conidex)
{
	conn_condiex.master1_condiex = conidex;
}
void app_master2_condix(uint8_t conidex)
{
	conn_condiex.master2_condiex = conidex;
}
void app_master3_condix(uint8_t conidex)
{
	conn_condiex.master3_condiex = conidex;
}
int gapc_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	

    conn_condiex.slave1_rssi = (int16_t)param->rssi;
    active_obj_sum--;
	  if(active_obj_sum == 0)
			printout_rssi_value();
    return (KE_MSG_CONSUMED);
}
int gapc_slave_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	

    conn_condiex.slave2_rssi = (int16_t)param->rssi;
    active_obj_sum--;	
	  if(active_obj_sum == 0)
			printout_rssi_value();	
    return (KE_MSG_CONSUMED);
}
int gapc_device1_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	
	printf("gapc_device1_link_rssi_ind_handler %x\n",param->rssi);
    conn_condiex.master1_rssi = (int16_t)param->rssi;
    active_obj_sum--;	
	  if(active_obj_sum == 0)
			printout_rssi_value();	
    return (KE_MSG_CONSUMED);
}
int gapc_device2_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	
    conn_condiex.master2_rssi = (int16_t)param->rssi;
    active_obj_sum--;	
	  if(active_obj_sum == 0)
			printout_rssi_value();	
    return (KE_MSG_CONSUMED);
}
int gapc_device3_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	
    conn_condiex.master3_rssi = (int16_t)param->rssi;
    active_obj_sum--;	
	  if(active_obj_sum == 0)
			printout_rssi_value();	
    return (KE_MSG_CONSUMED);
}
#endif
//返回电表当前连接的从设备的地址
void iris_dianbiao_slave_number_rsp(void)
{
#if DIANBIAO_AUTO_ENABLE 	
	if(app_iris_auto_connect_state())
	{
			printf("app_iris_auto_connect_state .......\n");
			return;
	}
#endif	
	cmd_ble_conn_info_handler(UART_CMD_REPORT);
}
//读取电表当前连接的主设备 地址
void iris_dianbiao_master_number_rsp(void)
{
	cmd_ble_conn_info_handler(UART_CMD_REPORT);	
}
UINT8 get_reset_state(void)
{
		UINT8 length;
		UINT8 set_flag = 0;
		length = IRIS_MAC_SETFLAG_LENGTH;
		if(rwip_param.get(IRIS_MAC_SETFLAG, &length, &set_flag) != PARAM_OK)
			set_flag = 0;	
		return set_flag;
}

#endif