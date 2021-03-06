/******************************************************************************
	File Name	:	device3_client_uart_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/6

	Description :   
	shan'hai  

	Changed Log	:
		Liujian  2019/12/6			- Creation

*****************************************************************************/
#include "rwip_config.h"

#if (BLE_DEVICE3_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE)

#include "device3_client_uart_task.h"
#include "device3_client_uart.h"

#include "co_utils.h"
#include "gap.h"
#include "att.h"
#include "gattc_task.h"
#include "gapc_task.h"
#include "ke_mem.h"
#include "ke_task.h"


static struct prf_char_def_uuid128 		uart_device3_collector_char[DEVICE3_UART_CHAR_MAX] =
{
    /// Tx Data
    [DEVICE3_UART_TX_CHAR]   = {
                                   { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E},
                                   ATT_MANDATORY,
                                   ATT_CHAR_PROP_NTF
		                           },
    /// Rx data
    [DEVICE3_UART_RX_CHAR]   = {
                                   { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E},
                                   ATT_MANDATORY,
                                   ATT_CHAR_PROP_WR_NO_RESP
		                           },
};


/// State machine used to retrieve Iris service characteristic description information
const struct prf_char_desc_def uart_device3_collector_desc[DEVICE3_UART_DESC_MAX] =
{
    [DEVICE3_UART_DESC_TX_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, ATT_MANDATORY, DEVICE3_UART_TX_CHAR},
};

static  int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if(state == DEVICE3_UART_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);

			  struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
			
     
        if(device3_uart_env->env[conidx]->nb_svc == 0)
        {
            prf_extract_svc_info128(ind, DEVICE3_UART_CHAR_MAX, &uart_device3_collector_char[0],  &device3_uart_env->env[conidx]->bps.chars[0],
                                      DEVICE3_UART_DESC_MAX, &uart_device3_collector_desc[0], &device3_uart_env->env[conidx]->bps.descs[0]);

            //Even if we get multiple responses we only store 1 range
            device3_uart_env->env[conidx]->bps.svc.shdl = ind->start_hdl;
            device3_uart_env->env[conidx]->bps.svc.ehdl = ind->end_hdl;
        }

        device3_uart_env->env[conidx]->nb_svc++;
    }

    return (KE_MSG_CONSUMED);
}




static int device3_client_uart_enable_req_handler(ke_msg_id_t const msgid,
                                   struct device3_client_uart_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	   // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(src_id);
    // Get state
    uint8_t state = ke_state_get(src_id);
	
		conidx = param->connection_index;

	 printf("device3_client_uart_enable_req_handler conidx = %d \n", conidx);
	
    // UART client Task Environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
	
	//	uint8_t uart_state = ke_state_get(((dest_id&0xff) + (conidx << 8)));
	
    //ASSERT_INFO(device2_uart_env != NULL, dest_id, src_id);
//    if((state == DEVICE3_UART_FREE) && (device3_uart_env->env[conidx] == NULL))
		if((state == DEVICE3_UART_FREE) && (device3_uart_env->env[conidx] == NULL))
    {
        // allocate environment variable for task instance
        device3_uart_env->env[conidx] = (struct device3_uart_cnx_env*) ke_malloc(sizeof(struct device3_uart_cnx_env),KE_MEM_ATT_DB);
        memset(device3_uart_env->env[conidx], 0, sizeof(struct device3_uart_cnx_env));

        //Config connection, start discovering
        if(param->con_type == PRF_CON_DISCOVERY)
        {
            //start discovering BPS on peer
           // prf_disc_svc_send(&(device1_uart_env->prf_env), conidx, ATT_SVC_IRIS_UART);
					
					  uint8_t uart_uuid[16] = { 0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};
						
						prf_disc_svc_send_uuid128(&(device3_uart_env->prf_env), conidx, uart_uuid);

            device3_uart_env->env[conidx]->last_uuid_req = 0xffff;

            // Go to DISCOVERING state
						ke_state_set(KE_BUILD_ID(dest_id, conidx), DEVICE3_UART_DISCOVERING);
           // ke_state_set(((dest_id&0xff) + (conidx << 8)) , DEVICE3_UART_DISCOVERING);
        }
        //normal connection, get saved att details
        else
        {
            device3_uart_env->env[conidx]->bps = param->bps;

            //send APP confirmation that can start normal connection to TH
            device3_uart_client_enable_rsp_send(device3_uart_env, conidx, GAP_ERR_NO_ERROR);
        }
    }

    else if(state != DEVICE3_UART_FREE)
    {
        status = PRF_ERR_REQ_DISALLOWED;
    }

    // send an error if request fails
    if(status != GAP_ERR_NO_ERROR)
    {
        device3_uart_client_enable_rsp_send(device3_uart_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
	
}


static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gattc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{

    // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
    // Status
    uint8_t status;
		
    if (device3_uart_env != NULL)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        printf("\n device3 gattc_cmp_evt_handler... src_id = %d ,dest_id = %d, conidx = %d state = %d ..\n", src_id, dest_id, conidx, state);

        if(state == DEVICE3_UART_DISCOVERING)
        {
            status = param->status;

            if ((status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
                (status == ATT_ERR_NO_ERROR))
            {

                if(device3_uart_env->env[conidx]->nb_svc == 1)
                {
                    status = prf_check_svc_char_validity128(DEVICE3_UART_CHAR_MAX,
                            device3_uart_env->env[conidx]->bps.chars,
                            uart_device3_collector_char);
                }
                // too much services
                else if (device3_uart_env->env[conidx]->nb_svc > 1)
                {
                    status = PRF_ERR_MULTIPLE_SVC;
                }
                // no services found
                else
                {
                    status = PRF_ERR_STOP_DISC_CHAR_MISSING;
                }

                // check descriptor validity
                if (status == GAP_ERR_NO_ERROR)
                {
					
                    status = prf_check_svc_char_desc_validity(DEVICE3_UART_DESC_MAX,
                            device3_uart_env->env[conidx]->bps.descs,
                            uart_device3_collector_desc,
                            device3_uart_env->env[conidx]->bps.chars);
									//printf("status 1 = %d \n", status);
                }
            }
					
            device3_uart_client_enable_rsp_send(device3_uart_env, conidx, status);
        }

        else if(DEVICE3_UART_BUSY)
        {
            switch(param->operation)
            {

                case GATTC_WRITE:
								{
									 printf("device3 enable server ok \n");
									 struct device3_uart_client_cfg_indntf_rsp *wr_cfm = KE_MSG_ALLOC(
                            DEVICE3_UART_CFG_INDNTF_RSP,
                            prf_dst_task_get(&(device3_uart_env->prf_env), conidx),
                            dest_id,
                            device3_uart_client_cfg_indntf_rsp);
                    //it will be a GATT status code
                    wr_cfm->status    = param->status;
                    // send the message
                    ke_msg_send(wr_cfm);
									
								}
								break;
								
                case GATTC_WRITE_NO_RESPONSE:
                {
									 printf("device3 send data ok \n");
                    struct device3_uart_client_tx_data_rsp *wr_cfm = KE_MSG_ALLOC(
                            DEVICE3_UART_CLIENT_TX_DATA_RSP,
                            prf_dst_task_get(&(device3_uart_env->prf_env), conidx),
                            dest_id,
                            device3_uart_client_tx_data_rsp);
                    //it will be a GATT status code
                    wr_cfm->status    = param->status;
                    // send the message
                    ke_msg_send(wr_cfm);
                }
                break;

                case GATTC_READ:
                {
                    if(param->status != GAP_ERR_NO_ERROR)
                    {
                         prf_client_att_info_rsp(
                                (prf_env_t*) device3_uart_env->env[conidx],
                                conidx, DEVICE3_UART_RD_TX_CHAR_RSP,
                                param->status, NULL);
                    }
                }
                break;

                default: break;
            }
						
						ke_state_set(dest_id, DEVICE3_UART_IDLE);
            ke_state_set(prf_src_task_get(&device3_uart_env->prf_env, conidx), DEVICE3_UART_IDLE);
        }
    }
    return (KE_MSG_CONSUMED);
}


static int uart_gattc_read_ind_handler(ke_msg_id_t const msgid,
        struct gattc_read_ind const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
		  // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    prf_client_att_info_rsp(&device3_uart_env->prf_env, conidx, DEVICE3_UART_RD_TX_CHAR_RSP,
            GAP_ERR_NO_ERROR, param);

    return (KE_MSG_CONSUMED);

}


static int uart_gattc_event_ind_handler(ke_msg_id_t const msgid,
                                        struct gattc_event_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
	  uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);

//		printf("uart_gattc_event_ind_handler conidx = %d \n", conidx);
	
    if(device3_uart_env->env[conidx] != NULL)
    {
        switch(param->type)
        {

            case GATTC_NOTIFY:
            {
                if((param->handle == device3_uart_env->env[conidx]->bps.chars[DEVICE3_UART_TX_CHAR].val_hdl))
                {
                    //build a UART_RX_DATA_IND message 
                    struct device3_uart_client_rx_data_ind * ind = KE_MSG_ALLOC(
                            DEVICE3_UART_CLIENT_RX_DATA_IND,
                            prf_dst_task_get(&(device3_uart_env->prf_env), conidx),
                            prf_src_task_get(&(device3_uart_env->prf_env), conidx),
                            device3_uart_client_rx_data_ind);

              
                   // ind->flag = ((param->type == GATTC_NOTIFY) ? 0x01 : 0x00);
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

/*
???????? server ?? ????

*/
static int device3_uart_cfg_indntf_req_handler(ke_msg_id_t const msgid,
                                       struct device3_uart_client_cfg_indntf_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
   struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
	
    // Get connection index
    uint8_t conidx = KE_IDX_GET(src_id);
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
	
		uint8_t state = ke_state_get(src_id);
	
	  uint8_t msg_status = KE_MSG_CONSUMED;
	

	  if(state == DEVICE3_UART_IDLE)
		{
				 if((param->cfg_val == PRF_CLI_STOP_NTFIND) || (param->cfg_val == PRF_CLI_START_NTF))
         {
						status = PRF_ERR_INVALID_PARAM;
					
					  uint16_t cfg_hdl = device3_uart_env->env[conidx]->bps.descs[DEVICE3_UART_DESC_TX_CLI_CFG].desc_hdl;
					
					  if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
						{
							
								device3_uart_env->env[conidx]->last_char_code = DEVICE3_UART_DESC_TX_CLI_CFG;
							
							  ke_state_set(KE_BUILD_ID(dest_id, conidx), DEVICE3_UART_BUSY);
								// Send GATT Write Request
								prf_gatt_write_ntf_ind(&device3_uart_env->prf_env, conidx, cfg_hdl, param->cfg_val);
							
								// Set status
								status = GAP_ERR_NO_ERROR;
						}
				 }
				 else
				 {
					   status = PRF_APP_ERROR;
				 }		
	  }
		
		else if(state == DEVICE3_UART_FREE)
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
        struct device3_uart_client_cfg_indntf_rsp *rsp = KE_MSG_ALLOC(DEVICE3_UART_CFG_INDNTF_RSP, src_id, dest_id, device3_uart_client_cfg_indntf_rsp);

        // It will be an PRF status code
        rsp->status    = status;

        // Send the message
        ke_msg_send(rsp);
    }

    return (msg_status);
}

static int uart_rd_char_req_handler(ke_msg_id_t const msgid,
                                        struct device3_uart_client_rd_char_req const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

	//  printf("uart_rd_char_req_handler ...\n");
    // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if((state == DEVICE3_UART_IDLE) && (device3_uart_env->env[conidx] != NULL))
    {
        uint16_t search_hdl = ATT_INVALID_SEARCH_HANDLE;

        if((param->char_code & DEVICE3_UART_DESC_MASK) == DEVICE3_UART_DESC_MASK)
        {
            search_hdl = device3_uart_env->env[conidx]->bps.descs[param->char_code & ~DEVICE3_UART_DESC_MASK].desc_hdl;
        }
        else
        {
            search_hdl = device3_uart_env->env[conidx]->bps.chars[param->char_code].val_hdl;
        }

        //check if handle is viable
        if (search_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_read_char_send(
                    &(device3_uart_env->prf_env), conidx,
                    device3_uart_env->env[conidx]->bps.svc.shdl,
                    device3_uart_env->env[conidx]->bps.svc.ehdl,
                    search_hdl);

            status = GAP_ERR_NO_ERROR;
        }
        else
        {
            status = PRF_ERR_INEXISTENT_HDL;
        }
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        prf_client_att_info_rsp(
                (prf_env_t*) device3_uart_env->env[conidx],
                conidx, DEVICE3_UART_RD_TX_CHAR_RSP,
                status, NULL);
    }

    return (KE_MSG_CONSUMED);
}


static int device3_uart_tx_data_to_server_req_handler(ke_msg_id_t const msgid,
                                       struct device3_uart_client_tx_data_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
	 // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(src_id);
    uint8_t status = GAP_ERR_NO_ERROR;

    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(src_id);
	
  printf("device3_uart_tx_data_to_server_req_handler state = %d src_id = %d, dest_id = %d, conidx = %d \n", state, src_id, dest_id, conidx);
	
    if(state == DEVICE3_UART_IDLE)
    {

        if(device3_uart_env->env[conidx] != NULL)
        {
            //this is mandatory readable if it is included in the peer's DB
            if (device3_uart_env->env[conidx]->bps.chars[DEVICE3_UART_RX_CHAR].char_hdl != ATT_INVALID_SEARCH_HANDLE)
            {
              //  if ((device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)
								if ((device3_uart_env->env[conidx]->bps.chars[DEVICE3_UART_RX_CHAR].prop & ATT_CHAR_PROP_WR_NO_RESP) == ATT_CHAR_PROP_WR_NO_RESP)
                {
                    device3_uart_env->env[conidx]->last_char_code = DEVICE3_UART_RX_CHAR;

//                    // Send GATT Write Request
//                    prf_gatt_write(&device1_uart_env->prf_env, conidx, device1_uart_env->env[conidx]->bps.chars[DEVICE1_UART_RX_CHAR].val_hdl,
//                            (uint8_t *)param->value, param->length, GATTC_WRITE);
									
									// Send GATT Write Request
                    prf_gatt_write(&device3_uart_env->prf_env, conidx, device3_uart_env->env[conidx]->bps.chars[DEVICE3_UART_RX_CHAR].val_hdl,
                            (uint8_t *)param->value, param->length, GATTC_WRITE_NO_RESPONSE);
									
									
                    // Go to the Busy state
                    ke_state_set(KE_BUILD_ID(dest_id, conidx), DEVICE3_UART_BUSY);
                }
                //write not allowed, so no point in continuing
                else
                {
                    status = PRF_ERR_NOT_WRITABLE;
                }
            }
            //send app error indication for inexistent handle for this characteristic
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
    else if (state == DEVICE3_UART_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
			  printf("device3_uart_tx_data_to_server_req_handler saved ..... \n");
        msg_status = KE_MSG_CONSUMED;
        status = GAP_ERR_NO_ERROR;
    }

    if(status != GAP_ERR_NO_ERROR)
    {
        struct device3_uart_client_tx_data_rsp *rsp = KE_MSG_ALLOC(DEVICE3_UART_CLIENT_TX_DATA_RSP, src_id, dest_id, device3_uart_client_tx_data_rsp);
        rsp->status    = status;
        // Send the message
        ke_msg_send(rsp);
    }

    return (msg_status);
	
}


/// Default State handlers definition
KE_MSG_HANDLER_TAB(device3_client_uart)
{
    {GATTC_SDP_SVC_IND,             								(ke_msg_func_t)gattc_sdp_svc_ind_handler},
		
    {DEVICE3_CLIENT_UART_ENABLE_REQ,                (ke_msg_func_t)device3_client_uart_enable_req_handler},
		
    {GATTC_CMP_EVT,                 								(ke_msg_func_t)gattc_cmp_evt_handler},
		
 //   {DEVICE3_UART_RD_TX_CHAR_REQ,           								(ke_msg_func_t)uart_rd_char_req_handler},
		
    {GATTC_READ_IND,                (ke_msg_func_t)uart_gattc_read_ind_handler},
		
    {GATTC_EVENT_IND,               (ke_msg_func_t)uart_gattc_event_ind_handler},
		
  //  {GATTC_EVENT_REQ_IND,           (ke_msg_func_t)uart_gattc_event_ind_handler},
		
    {DEVICE3_UART_CFG_INDNTF_REQ,           (ke_msg_func_t)device3_uart_cfg_indntf_req_handler},
		
		{DEVICE3_UART_CLIENT_TX_DATA_REQ,       (ke_msg_func_t)device3_uart_tx_data_to_server_req_handler},
};

void device3_connector_uart_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct device3_uart_client_env_tag *device3_uart_env = PRF_ENV_GET(DEVICE3_COLLECTOR_UART, device3_uart_client);

    task_desc->msg_handler_tab = device3_client_uart_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(device3_client_uart_msg_handler_tab);
    task_desc->state           = device3_uart_env->state;
    task_desc->idx_max         = UART_COUNT_IDX_MAX;
}


#endif

