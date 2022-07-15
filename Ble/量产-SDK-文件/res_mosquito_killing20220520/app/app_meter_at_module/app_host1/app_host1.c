/******************************************************************************
	File Name	:	  app_host1.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/09

	Description : 
      蓝牙模组, 可以通过 AT COMMAND 来控制


	Changed Log	:
	
		Liujian  2021/03/09	 - Creation

*****************************************************************************/
#include "app_host1.h"
#include "gattc.h"
#include "slave_com_task.h"
#if (COBRA_LOGGER_ENABLE == 1)
#include "elog.h"
#endif
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))

struct app_host_uart_env_tag app_host_env_1;
//extern struct app_host_uart_env_tag app_host_env_2;	
void app_host1_uart_init(void)
{
    // Reset the environment
    memset(&app_host_env_1, 0, sizeof(app_host_env_1));
}

void app_host1_uart_add_uarts(void)
{
    struct device1_uart_db_cfg* db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct device1_uart_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_128);  //NO_AUTH);    UNAUTH
	//	req->sec_lvl = PERM(SVC_AUTH, AUTH)|PERM(SVC_UUID_LEN, UUID_128);
		
    req->prf_task_id = TASK_ID_AT_CMD_SLAVE;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct device1_uart_db_cfg* ) req->param;
    // All features are supported
    db_cfg->features = DEVICE1_UART_ALL_FEAT_SUP;

//    db_cfg->meas_intv = app_host_env.uart_meas_intv;
    // Send the message
    ke_msg_send(req);

}

void app_host1_uart_enable_prf_app(uint8_t conidx)
{
    // Allocate the message
	
	 
	
    struct device1_uart_enable_req * req = KE_MSG_ALLOC(DEVICE1_UART_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_AT_CMD_SLAVE),
                                                TASK_APP,
                                               device1_uart_enable_req);

    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;
		
    // Send the message
    ke_msg_send(req);		
}

static int app_host1_uart_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

static int app_host1_uart_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                        struct device1_uart_cfg_indntf_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{

	  if(param->ntf_ind_cfg)
		{	
				if(param->conidx == app_host_env_1.connection_index)
				{
					  app_host_env_1.receive_begin = 0;
				}
#if (BLE_HOST2_UART)
				else if(param->conidx == app_host_env_2.connection_index)
				{
						app_host_env_2.receive_begin = 0;
				}
#endif
				
		}
		else
		{
				
		}
    return (KE_MSG_CONSUMED);
}


void app_meter_send_data_to_master(uint8_t length, uint8_t *p_buffer)
{
	
#if 0
    struct device1_uart_data_send_req * req = KE_MSG_ALLOC(DEVICE1_UART_TEMP_SEND_REQ,
                                                    prf_get_task_from_id(TASK_ID_AT_CMD_SLAVE),
                                                    TASK_APP,
                                                    device1_uart_data_send_req);

		req->connection_idx = app_env.conidx;
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
																															prf_get_task_from_id(TASK_ID_AT_CMD_SLAVE),
																															TASK_APP,
																															device1_uart_data_send_req);
						
						req->connection_idx = app_env.conidx;
						
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




static int app_host1_uart_rx_handler(ke_msg_id_t const msgid,
                                        struct device1_uart_rx_data const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{


			uint8_t * p_data;
			uint16_t  temp_length; 
	    uint8_t   i;
#if (COBRA_LOGGER_ENABLE == 1)
	   elog_raw("+DATA=0,%d,", param->length);	
	   for(i = 0; i < param->length; i++)
	   {
			   elog_raw("%c", param->rx_data[i]); 
		 }
     elog_raw("\n");
#endif	
#if 0	
	    printf("app_host_uart_rx_handler param->conidx = %d %d\n", param->conidx,app_host_env_1.connection_index);
			if(param->conidx == app_host_env_1.connection_index)
			{
					//printf("app_host_env_1.connection_index = %d \n", app_host_env_1.connection_index);
					p_data = param->rx_data;
					
					if(app_host_env_1.receive_begin == 0)
					{
							app_host_env_1.need_data_number = 0;
							app_host_env_1.receive_index = 0;
						  i = 0;
						  while(*p_data == 0xFE)
							{
								   p_data += 1;
								   app_host_env_1.need_data_number += 1;

							}						
						  printf("p_data = %d %d %d %d\n", p_data[0], p_data[1], p_data[2], p_data[3]);

							if(app_host_env_1.need_data_number == 0)
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
							printf("temp_length = %d\n",temp_length);
							printf("app_host_env_1.need_data_number = %d \n", app_host_env_1.need_data_number);
							
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
					printf("app_host_env_1.receive_begin = %d %d\n", app_host_env_1.receive_begin,app_host_env_1.need_data_number);	
					if(app_host_env_1.receive_begin)
					{
							if(app_host_env_1.need_data_number == 0)
							{
									app_host_env_1.receive_begin = 0;
						//			app_realtek_up_collection_data(app_host_env_1.connection_index, app_host_env_1.receive_index, app_host_env_1.receive_data);
							}
					}
			}
#endif
		  return (KE_MSG_CONSUMED);
}

static int app_host1_uart_enable_rsp(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{  
        // Do nothing
	     // printf("app_host_enable_rsp ...... \n");
        return (KE_MSG_CONSUMED);
}

static int app_host1_uart_send_rsp(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
				//	printf("app_host_uart_send_rsp..... \n");	
					return (KE_MSG_CONSUMED);											
}




/// Default State handlers definition
const struct ke_msg_handler app_host1_uart_msg_handler_list[] =
{
		// Note: first message is latest message checked by kernel so default is put on top.
		{KE_MSG_DEFAULT_HANDLER,         (ke_msg_func_t)app_host1_uart_msg_handler},
		
		{DEVICE1_UART_CFG_INDNTF_IND,    (ke_msg_func_t)app_host1_uart_cfg_indntf_ind_handler},
	
		{DEVICE1_UART_RX_DATA_IND,       (ke_msg_func_t)app_host1_uart_rx_handler},  
		
		{DEVICE1_UART_ENABLE_RSP,        (ke_msg_func_t)app_host1_uart_enable_rsp},  
		
		{DEVICE1_UART_TEMP_SEND_RSP,     (ke_msg_func_t)app_host1_uart_send_rsp},  
};

const struct app_subtask_handlers app_host1_uart_handlers = APP_HANDLERS(app_host1_uart);


#endif
