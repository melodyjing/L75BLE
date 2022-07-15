/******************************************************************************
	File Name	:	ota_master.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/26

	Description :   
	    用于发现 OTA 设备, 这个是 Master, 用来连接 OTA  设备

	Changed Log	:
		Liujian  2020/8/26		- Creation

*****************************************************************************/
#ifndef __OTA_MASTER_H__
#define __OTA_MASTER_H__
#include "rwip_config.h"
#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "rwip_config.h"
#include "gap.h"
#include "prf.h"
#include "ke_mem.h"
#include "co_utils.h"
#include "rdw_mcu.h"
#include "ota_master_task.h"


#if (APP_OTA_MATSER_ENABLE == 1)

#define OTA_COUNT_IDX_MAX  1


/// Possible states of the OTA master task
enum
{
    /// Free state
    OTA_MASTER_FREE,
    /// IDLE state
    OTA_MASTER_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    OTA_MASTER_DISCOVERING,
    /// Busy state
    OTA_MASTER_BUSY,

    /// Number of defined states.
    OTA_MASTER_STATE_MAX
};

struct ota_master_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct ota_master_cnx_env* env[OTA_COUNT_IDX_MAX];
    /// State of different task instances
    ke_state_t state[OTA_COUNT_IDX_MAX];
};


/// Parameters of the @ref OTA_MASTER_CMP_EVT message
struct ota_cmp_evt
{
    /// Operation code
    uint8_t operation;
    /// Status
    uint8_t status;
};

void ota_master_enable_rsp_send(struct ota_master_env_tag *ota_master_env, uint8_t conidx, uint8_t status);

void  ota_send_cmp_evt(struct ota_master_env_tag *ota_master_env, uint8_t conidx, uint8_t operation, uint8_t status);

const struct prf_task_cbs* ota_master_prf_itf_get(void);

#endif


#endif
