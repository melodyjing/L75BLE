/******************************************************************************
	File Name	  :	  ota_device_task.c
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/26

	Description :  
    用于和OTA 设备建立连接	

	Changed Log	:
		Liujian  2020/08/26	   - Creation

*****************************************************************************/
#include "ota_device_task.h"
#include "app_ota_scan.h"
#include "app_rdw_shell.h"
#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))

extern struct ota_device_env_tag  ota_device_env;

static int app_ota_device_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{

		if (ota_device_env.connection_state == APP_OTA_DEVICE_STATE_CREATING)
    {
        // Store the advertising activity index
        ota_device_env.act_id = p_param->actv_idx;
				ota_device_env.connection_state = APP_OTA_DEVICE_STATE_CREATED;
    }

    return (KE_MSG_CONSUMED);
	
}


static int app_ota_device_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
			//printf("ota device operation = %d \n", param->operation);
			switch(param->operation)
			{
					case (GAPM_RESET):
					{
						
					}
					break;
					
					case GAPM_SET_DEV_CONFIG:
					{
							//printf("GAPM_SET_DEV_CONFIG OK ... \n");
							app_ota_device_create();
					}
					break;
					case (GAPM_PROFILE_TASK_ADD):
					{
						  if(param->status != GAP_ERR_NO_ERROR)
							{
								 //printf("GAPM_PROFILE_TASK_ADD fail ... \n");
							}
							else
							{
									//printf("GAPM_PROFILE_TASK_ADD OK ... \n");
							}
					}
					break;
					
					case GAPM_START_ACTIVITY:
					{
						 // printf("GAPM_START_ACTIVITY ....... \n");
							if(param->status != GAP_ERR_NO_ERROR)
							{
										//printf("GAPM_START_ACTIVITY 1 failed...... \n");
										app_ota_device_reconnect();
							}
					}
						break;
					case GAPM_CREATE_INIT_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
								 //printf("GAPM_CREATE_INIT_ACTIVITY fail ... \n");
							}
							else
							{
									//printf("GAPM_CREATE_INIT_ACTIVITY OK ... \n");
									app_ota_device_add_profile();
							}
						
					}
					
					default:
						break;
				
			}
	   
			return (KE_MSG_CONSUMED);
}

static int app_ota_device_gapc_connection_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
		
		ota_device_env.conidx = KE_IDX_GET(src_id);
	
    // Check if the received Connection Handle was valid
    if (ota_device_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        ota_device_env.conhdl = param->conhdl;
					
				app_ota_device_exchange_mtu();     //连接后, 设置 交换 MTU 
			
			}
			return (KE_MSG_CONSUMED);
}

static int app_ota_device_gapc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
	
		return (KE_MSG_CONSUMED);
}

static int app_ota_device_gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
		app_rdw_shell_printf("ota device disconnect .... \n");
		app_ota_device_reconnect();
		app_ota_scan_start_scan();
	  app_ota_device_set_state(0);
		return (KE_MSG_CONSUMED);
}

static int app_ota_device_mtu_changed_handler(ke_msg_id_t const msgid,
                                        struct gattc_mtu_changed_ind  const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
		app_ota_scan_stop_scan();
		//printf("app_connetion_dev1 mtu is = %d \n", param->mtu);
		app_ota_device_find_service();   //然后再寻找服务 
		return (KE_MSG_CONSUMED);
}


static int app_ota_device_gapc_param_update_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    ota_device_env.conidx  = KE_IDX_GET(src_id);
	

    // Check if the received Connection Handle was valid
    if (ota_device_env.conidx != GAP_INVALID_CONIDX)
    {			
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KE_BUILD_ID(TASK_GAPC, ota_device_env.conidx), TASK_OTA_SCAN,
                gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;  //2
        cfm->ce_len_max = 4;  //4

        // Send message
        ke_msg_send(cfm);

    }
  

    return (KE_MSG_CONSUMED);
}

static int app_ota_device_enable_rsp_handler(ke_msg_id_t const msgid,
                                      struct ota_master_enable_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

		if(param->status == GAP_ERR_NO_ERROR)
		{
				app_rdw_shell_printf("connect ok, please load file ....\n");
			  //使能 server, 
			  // app_ota_device_enable_report(ota_device_env.conidx);
			  //  app_ota_device_set_memory_information();
			 app_ota_device_set_state(1);
		}
		return (KE_MSG_CONSUMED);
	
}

static int app_ota_device_cmp_handler(ke_msg_id_t const msgid,
                                      struct ota_cmp_evt const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
        if(param->operation == OTA_WRITE_OP_CODE)
				{
						if(param->status == GAP_ERR_NO_ERROR)
						{
								app_ota_device_patch_data_send_ok();
						}
				}					
}

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_ota_device)
{

		{GAPM_CMP_EVT,              (ke_msg_func_t)app_ota_device_gapm_cmp_evt_handler},
		
		{GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_ota_device_gapm_activity_created_ind_handler},
		
		{GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)app_ota_device_gapc_connection_req_ind_handler},
		
		{GAPC_CMP_EVT,              (ke_msg_func_t)app_ota_device_gapc_cmp_evt_handler},
		
		{GAPC_DISCONNECT_IND,       (ke_msg_func_t)app_ota_device_gapc_disconnect_ind_handler},
		
		
		{OTA_MASTER_ENABLE_RSP,     (ke_msg_func_t)app_ota_device_enable_rsp_handler},
		
		{OTA_MASTER_CMP_EVT,        (ke_msg_func_t)app_ota_device_cmp_handler},
//		
//		{VOICE_MASTER_RX_DATA_IND,   (ke_msg_func_t)app_voice_device_receiver_voice_ind_handler},   //
		
		{GATTC_MTU_CHANGED_IND,     (ke_msg_func_t)app_ota_device_mtu_changed_handler},  
//		
//		{GATTC_EVENT_REQ_IND,           (ke_msg_func_t)app_connetion_dev1_gattc_event_ind_handler},
//		
//		{GATTC_EVENT_IND,       (ke_msg_func_t)app_connetion_dev1_gattc_event_ind_handler},          
// 		
//		{GATTC_CMP_EVT,                 								(ke_msg_func_t)app_connetion_dev1_gattc_cmp_evt_handler},
// 		
		{GAPC_PARAM_UPDATE_REQ_IND, 					(ke_msg_func_t)app_ota_device_gapc_param_update_req_ind_handler},
 		
};


ke_state_t app_ota_device_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_OTA_DEVICE = {app_ota_device_msg_handler_tab, app_ota_device_state, 1, ARRAY_LEN(app_ota_device_msg_handler_tab)};



#endif