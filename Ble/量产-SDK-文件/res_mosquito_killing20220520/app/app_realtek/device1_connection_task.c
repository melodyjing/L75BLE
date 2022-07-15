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
#if (BLE_DEVICE1_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "device1_client_uart_task.h"
#include "device1_connection.h"
#include "device1_connection_task.h"
#include "app_realtek_console.h"
#include "app_realtek_device.h"



extern struct connection_device_env_tag  connection_devive1_env;

LOCAL INT app_connetion_dev1_gapm_activity_created_ind_handler(ke_msg_id_t CONST msgid,
                                             struct gapm_activity_created_ind CONST *p_param,
                                             ke_task_id_t CONST dest_id,
                                             ke_task_id_t CONST src_id) 
{
//		printf("app_connetion_dev1_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (connection_devive1_env.connection_state == APP_DEVICE_CONNECTION_STATE_CREATING)
    {
        // Store the advertising activity index
        connection_devive1_env.act_id = p_param->actv_idx;
				connection_devive1_env.connection_state = APP_DEVICE_CONNECTION_STATE_CREATED;
    }

    return (KE_MSG_CONSUMED);
	
}


LOCAL INT app_connetion_dev1_gapm_cmp_evt_handler(ke_msg_id_t CONST msgid,
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
										printf("GAPM_START_ACTIVITY 1 failed...... \n");
										app_device1_reconnect();
							}
					}
						break;
					case GAPM_CREATE_INIT_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
								 printf("GAPM_CREATE_INIT_ACTIVITY fail ... \n");
							}
							else
							{
									printf("GAPM_CREATE_INIT_ACTIVITY OK ... \n");
								  app_device1_set_task_ready();
							}
							break;
					}
					
					case GAPM_DELETE_ACTIVITY:
					{
							if(param->status != GAP_ERR_NO_ERROR)
							{
								 printf("DEVICE1 GAPM_DELETE_ACTIVITY fail ... \n");
							}
							else
							{
									printf("DEVICE1 GAPM_DELETE_ACTIVITY OK ... \n");
								  app_device1_connection_create();
							}
						
					}
					break;
					
					
					
					default:
						break;
				
			}
	   
			return (KE_MSG_CONSUMED);
}


LOCAL INT app_connetion_dev1_gapc_connection_req_ind_handler(ke_msg_id_t CONST msgid,
                                           struct gapc_connection_req_ind CONST *param,
                                           ke_task_id_t CONST dest_id,
                                           ke_task_id_t CONST src_id)
{
		
		connection_devive1_env.conidx = KE_IDX_GET(src_id);
	
    // Check if the received Connection Handle was valid
    if (connection_devive1_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        connection_devive1_env.conhdl = param->conhdl;
			
			  //printf("connection_devive1_env.conhdl dest_id = %d, src_id = %d \n", dest_id, src_id);

				app_iris_device_find_service(connection_devive1_env.conidx);

				app_device1_exchange_mtu();     //连接后, 设置 交换 MTU 
				#if CMD_LINK_RSSI_FUNCTION
				app_master1_condix(connection_devive1_env.conidx);
				#endif
			
			}
			return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev1_gapc_cmp_evt_handler(ke_msg_id_t CONST msgid,
                                struct gapc_cmp_evt CONST *param,
                                ke_task_id_t CONST dest_id,
                                ke_task_id_t CONST src_id)
{
	
		return (KE_MSG_CONSUMED);
}


LOCAL INT app_connetion_dev1_gapc_disconnect_ind_handler(ke_msg_id_t CONST msgid,
                                      struct gapc_disconnect_ind CONST *param,
                                      ke_task_id_t CONST dest_id,
                                      ke_task_id_t CONST src_id)
{
		printf("app dev1 connection uart task disconnect  reason = %d ...\n", param->reason);
	
		app_device1_disconnect();
	
		connection_devive1_env.conidx = GAP_INVALID_CONIDX;
	
		app_device1_reconnect();
		
		iris_dianbiao_slave_number_rsp();
	
	//  app_device1_delete_connection();
			

		return (KE_MSG_CONSUMED);
}



LOCAL INT app_connetion_dev1_mtu_changed_handler(ke_msg_id_t CONST msgid,
                                        struct gattc_mtu_changed_ind  CONST *param,
                                        ke_task_id_t CONST dest_id,
                                        ke_task_id_t CONST src_id)
{
	  printf("mtu = %d \n", param->mtu);
		return (KE_MSG_CONSUMED);
}

/*
 -------------------------------------------------------------------------------------------------
 |  Head  |  Two Byte Length     |       DATA                                             | Tail |
 -------------------------------------------------------------------------------------------------
 |  0x68  | Length_L  | Length_H |                                                        | 0x16 |
--------------------------------------------------------------------------------------------------
*/



LOCAL INT app_connetion_dev1_gattc_cmp_evt_handler(ke_msg_id_t CONST msgid,
                                struct gattc_cmp_evt CONST *param,
                                ke_task_id_t CONST dest_id,
                                ke_task_id_t CONST src_id)
{
		  return (KE_MSG_CONSUMED);
}


LOCAL INT device1_connection_gapc_param_update_req_ind_handler(ke_msg_id_t CONST msgid,
                                           struct gapc_param_update_req_ind CONST *param,
                                           ke_task_id_t CONST dest_id,
                                           ke_task_id_t CONST src_id)
{
    connection_devive1_env.conidx  = KE_IDX_GET(src_id);
	

    // Check if the received Connection Handle was valid
    if (connection_devive1_env.conidx != GAP_INVALID_CONIDX)
    {			
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KE_BUILD_ID(TASK_GAPC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
                gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;  //2
        cfm->ce_len_max = 4;  //4

        // Send message
        ke_msg_send(cfm);

    }
  

    return (KE_MSG_CONSUMED);
}


LOCAL INT app_connetion_dev1_gapc_bond_req_ind_handler(ke_msg_id_t CONST msgid,
                                     struct gapc_bond_req_ind CONST *param,
                                     ke_task_id_t CONST dest_id,
                                     ke_task_id_t CONST src_id)
{
		// printf("app_connetion_dev1_gapc_bond_req_ind_handler %d ... \n\n\n", param->request);
		struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             src_id, TASK_CONNECTION_DEVICE1,
                                             gapc_bond_cfm);
	
		 switch(param->request)
		 {
			 case GAPC_TK_EXCH:
			 {
//			      uint32_t pin_code = app_device1_get_pincode();  //从广播报文中得到对方的PIN 码
//			      printf("pin_code 111111111111\n");
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
            unsigned char *p_pincode = app_get_new_duanluqi1_pincode(); 
//				 	printf("%x %x %x %x %x %x\n",p_pincode[0],p_pincode[1],p_pincode[2],p_pincode[3],p_pincode[4],p_pincode[5]);
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

LOCAL INT app_connetion_dev1_gapc_bond_ind_handler(ke_msg_id_t CONST msgid,
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

LOCAL INT app_connetion_dev1_gapc_encrypt_req_ind_handler(ke_msg_id_t CONST msgid,
                                        struct gapc_encrypt_req_ind CONST *param,
                                        ke_task_id_t CONST dest_id,
                                        ke_task_id_t CONST src_id)
{

    struct gapc_encrypt_cfm *cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                                src_id, TASK_CONNECTION_DEVICE1,
                                                gapc_encrypt_cfm);

    cfm->found    = false;
		
		ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev1_gapc_encrypt_ind_handler(ke_msg_id_t CONST msgid,
                                    struct gapc_encrypt_ind CONST *param,
                                    ke_task_id_t CONST dest_id,
                                    ke_task_id_t CONST src_id)
{
	  //printf(" Prepare the app_connetion_dev1_gapc_encrypt_ind_handler message \n");
		return (KE_MSG_CONSUMED);
}

LOCAL INT app_connetion_dev1_gapc_security_ind_handler(ke_msg_id_t CONST msgid,
                                    struct gapc_security_ind CONST *param,
                                    ke_task_id_t CONST dest_id,
                                    ke_task_id_t CONST src_id)
{
	    //printf(" Prepare the app_connetion_dev1_gapc_encrypt_ind_handler message \n");
	    if(param->auth & GAP_AUTH_REQ_MASK)  //说明对方需要安全配对, 发起邦定请求
			{
					app_device1_bond_command();
			}

			return (KE_MSG_CONSUMED);
}
#if CMD_LINK_RSSI_FUNCTION
int gapc_device1_link_rssi_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_con_rssi_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id);
#endif																			
/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_connetion_dev1)
{

		{GAPM_CMP_EVT,              (ke_msg_func_t)app_connetion_dev1_gapm_cmp_evt_handler},
		
		{GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_connetion_dev1_gapm_activity_created_ind_handler},
		
		{GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)app_connetion_dev1_gapc_connection_req_ind_handler},
    {GAPC_CMP_EVT,              (ke_msg_func_t)app_connetion_dev1_gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (ke_msg_func_t)app_connetion_dev1_gapc_disconnect_ind_handler},
		

		{GATTC_CMP_EVT,             (ke_msg_func_t)app_connetion_dev1_gattc_cmp_evt_handler},
 		
		{GAPC_PARAM_UPDATE_REQ_IND, 	(ke_msg_func_t)device1_connection_gapc_param_update_req_ind_handler},
		
		{GAPC_BOND_REQ_IND,       (ke_msg_func_t)app_connetion_dev1_gapc_bond_req_ind_handler},
		{GAPC_BOND_IND,           (ke_msg_func_t)app_connetion_dev1_gapc_bond_ind_handler},
		
 		{GAPC_ENCRYPT_REQ_IND,    (ke_msg_func_t)app_connetion_dev1_gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (ke_msg_func_t)app_connetion_dev1_gapc_encrypt_ind_handler},
		
		{GAPC_SECURITY_IND,      (ke_msg_func_t)app_connetion_dev1_gapc_security_ind_handler},
		#if CMD_LINK_RSSI_FUNCTION
		{GAPC_CON_RSSI_IND,         (ke_msg_func_t)gapc_device1_link_rssi_ind_handler},
		#endif		
};

ke_state_t app_connetion_dev1_state[1];

// Application task descriptor
CONST struct ke_task_desc TASK_DESC_CONNECTION_DEV1 = {app_connetion_dev1_msg_handler_tab, app_connetion_dev1_state, 1, ARRAY_LEN(app_connetion_dev1_msg_handler_tab)};


#endif
