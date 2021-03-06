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
#if (BLE_DEVICE2_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "device1_client_uart_task.h"
#include "device2_connection.h"
#include "device2_connection_task.h"
#include "app_realtek_console.h"
#include "app_realtek_device.h"


extern struct connection_device2_env_tag  connection_devive2_env;

LOCAL INT app_connetion_dev2_gapm_activity_created_ind_handler(ke_msg_id_t CONST msgid,
                                             struct gapm_activity_created_ind CONST *p_param,
                                             ke_task_id_t CONST dest_id,
                                             ke_task_id_t CONST src_id)
{
//		printf("app_connetion_dev2_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (connection_devive2_env.connection_state == APP_DEVICE2_CONNECTION_STATE_CREATING)
    {
        // Store the advertising activity index
        connection_devive2_env.act_id = p_param->actv_idx;
				connection_devive2_env.connection_state = APP_DEVICE2_CONNECTION_STATE_CREATED;
    }

    return (KE_MSG_CONSUMED);
	
}


LOCAL INT app_connetion_dev2_gapm_cmp_evt_handler(ke_msg_id_t CONST msgid,
                                struct gapm_cmp_evt CONST *param,
                                ke_task_id_t CONST dest_id,
                                ke_task_id_t CONST src_id)
{

			switch(param->operation)
			{
					case GAPM_START_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
										printf("GAPM_START_ACTIVITY 2 failed...... \n");
										app_device2_reconnect();
							}
							break;
					}
					
					case GAPM_CREATE_INIT_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
//								 printf("DEVICE2 GAPM_CREATE_INIT_ACTIVITY fail ... \n");
							}
							else
							{
//									printf("DEVICE2 GAPM_CREATE_INIT_ACTIVITY OK ... \n");
								  app_device2_set_task_ready();
							}
							break;
						
					}
					case GAPM_DELETE_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
								 printf("DEVICE2 GAPM_DELETE_ACTIVITY fail ... \n");
							}
							else
							{
									printf("DEVICE2 GAPM_DELETE_ACTIVITY OK ... \n");
								  app_device2_connection_create();
							}
						
					}
					break;
					
									
					default:
						break;
				
			}
	   
			return (KE_MSG_CONSUMED);
}




LOCAL INT app_connetion_dev2_gapc_connection_req_ind_handler(ke_msg_id_t CONST msgid,
                                           struct gapc_connection_req_ind CONST *param,
                                           ke_task_id_t CONST dest_id,
                                           ke_task_id_t CONST src_id)
{

		connection_devive2_env.conidx = KE_IDX_GET(src_id);
	

    // Check if the received Connection Handle was valid
    if (connection_devive2_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        connection_devive2_env.conhdl = param->conhdl;

			//	printf("connection_devive2_env.conhdl dest_id = %d, src_id = %d \n", dest_id, src_id);
	
			  app_iris_device_find_service(connection_devive2_env.conidx);
					
			  app_device2_exchange_mtu();     //??????, ???? ???? MTU 
				#if CMD_LINK_RSSI_FUNCTION
				app_master2_condix(connection_devive2_env.conidx);
				#endif
			}
			return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev2_gapc_cmp_evt_handler(ke_msg_id_t CONST msgid,
                                struct gapc_cmp_evt CONST *param,
                                ke_task_id_t CONST dest_id,
                                ke_task_id_t CONST src_id)
{
	
		return (KE_MSG_CONSUMED);
}


LOCAL INT app_connetion_dev2_gapc_disconnect_ind_handler(ke_msg_id_t CONST msgid,
                                      struct gapc_disconnect_ind CONST *param,
                                      ke_task_id_t CONST dest_id,
                                      ke_task_id_t CONST src_id)
{
		printf("app dev2 connection uart task disconnect  reason = %d ...\n", param->reason);
	
		app_device2_disconnect();
	
  	connection_devive2_env.conidx = GAP_INVALID_CONIDX;
	
		app_device2_reconnect();
	
	  iris_dianbiao_slave_number_rsp();
	
//	  app_device2_delete_connection();

		return (KE_MSG_CONSUMED);
}





LOCAL INT app_connetion_dev2_mtu_changed_handler(ke_msg_id_t CONST msgid,
                                        struct gattc_mtu_changed_ind  CONST *param,
                                        ke_task_id_t CONST dest_id,
                                        ke_task_id_t CONST src_id)
{ 
		return (KE_MSG_CONSUMED);
}



LOCAL INT app_connetion_dev2_gattc_cmp_evt_handler(ke_msg_id_t CONST msgid,
                                struct gattc_cmp_evt CONST *param,
                                ke_task_id_t CONST dest_id,
                                ke_task_id_t CONST src_id)
{
    printf("app_connetion_dev2_gattc_cmp_evt_handler ...... \n");
		return (KE_MSG_CONSUMED);
}

LOCAL INT device2_connection_gapc_param_update_req_ind_handler(ke_msg_id_t CONST msgid,
                                           struct gapc_param_update_req_ind CONST *param,
                                           ke_task_id_t CONST dest_id,
                                           ke_task_id_t CONST src_id)
{
    connection_devive2_env.conidx  = KE_IDX_GET(src_id);
	

    // Check if the received Connection Handle was valid
    if (connection_devive2_env.conidx != GAP_INVALID_CONIDX)
    {			
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KE_BUILD_ID(TASK_GAPC, connection_devive2_env.conidx), TASK_CONNECTION_DEVICE2,
                gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;  //2
        cfm->ce_len_max = 4;  //4

        // Send message
        ke_msg_send(cfm);

    }
  

    return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev2_gapc_bond_req_ind_handler(ke_msg_id_t CONST msgid,
                                     struct gapc_bond_req_ind CONST *param,
                                     ke_task_id_t CONST dest_id,
                                     ke_task_id_t CONST src_id)
{

		struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             src_id, TASK_CONNECTION_DEVICE2,
                                             gapc_bond_cfm);
	
		 switch(param->request)
		 {
			 case GAPC_TK_EXCH:
			 {
			  //    uint32_t pin_code = app_device2_get_pincode();  //??????????????????????PIN ??
			      printf("pin_code 2222222222222\n");
			      cfm->accept  = true;
            cfm->request = GAPC_TK_EXCH;

            // Set the TK value
            memset(cfm->data.tk.key, 0, KEY_LEN);
#if 0
            cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
            cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
            cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
            cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
#else
            unsigned char *p_pincode = app_get_new_duanluqi2_pincode(); 
//            cfm->data.tk.key[0] = p_pincode[0];
//            cfm->data.tk.key[1] = p_pincode[1];
//            cfm->data.tk.key[2] = p_pincode[2]; 
//            cfm->data.tk.key[3] = p_pincode[3];
//				    cfm->data.tk.key[4] = p_pincode[4]; 
//            cfm->data.tk.key[5] = p_pincode[5];
				    uint32_t pin_code = app_get_pincode_convert_ascii_to_hex(p_pincode, 6);
					
//            printf("pin_code 111 = %d \n", pin_code);
					
            cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
            cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
            cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
            cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);				 
#endif				 
			 }
			 break;
			 default:
				 break;
		 }
		 
		 ke_msg_send(cfm);
		 
		 return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev2_gapc_bond_ind_handler(ke_msg_id_t CONST msgid,
                                 struct gapc_bond_ind CONST *param,
                                 ke_task_id_t CONST dest_id,
                                 ke_task_id_t CONST src_id)
{
		// printf("app_connetion_dev1_gapc_bond_ind_handler %d ... \n\n\n\n", param->info);
		 switch(param->info)
		 {
				case (GAPC_PAIRING_SUCCEED):
        {
						//app_device1_connection_find_service();
				}
				break;
				default:
					break;
			 
		 }
		 return (KE_MSG_CONSUMED);
	
}

LOCAL INT app_connetion_dev2_gapc_encrypt_req_ind_handler(ke_msg_id_t CONST msgid,
                                        struct gapc_encrypt_req_ind CONST *param,
                                        ke_task_id_t CONST dest_id,
                                        ke_task_id_t CONST src_id)
{

    struct gapc_encrypt_cfm *cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                                src_id, TASK_CONNECTION_DEVICE2,
                                                gapc_encrypt_cfm);

    cfm->found    = false;
		
		ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev2_gapc_encrypt_ind_handler(ke_msg_id_t CONST msgid,
                                    struct gapc_encrypt_ind CONST *param,
                                    ke_task_id_t CONST dest_id,
                                    ke_task_id_t CONST src_id)
{
	  //printf(" Prepare the app_connetion_dev1_gapc_encrypt_ind_handler message \n");
		return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev2_gapc_security_ind_handler(ke_msg_id_t CONST msgid,
                                    struct gapc_security_ind CONST *param,
                                    ke_task_id_t CONST dest_id,
                                    ke_task_id_t CONST src_id)
{
	    //printf(" Prepare the app_connetion_dev1_gapc_encrypt_ind_handler message \n");
	    if(param->auth & GAP_AUTH_REQ_MASK)  //????????????????????, ????????????
			{
					app_device2_bond_command();
			}

			return (KE_MSG_CONSUMED);
}


/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_connetion_dev2)
{

		{GAPM_CMP_EVT,              (ke_msg_func_t)app_connetion_dev2_gapm_cmp_evt_handler},
		
		{GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_connetion_dev2_gapm_activity_created_ind_handler},
		
		{GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)app_connetion_dev2_gapc_connection_req_ind_handler},
    {GAPC_CMP_EVT,              (ke_msg_func_t)app_connetion_dev2_gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (ke_msg_func_t)app_connetion_dev2_gapc_disconnect_ind_handler},
		
		{GATTC_CMP_EVT,              (ke_msg_func_t)app_connetion_dev2_gattc_cmp_evt_handler},
		
 		{GAPC_PARAM_UPDATE_REQ_IND,  (ke_msg_func_t)device2_connection_gapc_param_update_req_ind_handler},
		
		{GAPC_BOND_REQ_IND,       (ke_msg_func_t)app_connetion_dev2_gapc_bond_req_ind_handler},
		{GAPC_BOND_IND,           (ke_msg_func_t)app_connetion_dev2_gapc_bond_ind_handler},
		
 		{GAPC_ENCRYPT_REQ_IND,    (ke_msg_func_t)app_connetion_dev2_gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (ke_msg_func_t)app_connetion_dev2_gapc_encrypt_ind_handler},
		
		{GAPC_SECURITY_IND,      (ke_msg_func_t)app_connetion_dev2_gapc_security_ind_handler},
		
};

ke_state_t app_connetion_dev2_state[1];

// Application task descriptor
CONST struct ke_task_desc TASK_DESC_CONNECTION_DEV2 = {app_connetion_dev2_msg_handler_tab, app_connetion_dev2_state, 1, ARRAY_LEN(app_connetion_dev2_msg_handler_tab)};


#endif
