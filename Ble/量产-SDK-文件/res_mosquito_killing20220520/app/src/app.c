/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RADIAWAVE  2017-2020
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)

#include <string.h>
#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "gattc_task.h"
#include "co_bt.h"                   // Common BT Definition
#include "co_math.h"                 // Common Maths Definition

#if(APP_DWT_MASTER == 1)
#include "master_scan.h"
#include "slave_device.h"
#endif

#if(DIANBIAO_MODULE_ENABLE == 1)
#include "app_host_uart.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "app_master.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "slave_device1_connection.h"
#endif

#if (BLE_APP_RECEIVE_VOICE == 1)
#include "sound_scan.h"
#include "voice_device.h"
#endif

#if (BLE_APP_UART)
#include "app_new_uart.h"
#endif

#if (BLE_APP_BLOOD)
#include "app_blood.h"
#endif

#if (BLE_APP_HEART_RATE)
#include "app_heart_rate.h"
#endif

#if (BLE_APP_RDW_VOICE)
#include "app_rdw_voice.h"
#endif

#if (BLE_APP_BATT_NO_HID)
#include "app_batt.h" // Battery Module Definition
#include "bass_task.h"
#endif //(BLE_APP_BATT_NO_HID)

#if (BLE_APP_SEC)
#include "app_sec.h"                 // Application security Definition
#endif // (BLE_APP_SEC)

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
#include "app_host1.h"
#endif

#if (BLE_APP_MESH)

#if (BLE_APP_MESH_TIANMAO_LIGHT == 1)
#include "app_mesh_task.h"
#endif

#if (BLE_APP_MESH_CLIENT_LIGHT == 1)
#include "app_mesh_client_light.h"
#endif

#if (BLE_APP_MESH_SERVER_LIGHT == 1)
#include "app_mesh_server_light.h"
#endif

#if (BLE_APP_MESH_CLIENT_GATEWAY == 1)
#include "app_mesh_client_gateway.h"
#endif

#if (BLE_APP_MESH_SERVER_SWITCH == 1)
#include "app_mesh_server_switch.h"
#endif

#if (BLE_APP_MESH_TULIPE_LIGHT == 1)
#include "app_tulip_light.h"
#endif

#if (BLE_TUYA_MESH_SDK_ENABLE == 1)
#include "app_tuya_mesh_task.h"
#endif

#endif

#if (APP_OTA_MATSER_ENABLE)
#include "app_ota_scan.h"
#include "ota_device.h"
#endif

#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif //(NVDS_SUPPORT)

#if (BLE_APP_OTA)
#include "rdw_ota_task.h"
#endif

#include "cobra4_common.h"

#if(DIANBIAO_MODULE_ENABLE == 1)

#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
#include "app_realtek_device.h"
#include "app_realtek_console.h"
#else
#include "app_iris_device.h"
#endif
#endif

#if (BLE_APP_HID_ENABLE)
#include "app_hid.h"
#endif

#if (BLE_APP_LED_BUTTON)
#include "app_led_button.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan.h"
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
#include "tuya_app_ble_services.h"
#endif

#if(APP_WECHAT_ENABLE == 1)
#include "app_ble_wechat.h"
#endif

extern struct adv_env_tag  adv_device1_env;
#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1)
extern uint8_t adv_restart_enable[2];
#endif

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*appm_add_svc_func_t)(void);

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of service to add in the database
enum appm_svc_list
{
#if (BLE_HOST_UART)
    APPM_SVC_HOST_UART,
#endif

#if (BLE_DEVICE1_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE)
    APPM_SVC_DEVICE1_HOST_UART,
#endif

#if (BLE_APP_UART)
    APPM_SVC_UART,
#endif

#if (BLE_APP_BLOOD)
    APPM_SVC_BLOOD,
#endif

#if (BLE_APP_HEART_RATE)
    APPM_SVC_HEART_RATE,
#endif

#if (BLE_APP_BATT_NO_HID)
    APPM_SVC_BATT,
#endif

#if (BLE_APP_HT)
    APPM_SVC_HTS,
#endif

#if (BLE_APP_MESH)
    APPM_SVC_MESH,
#endif

#if (BLE_APP_HID_ENABLE)
    APPM_SVC_HIDS,
#endif

#if (BLE_APP_LED_BUTTON)
    APPM_SVC_LED_BUTTON,
#endif

#if (BLE_APP_RDW_VOICE)
    APPM_SVC_VOICE,
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
    APPM_SVC_AT_HOST1,
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    APPM_SVC_AT_MASTER,                //master profile, 作为host 的profile
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    APPM_SVC_TUYA_BLE,
#endif

#if(BLE_APP_OTA == 1)
    APPM_SVC_OTA,
#endif

#if (APP_WECHAT_ENABLE)
		APPM_SVC_WECHAT,
#endif

    APPM_SVC_LIST_STOP,
};

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
extern const struct ke_task_desc TASK_DESC_APP;

/// List of functions used to create the database
static const appm_add_svc_func_t appm_add_svc_func_list[APPM_SVC_LIST_STOP] =
{

#if (BLE_HOST_UART)
    (appm_add_svc_func_t)app_host_uart_add_uarts,
#endif

#if (BLE_DEVICE1_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE)
    (appm_add_svc_func_t)app_iris_device_add_profile,
#endif

#if (BLE_APP_UART)
    (appm_add_svc_func_t)app_uart_add_uarts,
#endif

#if (BLE_APP_BLOOD)
    (appm_add_svc_func_t)app_blood_add_server,
#endif

#if (BLE_APP_HEART_RATE)
    (appm_add_svc_func_t)app_heart_rate_add_hprs,
#endif

#if (BLE_APP_BATT_NO_HID)
    (appm_add_svc_func_t)app_batt_add_bas,
#endif

#if (BLE_APP_HT)
    (appm_add_svc_func_t)app_ht_add_hts,
#endif

#if (BLE_APP_MESH)
    (appm_add_svc_func_t)app_mesh_task_add_mesh,
#endif

#if (BLE_APP_HID_ENABLE)
    (appm_add_svc_func_t)app_hid_mouse_add_hids,
#endif

#if (BLE_APP_LED_BUTTON)
    (appm_add_svc_func_t)app_led_button_add_service,
#endif

#if (BLE_APP_RDW_VOICE)
    (appm_add_svc_func_t)app_voice_add_profile,
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
    (appm_add_svc_func_t)app_host1_uart_add_uarts,
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    (appm_add_svc_func_t)app_master_device_add_profile,
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    (appm_add_svc_func_t)app_tuya_ble_add_services,
#endif

#if (BLE_APP_OTA)
    (appm_add_svc_func_t)app_ota_add_otas,
#endif

#if (APP_WECHAT_ENABLE)
		(appm_add_svc_func_t)app_wechat_add_service,
#endif

};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// Application Environment Structure
#if (COBRA_POWER_DOWN_ENABLE)
struct app_env_tag app_env		__attribute__((section("retention_area"),zero_init));
#else
struct app_env_tag app_env;
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static unsigned int collection_2_ready;

#if CMD_ADV_FUNCTION
void appm_start_advertising(void)
#else
static void appm_start_advertising(void)
#endif
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
                                            TASK_GAPM, TASK_APP,
                                            gapm_activity_start_cmd);

    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = app_env.adv_actv_idx;

    p_cmd->u_param.adv_add_param.duration = 0;   //广播多长时间

    p_cmd->u_param.adv_add_param.max_adv_evt = 0;

    // Send the message
    ke_msg_send(p_cmd);

    // Keep the current operation
    app_env.adv_state = APP_ADV_STATE_STARTING;
    // And the next expected operation code for the command completed event
    app_env.adv_op = GAPM_START_ACTIVITY;
}


void appm_stop_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                         TASK_GAPM, TASK_APP,
                                         gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = app_env.adv_actv_idx;

    // Send the message
    ke_msg_send(cmd);

    // Update advertising state
    app_env.adv_state = APP_ADV_STATE_STOPPING;
    // And the next expected operation code for the command completed event
    app_env.adv_op = GAPM_STOP_ACTIVITY;
}


static void appm_set_adv_data(void)
{
    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                          TASK_GAPM, TASK_APP,
                                          gapm_set_adv_data_cmd,
                                          ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;

#if(TUYA_BLE_SDK_ENABLE == 1)
    tuya_ble_build_adv_data(ADV_DATA_LEN - 3, &p_cmd->length, &p_cmd->data[0]);
#else
    p_cmd->length = 0;
    // GAP will use 3 bytes for the AD Type
		#if ADV_DATA_SELF_DEFINE
		if(!copy_adv_data(&p_cmd->data[0],&(p_cmd->length)))
       appm_build_adv_data1(ADV_DATA_LEN - 3, &p_cmd->length, &p_cmd->data[0]);
		#else		
    appm_build_adv_data(ADV_DATA_LEN - 3, &p_cmd->length, &p_cmd->data[0]);
		#endif
#endif

    // Send the message
    ke_msg_send(p_cmd);

    // Update advertising state
    app_env.adv_state = APP_ADV_STATE_SETTING_ADV_DATA;
    // And the next expected operation code for the command completed event
    app_env.adv_op = GAPM_SET_ADV_DATA;
}

static void appm_set_scan_rsp_data(void)
{
    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                          TASK_GAPM, TASK_APP,
                                          gapm_set_adv_data_cmd,
                                          ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_SCAN_RSP_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;

#if (BLE_APP_HID_ENABLE == 1)
    p_cmd->length = 0; // 在HID 模式下, 没有 RSP 数据

#else

#if(DIANBIAO_MODULE_ENABLE == 1)
//    p_cmd->length = APP_SCNRSP_DATA_LEN;
//    memcpy(&p_cmd->data[0], APP_SCNRSP_DATA, APP_SCNRSP_DATA_LEN);

    p_cmd->length = 0;
    appm_build_scan_rsp_data(&p_cmd->length, &p_cmd->data[0]);
		
#elif(APP_WECHAT_ENABLE == 1)
		p_cmd->data[0] = 0x0C;
		p_cmd->data[1] = 0xFF;
		
		p_cmd->data[2] = 0x00;
		p_cmd->data[3] = 0x00;
		
		p_cmd->data[4] = 0xFE;
		p_cmd->data[5] = 0x01;
		p_cmd->data[6] = 0x01;
		
	
		uint8_t temp_length;
		struct bd_addr  temp_bdaddr;
    temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
		
    p_cmd->data[7]  =  temp_bdaddr.addr[5];
		p_cmd->data[8]  =  temp_bdaddr.addr[4];
		p_cmd->data[9]  =  temp_bdaddr.addr[3];
		p_cmd->data[10] =  temp_bdaddr.addr[2];
		p_cmd->data[11] =  temp_bdaddr.addr[1];
		p_cmd->data[12] =  temp_bdaddr.addr[0];	
		
		p_cmd->length = 0;
		
#elif(TUYA_BLE_SDK_ENABLE == 1)
    tuya_ble_build_scan_rsp_data(&p_cmd->length, &p_cmd->data[0]);

#else
    p_cmd->length = 0;
    appm_build_scan_rsp_data(&p_cmd->length, &p_cmd->data[0]);
    if(p_cmd->length < 2)
    {
        p_cmd->length = APP_SCNRSP_DATA_LEN;
        memcpy(&p_cmd->data[0], APP_SCNRSP_DATA, APP_SCNRSP_DATA_LEN);
    }
#endif

#endif

    // Send the message
    ke_msg_send(p_cmd);

    // Update advertising state
    app_env.adv_state = APP_ADV_STATE_SETTING_SCAN_RSP_DATA;
    // And the next expected operation code for the command completed event
    app_env.adv_op = GAPM_SET_SCAN_RSP_DATA;
}


void appm_send_gapm_reset_cmd(void)
{
    // Reset the stack
    struct gapm_reset_cmd *p_cmd = KE_MSG_ALLOC(GAPM_RESET_CMD,
                                   TASK_GAPM, TASK_APP,
                                   gapm_reset_cmd);

    p_cmd->operation = GAPM_RESET;

    ke_msg_send(p_cmd);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void appm_init()
{
#if ((APP_RF_NORMAL_PROTOCOL_ENABLE == 1) || (APP_ADC_SAMPLE_ENABLE == 1) || (APP_MOUSE_ENABLE == 1))
    return;
#endif

    // Reset the application manager environment
    memset(&app_env, 0, sizeof(app_env));

    // Create APP task
    ke_task_create(TASK_APP, &TASK_DESC_APP);

    // Initialize Task state
    ke_state_set(TASK_APP, APPM_INIT);

    // Get the Device Name to add in the Advertising Data (Default one or NVDS one)
#if (NVDS_SUPPORT)
    app_env.dev_name_len = APP_DEVICE_NAME_MAX_LEN;
    if (nvds_get(NVDS_TAG_DEVICE_NAME, &(app_env.dev_name_len), app_env.dev_name) != NVDS_OK)
#endif //(NVDS_SUPPORT)
    {
        // Get default Device Name (No name if not enough space)
        memcpy(app_env.dev_name, APP_DFLT_DEVICE_NAME, APP_DFLT_DEVICE_NAME_LEN);
        app_env.dev_name_len = APP_DFLT_DEVICE_NAME_LEN;
        // TODO update this value per profiles
    }

    /*------------------------------------------------------
     * INITIALIZE ALL MODULES
     *------------------------------------------------------*/

    // load device information:

#if (BLE_APP_SEC)
    // Security Module
    app_sec_init();
#endif // (BLE_APP_SEC)

#if(DIANBIAO_SPM_ENABLE == 1)
    app_sec2_init();
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    app_tuya_ble_init();
#endif

#if (BLE_APP_UART)
    app_uart_init();
#endif

#if (BLE_HOST_UART)
    app_host_uart_init();
#endif

#if (BLE_APP_HT)
    app_ht_init();
#endif

#if (BLE_APP_BATT_NO_HID)
    app_batt_init();
#endif

#if (BLE_APP_BLOOD)
    app_blood_init();
#endif

#if BLE_APP_MESH
    app_mesh_task_init();
#endif

#if (APP_OTA_MATSER_ENABLE)
    app_ota_scan_init();
    app_ota_device_init();
#endif

#if (BLE_APP_HID_ENABLE)
    app_hid_mouse_init();
#endif

#if (BLE_APP_RDW_VOICE)
    app_voice_init();
#endif

#if (BLE_APP_LED_BUTTON)
    app_led_button_init();
#endif

#if BLE_APP_RECEIVE_VOICE
    app_sound_scan_init();
    app_voice_device_init();
#endif
    collection_2_ready = 0;

#if(APP_WECHAT_ENABLE == 1)
   app_wechat_init();
#endif


#if((DIANBIAO_MODULE_ENABLE == 0) || (DIANBIAO_PROTOCOL_REALTEK_ENABLE))
    appm_send_gapm_reset_cmd();
#endif

#if ((APP_HOST2_ENABLE == 1) && (DIANBIAO_MODULE_ENABLE == 1))
    app_host2_init();
#endif

#if ((APP_SCAN_DEVICE_ENABLE == 1) && (DIANBIAO_MODULE_ENABLE == 1))
    app_device1_scan_init();

#if BLE_DEVICE1_UART_CLIENT_ENABLE
    app_device1_connection_init();
#endif

#if BLE_DEVICE2_UART_CLIENT_ENABLE
    app_device2_connection_init();
#endif

#if BLE_DEVICE3_UART_CLIENT_ENABLE
    app_device3_connection_init();
#endif

#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    master_scan_scan_init();
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
    app_host1_uart_init();
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    app_slave_device1_connection_init();
#endif

#if (APP_DWT_MASTER==1)
    app_master_scan_init();
    app_slave_device_init();
#endif

}

bool appm_add_svc(void)
{
    // Indicate if more services need to be added in the database
    bool more_svc = false;

    // Check if another should be added in the database
    if (app_env.next_svc != APPM_SVC_LIST_STOP)
    {
        ASSERT_INFO(appm_add_svc_func_list[app_env.next_svc] != NULL, app_env.next_svc, 1);

        // Call the function used to add the required service
        appm_add_svc_func_list[app_env.next_svc]();

        // Select following service to add
        app_env.next_svc++;
        more_svc = true;
    }

    return more_svc;
}

void appm_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                      KE_BUILD_ID(TASK_GAPC, app_env.conidx), TASK_APP,
                                      gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}

static void appm_create_advertising(void)
{
    if (app_env.adv_state == APP_ADV_STATE_IDLE)
    {
        // Prepare the GAPM_ACTIVITY_CREATE_CMD message
        struct gapm_activity_create_adv_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                TASK_GAPM, TASK_APP,
                gapm_activity_create_adv_cmd);

        // Set operation code
        p_cmd->operation = GAPM_CREATE_ADV_ACTIVITY;

        // Fill the allocated kernel message
        p_cmd->own_addr_type = GAPM_STATIC_ADDR;  //GAPM_STATIC_ADDR;  liujian modified  GAPM_GEN_RSLV_ADDR
        //如果是 GAPM_STATIC_ADDR, 需要提供一个地址,否则就是程序中的Default 值
#if 1
        p_cmd->own_addr_type = GAPM_STATIC_ADDR;
        p_cmd->adv_param.type = GAPM_ADV_TYPE_LEGACY;
        p_cmd->adv_param.prop = GAPM_ADV_PROP_UNDIR_CONN_MASK;
        p_cmd->adv_param.filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY;
        p_cmd->adv_param.prim_cfg.chnl_map = APP_ADV_CHMAP;
        p_cmd->adv_param.prim_cfg.phy = GAP_PHY_LE_1MBPS;
#else
        p_cmd->own_addr_type = GAPM_STATIC_ADDR;
        p_cmd->adv_param.type = GAPM_ADV_TYPE_EXTENDED;
        p_cmd->adv_param.prop = GAPM_EXT_ADV_PROP_UNDIR_CONN_MASK;
        p_cmd->adv_param.filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY;
        p_cmd->adv_param.prim_cfg.chnl_map = APP_ADV_CHMAP;
        p_cmd->adv_param.prim_cfg.phy = PHY_CODED_VALUE;
        p_cmd->adv_param.second_cfg.phy = PHY_CODED_VALUE;
#endif

#if (BLE_APP_HID_ENABLE)
        p_cmd->adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC;
        if (app_sec_get_bond_status())
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_FAST_INT;
            p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_FAST_INT;
        }
        else
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
            p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;
        }

#else    //////////////////////////////// HID else

        p_cmd->adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC;

#if((DIANBIAO_SLAVE_PARAM_SET_ENABE == 1) &&(DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1))

        unsigned short temp_period = app_slave_device_get_connection_interval();
        if(temp_period != 0)
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = temp_period/1.25;
            p_cmd->adv_param.prim_cfg.adv_intv_max = temp_period/1.25;
        }
        else
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
            p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;
#elif((ADV_INTER_SELF_DEFINE == 1) &&(DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1))
        if (app_device_get_adv_interval()>0)
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = app_device_get_adv_interval();
            p_cmd->adv_param.prim_cfg.adv_intv_max = app_device_get_adv_interval();
        }
        else
        {
            p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
            p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;

            app_device_set_adv_interval(APP_ADV_INT_MIN);
        }




#else
        p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
        p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;
#endif

#endif

        // Send the message
        ke_msg_send(p_cmd);

        // Keep the current operation
        app_env.adv_state = APP_ADV_STATE_CREATING;
        // And the next expected operation code for the command completed event
        app_env.adv_op = GAPM_CREATE_ADV_ACTIVITY;
    }
}

//liujian add set address
static void appm_set_bdaddr(void)
{
    struct gapm_gen_rand_addr_cmd *p_cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_ADDR_CMD,
                                           TASK_GAPM, TASK_APP,
                                           gapm_gen_rand_addr_cmd);

    // Set operation code
    p_cmd->operation = GAPM_GEN_RAND_ADDR;


}


void appm_delete_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_delete_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
            TASK_GAPM, TASK_APP,
            gapm_activity_delete_cmd);

    // Set operation code
    p_cmd->operation = GAPM_DELETE_ACTIVITY; //GAPM_DELETE_ALL_ACTIVITIES;
		
		p_cmd->actv_idx = app_env.adv_actv_idx;

    // Send the message
    ke_msg_send(p_cmd);

    // Keep the current operation
    // And the next expected operation code for the command completed event
    app_env.adv_op = GAPM_DELETE_ACTIVITY; //GAPM_DELETE_ALL_ACTIVITIES;
}

void appm_adv_fsm_next(void)
{
    switch (app_env.adv_state)
    {
    case (APP_ADV_STATE_IDLE):
    {
        // Create advertising
        appm_create_advertising();
    }
    break;

    case (APP_ADV_STATE_CREATING):
    {
        // Set advertising data
        appm_set_adv_data();
    }
    break;

    case (APP_ADV_STATE_SETTING_ADV_DATA):
    {
        // Set scan response data
        appm_set_scan_rsp_data();
    }
    break;

    case (APP_ADV_STATE_CREATED):
    case (APP_ADV_STATE_SETTING_SCAN_RSP_DATA):
    {
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)
        if(adv_device1_env.adv_ok == 0)  //如果另外一个广播没有工作, 那么就开始广播
#endif
        {
            // Start advertising activity
            appm_start_advertising();
        }
    }
    break;

    case (APP_ADV_STATE_STARTING):
    {
#if (DISPLAY_SUPPORT)
        // Update advertising state screen
        app_display_set_adv(true);
#endif //(DISPLAY_SUPPORT)

        // Go to started state
        app_env.adv_state = APP_ADV_STATE_STARTED;
        app_env.adv_ok = 1;
        printf("APP_ADV_STATE_STARTING ................. \n");
//						printf("APP_ADV_STATE_STARTED \n");
//						app_sleep_require_cmd(0xA0);
    }
    break;

    case (APP_ADV_STATE_STARTED):
    {
        // Stop advertising activity
        appm_stop_advertising();

    }
    break;

    case (APP_ADV_STATE_STOPPING):
    {
#if (DISPLAY_SUPPORT)
        // Update advertising state screen
        app_display_set_adv(false);
#endif //(DISPLAY_SUPPORT)

        // Go created state
        app_env.adv_state = APP_ADV_STATE_CREATED;
#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1)
				if(adv_restart_enable[0]==1)
				{
					adv_restart_enable[0]=0;
					appm_delete_advertising();
					app_env.adv_state = APP_ADV_STATE_IDLE;
				}	
#endif			
    }
    break;



    default:
    {
        ASSERT_ERR(0);
    }
    break;
    }
}

void host_appm_update_param(struct gapc_conn_param *conn_param, ke_task_id_t app_task)
{

    struct gapc_param_update_cmd *cmd = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                        KE_BUILD_ID(TASK_GAPC, app_env.conidx), app_task,
                                        gapc_param_update_cmd);
    cmd->operation  = GAPC_UPDATE_PARAMS;
    cmd->intv_min   = conn_param->intv_min;
    cmd->intv_max   = conn_param->intv_max;
    cmd->latency    = conn_param->latency;
    cmd->time_out   = conn_param->time_out;

    // not used by a slave device
    cmd->ce_len_min = 4;
    cmd->ce_len_max = 5;


    // Send the message；
    ke_msg_send(cmd);


}

void appm_ble_update_mtu(uint8_t idx)
{

    struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                    KE_BUILD_ID(TASK_GATTC, app_env.conidx), TASK_APP,
                                    gattc_exc_mtu_cmd);
    cmd->operation  = GATTC_MTU_EXCH;
    cmd->seq_num   = 0;
    ke_msg_send(cmd);

}


void appm_update_adv_state(bool start)
{
    // TODO [LT] - Check current advertising state

    // Start or stop advertising
    appm_adv_fsm_next();
}

#if (BLE_APP_MESH_IOS_ENABLE == 1)
//IOS 需要清掉 service 的 cache
void appm_svc_changed_command(void)
{

    struct gattc_send_svc_changed_cmd *cmd = KE_MSG_ALLOC(GATTC_SEND_SVC_CHANGED_CMD,
            KE_BUILD_ID(TASK_GATTC, app_env.conidx), TASK_APP,
            gattc_send_svc_changed_cmd);
    cmd->operation = GATTC_SVC_CHANGED;
    cmd->seq_num = 0;
    cmd->svc_shdl = 0x0001;
    cmd->svc_ehdl = 0xffff;

    ke_msg_send(cmd);

}

#endif


#if (APP_METER_AT_COMMAND == 1)

void appm_reset_adv_data(void)
{
    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                          TASK_GAPM, TASK_APP,
                                          gapm_set_adv_data_cmd,
                                          ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;


    p_cmd->length = 0;
    // GAP will use 3 bytes for the AD Type
    appm_build_adv_data(ADV_DATA_LEN - 3, &p_cmd->length, &p_cmd->data[0]);


    // Send the message
    ke_msg_send(p_cmd);

    // Update advertising state
    app_env.adv_state = APP_ADV_STATE_STARTING;

}

#endif

/////////////////////////////////////////////////////////////////////////

static struct bd_addr collection_address2 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};   //采集器2 地址

void app_collection_2_ready(void)
{
    collection_2_ready = 1;
}

void app_collection_2_not_ready(void)
{
    collection_2_ready = 0;
}

void app_iris_collection2_address(struct bd_addr * p_collection_address)
{
    memcpy(collection_address2.addr, p_collection_address->addr, BD_ADDR_LEN);

}

unsigned int app_collection_2_is_ready(void)
{
    return collection_2_ready;
}

void app_get_collection2_address(unsigned char *p_buffer)
{
    memcpy(p_buffer, collection_address2.addr, BD_ADDR_LEN);

}

//比较地址是否是采集器2 的地址
unsigned int app_device_compare_collection2_address(unsigned char * source_addr)
{
    if(!memcmp(source_addr, collection_address2.addr, BD_ADDR_LEN))
    {
        return 1;
    }
    return 0;
}

#if(DIANBIAO_MODULE_ENABLE == 0)
ke_task_id_t test_if_is_host2(uint8_t host_act_id)
{
    return APP_MAIN_TASK;
}
#endif

#endif //(BLE_APP_PRESENT)

/// @} APP
