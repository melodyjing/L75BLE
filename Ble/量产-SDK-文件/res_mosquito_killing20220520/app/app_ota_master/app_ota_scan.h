/******************************************************************************
	File Name	:	app_ota_scan.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/26

	Description :   
	    建立扫描任务

	Changed Log	:
		Liujian  2020/8/26		- Creation

*****************************************************************************/

#ifndef __APP_OTA_SCAN_H__
#define __APP_OTA_SCAN_H__

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
#include "rdw_mcu.h"

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))



#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


/// Gap Advertising Flags
#define GAP_ADTYPE_FLAGS                        0x01
/// Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_MORE                   0x02
/// Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_16BIT_COMPLETE               0x03


enum app_ota_scan_state
{
    /// scan activity does not exists
    APP_OTA_SCAN_STATE_IDLE = 0,
    /// Creating scan activity
    APP_OTA_SCAN_STATE_CREATING,
    

};

/// Scanning environment definition
struct ota_scan_device_env_tag
{
    ///Scan window interval (in slots)
    uint16_t                 interval;
    /// Status of the scanning
    uint8_t                  scan_state;
    /// Scanning activity identifier
    uint8_t                  act_id;
};

struct  ota_scan_data {
	   struct  gap_bdaddr bd_address;
	   int rssi;
};

extern struct  ota_scan_data   ota_device[];

void app_ota_scan_send_gapm_reset_cmd(void);
void app_ota_scan_init(void);
void app_ota_scan_set_device_config(void);
void app_ota_scan_start_scan(void);
void app_ota_scan_create_activity(void);

bool app_ota_scan_find_svc_uuid(uint16_t uuid, uint8_t *pData, uint16_t dataLen);

void app_ota_scan_stop_scan(void);

void app_ota_process_avd_data(struct gapm_ext_adv_report_ind* p_param);

unsigned char app_ota_get_ota_device_number(void);

void app_ota_scan_again(void);

#endif

#endif
