/******************************************************************************
	File Name	:	host_advertiser_task1.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/20

	Description :   
				liujian 2019/12/8 shanghai
	Changed Log	:
		Liujian  2019/11/20			- Creation

*****************************************************************************/

#ifndef __HOST_ADVERTISER_TASK1_H__
#define __HOST_ADVERTISER_TASK1_H__

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
#include "host_advertiser1.h"
#if APP_HOST2_ENABLE






/// States of device1 task
enum app_device1_state
{
    /// Initialization state
    APP_DEVICE1_INIT,
    /// Database create state
    APP_DEVICE1_CREATE_DB,
    /// Ready State
    APP_DEVICE1_READY,
    /// Connected state
    APP_DEVICE1_CONNECTED,

    /// Number of defined states.
    APP_DEVICE1_STATE_MAX
};




#define APP_DEVICE1_IDX_MAX                 (1)

#endif
#endif
