/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_hid.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	  2020/8/31

	Description :   HID …Ë±∏
	Changed Log	:

		Liujian  2020/8/31		- Creation
*****************************************************************************/
#ifndef __APP_HID_H__
#define __APP_HID_H__
#include "rwip_config.h"     // SW configuration
#if (BLE_APP_HID_ENABLE)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition

/// HID Application Module Environment Structure
struct app_hid_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Mouse timeout value
    uint16_t timeout;
    /// Internal state of the module
    uint8_t state;
    /// Timer enabled
    bool timer_enabled;
    /// Number of report that can be sent
    uint8_t nb_report;
};

struct ps2_mouse_msg
{
	int16_t x;
	int16_t y;
	
	uint8_t b; // button status
	uint8_t w;
	
};

/// Table of message handlers
extern const struct app_subtask_handlers app_hid_handlers;

/**
 ****************************************************************************************
 * @brief Initialize HID Application Module
 ****************************************************************************************
 */
void app_hid_mouse_init(void);

/**
 ****************************************************************************************
 * @brief Initialize the PS2 mouse driver so that it can be used by the application
 ****************************************************************************************
 */
void app_hid_mouse_start_mouse(void);

/**
 ****************************************************************************************
 * @brief Add a HID Service instance in the DB
 ****************************************************************************************
 */
void app_hid_mouse_add_hids(void);

/**
 ****************************************************************************************
 * @brief Enable the HID Over GATT Profile device role
 *
 * @param[in]:  conhdl - Connection handle for the connection
 ****************************************************************************************
 */
void app_hid_mouse_enable_prf(uint8_t conidx);


uint32_t app_hid_key_state_is_ready(void);

void app_hid_report_key(uint8_t *p_key_pattern, uint16_t pattern_len, uint16_t  pattern_offset);

#endif

#endif
