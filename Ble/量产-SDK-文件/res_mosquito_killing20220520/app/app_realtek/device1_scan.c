/************************************************************************
  Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
  Project Name:      
  File name:   
Description:

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/
#if (APP_SCAN_DEVICE_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "device1_scan.h"
#include "device1_scan_task.h"
#include "rdw_mcu.h"
#include "cobra_soft_timer.h"
#include "cobra_system.h"
#include "nvds.h"
#include "app_report_adv.h"
#include "app_realtek_console.h"

LOCAL struct bd_addr duanluqi_address1 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
LOCAL struct bd_addr duanluqi_address2 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
LOCAL struct bd_addr duanluqi_address3 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

LOCAL UCHAR  duanluqi_address1_pincode[6];
LOCAL UCHAR  duanluqi_address2_pincode[6];
LOCAL UCHAR  duanluqi_address3_pincode[6];

LOCAL UINT duanluqi_address1_connection_ok;       //是否继续稍描  断路器1 的地址
LOCAL UINT duanluqi_address2_connection_ok;
LOCAL UINT duanluqi_address3_connection_ok;

LOCAL UINT duanluqi_1_is_ready;
LOCAL UINT duanluqi_2_is_ready;
LOCAL UINT duanluqi_3_is_ready;

//请求断路器开始连接
//0 断开，  1  连接
LOCAL UINT duanluqi_1_request_connect = 0;
LOCAL UINT duanluqi_2_request_connect = 0;
LOCAL UINT duanluqi_3_request_connect = 0;

LOCAL UINT duanluqi_global_connect_flag;


struct scan_device_env_tag  scan_device1_env;

LOCAL UINT32 system_watchdog_timer_index;
LOCAL UINT32 system_reboot_flag;

/// Scanning interval - 30ms - 48 slots
#define M_ADV_SCAN_INTERVAL                  (24)

extern const struct ke_task_desc TASK_DESC_SCAN_DEV1;

void app_device1_send_gapm_reset_cmd(void);

void app_device1_watchdog_reboot(void);

UINT app_device_compare_duanluqi1_address(UCHAR * source_addr)
{
//		printf("444 %x %x %x %x",source_addr[0],source_addr[5],duanluqi_address1.addr[0],duanluqi_address1.addr[5]);
		if(!memcmp(source_addr, duanluqi_address1.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}

UINT app_device_compare_duanluqi2_address(UCHAR * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address2.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}

UINT app_device_compare_duanluqi3_address(UCHAR * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address3.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}


UINT app_device_connect_to_duanluqi_1(void)
{
	
		return duanluqi_1_request_connect;
}

void app_device_connect_to_duanluqi_1_enable(UINT enable)
{
	
		duanluqi_1_request_connect = enable;
}

UINT app_device_connect_to_duanluqi_2(void)
{
	
		return duanluqi_2_request_connect;
}

void app_device_connect_to_duanluqi_2_enable(UINT enable)
{
	
		duanluqi_2_request_connect = enable;
}

UINT app_device_connect_to_duanluqi_3(void)
{
	
		return duanluqi_3_request_connect;
}

void app_device_connect_to_duanluqi_3_enable(UINT enable)
{
	
		duanluqi_3_request_connect = enable;
}


UINT  app_device_continue_scan_duanluqi_address1(void)
{	
		return duanluqi_address1_connection_ok;
}

void app_device_enable_scan_duanluqi1_address(UINT scan_enable)
{	
	duanluqi_address1_connection_ok = scan_enable;
}

UINT  app_device_continue_scan_duanluqi_address2(void)
{	
		return duanluqi_address2_connection_ok;
}

void app_device_enable_scan_duanluqi2_address(UINT scan_enable)
{	
	duanluqi_address2_connection_ok = scan_enable;
}

UINT  app_device_continue_scan_duanluqi_address3(void)
{	
		return duanluqi_address3_connection_ok;
}

void app_device_enable_scan_duanluqi3_address(UINT scan_enable)
{	
	duanluqi_address3_connection_ok = scan_enable;
}


//设置断路器是否连接标志

void app_device_set_duanluqi_1_connected_flag(UINT connected_flag)
{
		duanluqi_1_is_ready = connected_flag;
}

void app_device_set_duanluqi_2_connected_flag(UINT connected_flag)
{
		duanluqi_2_is_ready = connected_flag;
}

void app_device_set_duanluqi_3_connected_flag(UINT connected_flag)
{
		duanluqi_3_is_ready = connected_flag;
}


//得到断路器连接是否OK
UINT app_get_duanluqi_1_connected_flag(void)
{
		return duanluqi_1_is_ready;
}


UINT app_get_duanluqi_2_connected_flag(void)
{
		return duanluqi_2_is_ready;
}

UINT app_get_duanluqi_3_connected_flag(void)
{
		return duanluqi_3_is_ready;
}

/////////////////////////////////////////

void app_set_new_duanluqi1_pincode(UCHAR * p_buffer)
{
	  memcpy(duanluqi_address1_pincode, p_buffer, 6);	
}

void app_set_new_duanluqi2_pincode(UCHAR * p_buffer)
{
	  memcpy(duanluqi_address2_pincode, p_buffer, 6);	
}

void app_set_new_duanluqi3_pincode(UCHAR * p_buffer)
{
	  memcpy(duanluqi_address3_pincode, p_buffer, 6);	
}

UCHAR * app_get_new_duanluqi1_pincode(void)
{
	  return duanluqi_address1_pincode;
}

UCHAR * app_get_new_duanluqi2_pincode(void)
{
	  return duanluqi_address2_pincode;
}

UCHAR * app_get_new_duanluqi3_pincode(void)
{
	  return duanluqi_address3_pincode;
}


void app_set_new_duanluqi1_address(UCHAR * p_buffer)
{
		memcpy(duanluqi_address1.addr, p_buffer, BD_ADDR_LEN);		
}

void app_set_new_duanluqi2_address(UCHAR * p_buffer)
{
		memcpy(duanluqi_address2.addr, p_buffer, BD_ADDR_LEN);		
}

void app_set_new_duanluqi3_address(UCHAR * p_buffer)
{
		memcpy(duanluqi_address3.addr, p_buffer, BD_ADDR_LEN);		
}

void app_get_duanluqi1_address(UCHAR * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address1.addr, BD_ADDR_LEN);	
}

void app_get_duanluqi2_address(UCHAR * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address2.addr, BD_ADDR_LEN);	
}

void app_get_duanluqi3_address(UCHAR * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address3.addr, BD_ADDR_LEN);	
}

UINT app_get_pincode_convert_ascii_to_hex(UCHAR *p_ascii, UCHAR length)
{
	  UCHAR i;
	  UCHAR ch;
	  UINT  temp_result = 0;
	  for(i = 0; i < length; i++)
	  {
			 ch = *p_ascii++;
//			printf("ch[%d]=%x",i,ch);
			 if((ch >= 0x30) && (ch <= 0x39))
			 {
				  ch -= 0x30;
			 }
//			 else if((ch >= 0x41)&&(ch <= 0x46))
//			 {
//				  ch -= 0x37;
//			 }
//			 else if((ch >= 0x61)&&(ch <= 0x66))
//			 {
//				  ch -= 0x57;
//			 }
			 else 
				 ch = 0xff;
			 temp_result *= 10;
			 temp_result += ch;
		}
		return temp_result;
	
}

UINT app_device1_scan_get_global_flag(void)
{
		return duanluqi_global_connect_flag;
}

void  app_device1_scan_set_global_flag(DEVICE_INDEX index)
{
		if(index == SERVER_DEVICE1_INDEX)
		{
				duanluqi_global_connect_flag |= 0x01;
		}
		else if(index == SERVER_DEVICE2_INDEX)
		{
				duanluqi_global_connect_flag |= 0x02;
		}
	  else if(index == SERVER_DEVICE3_INDEX)
		{
				duanluqi_global_connect_flag |= 0x04;
		}
}

void  app_device1_scan_clear_global_flag(DEVICE_INDEX index)
{
		if(index == SERVER_DEVICE1_INDEX)
		{
				duanluqi_global_connect_flag &= ~0x01;
		}
		else if(index == SERVER_DEVICE2_INDEX)
		{
				duanluqi_global_connect_flag &= ~0x02;
		}
	  else if(index == SERVER_DEVICE3_INDEX)
		{
				duanluqi_global_connect_flag &= ~0x04;
		}
}


//--------------------------------------------------------------------------------------------------------------
/*
建立一个 扫描任务


*/
void app_device1_scan_init(void)
{
	
		memset(&scan_device1_env, 0, sizeof(scan_device1_env));
	
		ke_task_create(TASK_SCAN_DEVICE1, &TASK_DESC_SCAN_DEV1);
	
		scan_device1_env.interval = M_ADV_SCAN_INTERVAL;
	
		duanluqi_address1_connection_ok = 0;
		duanluqi_address2_connection_ok = 0;
		duanluqi_address3_connection_ok = 0;
		
		duanluqi_1_is_ready = 0;
		duanluqi_2_is_ready = 0;
		duanluqi_3_is_ready = 0;
	
	  duanluqi_global_connect_flag = 0;
	
	  app_adv_report_init();
	
}


 void app_device1_send_gapm_reset_cmd(void)
{
    // Reset the stack
    struct gapm_reset_cmd *p_cmd = KE_MSG_ALLOC(GAPM_RESET_CMD,
                                                TASK_GAPM, TASK_SCAN_DEVICE1,
                                                gapm_reset_cmd);

    p_cmd->operation = GAPM_RESET;

    ke_msg_send(p_cmd);
}


/*
发送消息给 GAPM , 建立一个扫描 , 蓝牙扫描, 成为 MASTER 
*/
void app_device1_scan_create_activity(void)
{
			struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
																																							 TASK_SCAN_DEVICE1, gapm_activity_create_cmd);
			p_cmd->operation                       = GAPM_CREATE_SCAN_ACTIVITY;
			p_cmd->own_addr_type                   = GAPM_STATIC_ADDR  ; //GAPM_STATIC_ADDR;
	
			
			ke_msg_send(p_cmd);
			
			scan_device1_env.scan_state = APP_DEVICE_SCAN_STATE_CREATING;
			
}

#if 0
/*
   建立定时器, 检测系统是否正常工作
*/

void app_device1_watchdog_function(UINT param)
{
	   if(system_reboot_flag)
		 {
			  printf("system need reboot ......................................\n"); 
			  cobra_app_reboot_task();
		 }
		 app_device1_watchdog_timer_set(1);
}

void app_device1_watchdog_timer_set(UINT flag)
{
		system_reboot_flag = flag;
}

void app_device1_create_watchdog_timer(void)
{
		system_watchdog_timer_index = cobra_soft_timer_create(app_device1_watchdog_function);				
		cobra_soft_timer_start(system_watchdog_timer_index, COBRA_SOFT_TIMER_TICK*3, 1);  	  //3秒							
    app_device1_watchdog_timer_set(0);
	
}

void app_device1_watchdog_reboot(void)
{
		 UINT8   temp;
	   volatile UINT32  temp_32;
	   UINT32  i;
     UINT8   temp_breaker_addr[6];
	   UINT8   breaker_en;
	
	   GLOBAL_INT_STOP();
	
		 temp_32 = METER_REBOOT_MAGIC;
		 nvds_put(METER_REBOOT_FLAG, METER_REBOOT_FLAG_LEN, (UCHAR *)&temp_32);
	
		 nvds_put(BREAKER_ADDR1_FLAG, BREAKER_ADDR1_LEN, (UCHAR *)duanluqi_address1.addr);
	   nvds_put(BREAKER_ADDR2_FLAG, BREAKER_ADDR2_LEN, (UCHAR *)duanluqi_address2.addr);
	   nvds_put(BREAKER_ADDR3_FLAG, BREAKER_ADDR3_LEN, (UCHAR *)duanluqi_address3.addr);
	  
	   nvds_put(BREAKER1_ENABLE_FLAG, BREAKER1_ENABLE_LEN, (UCHAR *)&duanluqi_1_request_connect);
	   nvds_put(BREAKER2_ENABLE_FLAG, BREAKER2_ENABLE_LEN, (UCHAR *)&duanluqi_2_request_connect);
	   nvds_put(BREAKER3_ENABLE_FLAG, BREAKER3_ENABLE_LEN, (UCHAR *)&duanluqi_3_request_connect);
	
	   cobra_system_nvds_save_to_eflash();
     platform_reset(0x00);  						
}

#endif

void app_device1_set_device_config(void)
{
		// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
																												 TASK_GAPM, TASK_SCAN_DEVICE1,
																												 gapm_set_dev_config_cmd);
			// Set the operation
			cmd->operation = GAPM_SET_DEV_CONFIG;
			// Set the device role - Peripheral
			cmd->role      = GAP_ROLE_ALL;
			
			cmd->max_mtu = COBRA4_MTU;
			cmd->max_mps = COBRA4_MPS;
			// Set Data length parameters
			cmd->sugg_max_tx_octets = LE_MAX_OCTETS; //BLE_MIN_OCTETS;
			cmd->sugg_max_tx_time   = LE_MAX_TIME;
			
			cmd->privacy_cfg = 0;
#if DIANBIAO_ENABLE_PRIV_EN				
			cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
#endif
			memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
			
			// Send message
      ke_msg_send(cmd);
	
}

void app_device1_start_scan(void)
{
			#if (NVDS_SUPPORT)
			UINT8 len = NVDS_TAG_BLE_PARAM_LENGTH;
			UINT8 nv_para[5];
			#endif
			struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_SCAN_DEVICE1,
																																						 gapm_activity_start_cmd);
#if 0				 //扩展扫描	
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = scan_device1_env.act_id;
			p_start_cmd->u_param.scan_param.type                    = GAPM_SCAN_TYPE_OBSERVER;
			p_start_cmd->u_param.scan_param.prop                    = GAPM_SCAN_PROP_PHY_CODED_BIT ;
			p_start_cmd->u_param.scan_param.dup_filt_pol            = GAPM_DUP_FILT_DIS;
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 4*scan_device1_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*scan_device1_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;
#else
			//普通 1M  扫描
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = scan_device1_env.act_id;
			p_start_cmd->u_param.scan_param.type                    = GAPM_SCAN_TYPE_OBSERVER;
			p_start_cmd->u_param.scan_param.prop                    = GAPM_SCAN_PROP_PHY_1M_BIT ;
			p_start_cmd->u_param.scan_param.dup_filt_pol            = GAPM_DUP_FILT_DIS;
			
			#if (ADV_INTER_SELF_DEFINE)
			if (app_device_get_scan_interval()>0)
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = app_device_get_scan_interval();				
			else
			{
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval;    //old 32
			app_device_set_can_interval(32*scan_device1_env.interval);	
			}
      #else				
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval; //32*scan_device1_env.interval;    //old 32
			#endif
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*scan_device1_env.interval;
			printf("scan_intv=%x",p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv);
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*scan_device1_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;

#endif			
			// Send message
      ke_msg_send(p_start_cmd);
}


////////////////////////////////////////////////////////////////////////////////////////
//2020/3/10 
void app_device1_stop_scan(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_GAPM, TASK_SCAN_DEVICE1,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = scan_device1_env.act_id;

    // Send the message
    ke_msg_send(cmd);

}

void app_device1_autofind_duanluqi_start_scan(void)
{
			#if (NVDS_SUPPORT)
			UINT8 len = NVDS_TAG_BLE_PARAM_LENGTH;
			UINT8 nv_para[5];
			#endif
			struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_SCAN_DEVICE1,
																																						 gapm_activity_start_cmd);
			//普通 1M  扫描
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = scan_device1_env.act_id;
			p_start_cmd->u_param.scan_param.type                    = GAPM_SCAN_TYPE_OBSERVER;
			p_start_cmd->u_param.scan_param.prop                    = GAPM_SCAN_PROP_PHY_1M_BIT ;
			p_start_cmd->u_param.scan_param.dup_filt_pol            = GAPM_DUP_FILT_DIS;
			
			
			#if (ADV_INTER_SELF_DEFINE)
			if (app_device_get_scan_interval()>0)
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = app_device_get_scan_interval();				
			else
			{
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval;    //old 32
			app_device_set_can_interval(32*scan_device1_env.interval);	
			}
      #else				
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval;    //old 32
			#endif
			printf("scan_intv=%x",p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv);
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*scan_device1_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;

			
			// Send message
      ke_msg_send(p_start_cmd);
}

//寻找广播报中是否有 pin code
UINT app_device1_find_adv_pin_code(struct gapm_ext_adv_report_ind* p_param, UINT *pin_code_buffer)
{
		UCHAR * p = p_param->data;
	    UCHAR length = p_param->length;
		UCHAR adv_length;
		UCHAR adv_character;
	    UCHAR temp_length = 0;
		while((p != NULL)&&(temp_length < length))
		{
				adv_length = *p++;
				adv_character = *p;
				if(adv_character != 0xFF)
				{
						p += adv_length;
				}
				else
				{
/*
	 | 长度   |  类型  |  厂商代码   |  断路器设备类别码 |   断路器配对特征值 |
   | 0x09   | 0xFF   |  0xFF 0xFF  |   0xC4            |   0xE2 0x12        |  0xc3 | pincode
*/					
					 if(*(p+6) == 0xC3)
					 {
						   *pin_code_buffer = *(p+10);
						   *pin_code_buffer <<= 8;
						   *pin_code_buffer += *(p+9);
						   *pin_code_buffer <<= 8;						 
						   *pin_code_buffer += *(p+8);
						   *pin_code_buffer <<= 8;
						   *pin_code_buffer += *(p+7);						 
						   return 1;
					 }
				}
				temp_length ++;
				temp_length += adv_length;
		}
		return 0;

}	

/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//////////////////// 关于连接参数的一些函数 和变量
//基本连接参数 主
LOCAL USHORT global_connection_interval = 0;   //连接间隔       
LOCAL UCHAR  global_latency = 0;               //连接时延
LOCAL USHORT global_supervision_timeout = 0;   //连接超时

void app_device_set_connection_interval(USHORT value)
{
	   	global_connection_interval = value;
}

void app_device_set_latency(UCHAR value)
{
	   	global_latency = value;
}

void app_device_set_supervision_timeout(USHORT value)
{
	   	global_supervision_timeout = value;
}


USHORT app_device_get_connection_interval(void)
{
	   	return global_connection_interval ;
}


UCHAR app_device_get_latency(void)
{
	   	return global_latency ;
}

USHORT app_device_get_supervision_timeout(void)
{
	   	return global_supervision_timeout;
}


//////////////////////////////////////////////
//////////////////// 关于蓝牙发射功率的配置

LOCAL BLE_POWER_LEVEL global_ble_power_level = LEVEL_0DB;

BLE_POWER_LEVEL app_get_ble_power_level(void)
{
	  return global_ble_power_level;
}

void app_set_ble_power_level(BLE_POWER_LEVEL level)
{
	   global_ble_power_level = level;
	
	   if(rwip_rf.txpwr_cs_set == NULL) return;	
	
	   switch(level)
		 {
			 case LEVEL_4DB:    //4db
			 {
				// app_meter_add5db();
				 rwip_rf.txpwr_cs_set(0x0c);
				 break;  
			 }
			 case LEVEL_0DB:   //0db
			 {
				  rwip_rf.txpwr_cs_set(0x0c);
				  break;
			 }
			 case LEVEL_NEG_4DB:  //-4db
			 {
				  rwip_rf.txpwr_cs_set(0x09);
				  break;
			 }
			 case LEVEL_NEG_8DB:
			 {
				  rwip_rf.txpwr_cs_set(0x06);
				  break;
			 }
			 case LEVEL_NEG_20DB:
			 {
          rwip_rf.txpwr_cs_set(0x00);				 
				  break;
			 }
			 default:
			 {
				  rwip_rf.txpwr_cs_set(0x0c);				 
				  break;
			 }
		 }
}


//////////////////////////////////////////////////
//////////// 关于蓝牙的 SMP 等级的配置

LOCAL BLE_SMP_LEVEL  global_smp_level = SMP_JUST_WORK;

BLE_SMP_LEVEL app_get_ble_smp_level(void)
{
	 return global_smp_level;
	
}

void app_set_ble_smp_level(BLE_SMP_LEVEL level)
{
	 global_smp_level = level;
}

LOCAL BLE_SMP_LEVEL  realtek_smp_level[4];
BLE_SMP_LEVEL App_Get_Smp_Level(UINT8 index)
{
	 return realtek_smp_level[index];
	
}

void App_Set_Smp_Level(BLE_SMP_LEVEL level,UINT8 index)
{
	 realtek_smp_level[index] = level;
}

//////////////////////////////////////////////////////////////
/////////////// 校表方案

LOCAL struct bd_addr filter_mac[MAX_FILTER_NUMBER];
LOCAL UCHAR  filter_kind[MAX_FILTER_NUMBER];
LOCAL UCHAR filter_item_number;
ADV_FILTER_TYPE adv_filter_rule = FILTER_NULL;

ADV_FILTER_TYPE  app_adv_get_filter_kind(void)
{
	 return adv_filter_rule;
}

void app_adv_set_filter_rule(ADV_FILTER_TYPE type)
{
	adv_filter_rule = type;
}

void app_adv_set_filter_number(UCHAR number)
{
	filter_item_number = number;
}

UCHAR app_adv_get_filter_number(void)
{
	return filter_item_number;
}

void app_adv_set_filter_mac_address(UCHAR index, UCHAR *p_address)
{
	 memcpy(filter_mac[index].addr, p_address, BD_ADDR_LEN);	
}

UCHAR * app_adv_get_filter_mac_address(UCHAR index)
{
	 return filter_mac[index].addr;
}

void app_adv_set_filter_mac_kind(UCHAR index, UCHAR kind)
{
	 filter_kind[index] = kind;
}

UCHAR app_adv_get_filter_mac_kind(UCHAR index)
{
	 return filter_kind[index];
}



















#endif
