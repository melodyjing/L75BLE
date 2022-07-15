/**@file            ble_wechat_service.c
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.

*                 微信平台连接 的服务

**********************************************************************************/


#include "rwip_config.h"
#if(BLE_WECHAT_ENABLE == 1)

#include "ble_wechat_service.h"
#include "ble_wechat_task.h"
#include "attm.h"
#include "co_utils.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "ke_msg.h"
#include "gattc_task.h"
#include "co_utils.h"
#include "rdw_mcu.h"
#include "prf.h"



static uint16_t wechat_compute_att_table(uint16_t features);

const struct attm_desc wechat_att_db[WECHAT_IDX_NB] =
{
	// Wechat Service Declaration
    [WECHAT_IDX_SVC]                   =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

    // Wechat indication Characteristic Declaration
    [WECHAT_IDX_IND_CHAR]        =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // Wechat indication Characteristic Value
    [WECHAT_IDX_IND_VAL]         =   {BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS, PERM(NTF, ENABLE), 0, 0},
    // Wechat indication Characteristic - Client Characteristic Configuration Descriptor
    [WECHAT_IDX_IND_CFG]     =       {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

    // Wechat Read Feature Characteristic Declaration
    [WECHAT_IDX_READ_FEATURE_CHAR]             =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // Wechat read Feature Characteristic Value
    [WECHAT_IDX_READ_FEATURE_VAL]              =   {BLE_UUID_WECHAT_READ_CHARACTERISTICS, PERM(RD, ENABLE), PERM(RI, ENABLE), BLE_WECHAT_MAX_DATA_LEN},

		// Wechat write Feature Characteristic Declaration
    [WECHAT_IDX_WRITE_FEATURE_CHAR]             =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // Wechat write Feature Characteristic Value
    [WECHAT_IDX_WRITE_FEATURE_VAL]              =   {BLE_UUID_WECHAT_WRITE_CHARACTERISTICS, PERM(WRITE_REQ, ENABLE), PERM(RI,ENABLE), BLE_WECHAT_MAX_DATA_LEN},

};


static uint8_t wechat_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct wechat_db_cfg* params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    cfg_flag = wechat_compute_att_table(params->features);

    status = attm_svc_create_db(start_hdl, BLE_UUID_WECHAT_SERVICE, (uint8_t *)&cfg_flag,
               WECHAT_IDX_NB, NULL, env->task, &wechat_att_db[0],
              (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE) );

    if( status == ATT_ERR_NO_ERROR )
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct wechat_env_tag* wechat_env =
                (struct wechat_env_tag* ) ke_malloc(sizeof(struct wechat_env_tag), KE_MEM_ATT_DB);

        // allocate PROXR required environment variable
        env->env = (prf_env_t*) wechat_env;

        wechat_env->shdl     = *start_hdl;
        wechat_env->prf_env.app_task = app_task
                        | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        wechat_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_WECHAT;
        wechat_task_init(&(env->desc));

        //Save features on the environment
        wechat_env->features      = params->features;
        wechat_env->operation     = NULL;
        memset(wechat_env->ntf_ind_cfg, 0 , sizeof(wechat_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, WECHAT_IDLE);
    }
    return (status);
}

static void wechat_destroy(struct prf_task_env *env)
{
    struct wechat_env_tag *wechat_env = (struct wechat_env_tag *)env->env;

    // free profile environment variables
    if (wechat_env->operation != NULL)
    {
        ke_free(wechat_env->operation);
    }

    env->env = NULL;
    ke_free(wechat_env);
}

static void wechat_create(struct prf_task_env *env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct wechat_env_tag *wechat_env = (struct wechat_env_tag *)env->env;
    wechat_env->ntf_ind_cfg[conidx] = 0;
}

static void wechat_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct wechat_env_tag *wechat_env = (struct wechat_env_tag *)env->env;
    wechat_env->ntf_ind_cfg[conidx] = 0;
}



static uint16_t wechat_compute_att_table(uint16_t features)
{
    //wechat
    uint16_t att_table = WECHAT_PROFILE_MASK;
    return att_table;
}


const struct prf_task_cbs wechat_itf =
    {
        (prf_init_fnct)wechat_init,
        wechat_destroy,
        wechat_create,
        wechat_cleanup,
};
		

void wechat_exe_operation(void)
{
    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);

    ASSERT_ERR(wechat_env->operation != NULL);

    bool finished = true;

    while (wechat_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if (((wechat_env->ntf_ind_cfg[wechat_env->operation->cursor] & wechat_env->operation->op) != 0)
            // and event not filtered on current connection
            && (wechat_env->operation->conidx != wechat_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd *evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                              KE_BUILD_ID(TASK_GATTC, wechat_env->operation->cursor),
                                                              prf_src_task_get(&wechat_env->prf_env, 0),
                                                              gattc_send_evt_cmd,
                                                              wechat_env->operation->length);

            evt->operation = GATTC_NOTIFY;                    // (uart_env->operation->op != UART_CFG_INTERM_MEAS_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
            evt->length = wechat_env->operation->length;
            evt->handle = wechat_env->operation->handle;
            memcpy(evt->value, wechat_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            wechat_env->operation->cursor++;
            break;
        }
        wechat_env->operation->cursor++;
    }

    // check if operation is finished
    if (finished)
    {
        // do not send response if operation has been locally requested
        if (wechat_env->operation->dest_id != prf_src_task_get(&wechat_env->prf_env, 0))
        {
            // send response to requester
            struct wechat_data_send_rsp *rsp =
                KE_MSG_ALLOC(WECHAT_DATA_SEND_RSP,
                             wechat_env->operation->dest_id,
                             prf_src_task_get(&wechat_env->prf_env, 0),
                             wechat_data_send_rsp);
            rsp->status = GAP_ERR_NO_ERROR;
            ke_msg_send(rsp);
        }

        // free operation
        ke_free(wechat_env->operation);
        wechat_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(wechat_env->prf_env), 0), WECHAT_IDLE);
    }
}

uint8_t wechat_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);
    uint8_t status = GAP_ERR_NO_ERROR;

    if ((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;
    }
    else if (value == valid_val)
    {
        wechat_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        wechat_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if (status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct wechat_cfg_indntf_ind *ind = KE_MSG_ALLOC(WECHAT_CFG_INDNTF_IND,
                                                       prf_dst_task_get(&wechat_env->prf_env, conidx),
                                                       prf_src_task_get(&wechat_env->prf_env, conidx),
                                                       wechat_cfg_indntf_ind);
        ind->conidx = conidx;
        ind->ntf_ind_cfg = wechat_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return (status);
}

const struct prf_task_cbs *wechat_prf_itf_get(void)
{
    return &wechat_itf;
}

uint16_t wechat_att_hdl_get(struct wechat_env_tag *wechat_env, uint8_t att_idx)
{
    uint16_t handle = wechat_env->shdl;

    do
    {
        if (att_idx >= WECHAT_IDX_NB)
            return ATT_INVALID_HDL;

        handle += att_idx;
    } while (0);

    // check if handle found
    return handle;
}

uint8_t wechat_att_idx_get(struct wechat_env_tag *wechat_env, uint16_t handle)
{
    uint16_t handle_ref = wechat_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if (handle < handle_ref)
        {
            break;
        }

        att_idx = (handle - handle_ref) + WECHAT_IDX_SVC;

        if (att_idx >= WECHAT_IDX_NB)
            return ATT_INVALID_IDX;

        return att_idx;

    } while (0);

    return att_idx;
}

#endif

