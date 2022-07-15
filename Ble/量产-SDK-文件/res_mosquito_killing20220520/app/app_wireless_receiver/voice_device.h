/******************************************************************************
	File Name	:	voice_device.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/6

	Description :   

	Changed Log	:
		Liujian  2020/3/6		- Creation

*****************************************************************************/
#ifndef __VOICE_DEVICE_H__
#define __VOICE_DEVICE_H__
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

#if BLE_APP_RECEIVE_VOICE

enum app_voice_device_connetion_state
{
    /// scan activity does not exists
    APP_VOCIE_DEVICE_STATE_IDLE = 0,
    /// Creating connection activity
    APP_VOCIE_DEVICE_STATE_CREATING,
	
		//
		APP_VOCIE_DEVICE_STATE_CREATED,
    

};


/// connection environment definition
struct voice_device_env_tag
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



void app_voice_device_connection_start(struct gap_bdaddr peer_addr);
void app_voice_device_init(void);
void app_voice_device_config(void);
void app_voice_device_create(void);
void app_voice_device_exchange_mtu(void);

unsigned int app_voice_device_is_connected(void);
void app_voice_device_set_connected(void);

void app_voice_device_reconnect(void);
void app_voice_device_add_profile(void);

//发现对方设备的 服务
void app_voice_device_find_service(void);
//让 语音采集设备 可以上报数据
void app_voice_device_enable_report(unsigned int connection_index);

void app_voice_device_config(void);
#endif


#endif
