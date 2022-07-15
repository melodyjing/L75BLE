/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description: 国网三主两从方案

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/
#include "rwip_config.h"

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#include "master_com.h"
#include "master_com_task.h"
#include "co_utils.h"
#include "gap.h"
#include "att.h"
#include "gattc_task.h"
#include "gapc_task.h"
#include "ke_mem.h"
#include "ke_task.h"
#include "l2cc.h"
#include "prf_utils.h"

static struct prf_char_def_uuid128 		uart_device1_collector_char[DEVICE1_UART_CHAR_MAX] =
{
    /// Tx Data
	  // 数据发送UUID
    [DEVICE1_UART_TX_CHAR]  = {
                                { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E},
                                ATT_MANDATORY,
                                ATT_CHAR_PROP_NTF
                              },
    /// Rx data
		//数据接收 UUID
    [DEVICE1_UART_RX_CHAR]  = {
                                 { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E},
																   ATT_MANDATORY,
                                   ATT_CHAR_PROP_WR_NO_RESP
		                          },
	
};



/// State machine used to retrieve iris service characteristic description information
const struct prf_char_desc_def uart_device1_collector_desc[DEVICE1_UART_DESC_MAX] =
{
    [DEVICE1_UART_DESC_TX_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, ATT_MANDATORY, DEVICE1_UART_TX_CHAR},
};

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_SDP_SVC_IND_HANDLER message.
 * The handler stores the found service details for service discovery.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static  int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if(state == DEVICE1_UART_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
			
				 // Get the address of the environment
			  struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);

        if(device1_uart_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve iris characteristics and descriptors
            prf_extract_svc_info128(ind, DEVICE1_UART_CHAR_MAX, &uart_device1_collector_char[0],  &device1_uart_env->env[conidx]->bps.chars[0],
                                      DEVICE1_UART_DESC_MAX, &uart_device1_collector_desc[0], &device1_uart_env->env[conidx]->bps.descs[0]);

            //Even if we get multiple responses we only store 1 range
            device1_uart_env->env[conidx]->bps.svc.shdl = ind->start_hdl;
            device1_uart_env->env[conidx]->bps.svc.ehdl = ind->end_hdl;
        }

        device1_uart_env->env[conidx]->nb_svc++;
    }
    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles reception of the @ref DEVICE1_CLIENT_UART_ENABLE_REQ message.
 * The handler enables the Iris Profile Collector Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int device1_client_uart_enable_req_handler(ke_msg_id_t const msgid,
                                   struct device1_client_uart_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	   // Status
    uint8_t status     = PRF_ERR_REQ_DISALLOWED;
    // uint8_t connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get state
    uint8_t state = ke_state_get(dest_id);
	
	  printf("device1_client_uart_enable_req_handler src_id = %d, conidx = %d dest_id =%d \n", src_id, conidx, dest_id);

    // Iris Client Role Task Environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);

	  // Config connection, start discovering
		if((state == DEVICE1_UART_FREE) && (device1_uart_env->env[conidx] == NULL))
    {

        device1_uart_env->env[conidx] = (struct device1_uart_cnx_env*) ke_malloc(sizeof(struct device1_uart_cnx_env),KE_MEM_ATT_DB);
        memset(device1_uart_env->env[conidx], 0, sizeof(struct device1_uart_cnx_env));

        //Config connection, start discovering
        if(param->con_type == PRF_CON_DISCOVERY)
        {
            //start discovering BPS on peer
           // prf_disc_svc_send(&(device1_uart_env->prf_env), conidx, ATT_SVC_IRIS_UART);
					
					  uint8_t uart_uuid[16] = { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};
						
						prf_disc_svc_send_uuid128(&(device1_uart_env->prf_env), conidx, uart_uuid);

            device1_uart_env->env[conidx]->last_uuid_req = 0xffff;

            // Go to DISCOVERING state
            ke_state_set(dest_id, DEVICE1_UART_DISCOVERING);
						
        }
        //normal connection, get saved att details
        else
        {
            device1_uart_env->env[conidx]->bps = param->bps;

            //send APP confirmation that can start normal connection to TH
            device1_uart_client_enable_rsp_send(device1_uart_env, conidx, GAP_ERR_NO_ERROR);
        }
    }

    // send an error if request fails
    if(status != GAP_ERR_NO_ERROR)
    {
        device1_uart_client_enable_rsp_send(device1_uart_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
	
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gattc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{

    // Get the address of the environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);
    // Status
    uint8_t status;

	  
    if (device1_uart_env != NULL)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);
			
			 // printf("gattc_cmp_evt_handler dest_id = %d \n", dest_id);
			
        if(state == DEVICE1_UART_DISCOVERING)
        {
            status = param->status;
					
            if ((status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||(status == ATT_ERR_NO_ERROR))
            {
                // Discovery
                // check characteristic validity

                if(device1_uart_env->env[conidx]->nb_svc == 1)
                {
                    status = prf_check_svc_char_validity128(DEVICE1_UART_CHAR_MAX,
                            device1_uart_env->env[conidx]->bps.chars,
                            uart_device1_collector_char);
                }
                // too much services
                else if (device1_uart_env->env[conidx]->nb_svc > 1)
                {
                    status = PRF_ERR_MULTIPLE_SVC;
                }

                else
                {
                    status = PRF_ERR_STOP_DISC_CHAR_MISSING;
                }

                // check descriptor validity
                if (status == GAP_ERR_NO_ERROR)
                {
                    status = prf_check_svc_char_desc_validity(DEVICE1_UART_DESC_MAX,
                            device1_uart_env->env[conidx]->bps.descs,
                            uart_device1_collector_desc,
                            device1_uart_env->env[conidx]->bps.chars);
                }
            }
					
            device1_uart_client_enable_rsp_send(device1_uart_env, conidx, status);
        }

        else if(state == DEVICE1_UART_BUSY)
        {
            switch(param->operation)
            {

                case GATTC_WRITE:
								{

										device1_uart_client_send_cmp_evt(device1_uart_env, conidx, UART_CFG_INDNTF_CMD_OP_CODE, param->status);
									
								}
								break;
                case GATTC_WRITE_NO_RESPONSE:
                {
											device1_uart_client_send_cmp_evt(device1_uart_env, conidx, UART_TX_DATA_CMD_OP_CODE, param->status);
                }
                break;


                default: break;
            }
			      //ke_state_set(dest_id, DEVICE1_UART_IDLE);
            ke_state_set(prf_src_task_get(&device1_uart_env->prf_env, conidx), DEVICE1_UART_IDLE);
        }
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_EVENT_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int uart_gattc_event_ind_handler(ke_msg_id_t const msgid,
                                        struct gattc_event_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
	  uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);

    if(device1_uart_env->env[conidx] != NULL)
    {
        switch(param->type)
        {

            case GATTC_NOTIFY:
            {
                if(param->handle == device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_TX_CHAR].val_hdl) 
                {
                    //build a UART_RX_DATA_IND message 
                    struct uart_client_rx_data_ind * ind = KE_MSG_ALLOC(
                            UART_CLIENT_RX_DATA_IND,
                            prf_dst_task_get(&(device1_uart_env->prf_env), conidx),
                            prf_src_task_get(&(device1_uart_env->prf_env), conidx),
                            uart_client_rx_data_ind);
										
										ind->length = param->length;
										memcpy(ind->value, (uint8_t*)param->value, ind->length);
                    ke_msg_send(ind);
                }
            } break;

            default: break;
        }
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UART_CFG_INDNTF_REQ message.
 * It allows configuration of the peer ind/ntf/stop characteristic for a specified characteristic.
 * Will return an error code if that cfg char does not exist.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int uart_cfg_indntf_req_handler(ke_msg_id_t const msgid,
                                       struct uart_client_cfg_indntf_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
    // Get the address of the environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);
	
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
	
		uint8_t state = ke_state_get(dest_id);
	
	  uint8_t msg_status = KE_MSG_CONSUMED;
	

	  if(state == DEVICE1_UART_IDLE)
		{
				 if((param->cfg_val == PRF_CLI_STOP_NTFIND) || (param->cfg_val == PRF_CLI_START_NTF))
         {
						status = PRF_ERR_INVALID_PARAM;
					
					  cfg_hdl = device1_uart_env->env[conidx]->bps.descs[DEVICE1_UART_DESC_TX_CLI_CFG].desc_hdl;
					
					  if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
						{
							
								device1_uart_env->env[conidx]->last_char_code = DEVICE1_UART_DESC_TX_CLI_CFG;
							
							  ke_state_set(dest_id, DEVICE1_UART_BUSY);
								// Send GATT Write Request
								prf_gatt_write_ntf_ind(&device1_uart_env->prf_env, conidx, cfg_hdl, param->cfg_val);
							
								// Set status
								status = GAP_ERR_NO_ERROR;
						}
				 }
				 else
				 {
					   status = PRF_APP_ERROR;
				 }
							
	  }
		
		else if(state == DEVICE1_UART_FREE)
		{
			status = GAP_ERR_DISCONNECTED;
		}
		
		else
		{
			  msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
		}

    if (status != GAP_ERR_NO_ERROR)
    {
				  device1_uart_client_send_cmp_evt(device1_uart_env, conidx, UART_CFG_INDNTF_CMD_OP_CODE, status);
    }

    return (msg_status);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref DEVICE1_UART_CLIENT_TX_DATA_REQ message.
 * Check if the handle exists in profile(already discovered) and send request, otherwise
 * error to APP.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
extern struct l2cc_env_tag *l2cc_env[BLE_CONNECTION_MAX];

static int device1_uart_tx_data_to_server_req_handler(ke_msg_id_t const msgid,
                                       struct uart_client_tx_data_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
	 // Get the address of the environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
	
    uint8_t status = GAP_ERR_NO_ERROR;
	
		uint8_t state = ke_state_get(dest_id);
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
	
//	  app_iris_decrease_msg();

 //   if((state == DEVICE1_UART_IDLE) && (device1_uart_env->env[conidx] != NULL) && (l2cc_env[conidx]!= NULL) && (l2cc_env[conidx]->nb_tx_pending < 2))
    if(state == DEVICE1_UART_IDLE)   
		{

			  if(device1_uart_env->env[conidx] != NULL)
				{
				   //this is mandatory readable if it is included in the peer's DB
				   if (device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].char_hdl != ATT_INVALID_SEARCH_HANDLE)
				   {
					       //  if ((device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)
						    if ((device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].prop & ATT_CHAR_PROP_WR_NO_RESP) == ATT_CHAR_PROP_WR_NO_RESP)
						    {
								        device1_uart_env->env[conidx]->last_char_code = DEVICE1_UART_RX_CHAR;

								        prf_gatt_write(&device1_uart_env->prf_env, conidx, device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].val_hdl,
												    (uint8_t *)param->value, param->length, GATTC_WRITE_NO_RESPONSE);
								        // Go to the Busy state
								       ke_state_set(dest_id, DEVICE1_UART_BUSY);
						     }
						        //write not allowed, so no point in continuing
						     else
						     {
								       status = PRF_ERR_NOT_WRITABLE;
						     }
				    }
				    else
				    {
						     status = PRF_ERR_INEXISTENT_HDL;
				    }
			   }
				 else
				 {
					     status = PRF_APP_ERROR;
				 }
    }
		else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
//			  app_iris_increase_msg();

    }
		
//    else if (state != DEVICE1_UART_FREE)
//    {
//        status = PRF_ERR_REQ_DISALLOWED;
//    }

//    if(status != GAP_ERR_NO_ERROR)
//    {
//				  device1_uart_client_send_cmp_evt(device1_uart_env, conidx, UART_TX_DATA_CMD_OP_CODE, status);        
//    }
	
    return (msg_status);
	
}



/// Default State handlers definition
KE_MSG_HANDLER_TAB(device1_client_uart)
{
    {GATTC_SDP_SVC_IND,             								(ke_msg_func_t)gattc_sdp_svc_ind_handler},
		
    {DEVICE1_CLIENT_UART_ENABLE_REQ,                (ke_msg_func_t)device1_client_uart_enable_req_handler},
		
    {GATTC_CMP_EVT,                 								(ke_msg_func_t)gattc_cmp_evt_handler},
		
    {GATTC_EVENT_IND,               (ke_msg_func_t)uart_gattc_event_ind_handler},
		
    {UART_CFG_INDNTF_REQ,           (ke_msg_func_t)uart_cfg_indntf_req_handler},
		
		{DEVICE1_UART_CLIENT_TX_DATA_REQ,       (ke_msg_func_t)device1_uart_tx_data_to_server_req_handler},
};

void device1_connector_uart_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct device1_uart_client_env_tag *device1_uart_env = PRF_ENV_GET(DEVICE1_COLLECTOR_UART, device1_uart_client);

    task_desc->msg_handler_tab = device1_client_uart_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(device1_client_uart_msg_handler_tab);
    task_desc->state           = device1_uart_env->state;
    task_desc->idx_max         = UART_COUNT_IDX_MAX;
}


#endif

