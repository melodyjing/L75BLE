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
#if (BLE_DEVICE3_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "device3_connection.h"
#include "device3_connection_task.h"
#include "device1_client_uart_task.h"
#include "device1_scan.h"
#include "cobra_pinmux.h"
#include "rdw_gpio.h"
#include "rdw_mcu.h"
#include "prf_types.h"
#include "app_realtek_console.h"
#include "gattc.h"
#include "app_realtek_device.h"



#define M_ADV_CONNETION_INTERVAL                  (1*5)

extern CONST struct ke_task_desc TASK_DESC_CONNECTION_DEV3;

struct connection_device3_env_tag  connection_devive3_env;

LOCAL UINT device3_is_connected;


void app_device3_connection_init(void)
{
	
		memset(&connection_devive3_env, 0, sizeof(connection_devive3_env));
	
		ke_task_create(TASK_CONNECTION_DEVICE3, &TASK_DESC_CONNECTION_DEV3);
	
		connection_devive3_env.interval = M_ADV_CONNETION_INTERVAL;
	
		device3_is_connected = 0;
	  
	  connection_devive3_env.conidx = GAP_INVALID_CONIDX;
	
	  connection_devive3_env.task_ok = 0;
	
}

void app_device3_set_task_ready(void)
{
		connection_devive3_env.task_ok = 1;
}

UINT app_device3_task_is_ready(void)
{
	  return connection_devive3_env.task_ok;
}

void app_device3_connection_create(void)
{
		struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
												 TASK_CONNECTION_DEVICE3, gapm_activity_create_cmd);
			p_cmd->operation                       = GAPM_CREATE_INIT_ACTIVITY;
			p_cmd->own_addr_type                   = GAPM_STATIC_ADDR;
	
			
			ke_msg_send(p_cmd);
			
			connection_devive3_env.connection_state = APP_DEVICE3_CONNECTION_STATE_CREATING;
	
	
}


void app_device3_reconnect(void)
{
		app_device1_scan_clear_global_flag(SERVER_DEVICE3_INDEX);
	
	  app_device_enable_scan_duanluqi3_address(0); 
		app_device_set_duanluqi_3_connected_flag(0);  
	
		device3_is_connected = 0;
#if DIANBIAO_AUTO_ENABLE
		app_iris_attconnect_report_try_state(CONNECT_CHANNEL3, CONNECT_FAIL);
#endif
}

void app_device3_connected(void)
{
		device3_is_connected = 1;	
}

UINT app_device3_is_connected(void)
{
		return device3_is_connected;
}

void app_device3_connection_start(struct gap_bdaddr peer_addr)
{
	   USHORT connection_interval;
	   UCHAR  latency;
	   USHORT supervision_timeout;
	
		if(connection_devive3_env.connection_state == APP_DEVICE3_CONNECTION_STATE_CREATED)
		{
				if(!app_device3_is_connected())
				{
					  app_device3_connected();
					
				    struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, 
																				TASK_CONNECTION_DEVICE3,
																			   gapm_activity_start_cmd);

					 p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
					p_start_cmd->actv_idx                                   = connection_devive3_env.act_id;
					p_start_cmd->u_param.init_param.type                    = GAPM_INIT_TYPE_DIRECT_CONN_EST;
					p_start_cmd->u_param.init_param.prop                    = GAPM_INIT_PROP_1M_BIT ;
					p_start_cmd->u_param.init_param.conn_to            			= 0;
				
					p_start_cmd->u_param.init_param.scan_param_1m.scan_intv = connection_devive3_env.interval;
					p_start_cmd->u_param.init_param.scan_param_1m.scan_wd   = connection_devive3_env.interval;
#if (DIANBIAO_PARAM_CMD_ENABE == 1)
				  connection_interval = app_device_get_connection_interval();
				  latency = app_device_get_latency();
				  supervision_timeout = app_device_get_supervision_timeout();
				  if(connection_interval != 0)
					{
             p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = (float)connection_interval/1.25;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = (float)connection_interval/1.25;
					   p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = supervision_timeout/10; //1000; //100*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = latency;
					}
					else
					{
						 p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 15*connection_devive3_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 15*connection_devive3_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //1000; //100*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
					}
#else						
				
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 15*connection_devive3_env.interval;   
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 15*connection_devive3_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //100*connection_devive3_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
#endif
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_min = connection_devive3_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_max = connection_devive3_env.interval;
					p_start_cmd->u_param.init_param.peer_addr                = peer_addr;
					
					// Send message
					ke_msg_send(p_start_cmd);
				}
		}
	
}


//2019/11/29 ???? MTU

void app_device3_exchange_mtu(void)
{
	  struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                  KE_BUILD_ID(TASK_GATTC, connection_devive3_env.conidx), TASK_CONNECTION_DEVICE3,
                                                  gattc_exc_mtu_cmd);
		cmd->operation = GATTC_MTU_EXCH;
		
		ke_msg_send(cmd);
}



void app_connetion_device3_rx_data(struct uart_client_rx_data_ind  CONST *param)
{
	    UINT8 * p_data;
			UINT16  temp_length;

			p_data = param->value;
			
			if(connection_devive3_env.receive_begin == 0)
			{
					connection_devive3_env.need_data_number = 0;
					connection_devive3_env.receive_index = 0;
				
					while(*p_data == 0xFE)
					{
							 p_data += 1;
							 connection_devive3_env.need_data_number += 1;
					}	
							
//					if((p_data[0] == 0xFE)&&(p_data[1] == 0xFE)&&(p_data[2] == 0xFE)&&(p_data[3] == 0xFE))
//					{
//							p_data += 4;
//							connection_devive3_env.need_data_number += 4;
//					}
//					else
//					{
//							connection_devive3_env.receive_data[0] = 0xFE;
//							connection_devive3_env.receive_data[1] = 0xFE;
//							connection_devive3_env.receive_data[2] = 0xFE;
//							connection_devive3_env.receive_data[3] = 0xFE;
//							connection_devive3_env.receive_index  += 4;
//					}
					
				  if(connection_devive3_env.need_data_number == 0)
					{
							connection_devive3_env.receive_data[0] = 0xFE;
							connection_devive3_env.receive_data[1] = 0xFE;
							connection_devive3_env.receive_data[2] = 0xFE;
							connection_devive3_env.receive_data[3] = 0xFE;
							connection_devive3_env.receive_index  += 4;
					}	
					
					if(p_data[0] == 0x68)
					{
							connection_devive3_env.receive_begin = 1;	
					}
					else
					{
							return ;
					}
					
					temp_length = p_data[2];
					temp_length <<= 8;
					temp_length += p_data[1];
					connection_devive3_env.need_data_number += temp_length;  
					connection_devive3_env.need_data_number += 2;      //0x68, 0x16
					
					//printf("app_host_env_1.need_data_number = %d \n", app_host_env_1.need_data_number);
					
					memcpy(connection_devive3_env.receive_data + connection_devive3_env.receive_index, param->value, param->length);
					
					connection_devive3_env.receive_index += param->length;
					
					connection_devive3_env.need_data_number -= param->length;
			}
			else
			{
					memcpy(connection_devive3_env.receive_data + connection_devive3_env.receive_index, param->value, param->length);
					
					connection_devive3_env.receive_index += param->length;
					
					connection_devive3_env.need_data_number -= param->length;
				
			}
			
			if(connection_devive3_env.receive_begin)
			{
					if(connection_devive3_env.need_data_number == 0)
					{
							connection_devive3_env.receive_begin = 0;
							app_iris_dianbiao_uart_output(3, connection_devive3_env.receive_index, connection_devive3_env.receive_data);
					}
			}
	
}



//??????????  server ????
//2020/2/14 liujian 

void app_device3_send_data_to_uart_server(UCHAR length, UCHAR * p_buffer)
{
	UINT16 mtu_size;
		
  UINT16 value_len ;
		
	if(connection_devive3_env.conidx == GAP_INVALID_CONIDX) return;
		
	while(1)
	{
	
				if(length > 0)
				{
						struct uart_client_tx_data_req *req = KE_MSG_ALLOC(DEVICE1_UART_CLIENT_TX_DATA_REQ,
                                                prf_get_task_from_id(KE_BUILD_ID(TASK_ID_DEVICE1_COLLECTOR_UART, connection_devive3_env.conidx)),
                                                TASK_CONNECTION_DEVICE3,
                                                uart_client_tx_data_req);
						
//						req->connection_index = connection_devive3_env.conidx;
						
						mtu_size = gattc_get_mtu(connection_devive3_env.conidx);
						
						value_len = co_min(length, (mtu_size-(ATT_CODE_LEN + ATT_HANDLE_LEN)));
						
						req->length = value_len;
						
						length -= value_len;
						
						memcpy(req->value, p_buffer, req->length);	
						
						p_buffer += req->length;
						
						ke_msg_send(req);
						
						app_iris_increase_msg();
			}
			else
			{
					break;
			}
	}

}

void app_device3_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, connection_devive3_env.conidx), TASK_CONNECTION_DEVICE3,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}

void app_device3_delete_connection(void)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_delete_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
                                                              TASK_GAPM, TASK_CONNECTION_DEVICE3,
                                                              gapm_activity_delete_cmd);

    // Set operation code
    p_cmd->operation = GAPM_DELETE_ACTIVITY;
		p_cmd->actv_idx = connection_devive3_env.act_id;

    // Send the message
    ke_msg_send(p_cmd);
 
}

void app_device3_bond_command(void)
{
	
		struct  gapc_bond_cmd *cmd = KE_MSG_ALLOC(GAPC_BOND_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, connection_devive3_env.conidx), TASK_CONNECTION_DEVICE3,
                                                   gapc_bond_cmd);

		cmd->operation = GAPC_BOND;
		
		cmd->pairing.iocap = GAP_IO_CAP_KB_ONLY;
		cmd->pairing.oob = GAP_OOB_AUTH_DATA_NOT_PRESENT;
		cmd->pairing.auth = GAP_AUTH_REQ_MITM_NO_BOND;
		cmd->pairing.key_size = 16;
		cmd->pairing.ikey_dist = GAP_KDIST_NONE;
		cmd->pairing.rkey_dist = GAP_KDIST_ENCKEY;
		cmd->pairing.sec_req = GAP_NO_SEC;


    // Send the message
    ke_msg_send(cmd);
	
}


#endif
