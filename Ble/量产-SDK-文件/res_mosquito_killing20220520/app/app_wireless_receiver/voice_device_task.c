/******************************************************************************
	File Name	:	voice_device_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/06

	Description :   

	Changed Log	:
		Liujian  2020/03/06		- Creation

*****************************************************************************/
#include "voice_device_task.h"

#if BLE_APP_RECEIVE_VOICE

extern struct voice_device_env_tag  voice_device_env;

static int app_voice_device_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{

		if (voice_device_env.connection_state == APP_VOCIE_DEVICE_STATE_CREATING)
    {
        // Store the advertising activity index
        voice_device_env.act_id = p_param->actv_idx;
				voice_device_env.connection_state = APP_VOCIE_DEVICE_STATE_CREATED;
    }

    return (KE_MSG_CONSUMED);
	
}


static int app_voice_device_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
			printf("voice device operation = %d \n", param->operation);
			switch(param->operation)
			{
					case (GAPM_RESET):
					{
						
					}
					break;
					
					case GAPM_SET_DEV_CONFIG:
					{
							printf("GAPM_SET_DEV_CONFIG OK ... \n");
							app_voice_device_create();
					}
					break;
					case (GAPM_PROFILE_TASK_ADD):
					{
						  if(param->status != GAP_ERR_NO_ERROR)
							{
								 printf("GAPM_PROFILE_TASK_ADD fail ... \n");
							}
							else
							{
									printf("GAPM_PROFILE_TASK_ADD OK ... \n");
							}
							//app_voice_device_create();
					}
					break;
					
					case GAPM_START_ACTIVITY:
					{
						  printf("GAPM_START_ACTIVITY ....... \n");
							if(param->status != GAP_ERR_NO_ERROR)
							{
										printf("GAPM_START_ACTIVITY 1 failed...... \n");
										app_voice_device_reconnect();
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
									app_voice_device_add_profile();
							}
						
					}
					
					default:
						break;
				
			}
	   
			return (KE_MSG_CONSUMED);
}

static int app_voice_device_gapc_connection_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
		
		voice_device_env.conidx = KE_IDX_GET(src_id);
	
    // Check if the received Connection Handle was valid
    if (voice_device_env.conidx != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        voice_device_env.conhdl = param->conhdl;
					
				app_voice_device_exchange_mtu();     //连接后, 设置 交换 MTU 
			
			}
			return (KE_MSG_CONSUMED);
}

static int app_voice_device_gapc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
	
		return (KE_MSG_CONSUMED);
}

static int app_voice_device_gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
		printf("app_voice_device_gapc_disconnect_ind_handler .... \n");
		app_voice_device_reconnect();
		app_sound_scan_start_scan();
		return (KE_MSG_CONSUMED);
}

static int app_voice_device_mtu_changed_handler(ke_msg_id_t const msgid,
                                        struct gattc_mtu_changed_ind  const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
		app_sound_scan_stop_scan();
		printf("app_connetion_dev1 mtu is = %d \n", param->mtu);
		app_voice_device_find_service();   //然后再寻找服务 
		return (KE_MSG_CONSUMED);
}


static int app_voice_device_gapc_param_update_req_ind_handler(ke_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    voice_device_env.conidx  = KE_IDX_GET(src_id);
	

    // Check if the received Connection Handle was valid
    if (voice_device_env.conidx != GAP_INVALID_CONIDX)
    {			
        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KE_BUILD_ID(TASK_GAPC, voice_device_env.conidx), TASK_SOUND_RECIVER,
                gapc_param_update_cfm);

        cfm->accept = true;
        cfm->ce_len_min = 2;  //2
        cfm->ce_len_max = 4;  //4

        // Send message
        ke_msg_send(cfm);

    }
  

    return (KE_MSG_CONSUMED);
}

static int app_voice_device_enable_rsp_handler(ke_msg_id_t const msgid,
                                      struct voice_master_enable_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
		
		if(param->status == GAP_ERR_NO_ERROR)
		{
				printf("voice connection ok ...... \n");
			//使能 server, 让语音设备开始送声音过来
			  app_voice_device_enable_report(voice_device_env.conidx);
			
		//	  cobra_udma_enable_interrupt(CHANNEL15);	
			
		}
		return (KE_MSG_CONSUMED);
	
}

static int app_voice_device_receiver_voice_ind_handler(ke_msg_id_t const msgid,
                                        struct voice_master_rx_voice_data_ind  const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
	  
		uint32_t i;
#if BLE_VOICE_RECOGNIZE_IS_ENABLE	
//这个是用作语音识别吧
//科大讯飞 需要用到
#include "cobra_uart.h"	
			cobra_uart_write(M3_UART0, 0xAA);
			cobra_uart_write(M3_UART0, 0x55);
			for(i = 0; i < param->length; i++)
			{
					cobra_uart_write(M3_UART0, param->value[i]);
			}
#else
//			#include "cobra_uart.h"	
//			cobra_uart_write(M3_UART0, 0xAA);
//			cobra_uart_write(M3_UART0, 0x55);
//			for(i = 0; i < param->length; i++)
//			{
//					cobra_uart_write(M3_UART0, param->value[i]);
//			}
			
//			uint16_t pv_rx = BUILD_UINT16(pData.pValue[2], pData.pValue[3]);
//			uint8_t  si_rx = pData.pValue[1];
//			Codec1_decodeBuff((int16_t *)decode_bufferOut, (uint8_t *)&pData.pValue[4], \
//													 ADPCM_DECODE_SAMPLE*sizeof(int16_t),
//																							&si_rx, &pv_rx);
													 
  			app_voice_adpcm_decode_sound(param->value, 	param->length);								 
			//UART_write(test_handle, decode_bufferOut, ADPCM_DECODE_SAMPLE*sizeof(int16_t));
	
#endif
		return (KE_MSG_CONSUMED);
}

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_voice_device)
{

		{GAPM_CMP_EVT,              (ke_msg_func_t)app_voice_device_gapm_cmp_evt_handler},
		
		{GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_voice_device_gapm_activity_created_ind_handler},
		
		{GAPC_CONNECTION_REQ_IND,   (ke_msg_func_t)app_voice_device_gapc_connection_req_ind_handler},
		
		{GAPC_CMP_EVT,              (ke_msg_func_t)app_voice_device_gapc_cmp_evt_handler},
		
		{GAPC_DISCONNECT_IND,       (ke_msg_func_t)app_voice_device_gapc_disconnect_ind_handler},
		
		
		{VOICE_MASTER_ENABLE_RSP,   (ke_msg_func_t)app_voice_device_enable_rsp_handler},
//		
		{VOICE_MASTER_RX_DATA_IND,   (ke_msg_func_t)app_voice_device_receiver_voice_ind_handler},   //
		
		{GATTC_MTU_CHANGED_IND,     (ke_msg_func_t)app_voice_device_mtu_changed_handler},  
//		
//		{GATTC_EVENT_REQ_IND,           (ke_msg_func_t)app_connetion_dev1_gattc_event_ind_handler},
//		
//		{GATTC_EVENT_IND,       (ke_msg_func_t)app_connetion_dev1_gattc_event_ind_handler},          
// 		
//		{GATTC_CMP_EVT,                 								(ke_msg_func_t)app_connetion_dev1_gattc_cmp_evt_handler},
// 		
		{GAPC_PARAM_UPDATE_REQ_IND, 					(ke_msg_func_t)app_voice_device_gapc_param_update_req_ind_handler},
 		
};


ke_state_t app_voice_device_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_VOICE_DEVICE = {app_voice_device_msg_handler_tab, app_voice_device_state, 1, ARRAY_LEN(app_voice_device_msg_handler_tab)};

#endif
