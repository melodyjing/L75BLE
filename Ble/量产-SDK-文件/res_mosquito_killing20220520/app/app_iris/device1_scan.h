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
    uint16_t                 interval;
    /// Status of the scanning
    uint8_t                  scan_state;
    /// Scanning activity identifier
    uint8_t                  act_id;
 
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

unsigned int  app_device_continue_scan_duanluqi_address1(void);
unsigned int  app_device_continue_scan_duanluqi_address2(void);
unsigned int  app_device_continue_scan_duanluqi_address3(void);

void app_device_enable_scan_duanluqi1_address(unsigned int scan_enable);
void app_device_enable_scan_duanluqi2_address(unsigned int scan_enable);
void app_device_enable_scan_duanluqi3_address(unsigned int scan_enable);


unsigned int app_device_connect_to_duanluqi_1(void);
void app_device_connect_to_duanluqi_1_enable(unsigned int enable);

unsigned int app_device_connect_to_duanluqi_2(void);
void app_device_connect_to_duanluqi_2_enable(unsigned int enable);

unsigned int app_device_connect_to_duanluqi_3(void);
void app_device_connect_to_duanluqi_3_enable(unsigned int enable);


unsigned int app_device_compare_duanluqi1_address(unsigned char * source_addr);
unsigned int app_device_compare_duanluqi2_address(unsigned char * source_addr);
unsigned int app_device_compare_duanluqi3_address(unsigned char * source_addr);

//设置断路器是否连接标志

void app_device_set_duanluqi_1_connected_flag(unsigned int connected_flag);
void app_device_set_duanluqi_2_connected_flag(unsigned int connected_flag);
void app_device_set_duanluqi_3_connected_flag(unsigned int connected_flag);

//得到断路器连接是否OK
unsigned int app_get_duanluqi_1_connected_flag(void);
unsigned int app_get_duanluqi_2_connected_flag(void);
unsigned int app_get_duanluqi_3_connected_flag(void);


void app_set_new_duanluqi1_address(unsigned char * p_buffer);
void app_set_new_duanluqi2_address(unsigned char * p_buffer);
void app_set_new_duanluqi3_address(unsigned char * p_buffer);

void app_get_duanluqi1_address(unsigned char * p_buffer);
void app_get_duanluqi2_address(unsigned char * p_buffer);
void app_get_duanluqi3_address(unsigned char * p_buffer);

void app_device1_stop_scan(void);
void app_device1_autofind_duanluqi_start_scan(void);
unsigned int app_device1_find_adv_pin_code(struct gapm_ext_adv_report_ind* p_param, unsigned int *pin_code_buffer);


unsigned int app_device1_scan_get_global_flag(void);
void  app_device1_scan_clear_global_flag(DEVICE_INDEX index);
void  app_device1_scan_set_global_flag(DEVICE_INDEX index);

void app_device1_watchdog_timer_set(unsigned int flag);
void app_device1_create_watchdog_timer(void);

void app_device1_watchdog_reboot(void);

void app_set_new_duanluqi1_pincode(unsigned char * p_buffer);
void app_set_new_duanluqi2_pincode(unsigned char * p_buffer);
void app_set_new_duanluqi3_pincode(unsigned char * p_buffer);


unsigned char * app_get_new_duanluqi1_pincode(void);
unsigned char * app_get_new_duanluqi2_pincode(void);
unsigned char * app_get_new_duanluqi3_pincode(void);


void app_set_duanluqi1_name(unsigned char * p_buffer);
void app_set_duanluqi2_name(unsigned char * p_buffer);
void app_set_duanluqi3_name(unsigned char * p_buffer);


unsigned char * app_get_duanluqi1_name(void);
unsigned char * app_get_duanluqi2_name(void);
unsigned char * app_get_duanluqi3_name(void);
	

void app_master_device_set_connection_interval(unsigned short value);
void app_master_device_set_latency(unsigned char value);
void app_master_device_set_supervision_timeout(unsigned short value);
unsigned short app_master_device_get_connection_interval(void);
unsigned char app_master_device_get_latency(void);
unsigned short app_master_device_get_supervision_timeout(void);


#if(DIANBIAO_SLAVE_PARAM_SET_ENABE == 1)			
void app_slave_device_set_connection_interval(unsigned short value);
void app_slave_device_set_latency(unsigned char value);
void app_slave_device_set_supervision_timeout(unsigned short value);
unsigned short app_slave_device_get_connection_interval(void);
unsigned char app_slave_device_get_latency(void);
unsigned short app_slave_device_get_supervision_timeout(void);

#endif


typedef enum {
	 LEVEL_4DB = 0x00,
	 LEVEL_0DB = 0x01,
	 LEVEL_NEG_4DB = 0x02,
	 LEVEL_NEG_8DB = 0x03,
	 LEVEL_NEG_20DB = 0x04,	
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

void app_adv_set_filter_number(unsigned char number);
unsigned char app_adv_get_filter_number(void);

void app_adv_set_filter_mac_address(unsigned char index, unsigned char *p_address);
unsigned char * app_adv_get_filter_mac_address(unsigned char index);

ADV_FILTER_TYPE  app_adv_get_filter_kind(void);
void app_adv_set_filter_rule(ADV_FILTER_TYPE type);

void app_adv_set_filter_mac_kind(unsigned char index, unsigned char kind);
unsigned char app_adv_get_filter_mac_kind(unsigned char index);
unsigned int app_get_pincode_convert_ascii_to_hex(unsigned char *p_ascii, unsigned char length);

#if(DIANBIAO_ADV_CONTENT_SET_ENABLE == 1)

void app_set_ble_adv_content(unsigned char *p_buffer, unsigned char length);
unsigned char app_get_ble_adv_length(void);
unsigned char * app_get_ble_adv_content(void);
#endif

#if(DIANBIAO_BLE_WORK_MODE_SET_ENABLE == 1)
typedef enum {
	 BLE_WORK_MASTER_ONLY,
	 BLE_WORK_SLAVE_ONLY,
	 BLE_WORK_MASTER_SLAVE,
	 BLE_WORK_LOOP_MODE,
	 BLE_WORK_TEST_MODE,	
}BLE_WORK_MODE;

void app_set_ble_work_mode(BLE_WORK_MODE mode);
BLE_WORK_MODE app_get_ble_work_mode(void);

#endif

/////////////////////////////////////////////////////////////////////////////////
	
#endif

#endif
