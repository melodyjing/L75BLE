/******************************************************************************
	File Name	:	device1_scan.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/21

	Description :   

	Changed Log	:
		Liujian  2019/11/21		- Creation

*****************************************************************************/
#ifndef __DEVICE1_SCAN_H__
#define __DEVICE1_SCAN_H__
#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "ke_task.h"        // Kernel Task
#include "rwip_config.h"             // SW configuration
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "arch.h"                 // Platform Definitions
#include <string.h>
#include "co_utils.h"
#include "ke_timer.h"             // Kernel timer
#include "def_self.h"
#if APP_SCAN_DEVICE_ENABLE


enum app_device_scan_state
{
    /// scan activity does not exists
    APP_DEVICE_SCAN_STATE_IDLE = 0,
    /// Creating scan activity
    APP_DEVICE_SCAN_STATE_CREATING,
    

};


/// Scanning environment definition
struct scan_device_env_tag
{
    ///Scan window interval (in slots)
    UINT16                 interval;
    /// Status of the scanning
    UINT8                  scan_state;
    /// Scanning activity identifier
    UINT8                  act_id;
 
} ;

typedef enum {
		SERVER_DEVICE1_INDEX,
		SERVER_DEVICE2_INDEX,
	  SERVER_DEVICE3_INDEX	
}DEVICE_INDEX;



void app_device1_scan_init(void);


void app_device1_scan_create_activity(void);

void app_device1_set_device_config(void);

void app_device1_start_scan(void);

UINT  app_device_continue_scan_duanluqi_address1(void);
UINT  app_device_continue_scan_duanluqi_address2(void);
UINT  app_device_continue_scan_duanluqi_address3(void);

void app_device_enable_scan_duanluqi1_address(UINT scan_enable);
void app_device_enable_scan_duanluqi2_address(UINT scan_enable);
void app_device_enable_scan_duanluqi3_address(UINT scan_enable);


UINT app_device_connect_to_duanluqi_1(void);
void app_device_connect_to_duanluqi_1_enable(UINT enable);

UINT app_device_connect_to_duanluqi_2(void);
void app_device_connect_to_duanluqi_2_enable(UINT enable);

UINT app_device_connect_to_duanluqi_3(void);
void app_device_connect_to_duanluqi_3_enable(UINT enable);


UINT app_device_compare_duanluqi1_address(UCHAR * source_addr);
UINT app_device_compare_duanluqi2_address(UCHAR * source_addr);
UINT app_device_compare_duanluqi3_address(UCHAR * source_addr);

//设置断路器是否连接标志

void app_device_set_duanluqi_1_connected_flag(UINT connected_flag);
void app_device_set_duanluqi_2_connected_flag(UINT connected_flag);
void app_device_set_duanluqi_3_connected_flag(UINT connected_flag);

//得到断路器连接是否OK
UINT app_get_duanluqi_1_connected_flag(void);
UINT app_get_duanluqi_2_connected_flag(void);
UINT app_get_duanluqi_3_connected_flag(void);


void app_set_new_duanluqi1_address(UCHAR * p_buffer);
void app_set_new_duanluqi2_address(UCHAR * p_buffer);
void app_set_new_duanluqi3_address(UCHAR * p_buffer);

void app_get_duanluqi1_address(UCHAR * p_buffer);
void app_get_duanluqi2_address(UCHAR * p_buffer);
void app_get_duanluqi3_address(UCHAR * p_buffer);

void app_device1_stop_scan(void);
void app_device1_autofind_duanluqi_start_scan(void);
UINT app_device1_find_adv_pin_code(struct gapm_ext_adv_report_ind* p_param, UINT *pin_code_buffer);


UINT app_device1_scan_get_global_flag(void);
void  app_device1_scan_clear_global_flag(DEVICE_INDEX index);
void  app_device1_scan_set_global_flag(DEVICE_INDEX index);

void app_device1_watchdog_timer_set(UINT flag);
void app_device1_create_watchdog_timer(void);

void app_device1_watchdog_reboot(void);

void app_set_new_duanluqi1_pincode(UCHAR * p_buffer);
void app_set_new_duanluqi2_pincode(UCHAR * p_buffer);
void app_set_new_duanluqi3_pincode(UCHAR * p_buffer);


UCHAR * app_get_new_duanluqi1_pincode(void);
UCHAR * app_get_new_duanluqi2_pincode(void);
UCHAR * app_get_new_duanluqi3_pincode(void);


void app_device_set_connection_interval(USHORT value);
void app_device_set_latency(UCHAR value);
void app_device_set_supervision_timeout(USHORT value);
USHORT app_device_get_connection_interval(void);
UCHAR app_device_get_latency(void);
USHORT app_device_get_supervision_timeout(void);


typedef enum {
	 LEVEL_4DB = 0x00,
	 LEVEL_0DB = 0x01,
	 LEVEL_NEG_4DB = 0x02,
	 LEVEL_NEG_8DB = 0x03,
	 LEVEL_NEG_20DB = 0x04,	
   LEVEL_MAX_DB,	 
}BLE_POWER_LEVEL;


BLE_POWER_LEVEL app_get_ble_power_level(void);
void app_set_ble_power_level(BLE_POWER_LEVEL level);


typedef enum {
	 SMP_NUMERIC_COMPARISION = 0x00,
	 SMP_JUST_WORK           = 0x01,
	 SMP_PASSKEY_ENTRY       = 0x02,
	 SMP_OUT_OF_BAND         = 0x03,
}BLE_SMP_LEVEL;

BLE_SMP_LEVEL app_get_ble_smp_level(void);
void app_set_ble_smp_level(BLE_SMP_LEVEL level);
BLE_SMP_LEVEL App_Get_Smp_Level(UINT8 index);
void App_Set_Smp_Level(BLE_SMP_LEVEL level,UINT8 index);


#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
typedef struct {
//	  unsigned char   meter_number;   //表位编号
		unsigned char   pluse_type;     //脉冲类型
		unsigned char   delay_internaval;
		unsigned char   power_level;    //发射功率档位
		
    unsigned char   comm_address[6];
		unsigned char   frequency_number;        //通道数目
    unsigned short  frequency[5];	
}ble_meter_struct;
#endif

typedef enum {                 //广播报文的过滤规则
	FILTER_NULL,
	FILTER_BY_MAC,
	FILTER_BY_KIND,
}ADV_FILTER_TYPE;   

#define MAX_FILTER_NUMBER 10

void app_adv_set_filter_number(UCHAR number);
UCHAR app_adv_get_filter_number(void);

void app_adv_set_filter_mac_address(UCHAR index, UCHAR *p_address);
UCHAR * app_adv_get_filter_mac_address(UCHAR index);

ADV_FILTER_TYPE  app_adv_get_filter_kind(void);
void app_adv_set_filter_rule(ADV_FILTER_TYPE type);

void app_adv_set_filter_mac_kind(UCHAR index, UCHAR kind);
UCHAR app_adv_get_filter_mac_kind(UCHAR index);
UINT app_get_pincode_convert_ascii_to_hex(UCHAR *p_ascii, UCHAR length);

#endif

#endif
