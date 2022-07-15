/**@file            new_uart.c
* @brief            
* @details          
* @author      			
* @date             2018-04-03
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/04/03  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "rwip_config.h"

#if defined(CFG_PRF_UART)
#include "attm.h"
#include "new_uart.h"
#include "uart_new_task.h"
#include "co_utils.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "gattc_task.h"

#define USE_128_UUID 1

#if (COBRA_POWER_DOWN_ENABLE)
extern volatile bool ble_deep_sleep;
#endif
/*
 ****************************************************************************************
 */
/// Full UART Database Description - Used to add attributes into the database
#if USE_128_UUID
const struct attm_desc_128 uart_att_db[UART_IDX_NB] =
    {

        [UART_IDX_SVC] = {{0x00, 0x28}, PERM(RD, ENABLE), 0, 0},

        [UART_IDX_TX_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE), 0, 0},

        [UART_IDX_TX_VAL] = {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E}, 
                             PERM(NTF, ENABLE),
                             PERM(UUID_LEN, UUID_128), 
                             0},                // PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},

        [UART_IDX_TX_VAL_IND_CFG] = {{0x02, 0x29}, 
                                     PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 
                                     0, 0},

        [UART_IDX_RX_CHAR] = {{0x03, 0x28}, 
                              PERM(RD, ENABLE), 
                              0, 0},

        [UART_IDX_RX_VAL] = {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E},
                             PERM(WRITE_COMMAND, ENABLE)|PERM(WRITE_REQ, ENABLE),
                             PERM(UUID_LEN, UUID_128),
                             MAX_RX_UART},      // PERM(WRITE_COMMAND, ENABLE), 0, MAX_RX_UART } , //PERM(UUID_LEN, UUID_128), MAX_RX_UART},

        [UART_IDX_RX_VAL_CFG] = {{0x02, 0x29},
                                 PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE),
                                 0, 0},
};

#else

const struct attm_desc uart_att_db[UART_IDX_NB] =
    {
        // UART Service Declaration
        [UART_IDX_SVC] = {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

        [UART_IDX_TX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},

        [UART_IDX_TX_VAL] = {0x0003, PERM(NTF, ENABLE), PERM(RI, ENABLE), 0},

        [UART_IDX_TX_VAL_IND_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},

        [UART_IDX_RX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},

        [UART_IDX_RX_VAL] = {0x0004, PERM(WRITE_COMMAND, ENABLE), 0, MAX_RX_UART},

};

#endif

static uint16_t uart_compute_att_table(uint16_t features);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t new_uart_init(struct prf_task_env *env,
                             uint16_t *start_hdl,
                             uint16_t app_task,
                             uint8_t sec_lvl,
                             struct uart_db_cfg *params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

#if (COBRA_POWER_DOWN_ENABLE)
	if(ble_deep_sleep == 0)
	{
#endif
    cfg_flag = uart_compute_att_table(params->features);

#if USE_128_UUID
    uint8_t uart_uuid[16] = {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};
    status = attm_svc_create_db_128(start_hdl, uart_uuid, (uint8_t *)&cfg_flag,
                                    UART_IDX_NB, NULL, env->task,
                                    &uart_att_db[0], sec_lvl);

    //  (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE) );

#else
    status = attm_svc_create_db(start_hdl, 0x0001, (uint8_t *)&cfg_flag,
                                UART_IDX_NB, NULL, env->task, 
                                &uart_att_db[0],
                                (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE));

#endif

    if (status == ATT_ERR_NO_ERROR)
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct uart_env_tag *uart_env = (struct uart_env_tag *)ke_malloc(sizeof(struct uart_env_tag), KE_MEM_ATT_DB);

        // allocate PROXR required environment variable
        env->env = (prf_env_t *)uart_env;

        uart_env->shdl = *start_hdl;
        uart_env->prf_env.app_task = app_task | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        uart_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id = TASK_ID_UART;
        uart_task_init(&(env->desc));

        //Save features on the environment
        uart_env->features = params->features;
        uart_env->operation = NULL;
        memset(uart_env->ntf_ind_cfg, 0, sizeof(uart_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, UART_IDLE);
    }
#if (COBRA_POWER_DOWN_ENABLE)
	   }
#endif	
    return (status);
}

static void uart_destroy(struct prf_task_env *env)
{
    struct uart_env_tag *uart_env = (struct uart_env_tag *)env->env;

    // free profile environment variables
    if (uart_env->operation != NULL)
    {
        ke_free(uart_env->operation);
    }

    env->env = NULL;
    ke_free(uart_env);
}

static void uart_create(struct prf_task_env *env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct uart_env_tag *uart_env = (struct uart_env_tag *)env->env;
    uart_env->ntf_ind_cfg[conidx] = 0;
}

static void uart_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct uart_env_tag *uart_env = (struct uart_env_tag *)env->env;
    uart_env->ntf_ind_cfg[conidx] = 0;
}

static uint16_t uart_compute_att_table(uint16_t features)
{
    //Temperature Measurement Characteristic is mandatory
    uint16_t att_table = UART_PROFILE_MASK;
    return att_table;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// UART Task interface required by profile manager
const struct prf_task_cbs uart_itf =
    {
        (prf_init_fnct)new_uart_init,
        uart_destroy,
        uart_create,
        uart_cleanup,
};

void uart_exe_operation(void)
{
    struct uart_env_tag *uart_env = PRF_ENV_GET(UART, uart);

    ASSERT_ERR(uart_env->operation != NULL);

    bool finished = true;

    while (uart_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if (((uart_env->ntf_ind_cfg[uart_env->operation->cursor] & uart_env->operation->op) != 0)
            // and event not filtered on current connection
            && (uart_env->operation->conidx != uart_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd *evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                              KE_BUILD_ID(TASK_GATTC, uart_env->operation->cursor),
                                                              prf_src_task_get(&uart_env->prf_env, 0),
                                                              gattc_send_evt_cmd,
                                                              uart_env->operation->length);

            evt->operation = GATTC_NOTIFY;                    // (uart_env->operation->op != UART_CFG_INTERM_MEAS_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
            evt->length = uart_env->operation->length;
            evt->handle = uart_env->operation->handle;
            memcpy(evt->value, uart_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            uart_env->operation->cursor++;
            break;
        }
        uart_env->operation->cursor++;
    }

    // check if operation is finished
    if (finished)
    {
        // do not send response if operation has been locally requested
        if (uart_env->operation->dest_id != prf_src_task_get(&uart_env->prf_env, 0))
        {
            // send response to requester
            struct uart_meas_intv_upd_rsp *rsp =
                KE_MSG_ALLOC(((uart_env->operation->op == UART_CFG_MEAS_INTV_IND) ? UART_MEAS_INTV_UPD_RSP : UART_TEMP_SEND_RSP),
                             uart_env->operation->dest_id,
                             prf_src_task_get(&uart_env->prf_env, 0),
                             uart_meas_intv_upd_rsp);
            rsp->status = GAP_ERR_NO_ERROR;
            ke_msg_send(rsp);
        }

        // free operation
        ke_free(uart_env->operation);
        uart_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(uart_env->prf_env), 0), UART_IDLE);
    }
}

uint8_t uart_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct uart_env_tag *uart_env = PRF_ENV_GET(UART, uart);
    uint8_t status = GAP_ERR_NO_ERROR;

    if ((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;
    }
    else if (value == valid_val)
    {
        uart_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        uart_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if (status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct uart_cfg_indntf_ind *ind = KE_MSG_ALLOC(UART_CFG_INDNTF_IND,
                                                       prf_dst_task_get(&uart_env->prf_env, conidx),
                                                       prf_src_task_get(&uart_env->prf_env, conidx),
                                                       uart_cfg_indntf_ind);
        ind->conidx = conidx;
        ind->ntf_ind_cfg = uart_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return (status);
}

const struct prf_task_cbs *uart_prf_itf_get(void)
{
    return &uart_itf;
}

uint16_t uart_att_hdl_get(struct uart_env_tag *uart_env, uint8_t att_idx)
{
    uint16_t handle = uart_env->shdl;

    do
    {
#if 0			
        // Mandatory attribute handle
        if(att_idx > UART_IDX_TX_VAL_IND_CFG)
        {
            handle += UART_TX_ATT_NB;
        }
        else
        {
            handle += att_idx;
            break;
        }
#endif
        if (att_idx >= UART_IDX_NB)
            return ATT_INVALID_HDL;

        handle += att_idx;
    } while (0);

    // check if handle found
    return handle;
}

uint8_t uart_att_idx_get(struct uart_env_tag *uart_env, uint16_t handle)
{
    uint16_t handle_ref = uart_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if (handle < handle_ref)
        {
            break;
        }

        att_idx = (handle - handle_ref) + UART_IDX_SVC;

        if (att_idx >= UART_IDX_NB)
            return ATT_INVALID_IDX;

        return att_idx;
#if 0
        // Mandatory attribute handle
        handle_ref += UART_TX_ATT_NB;

        if(handle < handle_ref)
        {
            att_idx = UART_IDX_NB - (handle_ref - handle);
            break;
        }
#endif

    } while (0);

    return att_idx;
}

#endif //BLE_APP_UART
