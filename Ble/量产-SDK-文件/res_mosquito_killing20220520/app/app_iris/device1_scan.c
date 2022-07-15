/******************************************************************************
	File Name	:	device1_scan.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/21

	Description :   

	Changed Log	:
		Liujian  2019/11/21		- Creation

*****************************************************************************/
#include "device1_scan.h"
#include "device1_scan_task.h"
#include "rdw_mcu.h"
#include "cobra_soft_timer.h"
#include "cobra_system.h"
#include "nvds.h"
#include "app_report_adv.h"
#if((APP_SCAN_DEVICE_ENABLE  == 1) && (DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1))

static struct bd_addr duanluqi_address1 = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
static struct bd_addr duanluqi_address2 = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
static struct bd_addr duanluqi_address3 = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

static unsigned char duanluqi1_name[12];
static unsigned char duanluqi2_name[12];
static unsigned char duanluqi3_name[12];

static unsigned char  duanluqi_address1_pincode[6];
static unsigned char  duanluqi_address2_pincode[6];
static unsigned char  duanluqi_address3_pincode[6];

static unsigned int duanluqi_address1_connection_ok;       //是否继续稍描  断路器1 的地址
static unsigned int duanluqi_address2_connection_ok;
static unsigned int duanluqi_address3_connection_ok;

static unsigned int duanluqi_1_is_ready;
static unsigned int duanluqi_2_is_ready;
static unsigned int duanluqi_3_is_ready;

//请求断路器开始连接
//0 断开，  1  连接
static unsigned int duanluqi_1_request_connect = 0;
static unsigned int duanluqi_2_request_connect = 0;
static unsigned int duanluqi_3_request_connect = 0;

static unsigned int duanluqi_global_connect_flag;


struct scan_device_env_tag  scan_device1_env;

static uint32_t system_watchdog_timer_index;
static uint32_t system_reboot_flag;

/// Scanning interval - 30ms - 48 slots
#define M_ADV_SCAN_INTERVAL                  (24)

extern const struct ke_task_desc TASK_DESC_SCAN_DEV1;

void app_device1_send_gapm_reset_cmd(void);

void app_device1_watchdog_reboot(void);

unsigned int app_device_compare_duanluqi1_address(unsigned char * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address1.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}

unsigned int app_device_compare_duanluqi2_address(unsigned char * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address2.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}

unsigned int app_device_compare_duanluqi3_address(unsigned char * source_addr)
{
		if(!memcmp(source_addr, duanluqi_address3.addr, BD_ADDR_LEN))
		{
				return 1;
		}
		return 0;
	
}


unsigned int app_device_connect_to_duanluqi_1(void)
{
	
		return duanluqi_1_request_connect;
}

void app_device_connect_to_duanluqi_1_enable(unsigned int enable)
{
	
		duanluqi_1_request_connect = enable;
}

unsigned int app_device_connect_to_duanluqi_2(void)
{
	
		return duanluqi_2_request_connect;
}

void app_device_connect_to_duanluqi_2_enable(unsigned int enable)
{
	
		duanluqi_2_request_connect = enable;
}

unsigned int app_device_connect_to_duanluqi_3(void)
{
	
		return duanluqi_3_request_connect;
}

void app_device_connect_to_duanluqi_3_enable(unsigned int enable)
{
	
		duanluqi_3_request_connect = enable;
}


unsigned int  app_device_continue_scan_duanluqi_address1(void)
{	
		return duanluqi_address1_connection_ok;
}

void app_device_enable_scan_duanluqi1_address(unsigned int scan_enable)
{	
	duanluqi_address1_connection_ok = scan_enable;
}

unsigned int  app_device_continue_scan_duanluqi_address2(void)
{	
		return duanluqi_address2_connection_ok;
}

void app_device_enable_scan_duanluqi2_address(unsigned int scan_enable)
{	
	duanluqi_address2_connection_ok = scan_enable;
}

unsigned int  app_device_continue_scan_duanluqi_address3(void)
{	
		return duanluqi_address3_connection_ok;
}

void app_device_enable_scan_duanluqi3_address(unsigned int scan_enable)
{	
	duanluqi_address3_connection_ok = scan_enable;
}


//设置断路器是否连接标志

void app_device_set_duanluqi_1_connected_flag(unsigned int connected_flag)
{
		duanluqi_1_is_ready = connected_flag;
}

void app_device_set_duanluqi_2_connected_flag(unsigned int connected_flag)
{
		duanluqi_2_is_ready = connected_flag;
}

void app_device_set_duanluqi_3_connected_flag(unsigned int connected_flag)
{
		duanluqi_3_is_ready = connected_flag;
}


//得到断路器连接是否OK
unsigned int app_get_duanluqi_1_connected_flag(void)
{
		return duanluqi_1_is_ready;
}


unsigned int app_get_duanluqi_2_connected_flag(void)
{
		return duanluqi_2_is_ready;
}

unsigned int app_get_duanluqi_3_connected_flag(void)
{
		return duanluqi_3_is_ready;
}

/////////////////////////////////////////

void app_set_new_duanluqi1_pincode(unsigned char * p_buffer)
{
	  memcpy(duanluqi_address1_pincode, p_buffer, 6);	
}

void app_set_new_duanluqi2_pincode(unsigned char * p_buffer)
{
	  memcpy(duanluqi_address2_pincode, p_buffer, 6);	
}

void app_set_new_duanluqi3_pincode(unsigned char * p_buffer)
{
	  memcpy(duanluqi_address3_pincode, p_buffer, 6);	
}

unsigned char * app_get_new_duanluqi1_pincode(void)
{
	  return duanluqi_address1_pincode;
}

unsigned char * app_get_new_duanluqi2_pincode(void)
{
	  return duanluqi_address2_pincode;
}

unsigned char * app_get_new_duanluqi3_pincode(void)
{
	  return duanluqi_address3_pincode;
}


void app_set_new_duanluqi1_address(unsigned char * p_buffer)
{
		memcpy(duanluqi_address1.addr, p_buffer, BD_ADDR_LEN);		
}

void app_set_new_duanluqi2_address(unsigned char * p_buffer)
{
		memcpy(duanluqi_address2.addr, p_buffer, BD_ADDR_LEN);		
}

void app_set_new_duanluqi3_address(unsigned char * p_buffer)
{
		memcpy(duanluqi_address3.addr, p_buffer, BD_ADDR_LEN);		
}

void app_get_duanluqi1_address(unsigned char * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address1.addr, BD_ADDR_LEN);	
}

void app_get_duanluqi2_address(unsigned char * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address2.addr, BD_ADDR_LEN);	
}

void app_get_duanluqi3_address(unsigned char * p_buffer)
{		
	memcpy(p_buffer, duanluqi_address3.addr, BD_ADDR_LEN);	
}

void app_set_duanluqi1_name(unsigned char * p_buffer)
{
	 memcpy(duanluqi1_name, p_buffer, 12);	
}

void app_set_duanluqi2_name(unsigned char * p_buffer)
{
	 memcpy(duanluqi2_name, p_buffer, 12);	
}

void app_set_duanluqi3_name(unsigned char * p_buffer)
{
	 memcpy(duanluqi3_name, p_buffer, 12);	
}

unsigned char * app_get_duanluqi1_name(void)
{
	  return duanluqi1_name;
}

unsigned char * app_get_duanluqi2_name(void)
{
	  return duanluqi2_name;
}

unsigned char * app_get_duanluqi3_name(void)
{
	  return duanluqi3_name;
}


unsigned int app_get_pincode_convert_ascii_to_hex(unsigned char *p_ascii, unsigned char length)
{
	  unsigned char i;
	  unsigned char ch;
	  unsigned int  temp_result = 0;
	  for(i = 0; i < length; i++)
	  {
			 ch = *p_ascii++;
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


unsigned int app_device1_scan_get_global_flag(void)
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

void app_device1_watchdog_function(unsigned int param)
{
	   if(system_reboot_flag)
		 {
			  printf("system need reboot ......................................\n"); 
			  cobra_app_reboot_task();
		 }
		 app_device1_watchdog_timer_set(1);
}

void app_device1_watchdog_timer_set(unsigned int flag)
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
		 uint8_t   temp;
	   volatile uint32_t  temp_32;
	   uint32_t  i;
     uint8_t   temp_breaker_addr[6];
	   uint8_t   breaker_en;
	
	   GLOBAL_INT_STOP();
	
		 temp_32 = METER_REBOOT_MAGIC;
		 nvds_put(METER_REBOOT_FLAG, METER_REBOOT_FLAG_LEN, (unsigned char *)&temp_32);
	
		 nvds_put(BREAKER_ADDR1_FLAG, BREAKER_ADDR1_LEN, (unsigned char *)duanluqi_address1.addr);
	   nvds_put(BREAKER_ADDR2_FLAG, BREAKER_ADDR2_LEN, (unsigned char *)duanluqi_address2.addr);
	   nvds_put(BREAKER_ADDR3_FLAG, BREAKER_ADDR3_LEN, (unsigned char *)duanluqi_address3.addr);
	  
	   nvds_put(BREAKER1_ENABLE_FLAG, BREAKER1_ENABLE_LEN, (unsigned char *)&duanluqi_1_request_connect);
	   nvds_put(BREAKER2_ENABLE_FLAG, BREAKER2_ENABLE_LEN, (unsigned char *)&duanluqi_2_request_connect);
	   nvds_put(BREAKER3_ENABLE_FLAG, BREAKER3_ENABLE_LEN, (unsigned char *)&duanluqi_3_request_connect);
	
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
			
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval; //32*scan_device1_env.interval;    //old 32
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*scan_device1_env.interval;
			
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
			struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_SCAN_DEVICE1,
																																						 gapm_activity_start_cmd);
			//普通 1M  扫描
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = scan_device1_env.act_id;
			p_start_cmd->u_param.scan_param.type                    = GAPM_SCAN_TYPE_OBSERVER;
			p_start_cmd->u_param.scan_param.prop                    = GAPM_SCAN_PROP_PHY_1M_BIT ;
			p_start_cmd->u_param.scan_param.dup_filt_pol            = GAPM_DUP_FILT_DIS;
			
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 32*scan_device1_env.interval;    //old 32
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*scan_device1_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = scan_device1_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;

			
			// Send message
      ke_msg_send(p_start_cmd);
}

//寻找广播报中是否有 pin code
unsigned int app_device1_find_adv_pin_code(struct gapm_ext_adv_report_ind* p_param, unsigned int *pin_code_buffer)
{
		unsigned char * p = p_param->data;
	    unsigned char length = p_param->length;
		unsigned char adv_length;
		unsigned char adv_character;
	    unsigned char temp_length = 0;
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
#if(DIANBIAO_MASTER_PARAM_SET_ENABE == 1)			
static unsigned short global_master_connection_interval = 0;   //连接间隔       
static unsigned char  global_master_latency = 0;               //连接时延
static unsigned short global_master_supervision_timeout = 0;   //连接超时

void app_master_device_set_connection_interval(unsigned short value)
{
	   	global_master_connection_interval = value;
}

void app_master_device_set_latency(unsigned char value)
{
	   	global_master_latency = value;
}

void app_master_device_set_supervision_timeout(unsigned short value)
{
	   	global_master_supervision_timeout = value;
}


unsigned short app_master_device_get_connection_interval(void)
{
	   	return global_master_connection_interval ;
}


unsigned char app_master_device_get_latency(void)
{
	   	return global_master_latency ;
}

unsigned short app_master_device_get_supervision_timeout(void)
{
	   	return global_master_supervision_timeout;
}

#endif

//////////////////////////////////////////////
//////////////////// 关于蓝牙发射功率的配置

static BLE_POWER_LEVEL global_ble_power_level = LEVEL_0DB;

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

static BLE_SMP_LEVEL  global_smp_level = SMP_JUST_WORK;

BLE_SMP_LEVEL app_get_ble_smp_level(void)
{
	 return global_smp_level;
	
}

void app_set_ble_smp_level(BLE_SMP_LEVEL level)
{
	 global_smp_level = level;
}


///////////////////////////////////////////////////////////////////
////////////////////// 设置广播内容使能  //////////////////////////
#if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)
static unsigned char global_adv_content[31];
static unsigned char global_adv_length = 0;

void app_set_ble_adv_content(unsigned char *p_buffer, unsigned char length)
{
	  if(length > 31) return;
	  global_adv_length = length;
	
	  memcpy(global_adv_content, p_buffer, length);
	
}

unsigned char app_get_ble_adv_length(void)
{
	 return global_adv_length;
}

unsigned char * app_get_ble_adv_content(void)
{
	  return global_adv_content;
}

#endif

//////////////////////////////////////////////////////////////////
//////////////////设置蓝牙工作模式 
#if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)

static BLE_WORK_MODE global_ble_workmode = BLE_WORK_MASTER_SLAVE;

void app_set_ble_work_mode(BLE_WORK_MODE mode)
{
	 global_ble_workmode = mode;
}

BLE_WORK_MODE app_get_ble_work_mode(void)
{
	  return global_ble_workmode;
}

#endif		

//////////////////// 关于连接参数的一些函数 和变量
//基本连接参数 从
#if(DIANBIAO_SLAVE_PARAM_SET_ENABE == 1)			
static unsigned short global_slave_connection_interval = 0;   //连接间隔       
static unsigned char  global_slave_latency = 0;               //连接时延
static unsigned short global_slave_supervision_timeout = 0;   //连接超时

void app_slave_device_set_connection_interval(unsigned short value)
{
	   	global_slave_connection_interval = value;
}

void app_slave_device_set_latency(unsigned char value)
{
	   	global_slave_latency = value;
}

void app_slave_device_set_supervision_timeout(unsigned short value)
{
	   	global_slave_supervision_timeout = value;
}


unsigned short app_slave_device_get_connection_interval(void)
{
	   	return global_slave_connection_interval ;
}


unsigned char app_slave_device_get_latency(void)
{
	   	return global_slave_latency ;
}

unsigned short app_slave_device_get_supervision_timeout(void)
{
	   	return global_slave_supervision_timeout;
}

#endif


//////////////////////////////////////////////////////////////
/////////////// 校表方案

static struct bd_addr filter_mac[MAX_FILTER_NUMBER];
static unsigned char  filter_kind[MAX_FILTER_NUMBER];
static unsigned char filter_item_number;
ADV_FILTER_TYPE adv_filter_rule = FILTER_NULL;

ADV_FILTER_TYPE  app_adv_get_filter_kind(void)
{
	 return adv_filter_rule;
}

void app_adv_set_filter_rule(ADV_FILTER_TYPE type)
{
	adv_filter_rule = type;
}

void app_adv_set_filter_number(unsigned char number)
{
	filter_item_number = number;
}

unsigned char app_adv_get_filter_number(void)
{
	return filter_item_number;
}

void app_adv_set_filter_mac_address(unsigned char index, unsigned char *p_address)
{
	 memcpy(filter_mac[index].addr, p_address, BD_ADDR_LEN);	
}

unsigned char * app_adv_get_filter_mac_address(unsigned char index)
{
	 return filter_mac[index].addr;
}

void app_adv_set_filter_mac_kind(unsigned char index, unsigned char kind)
{
	 filter_kind[index] = kind;
}

unsigned char app_adv_get_filter_mac_kind(unsigned char index)
{
	 return filter_kind[index];
}



















#endif
