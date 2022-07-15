/******************************************************************************
	File Name	  :	  ota_device.h
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/26

	Description :  
    用于和OTA 设备建立连接	

	Changed Log	:
		Liujian  2020/08/26	   - Creation

*****************************************************************************/
#ifndef __OTA_DEVICE_H__
#define __OTA_DEVICE_H__
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
#include "gattc_task.h"
#include "prf.h"
#include "rdw_mcu.h"
#include "ota_master_task.h"

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))

enum app_ota_device_connetion_state
{
    /// scan activity does not exists
    APP_OTA_DEVICE_STATE_IDLE = 0,
    /// Creating connection activity
    APP_OTA_DEVICE_STATE_CREATING,
	
		//
		APP_OTA_DEVICE_STATE_CREATED,
    

};


/// connection environment definition
struct ota_device_env_tag
{
    ///connection window interval (in slots)
    uint16_t                 interval;
    /// Status of the connection
    uint8_t                  connection_state;
    /// connection activity identifier
    uint8_t                  act_id;
 
		/// Connection Index
    uint8_t  								conidx;
		/// Connection handle
    uint16_t 								conhdl;
	
};

#define    SUOTAR_REBOOT         0xFD
#define    SUOTAR_IMG_END        0xFE


void app_ota_device_init(void);

unsigned int app_ota_device_is_connected(void);

void app_ota_device_set_connected(void);

void app_ota_device_config(void);

void app_ota_device_create(void);

void app_ota_device_exchange_mtu(void);

void app_ota_device_add_profile(void);

void app_ota_device_find_service(void);

void app_ota_device_enable_report(unsigned int connection_index);

void app_ota_device_connection_start(struct gap_bdaddr peer_addr);

void app_ota_device_reconnect(void);


//设置OTA 设备的存储信息
void app_ota_device_set_memory_information(void);

void app_ota_device_set_state(unsigned char state);

unsigned char app_ota_device_ready(void);

void app_ota_device_patch_data(unsigned char *p_buffer, uint16_t length);

void app_ota_device_patch_data_send_ok(void);

unsigned char app_ota_device_patch_data_send_state(void);


void app_ota_device_ota_cmd(unsigned int cmd);

void app_ota_device_patch_len(uint16_t len );


unsigned int app_ota_device_comp_addr(struct gap_bdaddr peer_addr);

void app_ota_set_connect_addr(struct gap_bdaddr addr);

void app_ota_device_disconnect(void);

#endif


#endif