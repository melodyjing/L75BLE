/******************************************************************************
	File Name	:	ota_master_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/26

	Description :   
	    用于发现 OTA 设备, 这个是 Master, 用来连接 OTA  设备

	Changed Log	:
		Liujian  2020/8/26		- Creation

*****************************************************************************/

#include "ota_master_task.h"

#if (APP_OTA_MATSER_ENABLE == 1)

#define ATT_SVC_SUOTAR                   {0xF5, 0xFE}
#define SUOTA_MEM_DEV_UUID               {0x34, 0xcc, 0x54, 0xb9, 0xf9, 0x56, 0xc6, 0x91, 0x21, 0x40, 0xa6, 0x41, 0xa8, 0xca, 0x82, 0x80}
#define SUOTA_GPIO_MAP_UUID              {0x51, 0x86, 0xf0, 0x5a, 0x34, 0x42, 0x04, 0x88, 0x5f, 0x4b, 0xc3, 0x5e, 0xf0, 0x49, 0x42, 0x72}
#define SUOTA_MEM_INFO_UUID              {0xd4, 0x4f, 0x33, 0xfb, 0x92, 0x7c, 0x22, 0xa0, 0xfe, 0x45, 0xa1, 0x47, 0x25, 0xdb, 0x53, 0x6c}
#define SUOTA_PATCH_LEN_UUID             {0x31, 0xda, 0x3f, 0x67, 0x5b, 0x85, 0x83, 0x91, 0xd8, 0x49, 0x0c, 0x00, 0xa3, 0xb9, 0x84, 0x9d}
#define SUOTA_PATCH_DATA_UUID            {0xb2, 0x9c, 0x7b, 0xb1, 0xd0, 0x57, 0x16, 0x91, 0xa1, 0x4c, 0x16, 0xd5, 0xe8, 0x71, 0x78, 0x45}
#define SUOTA_SERV_STATUS_UUID           {0x88, 0x5c, 0x06, 0x6a, 0xeb, 0xb3, 0x0a, 0x99, 0xf5, 0x46, 0x8c, 0x79, 0x94, 0xdf, 0x78, 0x5f}


#define SUOTA_VERSION_UUID               {0x3a, 0x91, 0x3b, 0xdb, 0xc8, 0xac, 0x1d, 0xa2, 0x1b, 0x40, 0xe5, 0x0d, 0xb5, 0xe8, 0xb4, 0x64}
#define SUOTA_PATCH_DATA_CHAR_SIZE_UUID  {0x3b, 0xfb, 0x67, 0x52, 0x87, 0x8f, 0x54, 0x84, 0x9c, 0x4d, 0xbe, 0x77, 0xdd, 0xdf, 0xc3, 0x42}
#define SUOTA_MTU_UUID                   {0x3c, 0xe2, 0xfc, 0x3d, 0x90, 0xc4, 0xaf, 0xa3, 0xbb, 0x43, 0x3d, 0x82, 0xea, 0x1e, 0xde, 0xb7}



static struct prf_char_def_uuid128 		ota_master_char[OTA_MASTER_CHAR_MAX] =
{
    /// SUOTA_MEM_DEV_UUID
    [OTA_MEM_DEV_CHAR] = {  SUOTA_MEM_DEV_UUID,
                            ATT_MANDATORY,
                            ATT_CHAR_PROP_RD|ATT_CHAR_PROP_WR},
    /// SUOTA_GPIO_MAP_UUID
    [OTA_GPIO_MAP_CHAR] = {  SUOTA_GPIO_MAP_UUID,
                             ATT_MANDATORY,
                             ATT_CHAR_PROP_RD|ATT_CHAR_PROP_WR},
		/// SUOTA_MEM_INFO_UUID
    [OTA_MEM_INFO_CHAR] = { SUOTA_MEM_INFO_UUID,
                            ATT_MANDATORY,
                            ATT_CHAR_PROP_RD},
    /// SUOTA_PATCH_LEN_UUID
    [OTA_PATCH_LEN_CHAR] = { SUOTA_PATCH_LEN_UUID,
                             ATT_MANDATORY,
                             ATT_CHAR_PROP_RD|ATT_CHAR_PROP_WR},
		/// SUOTA_PATCH_DATA_UUID
    [OTA_PATCH_DATA_CHAR] = {SUOTA_PATCH_DATA_UUID,
                            ATT_MANDATORY,
                            ATT_CHAR_PROP_RD|ATT_CHAR_PROP_WR},
    /// SUOTA_SERV_STATUS_UUID
    [OTA_SERV_STATUS_CHAR] = {SUOTA_SERV_STATUS_UUID,
                             ATT_MANDATORY,
                              ATT_CHAR_PROP_NTF},
		/// SUOTA_VERSION_UUID
    [OTA_VERSION_CHAR] = {  SUOTA_VERSION_UUID,
                            ATT_MANDATORY,
                             ATT_CHAR_PROP_RD},
    /// SUOTA_PATCH_DATA_CHAR_SIZE_UUID
    [OTA_PATCH_DATA_CHAR_SIZE_CHAR] = {SUOTA_PATCH_DATA_CHAR_SIZE_UUID,
                                        ATT_MANDATORY,
                                         ATT_CHAR_PROP_RD},
		/// SUOTA_MTU_UUID
    [OTA_MTU_CHAR] = {      SUOTA_MTU_UUID,
                            ATT_MANDATORY,
                            ATT_CHAR_PROP_RD},
  
			
};

const struct prf_char_desc_def ota_master_desc[OTA_MASTER_DESC_MAX] =
{
    /// ota serv status config
    [OTA_MASTER_DESC_SERV_STATUS_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG, ATT_MANDATORY, OTA_SERV_STATUS_CHAR},

};



static  int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
		uint8_t state = ke_state_get(dest_id);

    if(state == OTA_MASTER_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);

       
				 // Get the address of the environment
			  struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);
			
        ASSERT_INFO(voice_master_env != NULL, dest_id, src_id);
        ASSERT_INFO(voice_master_env->env[conidx] != NULL, dest_id, src_id);

        if(ota_master_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve OTA characteristics and descriptors
            prf_extract_svc_info128(ind, OTA_MASTER_CHAR_MAX, &ota_master_char[0],  &ota_master_env->env[conidx]->bps.chars[0],
                                      OTA_MASTER_DESC_MAX, &ota_master_desc[0], &ota_master_env->env[conidx]->bps.descs[0]);

            //Even if we get multiple responses we only store 1 range
            ota_master_env->env[conidx]->bps.svc.shdl = ind->start_hdl;
            ota_master_env->env[conidx]->bps.svc.ehdl = ind->end_hdl;
        }

        ota_master_env->env[conidx]->nb_svc++;
    }

	//	app_device1_enable_blood_report();
    return (KE_MSG_CONSUMED);
}

static int ota_master_enable_req_handler(ke_msg_id_t const msgid,
                                   struct ota_master_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	   // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get state
    uint8_t state = ke_state_get(dest_id);
	
		conidx = param->connection_index;
	
		uint8_t ota_master_state = ke_state_get(((dest_id&0xff) + (conidx << 8)));
	
    // UART client Task Environment
    struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);

    ASSERT_INFO(voice_master_env != NULL, dest_id, src_id);
	
		if((ota_master_state == OTA_MASTER_FREE) && (ota_master_env->env[conidx] == NULL))
    {
        // allocate environment variable for task instance
        ota_master_env->env[conidx] = (struct ota_master_cnx_env*) ke_malloc(sizeof(struct ota_master_cnx_env),KE_MEM_ATT_DB);
        memset(ota_master_env->env[conidx], 0, sizeof(struct ota_master_cnx_env));

        //Config connection, start discovering
        if(param->con_type == PRF_CON_DISCOVERY)
        {
        
					 //OTA  设备的 profile
					  uint16_t ota_uuid =  0xFEF5;
						
						prf_disc_svc_send(&(ota_master_env->prf_env), conidx, ota_uuid);

            ota_master_env->env[conidx]->last_uuid_req = 0xffff;

						ke_state_set(((dest_id&0xff) + (conidx << 8)) , OTA_MASTER_DISCOVERING);
						
        }
        //normal connection, get saved att details
        else
        {
            ota_master_env->env[conidx]->bps = param->bps;

            //send APP confirmation that can start normal connection to voice device
            ota_master_enable_rsp_send(ota_master_env, conidx, GAP_ERR_NO_ERROR);
        }
    }

    else if(state != OTA_MASTER_FREE)
    {
        status = PRF_ERR_REQ_DISALLOWED;
    }

    // send an error if request fails
    if(status != GAP_ERR_NO_ERROR)
    {
        ota_master_enable_rsp_send(ota_master_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
	
}

static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gattc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{

    // Get the address of the environment
    struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);
    // Status
    uint8_t status;

	
    if (ota_master_env != NULL)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        if(state == OTA_MASTER_DISCOVERING)
        {
            status = param->status;

            if ((status == ATT_ERR_ATTRIBUTE_NOT_FOUND)|| (status == ATT_ERR_NO_ERROR))
            {
                // Discovery
                if(ota_master_env->env[conidx]->nb_svc == 1)
                {
                    status = prf_check_svc_char_validity128(OTA_MASTER_CHAR_MAX,
                            ota_master_env->env[conidx]->bps.chars,
                            ota_master_char);

                }

                else if (ota_master_env->env[conidx]->nb_svc > 1)
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
                    status = prf_check_svc_char_desc_validity(OTA_MASTER_DESC_MAX,
                              ota_master_env->env[conidx]->bps.descs,
                              ota_master_desc,
                              ota_master_env->env[conidx]->bps.chars);
                }
            }
					
            ota_master_enable_rsp_send(ota_master_env, conidx, status);
        }

        else if(state == OTA_MASTER_BUSY)
        {
            switch(param->operation)
            {

                case GATTC_WRITE:
                case GATTC_WRITE_NO_RESPONSE:
                {
									   ota_send_cmp_evt(ota_master_env, conidx, OTA_WRITE_OP_CODE, param->status);
                }
                break;

                case GATTC_READ:
                {
                    if(param->status != GAP_ERR_NO_ERROR)
                    {
											   ota_send_cmp_evt(ota_master_env, conidx, OTA_READ_OP_CODE, param->status);
                    }
                }
                break;

                default: break;
            }

        }
    }
    return (KE_MSG_CONSUMED);
}

static int ota_master_read_cmd_handler(ke_msg_id_t const msgid,
                                  struct  ota_master_read_cmd const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);

    if (ota_master_env != NULL)
    {
        // Attribute Handle
        uint16_t handle    = ATT_INVALID_SEARCH_HANDLE;
        // Status
        uint8_t status     = GAP_ERR_NO_ERROR;

        // Check the provided connection handle
        do
        {
            // Check the current state
            if (ke_state_get(dest_id) == OTA_MASTER_BUSY)
            {
                // Keep the request for later, state is GAP_ERR_NO_ERROR
                msg_status = KE_MSG_SAVED;
                break;
            }

            switch (param->read_code)
            {
                // Read OTA mem Status Characteristic Value
                case (OTA_RD_MEM_DEV):
                {
                    handle = ota_master_env->env[conidx]->bps.chars[OTA_RD_MEM_DEV].val_hdl;
                } break;
								
								case (OTA_RD_GPIO_MAP):
                {
                    handle = ota_master_env->env[conidx]->bps.chars[OTA_RD_GPIO_MAP].val_hdl;
                } break;
								
								case (OTA_RD_MEM_INFO):
                {
                    handle = ota_master_env->env[conidx]->bps.chars[OTA_RD_MEM_INFO].val_hdl;
                } break;
								
								case (OTA_RD_WR_SERV_STATUS_CFG):
                {
                    handle = ota_master_env->env[conidx]->bps.descs[OTA_MASTER_DESC_SERV_STATUS_CLI_CFG].desc_hdl;
                } break;
							
                default:
                {
                    // Handle is ATT_INVALID_SEARCH_HANDLE
                    status = PRF_ERR_INVALID_PARAM;
                } break;
            }

                if (status == GAP_ERR_NO_ERROR)
                {
                    // Check if handle is viable
                    if (handle != ATT_INVALID_SEARCH_HANDLE)
                    {
                        // Configure the environment for a read operation
                        ota_master_env->env[conidx]->operation      = OTA_READ_OP_CODE;
                        ota_master_env->env[conidx]->last_char_code = param->read_code;

                        // Send the read request
                        prf_read_char_send(&(ota_master_env->prf_env), conidx,
                                ota_master_env->env[conidx]->bps.svc.shdl,
                                ota_master_env->env[conidx]->bps.svc.ehdl,
                                handle);

                        // Go to the Busy state
                        ke_state_set(dest_id, OTA_MASTER_BUSY);
                    }
                    else
                    {
                        status = PRF_ERR_INEXISTENT_HDL;
                    }
                }
        } while(0);

        if (status != GAP_ERR_NO_ERROR)
        {
            ota_send_cmp_evt(ota_master_env, conidx, OTA_READ_OP_CODE, status);
        }
    }
    else
    {
        // No connection exists
        ota_send_cmp_evt(ota_master_env, conidx, OTA_READ_OP_CODE, PRF_ERR_REQ_DISALLOWED);
    }

    return (int)msg_status;
}

static int ota_master_write_cmd_handler(ke_msg_id_t const msgid,
                                   struct ota_master_write_cmd const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (ota_master_env != NULL)
    {
        do
        {
            // Check the current state
            if (ke_state_get(dest_id) != OTA_MASTER_IDLE)
            {
                // Another procedure is pending, keep the command for later
                msg_status = KE_MSG_SAVED;
                break;
            }

            // Attribute handle
            uint16_t handle    = ATT_INVALID_SEARCH_HANDLE;
            // Write type
            uint8_t wr_type;
            // Length
            uint8_t length;

            switch (param->write_code)
            {
                case (OTA_WD_PATCH_LEN):
                {
                      handle  = ota_master_env->env[conidx]->bps.chars[OTA_WD_PATCH_LEN].val_hdl;
                      length  = sizeof(uint16_t);
                      wr_type = GATTC_WRITE;
                } 
								break;
								
								case (OTA_WD_MEM_DEV):
								{
									  handle  = ota_master_env->env[conidx]->bps.chars[OTA_WD_MEM_DEV].val_hdl;
                    length  = sizeof(uint32_t);
                    wr_type = GATTC_WRITE;
								}
								break;
								
								case (OTA_WD_PATCH_DATA):
								{
									    handle  = ota_master_env->env[conidx]->bps.chars[OTA_WD_PATCH_DATA].val_hdl;
                      length  = param->data_length;
                      wr_type = GATTC_WRITE;
									
								}
								break;
							
                case (OTA_RD_WR_SERV_STATUS_CFG):
                {
                    if (param->value.patch_status_setting_ntf_cfg <= PRF_CLI_START_NTF)
                    {
                        handle  = ota_master_env->env[conidx]->bps.descs[OTA_MASTER_DESC_SERV_STATUS_CLI_CFG].desc_hdl;
                        length  = sizeof(uint16_t);
                        wr_type = GATTC_WRITE;
                    }
                    else
                    {
                        status = PRF_ERR_INVALID_PARAM;
                    }
                } 
								break;

                default:
                {
                    status = PRF_ERR_INVALID_PARAM;
                } 
								break;
            }

            if (status == GAP_ERR_NO_ERROR)
            {
                // Check if handle is viable
                if (handle != ATT_INVALID_SEARCH_HANDLE)
                {
                    // Send the write request
                    prf_gatt_write(&(ota_master_env->prf_env), conidx, handle, (uint8_t *)&param->value, length, wr_type);

                    // Configure the environment for a write operation
                    ota_master_env->env[conidx]->operation      = OTA_WRITE_OP_CODE;
                    ota_master_env->env[conidx]->last_char_code = param->write_code;

                    // Go to the Busy state
                    ke_state_set(dest_id, OTA_MASTER_BUSY);
                }
                else
                {
                    status = PRF_ERR_INEXISTENT_HDL;
                }
            }
        } while (0);
    }
    else
    {
        status = PRF_ERR_REQ_DISALLOWED;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        ota_send_cmp_evt(ota_master_env, conidx, OTA_WRITE_OP_CODE, status);
    }

    return (int)msg_status;
}


/// Default State handlers definition
KE_MSG_HANDLER_TAB(ota_master)
{

      { OTA_MASTER_ENABLE_REQ,                 (ke_msg_func_t)ota_master_enable_req_handler},
      
			{ OTA_MASTER_READ_CMD,                   (ke_msg_func_t)ota_master_read_cmd_handler},
      
      { OTA_MASTER_WRITE_CMD,                  (ke_msg_func_t)ota_master_write_cmd_handler},
		
      { GATTC_SDP_SVC_IND,             				 (ke_msg_func_t)gattc_sdp_svc_ind_handler},
				
      { GATTC_CMP_EVT,                 				 (ke_msg_func_t)gattc_cmp_evt_handler},

};



void ota_master_task_init(struct ke_task_desc *task_desc)
{
     // Get the address of the environment
     struct ota_master_env_tag *ota_master_env = PRF_ENV_GET(OTA_MASTER, ota_master);

     task_desc->msg_handler_tab = ota_master_msg_handler_tab;
     task_desc->msg_cnt         = ARRAY_LEN(ota_master_msg_handler_tab);
     task_desc->state           = ota_master_env->state;
     task_desc->idx_max         = OTA_COUNT_IDX_MAX;
	
}

#endif


