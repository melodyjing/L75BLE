/**
 ****************************************************************************************
 *
 * @file app_batt.c
 *
 * @brief Battery Application Module entry point
 *
 * Copyright (C) Radiawave 2017-2021
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_BATT_NO_HID)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_batt.h"                // Battery Application Module Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "bass_task.h"               // health thermometer functions
#include "co_bt.h"
#include "co_utils.h"
#include "prf_types.h"               // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include <string.h>

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Battery Application Module Environment Structure
#if (COBRA_POWER_DOWN_ENABLE)
struct app_batt_env_tag app_batt_env		__attribute__((section("retention_area"),zero_init));
#else
struct app_batt_env_tag app_batt_env;
#endif

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_batt_init(void)
{
    // Reset the environment
    memset(&app_batt_env, 0, sizeof(struct app_batt_env_tag));

    // Initial battery level: 100
    app_batt_env.batt_lvl = 100;
}

void app_batt_add_bas(void)
{
    struct bass_db_cfg* db_cfg;
    // Allocate the BASS_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                            TASK_GAPM, TASK_APP,
                                            gapm_profile_task_add_cmd, sizeof(struct bass_db_cfg));
    // Fill message
    req->operation   = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl     = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_BASS;
    req->app_task    = TASK_APP;
    req->start_hdl   = 0;

    // Set parameters
    db_cfg = (struct bass_db_cfg* ) req->param;

    // Add a BAS instance
    db_cfg->bas_nb      = 1;
    // Sending of notifications is supported
    db_cfg->features[0] = BAS_BATT_LVL_NTF_SUP;

    // Send the message
    ke_msg_send(req);
}

void app_batt_enable_prf(uint8_t conidx)
{
    app_batt_env.conidx = conidx;

    // Allocate the message
    struct bass_enable_req * req = KE_MSG_ALLOC(BASS_ENABLE_REQ,
                                   prf_get_task_from_id(TASK_ID_BASS),
                                   TASK_APP,
                                   bass_enable_req);

    // Fill in the parameter structure
    req->conidx             = conidx;

    // NTF initial status - Disabled
    req->ntf_cfg           = PRF_CLI_STOP_NTFIND;
    req->old_batt_lvl[0]   = 100;

    // Send the message
    ke_msg_send(req);

    printf("app_batt_enable_prf ..... \n");

//		ke_timer_set(APP_BATT_MEAS_INTV_TIMER, TASK_APP, 30*KERNEL_TIME_HZ);
//    app_batt_env.timer_enable = 1;

}

void app_batt_send_lvl(uint8_t batt_lvl)
{
    ASSERT_ERR(batt_lvl <= BAS_BATTERY_LVL_MAX);

    // Allocate the message
    struct bass_batt_level_upd_req * req = KE_MSG_ALLOC(BASS_BATT_LEVEL_UPD_REQ,
                                           prf_get_task_from_id(TASK_ID_BASS),
                                           TASK_APP,
                                           bass_batt_level_upd_req);

    // Fill in the parameter structure
    req->bas_instance = 0;
    req->batt_level   = batt_lvl;

    // Send the message
    ke_msg_send(req);
}

void app_batt_stop_timer(void)
{
    if(app_batt_env.timer_enable)
    {
        ke_timer_clear(APP_BATT_MEAS_INTV_TIMER, TASK_APP);
        app_batt_env.timer_enable = 0;
    }
}

static int bass_batt_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
        struct bass_batt_level_ntf_cfg_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    if(param->ntf_cfg)
    {
        //	  ke_timer_set(APP_BATT_MEAS_INTV_TIMER, TASK_APP, 30*KERNEL_TIME_HZ);
        app_batt_env.timer_enable = 1;
    }
    else
    {
        //		  ke_timer_clear(APP_BATT_MEAS_INTV_TIMER, TASK_APP);
        app_batt_env.timer_enable = 0;
    }
    return (KE_MSG_CONSUMED);
}

static int batt_level_upd_handler(ke_msg_id_t const msgid,
                                  struct bass_batt_level_upd_rsp const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}


static int bass_enable_rsp_handler(ke_msg_id_t const msgid,
                                   struct bass_enable_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_batt_msg_dflt_handler(ke_msg_id_t const msgid,
                                     void const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // Drop the message

    return (KE_MSG_CONSUMED);
}

static int app_batt_meas_timer_handler(ke_msg_id_t const msgid,
                                       void const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    static uint8_t level;
    level += 10;
    level %= 100;
    level = 90;
    app_batt_send_lvl(level);
    ke_timer_set(APP_BATT_MEAS_INTV_TIMER, TASK_APP, 10*KERNEL_TIME_HZ);

}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_batt_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_batt_msg_dflt_handler},

    {BASS_BATT_LEVEL_NTF_CFG_IND,   (ke_msg_func_t)bass_batt_level_ntf_cfg_ind_handler},
    {BASS_BATT_LEVEL_UPD_RSP,       (ke_msg_func_t)batt_level_upd_handler},
    {APP_BATT_MEAS_INTV_TIMER,      (ke_msg_func_t)app_batt_meas_timer_handler},
};

const struct app_subtask_handlers app_batt_handlers = APP_HANDLERS(app_batt);

#endif //BLE_APP_BATT_NO_HID

/// @} APP
