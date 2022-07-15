

#include "app_host2_uart.h"
#include "app.h"
#include "host_advertiser1.h"
#include "app_iris_console.h"
#include "gattc.h"

#if (BLE_HOST2_UART && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)
struct app_host_uart_env_tag app_host_env_2;

extern struct app_host_uart_env_tag app_host_env_1;	
extern struct adv_env_tag  adv_device1_env;	

void app_host2_uart_init(void)
{
    // Reset the environment
    memset(&app_host_env_2, 0, sizeof(app_host_env_2));

    // Initial measurement interval 
  //  app_host_env_2.uart_meas_intv   = 3;	
}

//void app_host2_uart_add_uarts(void)
//{
//    struct device1_uart_db_cfg* db_cfg;
//    // Allocate the HTPT_CREATE_DB_REQ
//    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
//                                                  TASK_GAPM, TASK_ADV_DEVICE1,
//                                                  gapm_profile_task_add_cmd, sizeof(struct device1_uart_db_cfg));
//    // Fill message
//    req->operation = GAPM_PROFILE_TASK_ADD;
//    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_128);  //NO_AUTH);    UNAUTH
//    req->prf_task_id = TASK_ID_DEVICE1_UART;
//    req->app_task = TASK_ADV_DEVICE1;
//    req->start_hdl = 0;

//    // Set parameters
//    db_cfg = (struct device1_uart_db_cfg* ) req->param;
//    // All features are supported
//    db_cfg->features = DEVICE1_UART_ALL_FEAT_SUP;

//    // Send the message
//    ke_msg_send(req);

//}

void app_host2_uart_enable_prf_app(uint8_t conidx)
{
	
		struct gapc_conn_param conn_param;
	
    // Allocate the message
    struct device1_uart_enable_req * req = KE_MSG_ALLOC(DEVICE1_UART_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_DEVICE1_UART),
                                                TASK_ADV_DEVICE1,
                                                device1_uart_enable_req);
		//printf("host2 uart conidx = %d \n", conidx);
    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;
		
//	  conn_param.intv_min = 256;
//		conn_param.intv_max = 256;
//		conn_param.latency = 3;
//		conn_param.time_out = 500;
//		host2_appm_update_param(&conn_param, TASK_ADV_DEVICE1);
		
		
    // Send the message
    ke_msg_send(req);		
}

static int app_host2_uart_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

static int app_host2_uart_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                        struct device1_uart_cfg_indntf_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Do nothing
  //printf("host2 cfg changed %d \n", param->ntf_ind_cfg);
	if(param->ntf_ind_cfg)
	{
				if(param->conidx == app_host_env_1.connection_index)
				{
					  app_host_env_1.receive_begin = 0;
				}
				else if(param->conidx == app_host_env_2.connection_index)
				{
						app_host_env_2.receive_begin = 0;
				}
	}
	else
	{
			
	}
    return (KE_MSG_CONSUMED);
}


void app_meter_send_data_to_collection1(unsigned char length, unsigned char *p_buffer)
{
#if 0
		

		struct device1_uart_data_send_req * req = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_REQ,
                                                    prf_get_task_from_id(TASK_ID_DEVICE1_UART),
                                                    TASK_ADV_DEVICE1,
                                                    device1_uart_data_send_req);
		req->connection_idx = adv_device1_env.conidx;
		mtu_size = gattc_get_mtu(req->connection_idx);
		uint16_t value_len = co_min(length, (mtu_size-(ATT_CODE_LEN + ATT_HANDLE_LEN)));
		
		req->length = length;
		memcpy(req->tx_data, p_buffer, req->length);
		
		ke_msg_send(req);
#else
	uint16_t mtu_size;
		
  uint16_t value_len ;
		
	while(1)
	{
	
				if(length > 0)
				{
						struct device1_uart_data_send_req * req = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_REQ,
																															prf_get_task_from_id(TASK_ID_DEVICE1_UART),
																															TASK_ADV_DEVICE1,
																															device1_uart_data_send_req);
						
						req->connection_idx = adv_device1_env.conidx;
						
						mtu_size = gattc_get_mtu(req->connection_idx);
						
						value_len = co_min(length, (mtu_size-(ATT_CODE_LEN + ATT_HANDLE_LEN)));
						
						req->length = value_len;
						
						length -= value_len;
						
						memcpy(req->tx_data, p_buffer, req->length);	
						
						p_buffer += req->length;
						
						ke_msg_send(req);
			}
			else
			{
					break;
			}
	}
	
#endif		
		
	
}


static int app_host2_uart_rx_handler(ke_msg_id_t const msgid,
                                        struct device1_uart_rx_data const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
			uint8_t * p_data;
			uint16_t  temp_length; 
	
	       // printf("app_host2_uart_rx_handler param->length = %d \n", param->length);
			if(param->conidx == app_host_env_1.connection_index)
			{
					//printf("app_host_env_1.connection_index = %d \n", app_host_env_1.connection_index);
					p_data = param->rx_data;
					
					if(app_host_env_1.receive_begin == 0)
					{
							app_host_env_1.need_data_number = 0;
							app_host_env_1.receive_index = 0;
						
							if((p_data[0] == 0xFE)&&(p_data[1] == 0xFE)&&(p_data[2] == 0xFE)&&(p_data[3] == 0xFE))
							{
									p_data += 4;
									app_host_env_1.need_data_number += 4;
							}
							else
							{
									app_host_env_1.receive_data[0] = 0xFE;
									app_host_env_1.receive_data[1] = 0xFE;
									app_host_env_1.receive_data[2] = 0xFE;
									app_host_env_1.receive_data[3] = 0xFE;
									app_host_env_1.receive_index  += 4;
							}
							
							if(p_data[0] == 0x68)
							{
									app_host_env_1.receive_begin = 1;	
							}
							else
							{
									return (KE_MSG_CONSUMED);
							}
							
							temp_length = p_data[2];
							temp_length <<= 8;
							temp_length += p_data[1];
							app_host_env_1.need_data_number += temp_length;  
							app_host_env_1.need_data_number += 2;      //0x68, 0x16
							
							//printf("app_host_env_1.need_data_number = %d \n", app_host_env_1.need_data_number);
							
							memcpy(app_host_env_1.receive_data + app_host_env_1.receive_index, param->rx_data, param->length);
							
							app_host_env_1.receive_index += param->length;
							
							app_host_env_1.need_data_number -= param->length;
					}
					else
					{
							memcpy(app_host_env_1.receive_data + app_host_env_1.receive_index, param->rx_data, param->length);
							
							app_host_env_1.receive_index += param->length;
							
							app_host_env_1.need_data_number -= param->length;
						
					}
					
					if(app_host_env_1.receive_begin)
					{
							if(app_host_env_1.need_data_number == 0)
							{
									app_host_env_1.receive_begin = 0;
									app_iris_up_collection_data(app_host_env_1.connection_index, app_host_env_1.receive_index, app_host_env_1.receive_data);
							}
					}
			}
			else if(param->conidx == app_host_env_2.connection_index)
			{
					//printf("app_host_env_2.connection_index = %d \n", app_host_env_2.connection_index);
					p_data = param->rx_data;
					
					if(app_host_env_2.receive_begin == 0)
					{
							app_host_env_2.need_data_number = 0;
						
							app_host_env_2.receive_index = 0;
						
							if((p_data[0] == 0xFE)&&(p_data[1] == 0xFE)&&(p_data[2] == 0xFE)&&(p_data[3] == 0xFE))
							{
									p_data += 4;
									app_host_env_2.need_data_number += 4;
							}
							else
							{
									app_host_env_2.receive_data[0] = 0xFE;
									app_host_env_2.receive_data[1] = 0xFE;
									app_host_env_2.receive_data[2] = 0xFE;
									app_host_env_2.receive_data[3] = 0xFE;
									app_host_env_2.receive_index  += 4;
							}
							
							if(p_data[0] == 0x68)
							{
									app_host_env_2.receive_begin = 1;	
							}
							else
							{
									return (KE_MSG_CONSUMED);
							}
							
							temp_length = p_data[2];
							temp_length <<= 8;
							temp_length += p_data[1];
							app_host_env_2.need_data_number += temp_length;  
							app_host_env_2.need_data_number += 2;      //0x68, 0x16
							
							//printf("app_host_env_2.need_data_number = %d \n", app_host_env_2.need_data_number);
							
							memcpy(app_host_env_2.receive_data + app_host_env_2.receive_index, param->rx_data, param->length);
							
							app_host_env_2.receive_index += param->length;
							
							app_host_env_2.need_data_number -= param->length;
					}
					else
					{
							memcpy(app_host_env_2.receive_data + app_host_env_2.receive_index, param->rx_data, param->length);
							
							app_host_env_2.receive_index += param->length;
							
							app_host_env_2.need_data_number -= param->length;
						
					}
					
					if(app_host_env_2.receive_begin)
					{
							if(app_host_env_2.need_data_number == 0)
							{
									app_host_env_2.receive_begin = 0;
									app_iris_up_collection_data(app_host_env_2.connection_index, app_host_env_2.receive_index, app_host_env_2.receive_data);
							}
					}
			}

		return (KE_MSG_CONSUMED);
}

static int app_host2_uart_enable_rsp(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	      //printf("app_host2_uart_enable_rsp ..... \n");	
        return (KE_MSG_CONSUMED);
}

static int app_host2_uart_send_rsp(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
					
          //printf("app_host2_uart_send_rsp ..... \n");	
					return (KE_MSG_CONSUMED);											
}



/// Default State handlers definition
const struct ke_msg_handler app_host2_uart_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_host2_uart_msg_handler},

//    {DEVICE1_UART_TEMP_SEND_RSP,            (ke_msg_func_t)uart_temp_send_rsp_handler},
		
    {DEVICE1_UART_CFG_INDNTF_IND,           (ke_msg_func_t)app_host2_uart_cfg_indntf_ind_handler},

//    {APP_DEVICE1_UART_MEAS_INTV_TIMER,        (ke_msg_func_t)app_uart_meas_intv_timer_handler},
		
		{DEVICE1_UART_RX_DATA_IND,              (ke_msg_func_t)app_host2_uart_rx_handler},  
		
		{DEVICE1_UART_ENABLE_RSP,           (ke_msg_func_t)app_host2_uart_enable_rsp},  
		
		{DEVICE1_UART_TEMP_SEND_RSP,         (ke_msg_func_t)app_host2_uart_send_rsp},  
};

const struct app_subtask_handlers app_host2_uart_handlers = APP_HANDLERS(app_host2_uart);


#endif
