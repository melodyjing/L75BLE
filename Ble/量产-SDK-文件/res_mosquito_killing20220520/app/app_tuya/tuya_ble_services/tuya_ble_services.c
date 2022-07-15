/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_ble_services.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/6

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/6			- Creation
*****************************************************************************/

#include "rwip_config.h"

#if(TUYA_BLE_SDK_ENABLE == 1)
#include "tuya_ble_services.h"
#include "tuya_ble_services_task.h"
#include "attm.h"
#include "co_utils.h"
#include "prf_utils.h"
#include "prf.h"
#include "ke_mem.h"
#include "co_utils.h"
#include "gattc_task.h"
#include "rdw_mcu.h"


#define TUYA_BLE_COMMDATA_SERVICE             0x1910          /**< The UUID of the Tuya Ble Service. */
#define TUYA_BLE_COMMDATA_TX_CHARACTERISTIC   0x2b10          /**< The UUID of the TX Characteristic. */
#define TUYA_BLE_COMMDATA_RX_CHARACTERISTIC   0x2b11          /**< The UUID of the RX Characteristic. */


const struct attm_desc tuya_ble_services_att_db[TUYA_BLE_IDX_NB] =
    {
        // Tuya Service Declaration
        [TUYA_BLE_IDX_SVC] = {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

        [TUYA_BLE_IDX_TX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},

        [TUYA_BLE_IDX_TX_VAL] = {TUYA_BLE_COMMDATA_TX_CHARACTERISTIC, PERM(NTF, ENABLE), PERM(RI, ENABLE), 0},

        [TUYA_BLE_IDX_TX_VAL_IND_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},

        [TUYA_BLE_IDX_RX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},

        [TUYA_BLE_IDX_RX_VAL] = {TUYA_BLE_COMMDATA_RX_CHARACTERISTIC, PERM(WRITE_COMMAND, ENABLE), 0, 27},

};
		
static uint16_t tuya_ble_services_compute_att_table(uint16_t features);

static uint8_t tuya_ble_services_init(struct prf_task_env *env,
                             uint16_t *start_hdl,
                             uint16_t app_task,
                             uint8_t sec_lvl,
                             struct tuya_ble_services_db_cfg *params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    cfg_flag = tuya_ble_services_compute_att_table(params->features);


    status = attm_svc_create_db(start_hdl, TUYA_BLE_COMMDATA_SERVICE, (uint8_t *)&cfg_flag,
                                TUYA_BLE_IDX_NB, NULL, env->task, 
                                &tuya_ble_services_att_db[0],
                                (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE));



    if (status == ATT_ERR_NO_ERROR)
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct tuya_ble_env_tag *tuya_ble_env = (struct tuya_ble_env_tag *)ke_malloc(sizeof(struct tuya_ble_env_tag), KE_MEM_ATT_DB);

        // allocate tuya ble required environment variable
        env->env = (prf_env_t *)tuya_ble_env;

        tuya_ble_env->shdl = *start_hdl;
        tuya_ble_env->prf_env.app_task = app_task | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        tuya_ble_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id = TASK_ID_TUYA_BLE;
        tuya_ble_task_init(&(env->desc));

        //Save features on the environment
        tuya_ble_env->features = params->features;
        tuya_ble_env->operation = NULL;
        memset(tuya_ble_env->ntf_ind_cfg, 0, sizeof(tuya_ble_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, TUYA_BLE_IDLE);
    }
    return (status);
}

static void tuya_ble_services_destroy(struct prf_task_env *env)
{
    struct tuya_ble_env_tag *uart_env = (struct tuya_ble_env_tag *)env->env;

    // free profile environment variables
    if (uart_env->operation != NULL)
    {
        ke_free(uart_env->operation);
    }

    env->env = NULL;
    ke_free(uart_env);
}

static void tuya_ble_services_create(struct prf_task_env *env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct tuya_ble_env_tag *tuya_ble_env = (struct tuya_ble_env_tag *)env->env;
    tuya_ble_env->ntf_ind_cfg[conidx] = 0;
}

static void tuya_ble_services_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct tuya_ble_env_tag *tuya_ble_env = (struct tuya_ble_env_tag *)env->env;
    tuya_ble_env->ntf_ind_cfg[conidx] = 0;
}

static uint16_t tuya_ble_services_compute_att_table(uint16_t features)
{
    
    uint16_t att_table = TUYA_BLE_PROFILE_MASK;
    return att_table;
}

/// TUYA BLE Task interface required by profile manager
const struct prf_task_cbs tuya_ble_itf =
{
        (prf_init_fnct)tuya_ble_services_init,
        tuya_ble_services_destroy,
        tuya_ble_services_create,
        tuya_ble_services_cleanup,
};

void tuya_ble_exe_operation(void)
{
    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);

    ASSERT_ERR(tuya_ble_env->operation != NULL);

    bool finished = true;

    while (tuya_ble_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if (((tuya_ble_env->ntf_ind_cfg[tuya_ble_env->operation->cursor] & tuya_ble_env->operation->op) != 0)
            // and event not filtered on current connection
            && (tuya_ble_env->operation->conidx != tuya_ble_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd *evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                              KE_BUILD_ID(TASK_GATTC, tuya_ble_env->operation->cursor),
                                                              prf_src_task_get(&tuya_ble_env->prf_env, 0),
                                                              gattc_send_evt_cmd,
                                                              tuya_ble_env->operation->length);

            evt->operation = GATTC_NOTIFY;                    
            evt->length = tuya_ble_env->operation->length;
            evt->handle = tuya_ble_env->operation->handle;
            memcpy(evt->value, tuya_ble_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            tuya_ble_env->operation->cursor++;
            break;
        }
        tuya_ble_env->operation->cursor++;
    }

    // check if operation is finished
    if (finished)
    {
        // do not send response if operation has been locally requested
        if (tuya_ble_env->operation->dest_id != prf_src_task_get(&tuya_ble_env->prf_env, 0))
        {
            // send response to requester
            struct tuya_ble_meas_intv_upd_rsp *rsp =
                KE_MSG_ALLOC(((tuya_ble_env->operation->op == TUYA_BLE_CFG_MEAS_INTV_IND) ? TUYA_BLE_MEAS_INTV_UPD_RSP : TUYA_BLE_DATA_SEND_RSP),
                             tuya_ble_env->operation->dest_id,
                             prf_src_task_get(&tuya_ble_env->prf_env, 0),
                             tuya_ble_meas_intv_upd_rsp);
            rsp->status = GAP_ERR_NO_ERROR;
            ke_msg_send(rsp);
        }

        // free operation
        ke_free(tuya_ble_env->operation);
        tuya_ble_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(tuya_ble_env->prf_env), 0), TUYA_BLE_IDLE);
    }
}


uint8_t tuya_ble_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);
	
    uint8_t status = GAP_ERR_NO_ERROR;

    if ((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;
    }
    else if (value == valid_val)
    {
        tuya_ble_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        tuya_ble_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if (status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct tuya_ble_cfg_indntf_ind *ind = KE_MSG_ALLOC(TUYA_BLE_CFG_INDNTF_IND,
                                                       prf_dst_task_get(&tuya_ble_env->prf_env, conidx),
                                                       prf_src_task_get(&tuya_ble_env->prf_env, conidx),
                                                       tuya_ble_cfg_indntf_ind);
        ind->conidx = conidx;
        ind->ntf_ind_cfg = tuya_ble_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return (status);
}

const struct prf_task_cbs *tuya_ble_prf_itf_get(void)
{
    return &tuya_ble_itf;
}

uint16_t tuya_ble_att_hdl_get(struct tuya_ble_env_tag *tuya_ble_env, uint8_t att_idx)
{
    uint16_t handle = tuya_ble_env->shdl;

    do
    {
        if (att_idx >= TUYA_BLE_IDX_NB)
            return ATT_INVALID_HDL;

        handle += att_idx;
    } while (0);

    // check if handle found
    return handle;
}

uint8_t tuya_ble_att_idx_get(struct tuya_ble_env_tag *tuya_ble_env, uint16_t handle)
{
    uint16_t handle_ref = tuya_ble_env->shdl;
	
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if (handle < handle_ref)
        {
            break;
        }

        att_idx = (handle - handle_ref) + TUYA_BLE_IDX_SVC;

        if (att_idx >= TUYA_BLE_IDX_NB)
            return ATT_INVALID_IDX;

        return att_idx;


    } while (0);

    return att_idx;
}
		
#endif