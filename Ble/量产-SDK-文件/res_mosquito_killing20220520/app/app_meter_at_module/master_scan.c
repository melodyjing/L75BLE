/******************************************************************************
	File Name	:	  master_scan.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/08

	Description : 
      蓝牙模组, 可以通过 AT COMMAND 来控制


	Changed Log	:
	
		Liujian  2021/03/08	 - Creation

*****************************************************************************/
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan_task.h"
#include "master_scan.h"
#include "module_report_adv.h"
#include "rdw_mcu.h"
#include "cobra_soft_timer.h"
#include <co_bt_defines.h>
#include <ke_msg.h>
#include <gapm_task.h>
#include <string.h>
#include <stdio.h>
#include <ke_task.h>

#include "slave_device1_connection.h"

#if (COBRA_LOGGER_ENABLE == 1)	
#include <elog.h>
#endif
#define M_ADV_MASTER_SCAN_INTERVAL                  (24)

static struct bd_addr duanluqi_address1 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
//LOCAL struct bd_addr duanluqi_address2 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
//LOCAL struct bd_addr duanluqi_address3 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

//LOCAL UCHAR  duanluqi_address1_pincode[6];
//LOCAL UCHAR  duanluqi_address2_pincode[6];
//LOCAL UCHAR  duanluqi_address3_pincode[6];

static uint8_t duanluqi_address1_connection_ok;       //是否继续稍描  断路器1 的地址
//LOCAL UINT duanluqi_address2_connection_ok;
//LOCAL UINT duanluqi_address3_connection_ok;

static uint8_t duanluqi_1_is_ready;
//LOCAL UINT duanluqi_2_is_ready;
//LOCAL UINT duanluqi_3_is_ready;

//请求断路器开始连接
//0 断开，  1  连接
static uint8_t duanluqi_1_request_connect = 0;

//LOCAL UINT duanluqi_2_request_connect = 0;
//LOCAL UINT duanluqi_3_request_connect = 0;

static unsigned int duanluqi_global_connect_flag;

static uint8_t scan_flag;   //0 stop scan , 1 start scan

static uint32_t scan_time_id;

struct scan_device_env_tag  master_scan_device_env;

static void  master_scan_result_callback(uint32_t param);

void master_scan_scan_init(void)
{
	
		memset(&master_scan_device_env, 0, sizeof(master_scan_device_env));
	
		ke_task_create(TASK_MASTER_SCAN, &TASK_DESC_MASTER_SCAN);
	
		master_scan_device_env.interval = M_ADV_MASTER_SCAN_INTERVAL;
	
	  scan_flag = 0;
	
	  scan_time_id = MAX_SOFT_TIMERS;
	
//		duanluqi_address1_connection_ok = 0;
//		duanluqi_address2_connection_ok = 0;
//		duanluqi_address3_connection_ok = 0;
//		
//		duanluqi_1_is_ready = 0;
//		duanluqi_2_is_ready = 0;
//		duanluqi_3_is_ready = 0;
//	
//	  duanluqi_global_connect_flag = 0;
//	
  	  app_adv_report_init();
	
}

void master_scan_set_state(uint8_t state)
{
	  scan_flag = state;
	  if(scan_flag)
		{
			  //开启定时器,扫描5秒钟, 然后显示扫描结果
			  cobra_soft_timer_stop(scan_time_id);
			  cobra_soft_timer_destroy(scan_time_id);
			  scan_time_id = cobra_soft_timer_create(master_scan_result_callback);
	      cobra_soft_timer_start(scan_time_id, 5*COBRA_SOFT_TIMER_TICK, 0);	
			
		}
		else
		{
			 //停止扫描
			 cobra_soft_timer_stop(scan_time_id);
			 cobra_soft_timer_destroy(scan_time_id);
		}
}

uint8_t master_scan_get_state(void)
{
	   return scan_flag;
}


void master_scan_set_device_config(void)
{
		// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
																												 TASK_GAPM, TASK_MASTER_SCAN,
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

/*
发送消息给 GAPM , 建立一个扫描 , 蓝牙扫描, 成为 MASTER 
*/
void master_scan_scan_create_activity(void)
{
			struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
																																							 TASK_MASTER_SCAN, gapm_activity_create_cmd);
			p_cmd->operation                       = GAPM_CREATE_SCAN_ACTIVITY;
			p_cmd->own_addr_type                   = GAPM_STATIC_ADDR  ; //GAPM_STATIC_ADDR;
	
			
			ke_msg_send(p_cmd);
			
			master_scan_device_env.scan_state = APP_DEVICE_SCAN_STATE_CREATING;
			
}

void master_scan_start_scan(void)
{

	
			struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_MASTER_SCAN,
																																						 gapm_activity_start_cmd);

			//普通 1M  扫描
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = master_scan_device_env.act_id;
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
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*master_scan_device_env.interval; //32*scan_device1_env.interval;    //old 32
			#endif
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*master_scan_device_env.interval;
			
//			printf("scan_intv = %d \n",p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv);
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*master_scan_device_env.interval;
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = master_scan_device_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;

		
			// Send message
      ke_msg_send(p_start_cmd);
}

static void  master_scan_result_callback(uint32_t param)
{
	   app_adv_report_to_module();	
}

//////////////////////////////////////////////////////////////////
/*
 如果有设备在连接的过程中, 则先不连接

*/
unsigned int app_device1_scan_get_global_flag(void)
{
		return duanluqi_global_connect_flag;
}

/*
  设置有设备在连接中
*/
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

/*
 表示设备没有在连接中,原因可能是连接成功了, 也许

  是连接失败了
*/
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

/*
 比较source_addr 的地址是否和要连接的地址相同
*/
uint32_t app_device_compare_duanluqi1_address(uint8_t * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address1.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}

/*
   设置要连接的设备的地址
*/
void app_set_connect_device1_address(unsigned char * p_buffer)
{
		memcpy(duanluqi_address1.addr, p_buffer, BD_ADDR_LEN);		
}

unsigned int app_device_connect_to_duanluqi_1(void)
{
	
		return duanluqi_1_request_connect;
}

void app_device_connect_to_duanluqi_1_enable(unsigned int enable)
{
	
		duanluqi_1_request_connect = enable;
}

unsigned int  app_device_continue_scan_duanluqi_address1(void)
{	
		return duanluqi_address1_connection_ok;
}

void app_device_enable_scan_duanluqi1_address(unsigned int scan_enable)
{	
	duanluqi_address1_connection_ok = scan_enable;
}

//设置断路器是否连接标志

void app_device_set_duanluqi_1_connected_flag(unsigned int connected_flag)
{
		duanluqi_1_is_ready = connected_flag;
}

//得到断路器连接是否OK
unsigned int app_get_duanluqi_1_connected_flag(void)
{
		return duanluqi_1_is_ready;
}

void app_device_show_connect_info(void)
{
#if (COBRA_LOGGER_ENABLE == 1)	  
    	log_i("+CHINFO{");
	    if(app_get_duanluqi_1_connected_flag())
			{
				  log_i("%02x%02x%02x%02x%02x%02x, 0 , 3", duanluqi_address1.addr[0], \
				                                           duanluqi_address1.addr[1], \
				                                           duanluqi_address1.addr[2], \
				                                           duanluqi_address1.addr[3], \
				                                           duanluqi_address1.addr[4], \
				                                           duanluqi_address1.addr[5]);
			}
	    log_i("+CHINFO}");
			log_i("OK");
	
#endif
	
}

/*
发送数据到对方设备
*/
void app_send_data_to_device(uint8_t channel, uint8_t length, uint8_t * p_send_buffer)
{
	  if(channel == 0)
		{
			   if(app_get_duanluqi_1_connected_flag())
				 {
					    app_slave_device1_send_data_to_uart_server(length, p_send_buffer);
				 }
		}
	
}

/*
 断开连接设备
*/
void app_disconnection_device(uint8_t channel)
{
	  unsigned char temp_buffer[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	  if(channel == 0)
		{
			   app_set_connect_device1_address(temp_buffer);
			   if(app_get_duanluqi_1_connected_flag())
				 {
					    app_set_connect_device1_address(temp_buffer);
					    app_slave_device1_disconnect();
				 }
		}
}




#endif
