/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description:

Author:   Liujian    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	Liujian            Create.         
************************************************************************/

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))

#include "rwip_config.h"

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "slave_com.h"
#include "slave_com_task.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include <string.h>
#include "app.h"

extern struct app_host_uart_env_tag app_host_env_1;
//extern struct app_host_uart_env_tag app_host_env_2;

#define NEW_UART_PROFILE				1    //以新的方式来实现 uart profile
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UART_ENABLE_REQ message.
 * The handler enables the Health Thermometer Profile Thermometer Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int uart_enable_req_handler(ke_msg_id_t const msgid,
                                   struct device1_uart_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

		printf("uart connection idx = %d \n", param->conidx);
	

    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
        uart_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;
				
    }

    // send response
    struct device1_uart_enable_rsp *rsp = KE_MSG_ALLOC(DEVICE1_UART_ENABLE_RSP, src_id, dest_id, device1_uart_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

		
    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UART_TEMP_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int uart_temp_send_req_handler(ke_msg_id_t const msgid,
                                      struct device1_uart_data_send_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

		// Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;

    if(gapc_get_conhdl(param->connection_idx) == GAP_INVALID_CONHDL)
    {
        status = PRF_ERR_DISCONNECTED;
    }
    else if(ke_state_get(dest_id) == UART_IDLE)
    {
        // Get the address of the environment
         struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);

				if(uart_env->ntf_ind_cfg[param->connection_idx] & PRF_CLI_START_NTF)
				{
							// Allocate the GATT notification message
							struct gattc_send_evt_cmd *evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
											KE_BUILD_ID(TASK_GATTC, param->connection_idx), dest_id,
											gattc_send_evt_cmd, param->length);
							
									evt->operation = GATTC_NOTIFY; //GATTC_NOTIFY;     //(uart_env->operation->op != UART_CFG_INTERM_MEAS_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
									evt->length    = param->length;
									evt->handle    = UART_HANDLE(UART_IDX_TX_VAL);
									memcpy(evt->value, param->tx_data, evt->length);
								 // printf("evt->length = %d \n", evt->length);
							
								// Send the event
								ke_msg_send(evt);
								// Go to busy state
								ke_state_set(dest_id, UART_BUSY);
				}
        
    }
    else
    {
        // Save it for later
        msg_status = KE_MSG_SAVED;
    }

    if(status != GAP_ERR_NO_ERROR)
    {
        // Send error message to application
        uart_temp_send_rsp_send(param->connection_idx, status);
    }

    return (msg_status);

		
}



/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
    uint8_t att_idx = UART_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;
	
	  printf("info att_idx = %d \n", att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        case UART_IDX_TX_VAL_IND_CFG:
        {
            cfm->length = UART_IND_NTF_CFG_MAX_LEN;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;
				
				case UART_IDX_RX_VAL:
        {
            cfm->length = MAX_RX_UART;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;
				

        default:
        {
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GL2C_CODE_ATT_WR_CMD_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
	
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_NO_ERROR;
    uint32_t msg_status = KE_MSG_CONSUMED;
	
		ke_task_id_t  temp_dest_id;

    // to check if confirmation message should be send
    bool send_cfm = true;
	
    // retrieve handle information
    uint8_t att_idx = UART_IDX(param->handle);

   
		switch(att_idx)
		{
			 
				case UART_IDX_TX_VAL_IND_CFG:
				{
					
							if(param->length != UART_IND_NTF_CFG_MAX_LEN)
							{
									status = PRF_ERR_UNEXPECTED_LEN;
							}
							else
							{

				//    status = uart_update_ntf_ind_cfg(conidx, UART_CFG_STABLE_MEAS_IND, PRF_CLI_START_IND, co_read16p(param->value));   //liujian indication
			
							status = device1_uart_update_ntf_ind_cfg(conidx, DEVICE1_UART_CFG_STABLE_MEAS_IND, PRF_CLI_START_NTF, co_read16p(param->value));   //liujian notify
							}
				}break;
				
				case UART_IDX_RX_VAL:
				{

//					  if(conidx == app_host_env_2.connection_index)
//						{
//					        struct device1_uart_rx_data * rx_data = KE_MSG_ALLOC(DEVICE1_UART_RX_DATA_IND,
//                                                               TASK_ADV_DEVICE1,
//                                                               dest_id, device1_uart_rx_data);
//									  rx_data->conidx = conidx;
//										rx_data->length = param->length;
//										memcpy(rx_data->rx_data, param->value, rx_data->length);

//										ke_msg_send(rx_data);			
//					  }
						if(conidx == app_host_env_1.connection_index)
						{
							 struct device1_uart_rx_data * rx_data = KE_MSG_ALLOC(DEVICE1_UART_RX_DATA_IND,
                                                               TASK_APP,
                                                               dest_id, device1_uart_rx_data);
							  rx_data->conidx = conidx;
								rx_data->length = param->length;
								memcpy(rx_data->rx_data, param->value, rx_data->length);

								ke_msg_send(rx_data); 
						}
					
					   status = GAP_ERR_NO_ERROR;	
				}		
					break;

		
				default:
				{
						status = ATT_ERR_REQUEST_NOT_SUPPORTED;
				}break;
		}
   

    if(send_cfm)
    {
			 //printf("notify confirm ....\n");
        //Send write response
        struct gattc_write_cfm * cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return (msg_status);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
	
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t value[10];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = UART_IDX(param->handle);
	
	  printf("att_idx = %d \n", att_idx);

    switch(att_idx)
    {

        case UART_IDX_TX_VAL_IND_CFG:
        {
            value_size = UART_IND_NTF_CFG_MAX_LEN;

						 co_write16p(value, ((uart_env->ntf_ind_cfg[conidx] & DEVICE1_UART_CFG_INTERM_MEAS_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);    //liujian notify 
					
        }break;


        default:
        {
            status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    // Send data to peer device
    struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, value_size);
    cfm->length = value_size;
    memcpy(cfm->value, value, value_size);
    cfm->handle = param->handle;
    cfm->status = status;

    // Send value to peer device.
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY and GATT_INDICATE message meaning
 * that Measurement notification/indication has been correctly sent to peer device
 *
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
   // device1_uart_exe_operation();
		uint8_t conidx = KE_IDX_GET(src_id);
	
//	  printf("device1 uart task gattc_cmp_evt_handler conidx = %d \n", conidx);

    switch(param->operation)
    {
        case GATTC_NOTIFY:
        case GATTC_INDICATE:
        {
            // Send confirmation
            uart_temp_send_rsp_send(conidx, param->status);
            // Go to idle state
            ke_state_set(dest_id, UART_IDLE);
        }
        break;

        default: break;
    }

    return (KE_MSG_CONSUMED);
		

}

static int gattc_write_without_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_cmd const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	
	 return (KE_MSG_CONSUMED);
}


void uart_temp_send_rsp_send(uint8_t conidx, uint8_t status)
{
    // Get the address of the environment
    struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
    if(conidx == app_host_env_1.connection_index)
		{
    struct device1_uart_send_rsp * rsp = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_RSP,
            TASK_APP,
            prf_src_task_get(&uart_env->prf_env, conidx),
            device1_uart_send_rsp);

    rsp->conidx = conidx;
    rsp->status = status;
		ke_msg_send(rsp);
	 }
		
//		else if(conidx == app_host_env_2.connection_index)
//		{
//			struct device1_uart_send_rsp * rsp = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_RSP,
//            TASK_ADV_DEVICE1,
//            prf_src_task_get(&uart_env->prf_env, conidx),
//            device1_uart_send_rsp);

//			rsp->conidx = conidx;
//			rsp->status = status;
//			ke_msg_send(rsp);
//		}
		

    
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
KE_MSG_HANDLER_TAB(device1_uart)
{
    {DEVICE1_UART_ENABLE_REQ,            (ke_msg_func_t) uart_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},
		{GATTC_WRITE_CMD,            (ke_msg_func_t) gattc_write_without_handler},

    {DEVICE1_UART_TEMP_SEND_REQ,         (ke_msg_func_t) uart_temp_send_req_handler},
	
 
};

void device1_uart_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
  //  struct device1_uart_env_tag *uart_env = PRF_ENV_GET(UART, uart);
		struct device1_uart_env_tag* uart_env =  PRF_ENV_GET(AT_CMD_SLAVE, device1_uart);
	
    task_desc->msg_handler_tab = device1_uart_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(device1_uart_msg_handler_tab);
    task_desc->state           = uart_env->state;
    task_desc->idx_max         = UART_IDX_MAX;
}

#endif 


