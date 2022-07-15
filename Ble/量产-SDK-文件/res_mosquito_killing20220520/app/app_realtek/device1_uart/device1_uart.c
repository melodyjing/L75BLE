/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description:

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/



#include "rwip_config.h"

#if (BLE_DEVICE1_UART_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "attm.h"
#include "device1_uart.h"
#include "device1_uart_task.h"
#include "co_utils.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "gattc_task.h"

#define USE_128_UUID   1
/*
 ****************************************************************************************
 */
/// Full UART Database Description - Used to add attributes into the database
#if USE_128_UUID
LOCAL CONST struct attm_desc_128 uart_att_db[UART_IDX_NB] =
{


		[UART_IDX_SVC]                  =   {{0x00, 0x28},  PERM(RD, ENABLE), 0, 0},

		[UART_IDX_TX_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},

		[UART_IDX_TX_VAL]         =   { { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E}, PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
		
		[UART_IDX_TX_VAL_IND_CFG]     =   {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

		
		
	  [UART_IDX_RX_CHAR]        =   {{0x03, 0x28},   PERM(RD, ENABLE), 0, 0},

		[UART_IDX_RX_VAL]         =   { { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E},  PERM(WRITE_COMMAND, ENABLE)|PERM(WRITE_REQ, ENABLE), PERM(UUID_LEN, UUID_128), MAX_RX_UART}, //PERM(WRITE_COMMAND, ENABLE), 0, MAX_RX_UART } , //PERM(UUID_LEN, UUID_128), MAX_RX_UART},
		
	//	[UART_IDX_RX_VAL_CFG]     =   {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},
		
};

#else

LOCAL CONST struct attm_desc uart_att_db[UART_IDX_NB] =
{
		 // UART Service Declaration
		[UART_IDX_SVC]                  =   {ATT_DECL_PRIMARY_SERVICE,  PERM(RD, ENABLE), 0, 0},

		[UART_IDX_TX_CHAR]        =   {ATT_DECL_CHARACTERISTIC,   PERM(RD, ENABLE), 0, 0},

		[UART_IDX_TX_VAL]         =   {ATT_CHAR_IRIS_DATA_OUT ,   PERM(NTF, ENABLE), PERM(RI, ENABLE), 0},
		
		[UART_IDX_TX_VAL_IND_CFG]     =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

		
		
	  [UART_IDX_RX_CHAR]        =   {ATT_DECL_CHARACTERISTIC,   PERM(RD, ENABLE), 0, 0},

		[UART_IDX_RX_VAL]         =   {ATT_CHAR_IRIS_DATA_IN,     PERM(WRITE_COMMAND, ENABLE), 0, MAX_RX_UART},
		
//		[UART_IDX_RX_VAL_CFG]     =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},
	
};

#endif


LOCAL UINT16 uart_compute_att_table(UINT16 features);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

LOCAL UINT8 device1_uart_init(struct prf_task_env* env, UINT16* start_hdl, UINT16 app_task,
                            UINT8 sec_lvl, struct device1_uart_db_cfg* params)
{
    // Service content flag
    UINT16 cfg_flag;
    // DB Creation Status
    UINT8 status = ATT_ERR_NO_ERROR;

    cfg_flag = uart_compute_att_table(params->features);


#if USE_128_UUID
		 UINT8 uart_uuid[16] = { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};
		 status = attm_svc_create_db_128(start_hdl, uart_uuid, (UINT8 *)&cfg_flag,
               UART_IDX_NB, NULL, env->task, &uart_att_db[0], sec_lvl);
		 
//		  status = attm_svc_create_db_128(start_hdl, uart_uuid, (UINT8 *)&cfg_flag,
//               UART_IDX_NB, NULL, env->task, &uart_att_db[0], 0 );
//		 
//            //  (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE) );

#else
		      status = attm_svc_create_db(start_hdl, ATT_SVC_IRIS_UART, (UINT8 *)&cfg_flag,
               UART_IDX_NB, NULL, env->task, &uart_att_db[0],
              (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)) | PERM(SVC_MI, DISABLE) );

#endif
		 
    if( status == ATT_ERR_NO_ERROR )
    {
		
        //-------------------- allocate memory required for the profile  ---------------------
        struct device1_uart_env_tag* uart_env =
                (struct device1_uart_env_tag* ) ke_malloc(sizeof(struct device1_uart_env_tag), KE_MEM_ATT_DB);

        // allocate PROXR required environment variable
        env->env = (prf_env_t*) uart_env;

        uart_env->shdl     = *start_hdl;
        uart_env->prf_env.app_task = app_task
                        | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        uart_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_DEVICE1_UART;
        device1_uart_task_init(&(env->desc));

        //Save features on the environment
        uart_env->features      = params->features;
        uart_env->operation     = NULL;
        memset(uart_env->ntf_ind_cfg, 0 , sizeof(uart_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, UART_IDLE);
    }
    return (status);
}

LOCAL void uart_destroy(struct prf_task_env* env)
{
    struct device1_uart_env_tag* uart_env = (struct device1_uart_env_tag*) env->env;

    // free profile environment variables
    if(uart_env->operation != NULL)
    {
        ke_free(uart_env->operation);
    }


    env->env = NULL;
    ke_free(uart_env);
}

LOCAL void uart_create(struct prf_task_env* env, UINT8 conidx)
{
    /* Clear configuration for this connection */
    struct device1_uart_env_tag* uart_env = (struct device1_uart_env_tag*) env->env;
    uart_env->ntf_ind_cfg[conidx] = 0;
	  printf("uart_create conidx = %d........................ \n", conidx);
}

LOCAL void uart_cleanup(struct prf_task_env* env, UINT8 conidx, UINT8 reason)
{
    /* Clear configuration for this connection */
    struct device1_uart_env_tag* uart_env = (struct device1_uart_env_tag*) env->env;
    uart_env->ntf_ind_cfg[conidx] = 0;
//	  printf("uart_cleanup........................ \n");
}


LOCAL UINT16 uart_compute_att_table(UINT16 features)
{
    //Temperature Measurement Characteristic is mandatory
    UINT16 att_table = UART_PROFILE_MASK;
    return att_table;
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// UART Task interface required by profile manager
LOCAL CONST struct prf_task_cbs device1_uart_itf =
{
    (prf_init_fnct) device1_uart_init,
    uart_destroy,
    uart_create,
    uart_cleanup,
};


void device1_uart_exe_operation()
{
   // struct uart_env_tag* uart_env = PRF_ENV_GET(UART, uart);
	
		struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE1_UART, device1_uart);
	

    ASSERT_ERR(uart_env->operation != NULL);

    bool finished = true;

    while(uart_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if(((uart_env->ntf_ind_cfg[uart_env->operation->cursor] & uart_env->operation->op) != 0)
            // and event not filtered on current connection
            && (uart_env->operation->conidx != uart_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd * evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC , uart_env->operation->cursor), prf_src_task_get(&uart_env->prf_env, 0),
                    gattc_send_evt_cmd, uart_env->operation->length);

            evt->operation = GATTC_NOTIFY;     //(uart_env->operation->op != UART_CFG_INTERM_MEAS_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
            evt->length    = uart_env->operation->length;
            evt->handle    = uart_env->operation->handle;
            memcpy(evt->value, uart_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            uart_env->operation->cursor++;
            break;
        }
        uart_env->operation->cursor++;
    }


    // check if operation is finished
    if(finished)
    {
        // do not send response if operation has been locally requested
        if(uart_env->operation->dest_id != prf_src_task_get(&uart_env->prf_env, 0))
        {
            // send response to requester
            struct device1_uart_meas_intv_upd_rsp * rsp =
                    KE_MSG_ALLOC(((uart_env->operation->op == DEVICE1_UART_CFG_MEAS_INTV_IND) ? DEVICE1_UART_MEAS_INTV_UPD_RSP : DEVICE1_UART_TEMP_SEND_RSP),
                                 uart_env->operation->dest_id, prf_src_task_get(&uart_env->prf_env, 0),
                                 device1_uart_meas_intv_upd_rsp);
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


UINT8 device1_uart_update_ntf_ind_cfg(UINT8 conidx, UINT8 cfg, UINT16 valid_val, UINT16 value)
{
    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE1_UART, device1_uart);
	
    UINT8 status = GAP_ERR_NO_ERROR;

    if((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
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

		printf("conidx......... %d \n", conidx);
    if(status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct device1_uart_cfg_indntf_ind * ind = KE_MSG_ALLOC(DEVICE1_UART_CFG_INDNTF_IND,
                prf_dst_task_get(&uart_env->prf_env, conidx), prf_src_task_get(&uart_env->prf_env, conidx),
                device1_uart_cfg_indntf_ind);
				
				printf("dest = %d, src = %d \n", prf_dst_task_get(&uart_env->prf_env, conidx), prf_src_task_get(&uart_env->prf_env, conidx));
        ind->conidx      = conidx;
        ind->ntf_ind_cfg = uart_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return (status);
}


CONST struct prf_task_cbs* device1_uart_prf_itf_get(void)
{
   return &device1_uart_itf;
}


UINT16 device1_uart_att_hdl_get(struct device1_uart_env_tag* uart_env, UINT8 att_idx)
{
    UINT16 handle = uart_env->shdl;

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
			 if(att_idx >= UART_IDX_NB)  return ATT_INVALID_HDL;
				
			 handle += att_idx;
			}while(0);
		
		 // check if handle found
    return handle;
}

UINT8  device1_uart_att_idx_get(struct device1_uart_env_tag*  uart_env, UINT16 handle)
{
    UINT16 handle_ref = uart_env->shdl;
    UINT8 att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if(handle < handle_ref)
        {
            break;
        }
				
				att_idx = (handle - handle_ref) + UART_IDX_SVC;
				
				if(att_idx >= UART_IDX_NB) return ATT_INVALID_IDX;
				
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

//void uart_temp_send_rsp_send(UINT8 conidx, UINT8 status)
//{
//    // Get the address of the environment
//    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE1_UART, device1_uart);

//    struct device1_uart_send_rsp * rsp = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_RSP,
//            prf_dst_task_get(&uart_env->prf_env, conidx),
//            prf_src_task_get(&uart_env->prf_env, conidx),
//            device1_uart_send_rsp);

//    rsp->conidx = conidx;
//    rsp->status = status;

//    ke_msg_send(rsp);
//}

#endif 


