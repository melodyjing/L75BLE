/******************************************************************************
	File Name	:	voice_master_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   连接声音模块的 host profile task

	Changed Log	:
		Liujian  2020/03/07		- Creation

*****************************************************************************/

#include "voice_master_task.h"

#if BLE_APP_RECEIVE_VOICE

static struct prf_char_def_uuid128 		voice_master_char[VOICE_MASTER_CHAR_MAX] =
{
    /// voice begin
    [VOICE_MASTER_BEGIN_CHAR]            = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x01, 0xB0, 0x00, 0xF0},
                                   ATT_MANDATORY,
                                   ATT_CHAR_PROP_NTF},
    /// voice data
    [VOICE_MASTER_DATA_CHAR]            = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x02, 0xB0, 0x00, 0xF0},
                                   ATT_MANDATORY,
                                   ATT_CHAR_PROP_NTF},
		
};


const struct prf_char_desc_def voice_master_desc[VOICE_MASTER_DESC_MAX] =
{
    /// voice start client config
    [VOICE_MASTER_DESC_BEGIN_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, ATT_MANDATORY, VOICE_MASTER_BEGIN_CHAR},
    /// voice data client config
   [VOICE_MASTER_DESC_DATA_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, ATT_MANDATORY, VOICE_MASTER_DATA_CHAR},
};

static  int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
		uint8_t state = ke_state_get(dest_id);

    if(state == VOICE_MASTER_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);

       
				 // Get the address of the environment
			  struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);
			
        ASSERT_INFO(voice_master_env != NULL, dest_id, src_id);
        ASSERT_INFO(voice_master_env->env[conidx] != NULL, dest_id, src_id);

        if(voice_master_env->env[conidx]->nb_svc == 0)
        {
			//			printf("gattc_sdp_svc_ind_handler %d  %d \n", ind->start_hdl, ind->end_hdl);
            // Retrieve BLS characteristics and descriptors
            prf_extract_svc_info128(ind, VOICE_MASTER_CHAR_MAX, &voice_master_char[0],  &voice_master_env->env[conidx]->bps.chars[0],
                                      VOICE_MASTER_DESC_MAX, &voice_master_desc[0], &voice_master_env->env[conidx]->bps.descs[0]);

            //Even if we get multiple responses we only store 1 range
            voice_master_env->env[conidx]->bps.svc.shdl = ind->start_hdl;
            voice_master_env->env[conidx]->bps.svc.ehdl = ind->end_hdl;
        }

        voice_master_env->env[conidx]->nb_svc++;
    }

	//	app_device1_enable_blood_report();
    return (KE_MSG_CONSUMED);
}


static int voice_master_enable_req_handler(ke_msg_id_t const msgid,
                                   struct voice_master_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	   // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get state
    uint8_t state = ke_state_get(dest_id);
	
	//  printf("old conidx = %d \n", conidx);
		conidx = param->connection_index;
	
		uint8_t voice_master_state = ke_state_get(((dest_id&0xff) + (conidx << 8)));
	
    // UART client Task Environment
    struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);

    ASSERT_INFO(voice_master_env != NULL, dest_id, src_id);
	
		if((voice_master_state == VOICE_MASTER_FREE) && (voice_master_env->env[conidx] == NULL))
    {
		//		printf("begin found servers ......\n");
        // allocate environment variable for task instance
        voice_master_env->env[conidx] = (struct voice_master_cnx_env*) ke_malloc(sizeof(struct voice_master_cnx_env),KE_MEM_ATT_DB);
        memset(voice_master_env->env[conidx], 0, sizeof(struct voice_master_cnx_env));

        //Config connection, start discovering
        if(param->con_type == PRF_CON_DISCOVERY)
        {
        
					 //声音设备的 profile 
					  uint8_t uart_uuid[16] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x00, 0xB0, 0x00, 0xF0};
						
						prf_disc_svc_send_uuid128(&(voice_master_env->prf_env), conidx, uart_uuid);

            voice_master_env->env[conidx]->last_uuid_req = 0xffff;

            // Go to DISCOVERING state
         //   ke_state_set(dest_id, DEVICE1_UART_DISCOVERING);
						
						ke_state_set(((dest_id&0xff) + (conidx << 8)) , VOICE_MASTER_DISCOVERING);
						
        }
        //normal connection, get saved att details
        else
        {
            voice_master_env->env[conidx]->bps = param->bps;

            //send APP confirmation that can start normal connection to voice device
            voice_master_enable_rsp_send(voice_master_env, conidx, GAP_ERR_NO_ERROR);
        }
    }

    else if(state != VOICE_MASTER_FREE)
    {
        status = PRF_ERR_REQ_DISALLOWED;
    }

    // send an error if request fails
    if(status != GAP_ERR_NO_ERROR)
    {
        voice_master_enable_rsp_send(voice_master_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
	
}


static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gattc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{

    // Get the address of the environment
    struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);
    // Status
    uint8_t status;

	
    if (voice_master_env != NULL)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        if(state == VOICE_MASTER_DISCOVERING)
        {
            status = param->status;

            if ((status == ATT_ERR_ATTRIBUTE_NOT_FOUND)|| (status == ATT_ERR_NO_ERROR))
            {
                // Discovery
                // check characteristic validity
//								printf("ok333333333333333333333333333333333333333 \n");
                if(voice_master_env->env[conidx]->nb_svc == 1)
                {
                    status = prf_check_svc_char_validity128(VOICE_MASTER_CHAR_MAX,
                            voice_master_env->env[conidx]->bps.chars,
                            voice_master_char);
//									printf("ok44444444444444444444444444444 \n");
//									printf("status 3 = %d \n", status);
                }
                // too much services
                else if (voice_master_env->env[conidx]->nb_svc > 1)
                {
//									 printf("device1_uart_env->env[conidx]->nb_svc = %d \n", voice_master_env->env[conidx]->nb_svc);
                    status = PRF_ERR_MULTIPLE_SVC;
                }
                else
                {
//										printf("no server \n");
                    status = PRF_ERR_STOP_DISC_CHAR_MISSING;
                }

                // check descriptor validity
                if (status == GAP_ERR_NO_ERROR)
                {
//										printf("ok666666666666666666666 \n");
                    status = prf_check_svc_char_desc_validity(VOICE_MASTER_DESC_MAX,
                            voice_master_env->env[conidx]->bps.descs,
                            voice_master_desc,
                            voice_master_env->env[conidx]->bps.chars);
							//		printf("status 1 = %d \n", status);
                }
            }
					
            voice_master_enable_rsp_send(voice_master_env, conidx, status);
        }

        else if(state == VOICE_MASTER_BUSY)
        {
		//			printf("VOICE_MASTER_BUSY ..... \n");
		
            switch(param->operation)
            {

                case GATTC_WRITE:
                case GATTC_WRITE_NO_RESPONSE:
                {
                    struct voice_master_cfg_indntf_rsp *wr_cfm = KE_MSG_ALLOC(
                            VOICE_MASTER_CFG_INDNTF_RSP,
                            prf_dst_task_get(&(voice_master_env->prf_env), conidx),
                            prf_src_task_get(&(voice_master_env->prf_env), conidx),
                            voice_master_cfg_indntf_rsp);
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
                                (prf_env_t*) voice_master_env->env[conidx],
                                conidx, VOICE_MASTER_START_CHAR_RSP,
                                param->status, NULL);
                    }
                }
                break;

                default: break;
            }
			//			printf("conidx = %d, task id = %d \n", conidx, prf_src_task_get(&device1_uart_env->prf_env, conidx) );
            ke_state_set(prf_src_task_get(&voice_master_env->prf_env, conidx), VOICE_MASTER_IDLE);
		
        }
    }
    return (KE_MSG_CONSUMED);
}

/*
使能语音采集设备
*/
static int voice_master_cfg_indntf_req_handler(ke_msg_id_t const msgid,
                                       struct voice_master_cfg_indntf_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
    // Get the address of the environment
    struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);
	
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
	
		conidx = param->connection_index;
	//	 printf("111111111111111111111 conidx= %d  1111111111111111111111 \n", conidx);
	
	  uint8_t voice_master_state = ke_state_get(((dest_id&0xff) + (conidx << 8)));
	
   // if((device1_uart_env->env[conidx] != NULL) && (ke_state_get(dest_id) == DEVICE1_UART_IDLE))
	
		if((voice_master_env->env[conidx] != NULL) && (voice_master_state == VOICE_MASTER_IDLE))
    {
        status = PRF_ERR_INVALID_PARAM;
        //get handle of the configuration characteristic to set and check if value matches property
        switch(param->char_code)
        {
            case VOICE_MASTER_DATA_CHAR://can only IND
            {
				
                if((param->cfg_val == PRF_CLI_STOP_NTFIND)||
                     (param->cfg_val == PRF_CLI_START_NTF))
                {
										
                    cfg_hdl = voice_master_env->env[conidx]->bps.descs[VOICE_MASTER_DESC_DATA_CLI_CFG].desc_hdl;
                }
            }break;
						case VOICE_MASTER_BEGIN_CHAR:
						{
							 if((param->cfg_val == PRF_CLI_STOP_NTFIND)||
                     (param->cfg_val == PRF_CLI_START_NTF))
                {
										
                    cfg_hdl = voice_master_env->env[conidx]->bps.descs[VOICE_MASTER_DESC_BEGIN_CLI_CFG].desc_hdl;
                }
						}
						break;

            default:
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }break;
        }
				
			//	printf("uart_cfg_indntf_req_handler 555555 \n");
				
        //check if the handle value exists
        if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
						//printf("device1 uart_cfg_indntf_req_handler 6666 \n");
            // Send GATT Write Request
            prf_gatt_write_ntf_ind(&voice_master_env->prf_env, param->connection_index, cfg_hdl, param->cfg_val);
            // Set status
            status = GAP_ERR_NO_ERROR;
        }
				// printf("device1 uart_cfg_indntf_req_handler 4444444 \n");
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        struct voice_master_cfg_indntf_rsp *rsp = KE_MSG_ALLOC(VOICE_MASTER_CFG_INDNTF_RSP, src_id, dest_id, voice_master_cfg_indntf_rsp);

        // It will be an PRF status code
        rsp->status    = status;

        // Send the message
        ke_msg_send(rsp);
    }

    return (KE_MSG_CONSUMED);
}

static int voice_master_event_ind_handler(ke_msg_id_t const msgid,
                                        struct gattc_event_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
	  uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);
	
	//	printf("uart_gattc_event_ind_handler conidx = %d \n", conidx);
	
    if(voice_master_env->env[conidx] != NULL)
    {
        switch(param->type)
        {
            case GATTC_INDICATE:
            {
                // confirm that indication has been correctly received
                struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
                cfm->handle = param->handle;
                ke_msg_send(cfm);
            }
            /* no break */

            case GATTC_NOTIFY:
            {
                if((param->handle == voice_master_env->env[conidx]->bps.chars[VOICE_MASTER_BEGIN_CHAR].val_hdl)
                    || ((param->handle == voice_master_env->env[conidx]->bps.chars[VOICE_MASTER_DATA_CHAR].val_hdl)))
                {
									
                    //build a VOICE_MASTER_RX_DATA_IND message 
                    struct voice_master_rx_voice_data_ind * ind = KE_MSG_ALLOC(
                            VOICE_MASTER_RX_DATA_IND,
                            prf_dst_task_get(&(voice_master_env->prf_env), conidx),
                            prf_src_task_get(&(voice_master_env->prf_env), conidx),
                            voice_master_rx_voice_data_ind);

              
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


/// Default State handlers definition
KE_MSG_HANDLER_TAB(voice_master)
{
    {GATTC_SDP_SVC_IND,             				 (ke_msg_func_t)gattc_sdp_svc_ind_handler},
		
    {VOICE_MASTER_ENABLE_REQ,                (ke_msg_func_t)voice_master_enable_req_handler},
		
    {GATTC_CMP_EVT,                 				 (ke_msg_func_t)gattc_cmp_evt_handler},
//		
//    {UART_RD_TX_CHAR_REQ,           								(ke_msg_func_t)uart_rd_char_req_handler},
//		
 //   {GATTC_READ_IND,                (ke_msg_func_t)uart_gattc_read_ind_handler},
		
    {GATTC_EVENT_IND,               (ke_msg_func_t)voice_master_event_ind_handler},
//		
//    {GATTC_EVENT_REQ_IND,           (ke_msg_func_t)uart_gattc_event_ind_handler},
//		
    {VOICE_MASTER_CFG_INDNTF_REQ,           (ke_msg_func_t)voice_master_cfg_indntf_req_handler},
//		
//		{UART_CLIENT_TX_DATA_REQ,       (ke_msg_func_t)device1_uart_tx_data_to_server_req_handler},
};



void voice_master_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct voice_master_env_tag *voice_master_env = PRF_ENV_GET(VOICE_MASTER, voice_master);

    task_desc->msg_handler_tab = voice_master_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(voice_master_msg_handler_tab);
    task_desc->state           = voice_master_env->state;
    task_desc->idx_max         = VOICE_COUNT_IDX_MAX;
}

#endif
