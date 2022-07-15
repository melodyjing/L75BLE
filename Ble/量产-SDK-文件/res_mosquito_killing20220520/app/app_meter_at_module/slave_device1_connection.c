/******************************************************************************
	File Name	:	device_connection.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/25

	Description :   

	Changed Log	:
		Liujian  2019/11/25			- Creation

*****************************************************************************/

#include "slave_device1_connection.h"
#include "cobra_pinmux.h"
#include "rdw_gpio.h"
#include "rdw_mcu.h"
#include "prf_types.h"
#include "gattc.h"
#include "app_master.h"
#include "master_scan.h"
#if (COBRA_LOGGER_ENABLE == 1)	
#include <elog.h>
#endif
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#define M_ADV_CONNETION_INTERVAL                  (1*5)

extern const struct ke_task_desc TASK_DESC_CONNECTION_DEV1;

struct connection_device_env_tag  connection_devive1_env;

static unsigned int device1_is_connected ;

void app_slave_device1_connection_init(void)
{
	
		memset(&connection_devive1_env, 0, sizeof(connection_devive1_env));
	
		ke_task_create(TASK_CONNECTION_DEVICE1, &TASK_DESC_CONNECTION_DEV1);
	
		connection_devive1_env.interval = M_ADV_CONNETION_INTERVAL;
	
		device1_is_connected = 0;
	
	  connection_devive1_env.conidx = GAP_INVALID_CONIDX;
	
	  connection_devive1_env.task_ok = 0;
}


void app_slave_device1_set_task_ready(void)
{
		connection_devive1_env.task_ok = 1;
}

/*
    任务已经建立后, 才可以使用
*/
unsigned int app_slave_device1_task_is_ready(void)
{
	  return connection_devive1_env.task_ok;
}


void app_slave_device1_connection_create(void)
{
		struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM, TASK_CONNECTION_DEVICE1, gapm_activity_create_cmd);
	
		p_cmd->operation                       = GAPM_CREATE_INIT_ACTIVITY;
		p_cmd->own_addr_type                   = GAPM_STATIC_ADDR;
	
	  ke_msg_send(p_cmd);
			
	  connection_devive1_env.connection_state = APP_DEVICE_CONNECTION_STATE_CREATING;
	
	
}


void app_slave_connetion_device1_rx_data(struct uart_client_rx_data_ind  const *param)
{
// 收到对方发送过来的数据	
#if (COBRA_LOGGER_ENABLE == 1)	
	
	   uint16_t i;
     elog_raw("+DATA = 0, %d, ", 	param->length);
	
	   for(i = 0; i < param->length; i++)
	   {
			   elog_raw("%02x", param->value[i]);
		 }
		 elog_raw("\n");
		 
#endif
	
}


void app_slave_device1_reconnect(void)
{	
	  app_device1_scan_clear_global_flag(SERVER_DEVICE1_INDEX);
	
	  app_device_enable_scan_duanluqi1_address(0);   //需要继续扫描
	
	  device1_is_connected = 0;
	
	  app_device_set_duanluqi_1_connected_flag(0);

}

void app_slave_device1_connected(void)
{
	 device1_is_connected = 1;
		
}

unsigned int app_device1_is_connected(void)
{
		return device1_is_connected;
}

void app_slave_device1_connection_start(struct gap_bdaddr peer_addr)
{
	   unsigned short connection_interval;
	   unsigned char  latency;
	   unsigned short supervision_timeout;
		if(connection_devive1_env.connection_state == APP_DEVICE_CONNECTION_STATE_CREATED)
		{
				if(!app_device1_is_connected())
				{
					app_slave_device1_connected();
					
				 struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_CONNECTION_DEVICE1,
																																								 gapm_activity_start_cmd);

				  p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
					p_start_cmd->actv_idx                                   = connection_devive1_env.act_id;
					p_start_cmd->u_param.init_param.type                    = GAPM_INIT_TYPE_DIRECT_CONN_EST;
					p_start_cmd->u_param.init_param.prop                    = GAPM_INIT_PROP_1M_BIT ;
					p_start_cmd->u_param.init_param.conn_to            			= 0;
				
					p_start_cmd->u_param.init_param.scan_param_1m.scan_intv = connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.scan_param_1m.scan_wd   = connection_devive1_env.interval;
					
#if (DIANBIAO_MASTER_PARAM_SET_ENABE == 1)
				  connection_interval = app_master_device_get_connection_interval();
				  latency = app_master_device_get_latency();
				  supervision_timeout = app_master_device_get_supervision_timeout();
				  if(connection_interval != 0)
					{
             p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = (float)connection_interval/1.25;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = (float)connection_interval/1.25;
					   p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = supervision_timeout/10; //1000; //100*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = latency;
					}
					else
					{
						 p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 15*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 15*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //1000; //100*connection_devive1_env.interval;
					   p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
					}
#else				 
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 15*connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 15*connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //1000; //100*connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
#endif
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_min = connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_max = connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.peer_addr                		 = peer_addr;
					
					// Send message
					ke_msg_send(p_start_cmd);
				}
		}
	
}


//2019/11/29 交换 MTU

void app_slave_device1_exchange_mtu(void)
{
	  struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                  KE_BUILD_ID(TASK_GATTC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
                                                  gattc_exc_mtu_cmd);
		cmd->operation = GATTC_MTU_EXCH;
		
		ke_msg_send(cmd);
}




//发送数据到  server 串口
//2020/2/14 liujian 
void app_slave_device1_send_data_to_uart_server(unsigned char length, unsigned char *p_buffer)
{

	uint16_t mtu_size;
		
  uint16_t value_len ;
	
  if(connection_devive1_env.conidx == GAP_INVALID_CONIDX) return;
	
	while(1)
	{
	
				if(length > 0)
				{
						struct uart_client_tx_data_req *req = KE_MSG_ALLOC(DEVICE1_UART_CLIENT_TX_DATA_REQ,
                                                 prf_get_task_from_id(KE_BUILD_ID(TASK_ID_DEVICE1_COLLECTOR_UART, connection_devive1_env.conidx)),
                                                 TASK_CONNECTION_DEVICE1,
                                                uart_client_tx_data_req);
						
			
						mtu_size = gattc_get_mtu(connection_devive1_env.conidx);
						
						value_len = co_min(length, (mtu_size-(ATT_CODE_LEN + ATT_HANDLE_LEN)));
						
						req->length = value_len;
						
						length -= value_len;
						
						memcpy(req->value, p_buffer, req->length);	
						
						p_buffer += req->length;
					
						ke_msg_send(req);
						
						app_master_increase_msg();
			}
			else
			{
					break;
			}
	}
								
}

void app_slave_device1_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}

void app_slave_device1_delete_connection(void)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_delete_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
                                                              TASK_GAPM, TASK_CONNECTION_DEVICE1,
                                                              gapm_activity_delete_cmd);

    // Set operation code
    p_cmd->operation = GAPM_DELETE_ACTIVITY;
		p_cmd->actv_idx = connection_devive1_env.act_id;

    // Send the message
    ke_msg_send(p_cmd);
 
}


void app_slave_device1_bond_command(void)
{
	
		struct  gapc_bond_cmd *cmd = KE_MSG_ALLOC(GAPC_BOND_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
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
