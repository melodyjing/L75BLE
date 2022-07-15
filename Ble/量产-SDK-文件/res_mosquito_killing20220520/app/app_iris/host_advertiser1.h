/******************************************************************************
	File Name	:	host_advertiser1.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/20

	Description :   
				shanhai 2019/12/8
	Changed Log	:
		Liujian  2019/11/20			- Creation

*****************************************************************************/
#ifndef __HOST_ADVERTISER1_H__
#define __HOST_ADVERTISER1_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "rwip_task.h"      // Task definitions
#include "ke_task.h"        // Kernel Task
#include <cobra_config.h>
#include "smpc.h"
#include "gapc.h"
#include "host_advertiser_task1.h"

#if APP_HOST2_ENABLE

/// Maximal length of the Device Name value
#define APP_DEVICE1_NAME_MAX_LEN      (18)

struct adv_env_tag
{
    /// Connection handle
    uint16_t conhdl;
    /// Connection Index
    uint8_t  conidx;

    /// Advertising activity index
    uint8_t adv_actv_idx;
    /// Current advertising state (@see enum app_adv_state)
    uint8_t adv_state;
    /// Next expected operation completed event
    uint8_t adv_op;

    /// Last initialized profile
    uint8_t next_svc;
	
	  /// Device Name length
    uint8_t dev_name_len;
    /// Device Name
    uint8_t dev_name[APP_DEVICE1_NAME_MAX_LEN];

		/// Local device IRK
    uint8_t loc_irk[KEY_LEN];

    /// Secure Connections on current link
    bool sec_con_enabled;
	
		/// Counter used to generate IRK
    uint8_t rand_cnt;
		
		struct bd_addr peer_addr;
		
		uint8_t adv_ok;            //ÊÇ·ñÔÚ¹ã²¥
};

enum app_device1_adv_state
{
    /// Advertising activity does not exists
    APP_DEVICE1_ADV_STATE_IDLE = 0,
    /// Creating advertising activity
    APP_DEVICE1_ADV_STATE_CREATING,
    /// Setting advertising data
    APP_DEVICE1_ADV_STATE_SETTING_ADV_DATA,
    /// Setting scan response data
    APP_DEVICE1_ADV_STATE_SETTING_SCAN_RSP_DATA,

    /// Advertising activity created
    APP_DEVICE1_ADV_STATE_CREATED,
    /// Starting advertising activity
    APP_DEVICE1_ADV_STATE_STARTING,
    /// Advertising activity started
    APP_DEVICE1_ADV_STATE_STARTED,
    /// Stopping advertising activity
    APP_DEVICE1_ADV_STATE_STOPPING,

};


/// Structure containing information about the handlers for an application subtask
struct app_device1_subtask_handlers
{
    /// Pointer to the message handler table
    const struct ke_msg_handler *p_msg_handler_tab;
    /// Number of messages handled
    uint16_t msg_cnt;
};

//#define APP_DEV2_ADV_INT_MAX						(64)
//#define APP_DEV1_ADV_INT_MIN						(64)
//#define APP_DEVICE1_ADV_CHMAP           (0x07)

void app_host2_init(void);


void app_host2_reset(void);

void app_device1_adv_fsm_next(void);


uint8_t app_device1_get_dev_name(uint8_t* name);


void host2_appm_update_param(struct gapc_conn_param *conn_param, ke_task_id_t app_task);

unsigned int app_collection_1_is_ready(void);

unsigned int app_device_compare_collection1_address(unsigned char * source_addr);

void app_get_collection1_address(unsigned char * p_buffer);

void app_collection_1_ready(void);

void app_iris_collection1_address(struct bd_addr * p_collection_address);

void app_collection_1_not_ready(void);

void app_host2_begin_work(void);

void app_collection1_disconnect(void);
#endif

#endif

