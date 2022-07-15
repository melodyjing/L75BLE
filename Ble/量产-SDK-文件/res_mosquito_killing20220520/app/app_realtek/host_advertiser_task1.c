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
#if (APP_HOST2_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "host_advertiser1.h"
#include "app_host2_uart.h"
#include "rdw_mcu.h"
#include "app.h"
#include "app_realtek_console.h"
#include "app_sec2.h"



extern struct adv_env_tag  adv_device1_env;
extern struct app_host_uart_env_tag app_host_env_2;

LOCAL UINT8 app_device1_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               ke_msg_id_t msgid,
                               void *param,
                               ke_task_id_t src_id)
{
    // Counter
    UINT8 counter;

    // Get the message handler function by parsing the message table
    for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--)
    {
        struct ke_msg_handler handler
                = (struct ke_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

        if ((handler.id == msgid) ||
            (handler.id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERR(handler.func);

            return (UINT8)(handler.func(msgid, param, TASK_ADV_DEVICE1, src_id));
        }
    }

    // If we are here no handler has been found, drop the message
    return (KE_MSG_CONSUMED);
}

/////消息处理函数 
LOCAL INT app_device1_msg_handler(ke_msg_id_t const msgid,
                            void *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
		// Retrieve identifier of the task from received message
    ke_task_id_t src_task_id = MSG_T(msgid);
    // Message policy
    UINT8 msg_pol = KE_MSG_CONSUMED;
		
		switch (src_task_id)
    {

				case (TASK_ID_DEVICE1_UART):    //这个任务只处理串口消息
        {
            // Call the Health Thermometer Module
            msg_pol = app_device1_get_handler(&app_host2_uart_handlers, msgid, param, src_id);
        } break;
        case (TASK_ID_GAPC):
        {
#if (DIANBIAO_SPM_ENABLE == 1)
            if ((msgid >= GAPC_BOND_CMD) && (msgid <= GAPC_SECURITY_IND))
            {
                msg_pol = app_device1_get_handler(&app_sec2_handlers, msgid, param, src_id);
            }
#endif
            // else drop the message
        } break;
				
			default:
				break;
		}
		
		return (msg_pol);
}


////&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
////&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
////&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
////&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
///**********************************************GAPC message ******************************************************************
LOCAL INT app_device1_gapc_get_dev_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_get_dev_info_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
		switch(param->req)
    {
        case GAPC_DEV_NAME:
        {
            struct gapc_get_dev_info_cfm * cfm = KE_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                                                    src_id, dest_id,
                                                    gapc_get_dev_info_cfm, APP_DEVICE1_NAME_MAX_LEN);
            cfm->req = param->req;
            cfm->info.name.length = appm_get_dev_name(cfm->info.name.value);

            // Send message
            ke_msg_send(cfm);
        } break;

        case GAPC_DEV_APPEARANCE:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KE_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                                             src_id, dest_id,
                                                             gapc_get_dev_info_cfm);
            cfm->req = param->req;
     
            // No appearance
            cfm->info.appearance = 728;    //liujian
     

            // Send message
            ke_msg_send(cfm);
        } break;

        case GAPC_DEV_SLV_PREF_PARAMS:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KE_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                    src_id, dest_id,
                                                            gapc_get_dev_info_cfm);
            cfm->req = param->req;
            // Slave preferred Connection interval Min
            cfm->info.slv_pref_params.con_intv_min = 200;
            // Slave preferred Connection interval Max
            cfm->info.slv_pref_params.con_intv_max = 200;
            // Slave preferred Connection latency
            cfm->info.slv_pref_params.slave_latency  = 5;
            // Slave preferred Link supervision timeout
            cfm->info.slv_pref_params.conn_timeout    = 500;  // 2s (500*10ms)

						printf("GAPC_DEV_SLV_PREF_PARAMS............................................\n");
            // Send message
            ke_msg_send(cfm);
        } break;

        default: /* Do Nothing */ break;
    }
		
	
		return (KE_MSG_CONSUMED);
}



LOCAL INT app_device1_gapc_set_dev_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gapc_set_dev_info_req_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
		 // Set Device configuration
    struct gapc_set_dev_info_cfm* cfm = KE_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, src_id, dest_id,
                                                     gapc_set_dev_info_cfm);
    // Reject to change parameters
    cfm->status = GAP_ERR_REJECTED;
    cfm->req = param->req;
    // Send message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



LOCAL INT app_device1_gapc_connection_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
	
	   adv_device1_env.conidx = KE_IDX_GET(src_id);
	
		 app_host_env_2.connection_index = adv_device1_env.conidx;

		 struct gap_bdaddr* p_addr = gapc_get_bdaddr(adv_device1_env.conidx, SMPC_INFO_PEER);

			memcpy(adv_device1_env.peer_addr.addr, p_addr->addr.addr, BD_ADDR_LEN);
	
			app_collection_1_ready();
			#if CMD_LINK_RSSI_FUNCTION
			app_slave2_condix(adv_device1_env.conidx);
			#endif	
	
			app_iris_collection1_address(&(p_addr->addr));
	
    // Check if the received Connection Handle was valid
    if (adv_device1_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        adv_device1_env.conhdl = param->conhdl;
			

        // Send connection confirmation
        struct gapc_connection_cfm *cfm = KE_MSG_ALLOC(GAPC_CONNECTION_CFM,
                KE_BUILD_ID(TASK_GAPC, adv_device1_env.conidx), TASK_ADV_DEVICE1,
                gapc_connection_cfm);

 
       
#if(DIANBIAO_SPM_ENABLE == 1)				
        cfm->auth      = app_sec2_get_bond_status() ? GAP_AUTH_REQ_NO_MITM_BOND : GAP_AUTH_REQ_NO_MITM_NO_BOND; 
#else
				cfm->auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
#endif
				//cfm->auth      = GAP_AUTH_REQ_NO_MITM_BOND;
        // Send the message
        ke_msg_send(cfm);


       printf("22222222222222222222222222222222222222222222222222222222222222222222222 \n");
				//----------------------liujian added
			  // 使能 profile
			  app_host2_uart_enable_prf_app(adv_device1_env.conhdl);
			
        // We are now in connected State
        ke_state_set(dest_id, APP_DEVICE1_CONNECTED);
				
#if (DIANBIAO_SPM_ENABLE == 1)
        if(app_get_ble_smp_level() != SMP_JUST_WORK)
				{
						printf("app_sec_send_security_req_adv2 \n");
            // Ask for the peer device to either start encryption
            app_sec2_send_security_req(adv_device1_env.conidx);
        }
#endif
				
				adv_device1_env.adv_ok = 0;   //不再广播了 设备被连接上了
				
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)					
				if(app_env.adv_ok == 0 && !app_collection_2_is_ready())  //让另外一个广播了，要继续另外一个广播
				{
					  appm_adv_fsm_next();
				}
#endif				
				
//2020/1/10
//反馈当前连接的主设备状态
				iris_dianbiao_master_number_rsp();

    }
    else
    {
        // No connection has been established, restart advertising
        app_device1_adv_fsm_next();
    }

		

    return (KE_MSG_CONSUMED);
	
}



LOCAL INT app_device1_gapc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
	printf("device1 gapc_cmp_evt_handler = %d \n", param->operation);
		switch(param->operation)
    {
        case (GAPC_UPDATE_PARAMS):
        {
						printf("host2 GAPC_UPDATE_PARAMS ...... \n");
            if (param->status != GAP_ERR_NO_ERROR)
            {
									printf("Error 0x%x ...... \n", param->status);
            }
        } break;

        default:
        {
						break;
        } 
    }
		
		return (KE_MSG_CONSUMED);
}


LOCAL INT app_device1_gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
		printf("pc disconnection .id= %d, ..reason = %x ..\n",param->conhdl,  param->reason);
		// Go to the ready state
    ke_state_set(TASK_ADV_DEVICE1, APP_DEVICE1_READY);
	
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)	
    if(app_env.adv_ok == 0 && get_devicel_adv() == 0)
		{
			 app_device1_adv_fsm_next();
		}
#else		
		app_device1_adv_fsm_next();   //重新广播, 断开连接后, 就又重新广播
#endif		
	
		app_collection_1_not_ready();
	
	  iris_dianbiao_master_number_rsp();
	
//		appm_disconnect();
	
		return (KE_MSG_CONSUMED);
}																				

////**********************************************************************************************************************
///***********************************************************************************************************************
////**********************************************************************************************************************
///***********************************************************************************************************************
////**********************************************************************************************************************
///***********************************************************************************************************************
////**********************************************************************************************************************
///***********************************************************************************************************************


///$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ GAPM Message $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

LOCAL INT app_device1_gapm_gen_rand_nb_ind_handler(ke_msg_id_t const msgid, struct gapm_gen_rand_nb_ind *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	
		if (adv_device1_env.rand_cnt==1)      // First part of IRK
    {
        memcpy(&adv_device1_env.loc_irk[0], &param->randnb.nb[0], 8);
    }
    else if (adv_device1_env.rand_cnt==2) // Second part of IRK
    {
        memcpy(&adv_device1_env.loc_irk[8], &param->randnb.nb[0], 8);
    }

    return KE_MSG_CONSUMED;
}

LOCAL INT app_device1_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
//		printf("app_device1_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (adv_device1_env.adv_state == APP_DEVICE1_ADV_STATE_CREATING)
    {
        // Store the advertising activity index
        adv_device1_env.adv_actv_idx = p_param->actv_idx;
    }

    return (KE_MSG_CONSUMED);
	
}


LOCAL INT app_device1_gapm_activity_stopped_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_stopped_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
		if (adv_device1_env.adv_state == APP_DEVICE1_ADV_STATE_STARTED)
    {
        // Act as if activity had been stopped by the application
        adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_STOPPING;
				
			//  printf("app_device1_gapm_activity_stopped_ind_handler .... \n");
        // Perform next operation
        app_device1_adv_fsm_next();
    }

    return (KE_MSG_CONSUMED);
}


LOCAL INT app_device1_gapm_profile_added_ind_handler(ke_msg_id_t const msgid,
                                          struct gapm_profile_added_ind *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
	
		 // Current State
    ke_state_t state = ke_state_get(dest_id);

    if (state == APP_DEVICE1_CREATE_DB)
    {
        switch (param->prf_task_id)
        {
            default: /* Nothing to do */ break;
        }
    }
    else
    {
        ASSERT_INFO(0, state, src_id);
    }

    return KE_MSG_CONSUMED;
}



LOCAL INT app_device1_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
			printf("host2 operation = %d \n", param->operation);
	   
		  switch(param->operation)
			{
//					case (GAPM_PROFILE_TASK_ADD):  // 第二次消息
//					{
//							struct gapm_gen_rand_nb_cmd *cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
//                                                                TASK_GAPM, TASK_ADV_DEVICE1,
//                                                                gapm_gen_rand_nb_cmd);
//                cmd->operation   = GAPM_GEN_RAND_NB;
//                adv_device1_env.rand_cnt = 1;
//                ke_msg_send(cmd);
//								break;
//					}
					case (GAPM_GEN_RAND_NB):    //第三次消息, 第四次消息
					{
							if (adv_device1_env.rand_cnt == 1)
							{
									// Generate a second random number
									adv_device1_env.rand_cnt++;
									struct gapm_gen_rand_nb_cmd *cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
																																	TASK_GAPM, TASK_ADV_DEVICE1,
																																	gapm_gen_rand_nb_cmd);
									cmd->operation = GAPM_GEN_RAND_NB;
									ke_msg_send(cmd);
							}
							else
							{
									struct gapm_set_irk_cmd *cmd = KE_MSG_ALLOC(GAPM_SET_IRK_CMD,
																													TASK_GAPM, TASK_ADV_DEVICE1,
																													gapm_set_irk_cmd);
									adv_device1_env.rand_cnt=0;
									///  - GAPM_SET_IRK
									cmd->operation = GAPM_SET_IRK;
									memcpy(&cmd->irk.key[0], &adv_device1_env.loc_irk[0], KEY_LEN);
									ke_msg_send(cmd);
							}
							break;
					}
					case (GAPM_SET_IRK):   //第五次消息
					{
							adv_device1_env.rand_cnt = 0;   //不再添加 profile 了
							// Go to the ready state
              ke_state_set(TASK_ADV_DEVICE1, APP_DEVICE1_READY);
						
						  //从这里开始进入广播状态机
							app_device1_adv_fsm_next();   //进入    广播设备的状态机
						
							break;
					}
					case (GAPM_SET_DEV_CONFIG):   // 第一次消息
					{
							// Go to the create db state
							ke_state_set(TASK_ADV_DEVICE1, APP_DEVICE1_CREATE_DB);

							//app_device1_test_send_random();
							// Add the first required service in the database
							// and wait for the PROFILE_ADDED_IND
						  //这个消息中需要支持是 TASK_DEVICE1 发送的
							//app_host2_uart_add_uarts();       //这里只添加一个profile, 以支持IRIS的要求
							break;
					}
					case (GAPM_CREATE_ADV_ACTIVITY):
					case (GAPM_STOP_ACTIVITY):
					case (GAPM_START_ACTIVITY):
					case (GAPM_DELETE_ACTIVITY):
					case (GAPM_SET_ADV_DATA):
					case (GAPM_SET_SCAN_RSP_DATA):
							// Perform next operation
							app_device1_adv_fsm_next();      //广播设备的状态机
					break;
					case (GAPM_DELETE_ALL_ACTIVITIES) :
					{
							// Re-Invoke Advertising
							adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_IDLE;
							app_device1_adv_fsm_next();
							break;
					}
					default:
						break;
				
			}
			
			return (KE_MSG_CONSUMED);
}

LOCAL INT gapc_param_update_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    adv_device1_env.conidx = KE_IDX_GET(src_id);
	
	  printf("adv_device1_env.conidx ======= %d \n", adv_device1_env.conidx);

    // Check if the received Connection Handle was valid
    if (adv_device1_env.conidx != GAP_INVALID_CONIDX)
    {
			
			{
					struct gapc_conn_param conn_param;
					conn_param.intv_min = 256;
					conn_param.intv_max = 256;
					conn_param.latency = 3;
					conn_param.time_out = 500;
			}
			
	
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KE_BUILD_ID(TASK_GAPC, adv_device1_env.conidx), TASK_ADV_DEVICE1,
                gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;
        cfm->ce_len_max = 4;

        // Send message
        ke_msg_send(cfm);

    }
    else
    {
        // No connection has been established, restart advertising
        app_device1_adv_fsm_next();
    }

    return (KE_MSG_CONSUMED);
}
#if CMD_LINK_RSSI_FUNCTION
int gapc_slave_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id);
#endif	
/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_device1)
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)app_device1_msg_handler},

    // GAPM messages
    {GAPM_PROFILE_ADDED_IND,    (ke_msg_func_t)app_device1_gapm_profile_added_ind_handler},
    {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_device1_gapm_activity_created_ind_handler},
    {GAPM_ACTIVITY_STOPPED_IND, (ke_msg_func_t)app_device1_gapm_activity_stopped_ind_handler},
    {GAPM_CMP_EVT,              (ke_msg_func_t)app_device1_gapm_cmp_evt_handler},
    {GAPM_GEN_RAND_NB_IND,      (ke_msg_func_t)app_device1_gapm_gen_rand_nb_ind_handler},

    // GAPC messages
    {GAPC_GET_DEV_INFO_REQ_IND, (ke_msg_func_t)app_device1_gapc_get_dev_info_req_ind_handler},
    {GAPC_SET_DEV_INFO_REQ_IND, (ke_msg_func_t)app_device1_gapc_set_dev_info_req_ind_handler},
    {GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)app_device1_gapc_connection_req_ind_handler},
    {GAPC_CMP_EVT,              (ke_msg_func_t)app_device1_gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (ke_msg_func_t)app_device1_gapc_disconnect_ind_handler},
		{GAPC_PARAM_UPDATE_REQ_IND, (ke_msg_func_t)gapc_param_update_req_ind_handler},
		#if CMD_LINK_RSSI_FUNCTION
		{GAPC_CON_RSSI_IND,         (ke_msg_func_t)gapc_slave_link_rssi_ind_handler},
		#endif		
};





/* Defines the place holder for the states of all the task instances. */
ke_state_t app_device1_state[APP_DEVICE1_IDX_MAX];

// Application task descriptor
const struct ke_task_desc TASK_DESC_DEVICE1 = {app_device1_msg_handler_tab, app_device1_state, APP_DEVICE1_IDX_MAX, ARRAY_LEN(app_device1_msg_handler_tab)};


#endif
