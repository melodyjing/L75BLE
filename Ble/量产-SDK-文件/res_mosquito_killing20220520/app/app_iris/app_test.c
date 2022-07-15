

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_TEST)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_test.h"                  // Health Thermometer Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "htpt_task.h"               // health thermometer functions
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions

#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "rdw_mcu.h"
#include "cobra_rtc.h"
#if (DISPLAY_SUPPORT)
#include "app_display.h"
#include "display.h"
#endif //DISPLAY_SUPPORT

/*
 * DEFINES
 ****************************************************************************************
 */

/// Initial Temperature Value : 37c
#define APP_HT_TEMP_VALUE_INIT       (3700)
/// Temperature Step
#define APP_HT_TEMP_STEP_INIT        (10)
/// Measurement Interval Value Min
#define APP_HT_MEAS_INTV_MIN         (1)
/// Measurement Interval Value Max
#define APP_HT_MEAS_INTV_MAX         (30)

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// health thermometer application environment structure
struct app_ht_env_tag app_ht_env;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void app_ht_temp_send(void)
{
#if 1
		COBRA_RTC_TIME_DATE_T rtc_time;
		cobra_rtc_get_current_time_date(&rtc_time);
    // Temperature Value
    int32_t value = (int32_t)(app_ht_env.temp_value);

		//printf("temp value = 0x%x\n", value);
    // The value is a float value, set the exponent
    value |= 0xFE000000;

    // Allocate the HTPT_TEMP_SEND_REQ message
    struct htpt_temp_send_req * req = KE_MSG_ALLOC(HTPT_TEMP_SEND_REQ,
                                                    prf_get_task_from_id(TASK_ID_HTPT),
                                                    TASK_ADV_DEVICE1,
                                                    htpt_temp_send_req);

    // Stable => Temperature Measurement Char.
    req->stable_meas = 0x01;

    // Temperature Measurement Value
    req->temp_meas.temp         = value;
    req->temp_meas.time_stamp.year = rtc_time.YEAR + 2000;
		req->temp_meas.time_stamp.month = rtc_time.MONTH;
		req->temp_meas.time_stamp.day = rtc_time.DAY ;
		req->temp_meas.time_stamp.hour = rtc_time.HOUR;
		req->temp_meas.time_stamp.min = rtc_time.MIN ;
		req->temp_meas.time_stamp.sec = rtc_time.SEC ;

		
    req->temp_meas.flags        = HTP_FLAG_CELSIUS | HTP_FLAG_TIME| HTP_FLAG_TYPE;
    req->temp_meas.type         = app_ht_env.temp_meas_type;
		
		

    ke_msg_send(req);
#endif
}

#if (DISPLAY_SUPPORT)
static void app_ht_update_type_string(uint8_t temp_type)
{
    switch (temp_type)
    {
        case 0:
            strcpy(app_ht_env.temp_type_string, "NONE");
            break;
        case 1:
            strcpy(app_ht_env.temp_type_string, "ARMPIT");
            break;
        case 2:
            strcpy(app_ht_env.temp_type_string, "BODY");
            break;
        case 3:
            strcpy(app_ht_env.temp_type_string, "EAR");
            break;
        case 4:
            strcpy(app_ht_env.temp_type_string, "FINGER");
            break;
        case 5:
            strcpy(app_ht_env.temp_type_string, "GASTRO-INT");
            break;
        case 6:
            strcpy(app_ht_env.temp_type_string, "MOUTH");
            break;
        case 7:
            strcpy(app_ht_env.temp_type_string, "RECTUM");
            break;
        case 8:
            strcpy(app_ht_env.temp_type_string, "TOE");
            break;
        case 9:
            strcpy(app_ht_env.temp_type_string, "TYMPANUM");
            break;
        default:
            strcpy(app_ht_env.temp_type_string, "UNKNOWN");
            break;
    }
}
#endif //(DISPLAY_SUPPORT)

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_ht_init(void)
{
    // Reset the environment
    memset(&app_ht_env, 0, sizeof(app_ht_env));

    // Initial measurement interval : 0s
    app_ht_env.htpt_meas_intv   = 1;
    // Initial temperature value : 37.00 C
    app_ht_env.temp_value       = APP_HT_TEMP_VALUE_INIT;
    // Initial temperature step : 0.20 C
    app_ht_env.temp_step        = APP_HT_TEMP_STEP_INIT;
    // Initial temperature type : ARMPIT
    app_ht_env.temp_meas_type   = 1;

    //TODO: Add a state for the module
}

void app_stop_timer (void)
{
    // Stop the timer used for the measurement interval if enabled
    if (app_ht_env.timer_enable)
    {
//        ke_timer_clear(APP_HT_MEAS_INTV_TIMER, TASK_APP);
        app_ht_env.timer_enable = false;
    }
}

void app_ht_add_hts(void)
{
    struct htpt_db_cfg* db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_ADV_DEVICE1,
                                                  gapm_profile_task_add_cmd, sizeof(struct htpt_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_HTPT;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct htpt_db_cfg* ) req->param;
    // All features are supported
    db_cfg->features = HTPT_ALL_FEAT_SUP;

    // Measurement Interval range
    db_cfg->valid_range_min = APP_HT_MEAS_INTV_MIN;
    db_cfg->valid_range_max = APP_HT_MEAS_INTV_MAX;

    // Measurement
    db_cfg->temp_type = app_ht_env.temp_meas_type;
    db_cfg->meas_intv = app_ht_env.htpt_meas_intv;

    // Send the message
    ke_msg_send(req);
		

}

void app_ht_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct htpt_enable_req * req = KE_MSG_ALLOC(HTPT_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_HTPT),
                                                TASK_ADV_DEVICE1,
                                                htpt_enable_req);

    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;

    // Send the message
    ke_msg_send(req);
		
//		ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*100);
//     app_ht_env.timer_enable = true;
		
}

/**
 ****************************************************************************************
 * Health Thermometer Application Functions
 ****************************************************************************************
 */

void app_ht_temp_inc(void)
{
    app_ht_env.temp_value += app_ht_env.temp_step;

    #if (DISPLAY_SUPPORT)
    app_display_update_temp_val_screen(app_ht_env.temp_value);
    #endif //DISPLAY_SUPPORT

    app_ht_temp_send();
}

void app_ht_temp_dec(void)
{
    app_ht_env.temp_value -= app_ht_env.temp_step;

    #if (DISPLAY_SUPPORT)
    app_display_update_temp_val_screen(app_ht_env.temp_value);
    #endif //DISPLAY_SUPPORT

    app_ht_temp_send();
}

void app_ht_temp_type_inc(void)
{
    app_ht_env.temp_meas_type = (uint8_t)(((int)app_ht_env.temp_meas_type + 1)%10);

    #if (DISPLAY_SUPPORT)
    app_ht_update_type_string(app_ht_env.temp_meas_type);
    app_display_update_temp_type_screen(app_ht_env.temp_type_string);
    #endif //DISPLAY_SUPPORT
}

void app_ht_temp_type_dec(void)
{
    if (((int)app_ht_env.temp_meas_type-1) < 0)
    {
        app_ht_env.temp_meas_type = 0x09;
    }
    else
    {
        app_ht_env.temp_meas_type = app_ht_env.temp_meas_type - 1;
    }

    #if DISPLAY_SUPPORT
    app_ht_update_type_string(app_ht_env.temp_meas_type);
    app_display_update_temp_type_screen(app_ht_env.temp_type_string);
    #endif //DISPLAY_SUPPORT
}

/****************************************************************************************
 * MESSAGE HANDLERS
 ****************************************************************************************/

/**
 ****************************************************************************************
 * @brief Handles measurement interval start indication from the Health Thermometer
 *        profile.
 *        Start or stop a timer following the value of the param intv.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int htpt_meas_intv_chg_req_ind_handler(ke_msg_id_t const msgid,
                                          struct htpt_meas_intv_chg_req_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Store the received Measurement Interval value
    app_ht_env.htpt_meas_intv = param->intv;
		//printf("app_ht_env.htpt_meas_intv = %d \n", app_ht_env.htpt_meas_intv);
    // Check the new Measurement Interval Value
    if (app_ht_env.htpt_meas_intv != 0)
    {
        // Check if a Timer already exists
        if (!app_ht_env.timer_enable)
        {
            // Set a Timer
  //          ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*KERNEL_TIME_HZ);
            app_ht_env.timer_enable = true;
        }
        else
        {
            // Clear the previous timer
     //       ke_timer_clear(APP_HT_MEAS_INTV_TIMER, TASK_APP);
            // Create a new timer with the received measurement interval
      //      ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*KERNEL_TIME_HZ);
        }
    }
    else
    {
        // Check if a Timer exists
        if (app_ht_env.timer_enable)
        {
            // Measurement Interval is 0, clear the timer
    //        ke_timer_clear(APP_HT_MEAS_INTV_TIMER, TASK_APP);
            app_ht_env.timer_enable = false;
        }
    }

    // Allocate the message
    struct htpt_meas_intv_chg_cfm * cfm = KE_MSG_ALLOC(HTPT_MEAS_INTV_CHG_CFM,
                                                prf_get_task_from_id(TASK_ID_HTPT),
                                                TASK_ADV_DEVICE1,
                                                htpt_meas_intv_chg_cfm);

    // Set data
    cfm->conidx = KE_IDX_GET(dest_id);
    cfm->status = 0;

    // Send the message
    ke_msg_send(cfm);


    return (KE_MSG_CONSUMED);
}

static int htpt_temp_send_rsp_handler(ke_msg_id_t const msgid,
                                        struct htpt_temp_send_rsp const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Do nothing
//		ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*KERNEL_TIME_HZ);
	//  printf("htpt_temp_send_rsp_handler .... \n");
    return (KE_MSG_CONSUMED);
}

static int htpt_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                        struct htpt_cfg_indntf_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Do nothing
   // printf("ind = %d msgid = %d \n", param->ntf_ind_cfg, msgid);
		if(param->ntf_ind_cfg & 0x01)
		{
			//			ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*KERNEL_TIME_HZ);
            app_ht_env.timer_enable = true;
		}
		else
		{
						// Clear the previous timer
    //        ke_timer_clear(APP_HT_MEAS_INTV_TIMER, TASK_APP);
            app_ht_env.timer_enable = false;
		}
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles health thermometer timer
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_ht_meas_intv_timer_handler(ke_msg_id_t const msgid,
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
    // Increase or decrease the temperature value
    sign = (int8_t)(rand_temp_step & 0x00000001);

    if (!sign)
    {
        sign = -1;
    }

    app_ht_env.temp_value += sign*rand_temp_step;
		
		if(app_ht_env.temp_value < 0) app_ht_env.temp_value = 30;
		if(app_ht_env.temp_value > 250) app_ht_env.temp_value = 250;

    // Send the new temperature
    app_ht_temp_send();

    #if (DISPLAY_SUPPORT)
    app_display_update_temp_val_screen(app_ht_env.temp_value);
    #endif //DISPLAY_SUPPORT

    // Reset the Timer (Measurement Interval is not 0 if we are here)
  //  ke_timer_set(APP_HT_MEAS_INTV_TIMER, TASK_APP, app_ht_env.htpt_meas_intv*100);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles health thermometer timer
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_ht_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_ht_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_ht_msg_handler},

//  {HTPT_ENABLE_RSP,               (ke_msg_func_t)htpt_enable_rsp_handler},
    {HTPT_TEMP_SEND_RSP,            (ke_msg_func_t)htpt_temp_send_rsp_handler},
    {HTPT_MEAS_INTV_CHG_REQ_IND,    (ke_msg_func_t)htpt_meas_intv_chg_req_ind_handler},
    {HTPT_CFG_INDNTF_IND,           (ke_msg_func_t)htpt_cfg_indntf_ind_handler},

//    {APP_HT_MEAS_INTV_TIMER,        (ke_msg_func_t)app_ht_meas_intv_timer_handler},
};

const struct app_subtask_handlers app_ht_handlers = APP_HANDLERS(app_ht);

#endif //BLE_APP_HT

/// @} APP
