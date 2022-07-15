/**@file            app_heart_rate.c
* @brief            Heart Rate Application entry point
* @details
* @author
* @date             2019/6/23
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/6/23  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/
#include "app_heart_rate.h"
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions

#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions

#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "rdw_mcu.h"

#if (BLE_APP_HEART_RATE)

#include "hrps_task.h"               // Heart Rate profile functions
#if (COBRA_POWER_DOWN_ENABLE)
static struct app_hrpt_env_tag app_hrt_env		__attribute__((section("retention_area"),zero_init));
#else
static struct app_hrpt_env_tag app_hrt_env;
#endif

/*
   初始化Profile 函数
	 这个函数一般在 appm_init() 函数中调用

*/
void app_heart_rate_init(void)
{
    // Reset the environment
    memset(&app_hrt_env, 0, sizeof(app_hrt_env));

    app_hrt_env.body_sensor_loc   = HRS_LOC_OTHER;

    app_hrt_env.energy_expended = 0;

    app_hrt_env.heart_rate = 60;

    app_hrt_env.hrp_meas_intv = 10;

}

/*
 这个函数是将 Heart Rate 心率计的 profile 添加到应用中去

 把这个函数 放到下面这个数组中, 系统会在适当的时候调用这个函数, 从而
 把 Profile 添加到应用中

 static const appm_add_svc_func_t appm_add_svc_func_list[APPM_SVC_LIST_STOP]
*/
void app_heart_rate_add_hprs(void)
{
    struct hrps_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                            TASK_GAPM, TASK_APP,
                                            gapm_profile_task_add_cmd, sizeof(struct hrps_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_HRPS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct hrps_db_cfg* )req->param;
    db_cfg->features = HRS_F_BODY_SENSOR_LOCATION_SUPPORTED|HRS_F_ENERGY_EXPENDED_SUPPORTED;
    db_cfg->body_sensor_loc = HRS_LOC_FINGER;               //TBD

    // Send the message
    ke_msg_send(req);

}

/*
 这个函数是  使能 Heart rate profile 　.
 conidx  是当手机或其他主设备连接本外设也建立起来的 连接 index.
 该函数建议在  下面这个函数中调用， 下面这个函数是建立连接后的处理函数，
  所以建议在此函数中 使能 Profile
static int gapc_connection_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)

*/
void app_heart_rate_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct hrps_enable_req * req = KE_MSG_ALLOC(HRPS_ENABLE_REQ,
                                   prf_get_task_from_id(TASK_ID_HRPS),
                                   TASK_APP,
                                   hrps_enable_req);

    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->hr_meas_ntf   = PRF_CLI_STOP_NTFIND;

    // Send the message
    ke_msg_send(req);
}

/*
 外设发送心律数据到 手机或其他主设备
*/
static void app_heart_rate_send_data(void)
{
    // Allocate the HRPS_MEAS_SEND_REQ message
    struct hrps_meas_send_req * req = KE_MSG_ALLOC(HRPS_MEAS_SEND_REQ,
                                      prf_get_task_from_id(TASK_ID_HRPS),
                                      TASK_APP,
                                      hrps_meas_send_req);

    req->meas_val.flags = HRS_FLAG_HR_16BITS_VALUE|HRS_FLAG_ENERGY_EXPENDED_PRESENT;

    req->meas_val.heart_rate = app_hrt_env.heart_rate;

    req->meas_val.energy_expended = app_hrt_env.energy_expended;


    ke_msg_send(req);
}



void app_heart_rate_stop_timer(void)
{
    // Stop the timer used for the measurement interval if enabled
    if (app_hrt_env.timer_enable)
    {
        ke_timer_clear(APP_HRP_MEAS_INTV_TIMER, TASK_APP);
        app_hrt_env.timer_enable = false;
    }
}


static int app_heart_rate_msg_handler(ke_msg_id_t const msgid,
                                      void const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

static int app_heart_rate_meas_send_rsp_handler(ke_msg_id_t const msgid,
        struct hrps_meas_send_rsp const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}


static int app_heart_rate_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
        struct hrps_cfg_indntf_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Do nothing
    app_hrt_env.cfg_val = param->cfg_val;

    if(app_hrt_env.cfg_val != 0)
    {
        if (!app_hrt_env.timer_enable)
        {
            // Set a Timer
            ke_timer_set(APP_HRP_MEAS_INTV_TIMER, TASK_APP, app_hrt_env.hrp_meas_intv*KERNEL_TIME_HZ);
            app_hrt_env.timer_enable = true;
        }
        else
        {
            // Clear the previous timer
            ke_timer_clear(APP_HRP_MEAS_INTV_TIMER, TASK_APP);
            // Create a new timer with the received measurement interval
            ke_timer_set(APP_HRP_MEAS_INTV_TIMER, TASK_APP, app_hrt_env.hrp_meas_intv*KERNEL_TIME_HZ);
        }

    }
    else
    {
        if (app_hrt_env.timer_enable)
        {

            ke_timer_clear(APP_HRP_MEAS_INTV_TIMER, TASK_APP);
            app_hrt_env.timer_enable= false;
        }

    }

    return (KE_MSG_CONSUMED);
}


static int app_heart_rate_energy_exp_reset_ind_handler(ke_msg_id_t const msgid,
        struct hrps_energy_exp_reset_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Do nothing

    app_hrt_env.energy_expended = 0;

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles Heart Rate timer
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_heart_rate_meas_intv_timer_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Random generation of a temperature value
    uint32_t rand_temp_step;
    // Sign used to know if the temperature will be increased or decreased
    int8_t sign;

    // Generate temperature step
    rand_temp_step = (uint32_t)(co_rand_word()%20);

    rand_temp_step %= 80;
    // Increase or decrease the temperature value
    sign = (int8_t)(rand_temp_step & 0x00000001);

    if (!sign)
    {
        sign = -1;
    }

    //app_hrt_env.heart_rate += sign*rand_temp_step;
    app_hrt_env.heart_rate += 10;
    if(app_hrt_env.heart_rate >= 280) app_hrt_env.heart_rate = 60;

    // Send the new heart rate
    app_heart_rate_send_data();


    // Reset the Timer (Measurement Interval is not 0 if we are here)
    ke_timer_set(APP_HRP_MEAS_INTV_TIMER, TASK_APP, app_hrt_env.hrp_meas_intv*KERNEL_TIME_HZ);

    return (KE_MSG_CONSUMED);
}

//  下面都是标准写法， 对应有什么消息， 然后用哪个函数来处理
/// Default State handlers definition
const struct ke_msg_handler app_heart_rate_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_heart_rate_msg_handler},

    {HRPS_MEAS_SEND_RSP,            (ke_msg_func_t)app_heart_rate_meas_send_rsp_handler},

    {HRPS_CFG_INDNTF_IND, 			    (ke_msg_func_t)app_heart_rate_cfg_indntf_ind_handler},

    {HRPS_ENERGY_EXP_RESET_IND, 	  (ke_msg_func_t)app_heart_rate_energy_exp_reset_ind_handler},

    {APP_HRP_MEAS_INTV_TIMER,       (ke_msg_func_t)app_heart_rate_meas_intv_timer_handler},

};

const struct app_subtask_handlers app_heart_rate_handlers = APP_HANDLERS(app_heart_rate);


#endif
