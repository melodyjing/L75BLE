/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_app_ble_services.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/18

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/18			- Creation
*****************************************************************************/

#include "rwip_config.h" // SW configuration

#if(TUYA_BLE_SDK_ENABLE == 1)

#include "tuya_ble_api.h"
#include "tuya_app_ble_services.h"
#include "app.h"
#include "co_utils.h"
#include "prf.h"
#include "prf_types.h"


struct app_tuya_ble_env_tag   app_tuya_ble_env;


void app_tuya_ble_init(void)
{
    // Reset the environment
    memset(&app_tuya_ble_env, 0, sizeof(app_tuya_ble_env));
}

void app_tuya_ble_add_services(void)
{
    struct tuya_ble_services_db_cfg *db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM, TASK_APP,
                                                             gapm_profile_task_add_cmd, sizeof(struct tuya_ble_services_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_TUYA_BLE;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct tuya_ble_services_db_cfg *)req->param;
    // All features are supported
    db_cfg->features = TUYA_BLE_ALL_FEAT_SUP;

//    db_cfg->meas_intv = app_uart_env.uart_meas_intv;

    // Send the message
    ke_msg_send(req);
}


void app_tuya_ble_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct tuya_ble_enable_req *req = KE_MSG_ALLOC(TUYA_BLE_ENABLE_REQ,
                                               prf_get_task_from_id(TASK_ID_TUYA_BLE),
                                               TASK_APP,
                                               tuya_ble_enable_req);

    // Fill in the parameter structure
    req->conidx = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg = PRF_CLI_STOP_NTFIND;

    // Send the message
    ke_msg_send(req);
}

void app_tuya_ble_send_mtu(const uint8_t * data, uint16_t length)
{
    // Allocate the message
    struct tuya_ble_data_send_req *req = KE_MSG_ALLOC(TUYA_BLE_DATA_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_TUYA_BLE),
                                                  TASK_APP,
                                                  tuya_ble_data_send_req);

    memcpy(req->tx_data, data, length);

    req->length = length;

    // Send the message
    ke_msg_send(req);

}



static int app_tuya_ble_send_data_rsp_handler(ke_msg_id_t const msgid,
                                      struct tuya_ble_send_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}


static int app_tuya_ble_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                       struct tuya_ble_cfg_indntf_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{

    if (param->ntf_ind_cfg)
    {
        //profile_uart_read(app_uart_env.rx_data, 16, (profile_callback)&send_to_phone, &app_uart_env);
    }
    else
    {
        //profile_uart_read(app_uart_env.rx_data, 16, NULL, &app_uart_env);
    }
    return (KE_MSG_CONSUMED);
}


static int app_tuya_ble_msg_handler(ke_msg_id_t const msgid,
                                void const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}


static int app_tuya_ble_rx_handler(ke_msg_id_t const msgid,
                               struct tuya_ble_rx_data const *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id)
{
	  tuya_ble_gatt_receive_data((uint8_t*)(param->rx_data),param->length);
    return (KE_MSG_CONSUMED);
}


const struct ke_msg_handler app_tuya_ble_msg_handler_list[] =
{
        // Note: first message is latest message checked by kernel so default is put on top.
        {KE_MSG_DEFAULT_HANDLER,   (ke_msg_func_t)app_tuya_ble_msg_handler},

        {TUYA_BLE_DATA_SEND_RSP,    (ke_msg_func_t)app_tuya_ble_send_data_rsp_handler},

        {TUYA_BLE_CFG_INDNTF_IND,      (ke_msg_func_t)app_tuya_ble_cfg_indntf_ind_handler},

        {TUYA_BLE_RX_DATA_IND,         (ke_msg_func_t)app_tuya_ble_rx_handler},
};


const struct app_subtask_handlers app_tuya_ble_handlers = APP_HANDLERS(app_tuya_ble);

#endif

