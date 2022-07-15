/**@file            app_blood.c
* @brief            Blood Application entry point
* @details
* @author
* @date             2020/8/14
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/8/14   <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_BLOOD)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_blood.h"
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "blps_task.h"               // health thermometer functions
#include "co_bt.h"
#include "co_utils.h"
#include "prf_types.h"               // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include <string.h>
#include "ke_timer.h"     // kernel timer defines

#if (COBRA_POWER_DOWN_ENABLE)
struct app_blood_env_tag  app_blood_env		__attribute__((section("retention_area"),zero_init));
#else
struct app_blood_env_tag  app_blood_env;
#endif



void app_blood_init(void)
{
    // Reset the environment
    memset(&app_blood_env, 0, sizeof(app_blood_env));
    app_blood_env.measure_time = 1; //1s
    app_blood_env.timer_enable = false;
    app_blood_env.year = 2018;
    app_blood_env.day = 10;
    app_blood_env.month = 4;
    app_blood_env.hour = 16;
    app_blood_env.min = 1;
    app_blood_env.sec = 0;

}


void app_blood_add_server(void)
{
    struct blps_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                            TASK_GAPM, TASK_APP,
                                            gapm_profile_task_add_cmd, sizeof(struct blps_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_BLPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct blps_db_cfg* ) req->param;

    db_cfg->features = BLPS_INTM_CUFF_PRESS_SUP;
    db_cfg->prfl_cfg = BLPS_INTM_CUFF_PRESS_SUP;

    // Send the message
    ke_msg_send(req);

}


void app_blood_enable_prf(uint8_t conidx)
{
    // Allocate the message

    app_blood_env.conidx = conidx;

    struct blps_enable_req * req = KE_MSG_ALLOC(BLPS_ENABLE_REQ,
                                   prf_get_task_from_id(TASK_ID_BLPS),
                                   TASK_APP,
                                   blps_enable_req);

    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->bp_meas_ind_en   = PRF_CLI_STOP_NTFIND;

    req->interm_cp_ntf_en = PRF_CLI_STOP_NTFIND;



    // Send the message
    ke_msg_send(req);

}


static int app_blood_msg_handler(ke_msg_id_t const msgid,
                                 void const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}


void app_blood_stop_timer (void)
{
    // Stop the timer used for the measurement interval if enabled
    if (app_blood_env.timer_enable)
    {
        ke_timer_clear(APP_BLOOD_MEAS_INTV_TIMER, TASK_APP);
        app_blood_env.timer_enable = false;
    }
}

static int app_blood_indntf_handler(ke_msg_id_t const msgid,
                                    struct blps_cfg_indntf_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Do nothing
    //struct blps_cfg_indntf_ind *p =

    app_blood_env.conidx = param->conidx;

    if(param->char_code & BPS_BP_MEAS_CODE)
    {
        if(param->cfg_val)
        {
            ke_timer_set(APP_BLOOD_MEAS_INTV_TIMER, TASK_APP, app_blood_env.measure_time*100);
            app_blood_env.timer_enable = true;
        }
        else
        {
            app_blood_stop_timer();
        }
    }
    else if(param->char_code & BPS_INTERM_CP_CODE)
    {
        if(param->cfg_val)
        {
            ke_timer_set(APP_BLOOD_MEAS_INTV_TIMER, TASK_APP, app_blood_env.measure_time*100);
            app_blood_env.timer_enable = true;
        }
        else
        {
            app_blood_stop_timer();
        }
    }

    return (KE_MSG_CONSUMED);
}


static void app_blood_value_send(void)
{

    // uint16_t value = (uint16_t)(app_blood_env.temp_value);

    // The value is a float value, set the exponent
    // value &= 0x0eff;

    // Allocate the BLPS_MEAS_SEND_REQ message
    struct blps_meas_send_req * req = KE_MSG_ALLOC(BLPS_MEAS_SEND_REQ,
                                      prf_get_task_from_id(TASK_ID_BLPS),
                                      TASK_APP,
                                      blps_meas_send_req);

    //req->conidx = app_blood_env.conidx;

    req->flag_interm_cp = 0x00;
    req->conidx = app_blood_env.conidx;
    req->meas_val.flags = 0xff; //BPS_FLAG_MEAS_STATUS_PRESENT|BPS_FLAG_KPA|BPS_FLAG_PULSE_RATE_PRESENT|BPS_FLAG_TIME_STAMP_PRESENT|BPS_FLAG_USER_ID_PRESENT; //BPS_FLAG_KPA|BPS_FLAG_PULSE_RATE_PRESENT;
    req->meas_val.user_id = 0x01;
    req->meas_val.systolic = app_blood_env.systolic;
    req->meas_val.diastolic = app_blood_env.diastolic;
    req->meas_val.mean_arterial_pressure = app_blood_env.mean_arterial_pressure;

    req->meas_val.meas_status = 0x3f;

    req->meas_val.pulse_rate = app_blood_env.pulse_rate;



    req->meas_val.time_stamp.year = app_blood_env.year;
    req->meas_val.time_stamp.month = app_blood_env.month;
    req->meas_val.time_stamp.day = app_blood_env.day;
    req->meas_val.time_stamp.hour = app_blood_env.hour;
    req->meas_val.time_stamp.min = app_blood_env.min;
    req->meas_val.time_stamp.sec = app_blood_env.sec;




    ke_msg_send(req);

}

static int app_blood_meas_intv_timer_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

    uint32_t rand_temp_step;

    int8_t sign;

    // Generate temperature step
    rand_temp_step = (uint32_t)(co_rand_word()%20);

    sign = (int8_t)(rand_temp_step & 0x00000001);

    if (!sign)
    {
        sign = -1;
    }

    app_blood_env.systolic += sign*rand_temp_step;

    app_blood_env.systolic &= 0x00ff;

    rand_temp_step = (uint32_t)(co_rand_word()%20);
    app_blood_env.diastolic += sign*rand_temp_step;
    app_blood_env.diastolic &= 0x00ff;

    rand_temp_step = (uint32_t)(co_rand_word()%20);
    app_blood_env.mean_arterial_pressure += sign*rand_temp_step;
    app_blood_env.mean_arterial_pressure &= 0x00ff;

    rand_temp_step = (uint32_t)(co_rand_word()%20);
    app_blood_env.pulse_rate += sign*rand_temp_step;
    app_blood_env.pulse_rate  &= 0x00ff;

    app_blood_env.sec++;
    if(app_blood_env.sec >= 60)
    {
        app_blood_env.sec = 0;
        app_blood_env.min++;
        if(app_blood_env.min >= 60)
        {
            app_blood_env.min = 0;
            app_blood_env.hour++;
            if(app_blood_env.hour >= 24)
            {
                app_blood_env.hour = 0;
            }
        }
    }







    app_blood_value_send();


    ke_timer_set(APP_BLOOD_MEAS_INTV_TIMER, TASK_APP, app_blood_env.measure_time*100);

    return (KE_MSG_CONSUMED);

}

void app_blood_disable_profile (void)
{

}

/// Default State handlers definition
const struct ke_msg_handler app_blood_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_blood_msg_handler},
    {BLPS_ENABLE_RSP, 				      (ke_msg_func_t)app_blood_msg_handler},
    {BLPS_CFG_INDNTF_IND,           (ke_msg_func_t)app_blood_indntf_handler},

    {APP_BLOOD_MEAS_INTV_TIMER,        (ke_msg_func_t)app_blood_meas_intv_timer_handler},

};

const struct app_subtask_handlers app_blood_handlers = APP_HANDLERS(app_blood);




#endif //BLE_APP_BLOOD

/// @} APP
