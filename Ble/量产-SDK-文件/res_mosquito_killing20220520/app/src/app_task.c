/**
 ****************************************************************************************
 *
 * @file appm_task.c
 *
 * @brief RW APP Task implementation
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>        // for memcpy

#include "rwip_config.h"          // SW configuration

#if (BLE_APP_PRESENT)

#include "app_task.h"             // Application Manager Task API
#include "app.h"                  // Application Manager Definition
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "arch.h"                 // Platform Definitions
#include "gattc_task.h"
#include <string.h>
#include "co_utils.h"
#include "ke_timer.h"             // Kernel timer
#include "cobra_util.h"
#include "cobra4_common.h"
#if(DIANBIAO_MODULE_ENABLE == 1)
#include "host_advertiser1.h"
#include "device1_connection.h"
#include "app_iris_console.h"
#endif

#if(APP_DWT_MASTER == 1)
#include "master_scan.h"
#include "slave_device.h"
#endif

#if APP_SCAN_DEVICE_ENABLE
#include "device1_scan.h"
#endif

#if (BLE_HOST_UART)
#include "app_host_uart.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "app_master.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "slave_device1_connection.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
#include "app_host1.h"
#endif

#if (BLE_APP_RECEIVE_VOICE == 1)
#include "sound_scan.h"
#include "voice_device.h"
#endif

#if (APP_OTA_MATSER_ENABLE == 1)
#include "app_ota_scan.h"
#include "ota_device.h"
#endif

#if (BLE_APP_SEC)
#include "app_sec.h"              // Security Module Definition
#endif //(BLE_APP_SEC)

#if (BLE_APP_HT)
#include "app_ht.h"               // Health Thermometer Module Definition
#include "htpt_task.h"
#endif //(BLE_APP_HT)



#if (BLE_APP_DIS)
#include "app_dis.h"              // Device Information Module Definition
#include "diss_task.h"
#endif //(BLE_APP_DIS)

#if (BLE_APP_BATT)
#include "app_batt.h"             // Battery Module Definition
#include "bass_task.h"
#endif //(BLE_APP_BATT)

#if (BLE_APP_HID)
#include "app_hid.h"              // HID Module Definition
#include "hogpd_task.h"
#endif //(BLE_APP_HID)

#if (BLE_APP_AM0)
#include "app_am0.h"             // Audio Mode 0 Application
#endif //(BLE_APP_AM0)

#if (DISPLAY_SUPPORT)
#include "app_display.h"          // Application Display Definition
#endif //(DISPLAY_SUPPORT)

#if (BLE_APP_LED_BUTTON)
#include "app_led_button.h"
#endif

#if (BLE_APP_HRP)
#include "app_hrp.h"
#endif

#if (BLE_APP_UART)
#include "app_new_uart.h"
#endif

#if (BLE_APP_BLOOD)
#include "app_blood.h"
#endif

#if (BLE_APP_CPPS)
#include "app_cpps.h"
#endif

#if (BLE_APP_HEART_RATE)
#include "app_heart_rate.h"
#endif

#if (BLE_APP_OTA)
#include "app_ota_task.h"
#endif

#if (BLE_NORDIC_APP_OTA)

#include "app_nordic_ota_task.h"

#endif

#if (BLE_APP_RDW_VOICE)
#include "app_rdw_voice.h"
#endif

#if (BLE_APP_PROXR)
#include "app_proxr.h"
#endif

#if (BLE_APP_HID_ENABLE)
#include "app_hid.h" // HID Module Definition
#include "hogpd_task.h"
#endif //(BLE_APP_HID)


#if (BLE_APP_MESH)
#include "cobra4_common.h"

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

#if ((BLE_APP_MESH == 1) && (APP_MESH_CWHSL_LIGHT_FUNTION == 1))
#include "app_lab_test.h"
#endif

	 
#if(TUYA_BLE_SDK_ENABLE == 1)
#include "tuya_app_ble_services.h"
#include "tuya_ble_api.h"
#endif


#if(DIANBIAO_MODULE_ENABLE == 1)
#include "app_iris_device.h"
#endif

#if (APP_AIRCLEAN_ENABLE == 1)
#include "app_airclean.h"
#endif

#if (APP_RAINBOW_ENABLE == 1)
#include "app_rainbow.h"
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "app_mosquito_killing.h"
#endif

#if (UART_MODEL_TIMEOUT_ENABLE == 1)
#include "app_uart_module.h"
#endif

#if(APP_WECHAT_ENABLE == 1)
#include "app_ble_wechat.h"
#endif

static uint32_t update_again_number = 0;

#if (BLE_HOST_UART)
extern struct app_host_uart_env_tag app_host_env_1;
#endif

extern struct adv_env_tag  adv_device1_env;
#if CHECK_SCAN_FOR_RESTARE
extern uint8_t bz_restare;
#endif	
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t app_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               ke_msg_id_t msgid,
                               void *param,
                               ke_task_id_t src_id)
{
    // Counter
    uint8_t counter;

    // Get the message handler function by parsing the message table
    for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--)
    {
        struct ke_msg_handler handler
            = (struct ke_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

        if ((handler.id == msgid) ||
                (handler.id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERR(handler.func);
//						printf("handler.func..... \n");
            return (uint8_t)(handler.func(msgid, param, TASK_APP, src_id));
        }
    }

    // If we are here no handler has been found, drop the message
    return (KE_MSG_CONSUMED);
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_CREATED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
        struct gapm_activity_created_ind const *p_param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    if (app_env.adv_state == APP_ADV_STATE_CREATING)
    {
        // Store the advertising activity index
        app_env.adv_actv_idx = p_param->actv_idx;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_STOPPED_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_stopped_ind_handler(ke_msg_id_t const msgid,
        struct gapm_activity_stopped_ind const *p_param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

//		printf("gapm_activity_stopped_ind_handler reason = %d \n", p_param->reason);

    if (app_env.adv_state == APP_ADV_STATE_STARTED)
    {
        // Act as if activity had been stopped by the application
        app_env.adv_state = APP_ADV_STATE_STOPPING;

        // Perform next operation
        appm_adv_fsm_next();
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_PROFILE_ADDED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_profile_added_ind_handler(ke_msg_id_t const msgid,
        struct gapm_profile_added_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Current State
    ke_state_t state = ke_state_get(dest_id);

    if (state == APPM_CREATE_DB)
    {
        switch (param->prf_task_id)
        {
#if (BLE_APP_AM0)
        case (TASK_ID_AM0_HAS):
        {
            app_am0_set_prf_task(param->prf_task_nb);
        }
        break;
#endif //(BLE_APP_AM0)

        default: /* Nothing to do */
            break;
        }
    }
    else
    {
        ASSERT_INFO(0, state, src_id);
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP manager command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
#if (NVDS_SUPPORT)
    uint8_t key_len = KEY_LEN;
#endif //(NVDS_SUPPORT)
//liujian
#if CHECK_SCAN_FOR_RESTARE
    if(bz_restare != 1)
#endif	
    printf("operation = %d \n", param->operation);
    switch(param->operation)
    {
        // Reset completed
    case (GAPM_RESET):
    {

        if(param->status == GAP_ERR_NO_ERROR)
        {
#if ((BLE_APP_MESH == 1) && (APP_MESH_CWHSL_LIGHT_FUNTION == 1))
           if(lt_is_ate_mode())
					 {
						  break;
					 }
#endif					

#if (APP_DWT_MASTER == 1)
            app_master_scan_set_device_config();
            app_slave_device_config();
#endif

#if (BLE_APP_RECEIVE_VOICE == 1)
            app_sound_scan_set_device_config();
            app_voice_device_config();
#endif

#if APP_SCAN_DEVICE_ENABLE
            app_device1_set_device_config();
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
            master_scan_set_device_config();
#endif

#if (APP_OTA_MATSER_ENABLE == 1)
            app_ota_scan_set_device_config();
            app_ota_device_config();
#endif

#if (NVDS_SUPPORT)
            nvds_tag_len_t len;
#endif //(NVDS_SUPPORT)
#if (BLE_APP_HID)
            app_hid_start_mouse();
#endif //(BLE_APP_HID)

            // Set Device configuration
            struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_set_dev_config_cmd);
            // Set the operation
            cmd->operation = GAPM_SET_DEV_CONFIG;
            // Set the device role - Peripheral
            cmd->role      = GAP_ROLE_ALL; //GAP_ROLE_ALL;

#if (BLE_APP_SEC_CON)
            // The Max MTU is increased to support the Public Key exchange
            // HOWEVER, with secure connections enabled you cannot sniff the
            // LEAP and LEAS protocols
            cmd->max_mtu = 160;
            cmd->pairing_mode = GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY;
#else // !(BLE_APP_SEC_CON)
            // Do not support secure connections
            cmd->pairing_mode = GAPM_PAIRING_LEGACY;
#endif //(BLE_APP_SEC_CON)

            cmd->max_mtu = COBRA4_MTU;
            cmd->max_mps = COBRA4_MPS;

#if BLE_APP_MESH

            cmd->max_mtu = 672;
            cmd->max_mps = 672;
            cmd->max_nb_lecb = 15;
            cmd->pairing_mode = GAPM_PAIRING_LEGACY|GAPM_PAIRING_SEC_CON;
            cmd->renew_dur = 150;
#endif

            // Set Data length parameters
            cmd->sugg_max_tx_octets = LE_MAX_OCTETS; //BLE_MIN_OCTETS;
            cmd->sugg_max_tx_time   = LE_MAX_TIME;

#if (BLE_APP_HID_ENABLE)
            // Enable Slave Preferred Connection Parameters present
            cmd->att_cfg = GAPM_MASK_ATT_SLV_PREF_CON_PAR_EN;
#endif //(BLE_APP_HID)

#if (BLE_APP_MESH_IOS_ENABLE == 1)
            cmd->att_cfg |= GAPM_MASK_ATT_SVC_CHG_EN;
#endif
            // Host privacy enabled by default
            cmd->privacy_cfg = 0;

            update_again_number = 0;

#if (NVDS_SUPPORT)

            len = NVDS_LEN_BD_ADDRESS;

            if (nvds_get(NVDS_TAG_BD_ADDRESS, &len, &cmd->addr.addr[0]) == NVDS_OK)
            {
                // Check if address is a static random address
                if (cmd->addr.addr[5] & 0xC0)
                {
                    // Host privacy enabled by default
                    cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
                }
            }
#endif //(NVDS_SUPPORT)

#if (BLE_APP_AM0)
            cmd->audio_cfg   = GAPM_MASK_AUDIO_AM0_SUP;
            cmd->att_cfg    |= GAPM_MASK_ATT_SVC_CHG_EN;
#endif //(BLE_APP_AM0)

#if BLE_APP_MESH
            cmd->att_cfg    |= GAPM_MASK_ATT_SVC_CHG_EN;
#endif
#if (NVDS_SUPPORT)
            if ((app_sec_get_bond_status()==true) &&
                    (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_env.loc_irk) == NVDS_OK))
            {
                memcpy(cmd->irk.key, app_env.loc_irk, 16);
            }
            else
#endif //(NVDS_SUPPORT)
            {
                memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
            }
            // Send message
            ke_msg_send(cmd);
        }
        else
        {
            ASSERT_ERR(0);
        }
    }
    break;

    case (GAPM_PROFILE_TASK_ADD):
    {
        if (app_sec_get_bond_status()==true)
        {
#if (NVDS_SUPPORT)
            // If Bonded retrieve the local IRK from NVDS
            if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_env.loc_irk) == NVDS_OK)
            {
                // Set the IRK in the GAP
                struct gapm_set_irk_cmd *cmd = KE_MSG_ALLOC(GAPM_SET_IRK_CMD,
                                               TASK_GAPM, TASK_APP,
                                               gapm_set_irk_cmd);
                ///  - GAPM_SET_IRK:
                cmd->operation = GAPM_SET_IRK;
                memcpy(&cmd->irk.key[0], &app_env.loc_irk[0], KEY_LEN);
                ke_msg_send(cmd);
            }
            else
#endif //(NVDS_SUPPORT)
            {
                // If cannot read IRK from NVDS ASSERT
                ASSERT_ERR(0);
            }
        }
        else // Need to start the generation of new IRK
        {
            struct gapm_gen_rand_nb_cmd *cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
                                               TASK_GAPM, TASK_APP,
                                               gapm_gen_rand_nb_cmd);
            cmd->operation   = GAPM_GEN_RAND_NB;
            app_env.rand_cnt = 1;
            ke_msg_send(cmd);
        }
    }
    break;

    case (GAPM_GEN_RAND_NB) :
    {
        if (app_env.rand_cnt == 1)
        {
            // Generate a second random number
            app_env.rand_cnt++;
            struct gapm_gen_rand_nb_cmd *cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
                                               TASK_GAPM, TASK_APP,
                                               gapm_gen_rand_nb_cmd);
            cmd->operation = GAPM_GEN_RAND_NB;
            ke_msg_send(cmd);
        }
        else
        {
            struct gapm_set_irk_cmd *cmd = KE_MSG_ALLOC(GAPM_SET_IRK_CMD,
                                           TASK_GAPM, TASK_APP,
                                           gapm_set_irk_cmd);
            app_env.rand_cnt=0;
            ///  - GAPM_SET_IRK
            cmd->operation = GAPM_SET_IRK;
            memcpy(&cmd->irk.key[0], &app_env.loc_irk[0], KEY_LEN);
            ke_msg_send(cmd);
        }
    }
    break;

    case (GAPM_SET_IRK):
    {
        // ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);

        // If not Bonded already store the generated value in NVDS
        if (app_sec_get_bond_status()==false)
        {
#if (NVDS_SUPPORT)
            if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_env.loc_irk) != NVDS_OK)
#endif //(NVDS_SUPPORT)
            {
                ASSERT_INFO(0, 0, 0);
            }
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))

            bool bonded = true;
            if (nvds_put(NVDS_TAG_PERIPH_BONDED, NVDS_LEN_PERIPH_BONDED,
                         (uint8_t *)&bonded) != NVDS_OK)
            {
                // An error has occurred during access to the NVDS
                ASSERT_ERR(0);
            }
#endif

        }

        app_env.rand_cnt = 0;
        // Add the next requested service
        if (!appm_add_svc())
        {
            // Go to the ready state
            ke_state_set(TASK_APP, APPM_READY);

#if (BLE_APP_HID_ENABLE == 1)
            if(cobra_app_whitelist_add())
            {
                break;
            }
#endif

#if (BLE_APP_MESH == 0)
            // No more service to add, start advertising
            appm_update_adv_state(true);
#else
            //   appm_update_adv_state(true);   //如果没有进入配网状态, 不要广播其他信息
            app_mesh_task_begin();
#endif
        }
    }
    break;

    // Device Configuration updated
    case (GAPM_SET_DEV_CONFIG):
    {
        ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);

#if APP_SCAN_DEVICE_ENABLE

#if BLE_DEVICE1_UART_CLIENT_ENABLE
        app_device1_connection_create();
#endif

#if BLE_DEVICE2_UART_CLIENT_ENABLE
        app_device2_connection_create();
#endif

#if BLE_DEVICE3_UART_CLIENT_ENABLE
        app_device3_connection_create();
#endif

#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
        app_slave_device1_connection_create();
#endif

        // Go to the create db state
        ke_state_set(TASK_APP, APPM_CREATE_DB);

#if(APP_HOST2_ENABLE == 1)
        app_host2_begin_work();   //2020/7/1
#endif
        // Add the first required service in the database
        // and wait for the PROFILE_ADDED_IND
        appm_add_svc();
    }
    break;

    case (GAPM_CREATE_ADV_ACTIVITY):
    case (GAPM_STOP_ACTIVITY):
    case (GAPM_START_ACTIVITY):
    case (GAPM_DELETE_ACTIVITY):
    case (GAPM_SET_ADV_DATA):
    case (GAPM_SET_SCAN_RSP_DATA):
    {
        // Sanity checks
        ASSERT_INFO(app_env.adv_op == param->operation, app_env.adv_op, param->operation);
        ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->status, app_env.adv_op);

        // Perform next operation
        appm_adv_fsm_next();
    }
    break;

    case (GAPM_DELETE_ALL_ACTIVITIES) :
    {
        // Re-Invoke Advertising
        app_env.adv_state = APP_ADV_STATE_IDLE;
        appm_adv_fsm_next();
    }
    break;

    case (GAPM_SET_WL):
    {
#if (BLE_APP_HID_ENABLE == 1)
        appm_update_adv_state(true);
#endif
    }
    break;

    default:
    {
        // Drop the message
    }
    break;
    }

    return (KE_MSG_CONSUMED);
}

static int gapc_get_dev_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_get_dev_info_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    switch(param->req)
    {
    case GAPC_DEV_NAME:
    {
        struct gapc_get_dev_info_cfm * cfm = KE_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                                             src_id, dest_id,
                                             gapc_get_dev_info_cfm, APP_DEVICE_NAME_MAX_LEN);
        cfm->req = param->req;
        cfm->info.name.length = appm_get_dev_name(cfm->info.name.value);

        // Send message
        ke_msg_send(cfm);
    }
    break;

    case GAPC_DEV_APPEARANCE:
    {
        // Allocate message
        struct gapc_get_dev_info_cfm *cfm = KE_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                            src_id, dest_id,
                                            gapc_get_dev_info_cfm);
        cfm->req = param->req;
        // Set the device appearance
#if (BLE_APP_HT)
        // Generic Thermometer - TODO: Use a flag
        cfm->info.appearance = 728;
#elif (BLE_APP_HID_ENABLE)
        // HID Mouse
        cfm->info.appearance = BLE_APPEARANCE_HID_KEYBOARD;
#else
        // No appearance
        cfm->info.appearance = 728;    //liujian
#endif

        // Send message
        ke_msg_send(cfm);
    }
    break;

    case GAPC_DEV_SLV_PREF_PARAMS:
    {
        // Allocate message
        struct gapc_get_dev_info_cfm *cfm = KE_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                            src_id, dest_id,
                                            gapc_get_dev_info_cfm);
        cfm->req = param->req;
        // Slave preferred Connection interval Min
        cfm->info.slv_pref_params.con_intv_min = 400;
        // Slave preferred Connection interval Max
        cfm->info.slv_pref_params.con_intv_max = 400;
        // Slave preferred Connection latency
        cfm->info.slv_pref_params.slave_latency  = 6;
        // Slave preferred Link supervision timeout
        cfm->info.slv_pref_params.conn_timeout    = 600;  // 2s (500*10ms)

        printf("app GAPC_DEV_SLV_PREF_PARAMS####################################################################\n");
        // Send message
        ke_msg_send(cfm);
    }
    break;

    default: /* Do Nothing */
        break;
    }


    return (KE_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handles GAPC_SET_DEV_INFO_REQ_IND message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_set_dev_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_set_dev_info_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Set Device configuration
    struct gapc_set_dev_info_cfm* cfm = KE_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, src_id, dest_id,
                                        gapc_set_dev_info_cfm);
    // Reject to change parameters
    cfm->status = GAP_ERR_REJECTED;
    cfm->req = param->req;
    // Send message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_connection_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_connection_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

    app_env.conidx = KE_IDX_GET(src_id);

#if (BLE_HOST_UART)
    app_host_env_1.connection_index = app_env.conidx;
#endif
    app_collection_2_ready();


    struct gap_bdaddr* p_addr = gapc_get_bdaddr(app_env.conidx, SMPC_INFO_PEER);

    app_iris_collection2_address(&(p_addr->addr));

    // Check if the received Connection Handle was valid
    if (app_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        app_env.conhdl = param->conhdl;

        // Send connection confirmation
        struct gapc_connection_cfm *cfm = KE_MSG_ALLOC(GAPC_CONNECTION_CFM,
                                          KE_BUILD_ID(TASK_GAPC, app_env.conidx), TASK_APP,
                                          gapc_connection_cfm);

#if(BLE_APP_SEC)
        cfm->auth      = app_sec_get_bond_status() ? GAP_AUTH_REQ_NO_MITM_BOND : GAP_AUTH_REQ_NO_MITM_NO_BOND; // TODO [FBE] restore valid data
#else // !(BLE_APP_SEC)
        cfm->auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
#endif // (BLE_APP_SEC)

#if BLE_APP_MESH_TULIPE_LIGHT
        cfm->svc_changed_ind_enable = 1;
#endif

        ke_msg_send(cfm);

        printf("1111111111111111111111111111111111 \n");

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
	app_mosquito_killing_bt_led_onoff(1);
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
        tuya_ble_connected_handler();
#endif

//IOS 需要
#if (BLE_APP_MESH_IOS_ENABLE == 1)
        gapc_svc_chg_ccc_set(app_env.conidx, 1);
        appm_svc_changed_command();
#endif

#if (APP_SLEEP_MODE_ENABLE == 1)
        {
            struct gapc_conn_param conn_param;
            conn_param.intv_min = 200;
            conn_param.intv_max = 200;
            conn_param.latency  = 3;
            conn_param.time_out = 500;
            host_appm_update_param(&conn_param, TASK_APP);
            rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
        }
#endif

#if (BLE_HOST_UART)
        //CEVA 原来的代码为 app_env.conhdl
        app_host_uart_enable_prf_app(app_env.conhdl);
#endif

#if (BLE_APP_LED_BUTTON)
        app_led_button_enable_prf(app_env.conidx);
#endif

#if (BLE_APP_RDW_VOICE)
        app_voice_enable_prf(app_env.conidx);
        printf("BLE_APP_RDW_VOICE \n");
#endif

#if (BLE_APP_OTA)
        app_ota_enable_prf(app_env.conhdl);
  //      printf("BLE_APP_OTA \n");
#endif

#if (BLE_APP_HID_ENABLE)
        // Enable HID Service
        app_hid_mouse_enable_prf(app_env.conidx);
        printf("HID connection \n");
        printf("addr 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", param->peer_addr.addr[0],
               param->peer_addr.addr[1],
               param->peer_addr.addr[2],
               param->peer_addr.addr[3],
               param->peer_addr.addr[4],
               param->peer_addr.addr[5]);
#endif //(BLE_APP_HID_ENABLE)


#if(DIANBIAO_MODULE_ENABLE == 1)
//2020/1/10
        iris_dianbiao_master_number_rsp();
#endif

#if (BLE_APP_MESH)

#if (BLE_APP_MESH_TIANMAO_LIGHT == 1)   //先不改变  天猫精灵
        app_mesh_task_enable_profile();
#endif
        printf("Mesh Prox connection ************** \n");
        app_ble_mesh_update_param();
#endif


//设备端请求和 PC 或手机MTU 的设置
//2021/7/8 liujian 去掉这个
#if (UART_MODEL_TIMEOUT_ENABLE == 1)
        app_uart_mtu_timer_init();
#endif


#if (COBRA_POWER_DOWN_ENABLE==1)
        rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
#endif
        // We are now in connected State
        ke_state_set(dest_id, APPM_CONNECTED);

//#if(APP_WECHAT_ENABLE == 1)
////需要BOND
//     //   app_sec_send_security_req(app_env.conidx);

//#endif

#if (DIANBIAO_SPM_ENABLE == 1)
        if(app_get_ble_smp_level() != SMP_JUST_WORK)
        {
            printf("app_sec_send_security_req \n");
            // Ask for the peer device to either start encryption
            app_sec_send_security_req(app_env.conidx);
        }
#endif
        app_env.adv_ok = 0;   //不再广播了 设备被连接上了
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)
        if(adv_device1_env.adv_ok  && !app_collection_1_is_ready())
        {
            app_device1_adv_fsm_next();
        }
#endif

    }
    else
    {
        // No connection has been established, restart advertising
        appm_update_adv_state(true);
    }



    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAP controller command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{

    printf("GAPC param->operation = %d \n", param->operation);

    switch(param->operation)
    {
    case (GAPC_UPDATE_PARAMS):
    {
        printf("GAPC_UPDATE_PARAMS OK ...... \n");
        if (param->status != GAP_ERR_NO_ERROR)
        {
            printf("Error 0x%x ...... \n", param->status);
//                appm_disconnect();
        }

#if BLE_APP_RDW_VOICE
        app_audio_record_begin();
#endif

    }
    break;

    case (GATTC_SVC_CHANGED):
    {
        printf("GATTC_SVC_CHANGED ..... %d \n", param->status);
        break;
    }

    default:
    {
    } break;
    }

    return (KE_MSG_CONSUMED);
}




/**
 ****************************************************************************************
 * @brief Handles disconnection complete event from the GAP.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                       struct gapc_disconnect_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    // Go to the ready state
    ke_state_set(TASK_APP, APPM_READY);


#if (!BLE_APP_HID_ENABLE)
#if (BLE_APP_MESH == 0)
    // Restart Advertising
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)	
	  if(adv_device1_env.adv_ok == 0 && app_env.adv_ok == 0)
		{
			 appm_update_adv_state(true);
		}
#else	
    appm_update_adv_state(true);
#endif	
#else
    app_mesh_task_disconnection();
#endif
#endif //(!BLE_APP_HID)

#if (BLE_APP_BLOOD)
    app_blood_disable_profile();
#endif

#if (BLE_APP_RDW_VOICE)
    app_voice_stop_timer(); //liujian add
#endif

    app_collection_2_not_ready();

    printf("app disconnection id= %d, reason = 0x%x ..\n",param->conhdl,  param->reason);
#if(DIANBIAO_MODULE_ENABLE == 1)
    iris_dianbiao_master_number_rsp();
#endif

#if COBRA_POWER_DOWN_ENABLE
    rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    tuya_ble_disconnected_handler();
    //	tuya_ota_init_disconnect();
#endif

#if (BLE_APP_OTA)
    app_ota_disconnection();
#endif

#if(APP_WECHAT_ENABLE == 1)
   app_wechat_disconnection();
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
		app_mosquito_killing_bt_led_onoff(0);
#endif

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of all messages sent from the lower layers to the application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int appm_msg_handler(ke_msg_id_t const msgid,
                            void *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    // Retrieve identifier of the task from received message
    ke_task_id_t src_task_id = MSG_T(msgid);
    // Message policy
    uint8_t msg_pol = KE_MSG_CONSUMED;

    switch (src_task_id)
    {
    case (TASK_ID_GAPC):
    {
#if (BLE_APP_SEC)
        if ((msgid >= GAPC_BOND_CMD) &&
                (msgid <= GAPC_SECURITY_IND))
        {
            // Call the Security Module
            msg_pol = app_get_handler(&app_sec_handlers, msgid, param, src_id);
        }
#endif //(BLE_APP_SEC)
        // else drop the message
    }
    break;


#if(DIANBIAO_MODULE_ENABLE == 1)
    case TASK_ID_DEVICE1_UART:
    {
        msg_pol = app_get_handler(&app_host_uart_handlers, msgid, param, src_id);
        break;
    }

    case TASK_ID_DEVICE1_COLLECTOR_UART:
    {
        msg_pol = app_get_handler(&app_iris_device_handlers, msgid, param, src_id);
        break;
    }

#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    case TASK_ID_DEVICE1_COLLECTOR_UART:
    {
        msg_pol = app_get_handler(&app_master_device_handlers, msgid, param, src_id);
        break;
    }
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
    case TASK_ID_AT_CMD_SLAVE:
    {
        msg_pol = app_get_handler(&app_host1_uart_handlers, msgid, param, src_id);
        break;
    }
#endif

#if (BLE_APP_UART)
    case (TASK_ID_UART):
    {
        msg_pol = app_get_handler(&app_uart_handlers, msgid, param, src_id);
        break;
    }
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    case (TASK_ID_TUYA_BLE):
    {
        msg_pol = app_get_handler(&app_tuya_ble_handlers, msgid, param, src_id);
        break;
    }
#endif

//Mesh 功能开始使用
#if (BLE_APP_MESH)
    case TASK_ID_MESH:
    {
        msg_pol = app_get_handler(&app_mesh_handlers, msgid, param, src_id);
        break;
    }
#endif

#if (BLE_APP_BLOOD)
    case (TASK_ID_BLPS):
    {
        msg_pol = app_get_handler(&app_blood_handlers, msgid, param, src_id);
        break;
    }
#endif

#if (BLE_APP_HEART_RATE)
    case (TASK_ID_HRPS):
    {
        msg_pol = app_get_handler(&app_heart_rate_handlers, msgid, param, src_id);
        break;
    }
#endif

//语音遥控器
#if (BLE_APP_RDW_VOICE)
    case TASK_ID_VOICE:
    {
        msg_pol = app_get_handler(&app_voice_handlers, msgid, param, src_id);
    }
    break;
#endif

#if (BLE_APP_BATT_NO_HID)
    case (TASK_ID_BASS):
    {
        msg_pol = app_get_handler(&app_batt_handlers, msgid, param, src_id);
        break;
    }
#endif

#if (BLE_APP_LED_BUTTON)
    case (TASK_ID_LED_BUTTON):
    {
        msg_pol = app_get_handler(&app_led_button_handlers, msgid, param, src_id);
    }
    break;
#endif

#if (BLE_APP_HID_ENABLE)
    case (TASK_ID_HOGPD):
    {
        // Call the HID Module
        msg_pol = app_get_handler(&app_hid_handlers, msgid, param, src_id);
    }
    break;
#endif //(BLE_APP_HID_ENABLE)		

#if (BLE_APP_OTA)
    case 	TASK_ID_OTA:
    {
        msg_pol = app_get_handler(&app_ota_handlers, msgid, param, src_id);
    }
    break;
#endif
		
#if(APP_WECHAT_ENABLE == 1)
     case (TASK_ID_WECHAT):
     {
         // Call the Device Information Module
         msg_pol = app_get_handler(&app_wechat_handlers, msgid, param, src_id);
     } break;

#endif		

    default:
    {
#if (BLE_APP_BLOOD)
        if (msgid == APP_BLOOD_MEAS_INTV_TIMER)
        {
            msg_pol = app_get_handler(&app_blood_handlers, msgid, param, src_id);
        }
#endif

#if (BLE_APP_HEART_RATE)
        if (msgid == APP_HRP_MEAS_INTV_TIMER)
        {
            msg_pol = app_get_handler(&app_heart_rate_handlers, msgid, param, src_id);
        }
#endif

#if (BLE_APP_HID_ENABLE)
        if (msgid == APP_HID_MOUSE_TIMEOUT_TIMER)
        {
            msg_pol = app_get_handler(&app_hid_handlers, msgid, param, src_id);
        }
#endif //(BLE_APP_HID_ENABLE)

#if (BLE_APP_RDW_VOICE)
        if (msgid == APP_VOICE_KEY_TIMER)
        {
            msg_pol = app_get_handler(&app_voice_handlers, msgid, param, src_id);
        }
#endif

#if (APP_AIRCLEAN_ENABLE == 1)
        if ((msgid >= APP_AIRCLEAN_DELAY_POWER_DOWN_TIMER) && (msgid < APP_AIRCLEAN__TIMER_MAX))
        {
            msg_pol = app_get_handler(&app_airclean_handlers, msgid, param, src_id);
        }
#endif

#if (APP_RAINBOW_ENABLE == 1)
        if ((msgid >= APP_RAINBOW_DEFAULT_TIMER) && (msgid < APP_RAINBOW_TIMER_MAX))
        {
            msg_pol = app_get_handler(&app_rainbow_handlers, msgid, param, src_id);
        }
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
				if ((msgid >= APP_MOSQUITO_KILLING_DEFAULT_TIMER) && (msgid < APP_MOSQUITO_KILLING_TIMER_MAX))
				{
					msg_pol = app_get_handler(&app_mosquito_killing_handlers, msgid, param, src_id);
				}
#endif

    }   //default
    break;

    }

    return (msg_pol);
}

/**
 ****************************************************************************************
 * @brief Handles reception of random number generated message
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_gen_rand_nb_ind_handler(ke_msg_id_t const msgid, struct gapm_gen_rand_nb_ind *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if (app_env.rand_cnt==1)      // First part of IRK
    {
        memcpy(&app_env.loc_irk[0], &param->randnb.nb[0], 8);
    }
    else if (app_env.rand_cnt==2) // Second part of IRK
    {
        memcpy(&app_env.loc_irk[8], &param->randnb.nb[0], 8);
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_update_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_param_update_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

    app_env.conidx = KE_IDX_GET(src_id);

    // Check if the received Connection Handle was valid
    if (app_env.conidx != GAP_INVALID_CONIDX)
    {
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                                            KE_BUILD_ID(TASK_GAPC, app_env.conidx), TASK_APP,
                                            gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;  //2
        cfm->ce_len_max = 4;  //4

        // Send message
        ke_msg_send(cfm);

#if (COBRA_POWER_DOWN_ENABLE==1)
        if(param->intv_min <= 0x10)
        {
            // Requested connection parameters
            struct gapc_conn_param conn_param;

            conn_param.intv_min = APP_CON_INT_MIN;
            conn_param.intv_max = APP_CON_INT_MAX;
            conn_param.latency  = APP_CON_LATENCY;
            conn_param.time_out = APP_CON_TIMEOUT;

            host_appm_update_param(&conn_param,TASK_APP);
            rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
        }
        else
        {
            rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
        }
#endif

    }
    else
    {
        // No connection has been established, restart advertising
        appm_update_adv_state(true);
    }

    return (KE_MSG_CONSUMED);
}


static int gapc_mtu_changed_ind_handler(ke_msg_id_t const msgid,
                                        struct gattc_mtu_changed_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    printf("mtu changed = %d \n", param->mtu);

#if (UART_MODEL_TIMEOUT_ENABLE == 1)
    app_uart_mtu_ready();
#endif

    return (KE_MSG_CONSUMED);
}

static int gapc_param_update_ind_handler(ke_msg_id_t const msgid,
        struct gapc_param_updated_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

    printf("con_interval = %d, con_latency = %d, time_out = %d \n",  \
           param->con_interval, param->con_latency, param->sup_to);
#if (COBRA_POWER_DOWN_ENABLE==1)
    if(param->con_interval>=APP_CON_INT_MIN)
    {
        rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
    }
    else
    {
        // Requested connection parameters
        struct gapc_conn_param conn_param;

        /*
         * Requested connection interval: 125ms
         * Latency: 25
         * Supervision Timeout: 2s
         */
        conn_param.intv_min = APP_CON_INT_MIN;
        conn_param.intv_max = APP_CON_INT_MAX;
        conn_param.latency  = APP_CON_LATENCY;
        conn_param.time_out = APP_CON_TIMEOUT;

        host_appm_update_param(&conn_param,TASK_APP);
#if (APP_DBG)
        printf("updateFlag 1:%d\n",updateFlag);
#endif
        rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);

    }
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
	if(param->con_interval<20)
	{

		struct gapc_conn_param conn_param;

		/*
		 * Requested connection interval: 125ms
		 * Latency: 25
		 * Supervision Timeout: 2s
		 */
//		conn_param.intv_min = APP_CON_INT_MIN;
//		conn_param.intv_max = APP_CON_INT_MAX;
//		conn_param.latency	= APP_CON_LATENCY;
//		conn_param.time_out = APP_CON_TIMEOUT;

		conn_param.intv_min = 20;
		conn_param.intv_max = 40;
		conn_param.latency	= 20;
		conn_param.time_out = 40*20;

		host_appm_update_param(&conn_param,TASK_APP);
	}
#endif

#if (APP_SLEEP_MODE_ENABLE == 1)
    rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
#endif
    return (KE_MSG_CONSUMED);
}


/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(appm)
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)appm_msg_handler},

    // GAPM messages
    {GAPM_PROFILE_ADDED_IND,    (ke_msg_func_t)gapm_profile_added_ind_handler},
    {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)gapm_activity_created_ind_handler},
    {GAPM_ACTIVITY_STOPPED_IND, (ke_msg_func_t)gapm_activity_stopped_ind_handler},
    {GAPM_CMP_EVT,              (ke_msg_func_t)gapm_cmp_evt_handler},
    {GAPM_GEN_RAND_NB_IND,      (ke_msg_func_t)gapm_gen_rand_nb_ind_handler},

    // GAPC messages
    {GAPC_GET_DEV_INFO_REQ_IND, (ke_msg_func_t)gapc_get_dev_info_req_ind_handler},
    {GAPC_SET_DEV_INFO_REQ_IND, (ke_msg_func_t)gapc_set_dev_info_req_ind_handler},
    {GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)gapc_connection_req_ind_handler},
    {GAPC_CMP_EVT,              (ke_msg_func_t)gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (ke_msg_func_t)gapc_disconnect_ind_handler},
    {GAPC_PARAM_UPDATE_REQ_IND, (ke_msg_func_t)gapc_param_update_req_ind_handler},
    {GATTC_MTU_CHANGED_IND,     (ke_msg_func_t)gapc_mtu_changed_ind_handler},
    {GAPC_PARAM_UPDATED_IND,    (ke_msg_func_t)gapc_param_update_ind_handler},
};

/* Defines the place holder for the states of all the task instances. */
#if (COBRA_POWER_DOWN_ENABLE==1)
ke_state_t appm_state[APP_IDX_MAX] __attribute__((section("retention_area"),zero_init));
#else
ke_state_t appm_state[APP_IDX_MAX];
#endif

// Application task descriptor
const struct ke_task_desc TASK_DESC_APP = {appm_msg_handler_tab, appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)};

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
