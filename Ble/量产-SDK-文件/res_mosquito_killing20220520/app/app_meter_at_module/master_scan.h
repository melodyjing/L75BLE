/******************************************************************************
	File Name	:	  master_scan.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/08

	Description : 
      蓝牙模组, 可以通过 AT COMMAND 来控制


	Changed Log	:
	
		Liujian  2021/03/08	 - Creation

*****************************************************************************/
#ifndef __MASTER_SCAN_H__
#define __MASTER_SCAN_H__

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#include <stdint.h>

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


void master_scan_set_device_config(void);

void master_scan_scan_create_activity(void);

void master_scan_start_scan(void);

void master_scan_scan_init(void);

void master_scan_set_state(uint8_t state);

uint8_t master_scan_get_state(void);

unsigned int app_device1_scan_get_global_flag(void);

void  app_device1_scan_set_global_flag(DEVICE_INDEX index);

void  app_device1_scan_clear_global_flag(DEVICE_INDEX index);

void app_set_connect_device1_address(unsigned char * p_buffer);

uint32_t app_device_compare_duanluqi1_address(uint8_t * source_addr);

void app_send_data_to_device(uint8_t channel, uint8_t length, uint8_t * p_send_buffer);
void app_disconnection_device(uint8_t channel);

unsigned int  app_device_continue_scan_duanluqi_address1(void);
unsigned int app_device_connect_to_duanluqi_1(void);
void app_device_connect_to_duanluqi_1_enable(unsigned int enable);
void app_device_enable_scan_duanluqi1_address(unsigned int scan_enable);
void app_device_set_duanluqi_1_connected_flag(unsigned int connected_flag);
unsigned int app_get_duanluqi_1_connected_flag(void);
unsigned int app_device_connect_to_duanluqi_1(void);

void app_device_show_connect_info(void);


#endif
#endif
