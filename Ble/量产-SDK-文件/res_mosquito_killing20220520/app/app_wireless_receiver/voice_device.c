/******************************************************************************
	File Name	:	voice_device.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/6

	Description :   

	Changed Log	:
		Liujian  2020/3/6		- Creation

*****************************************************************************/

#include "voice_device.h"
#include "voice_master_task.h"
#include "rdw_mcu.h"

#if BLE_APP_RECEIVE_VOICE

#define VOCIE_DEVICE_CONNETION_INTERVAL                  (1*5)

extern const struct ke_task_desc TASK_DESC_VOICE_DEVICE;

struct voice_device_env_tag  voice_device_env;

static unsigned int voice_device_is_connected ;

void app_voice_device_init(void)
{
	
		memset(&voice_device_env, 0, sizeof(voice_device_env));
	
		ke_task_create(TASK_VOICE_DEVICE, &TASK_DESC_VOICE_DEVICE);
	
		voice_device_env.interval = VOCIE_DEVICE_CONNETION_INTERVAL;
		voice_device_env.connection_state = APP_VOCIE_DEVICE_STATE_IDLE;
		voice_device_is_connected = 0;
}

void app_voice_device_config(void)
{
		// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD, TASK_GAPM, TASK_VOICE_DEVICE, gapm_set_dev_config_cmd);
			// Set the operation
			cmd->operation = GAPM_SET_DEV_CONFIG;
			// Set the device role - Peripheral
			cmd->role      = GAP_ROLE_ALL;
			
			cmd->max_mtu = COBRA4_MTU;
			cmd->max_mps = COBRA4_MPS;
			// Set Data length parameters
			cmd->sugg_max_tx_octets = LE_MAX_OCTETS; //BLE_MIN_OCTETS;
			cmd->sugg_max_tx_time   = LE_MAX_TIME;
			
			cmd->privacy_cfg = 0;
#if DIANBIAO_ENABLE_PRIV_EN	
			cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
#endif	
			memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
			
			// Send message
			ke_msg_send(cmd);
	
}

void app_voice_device_create(void)
{
		struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM, TASK_VOICE_DEVICE, gapm_activity_create_cmd);
	
		p_cmd->operation                       = GAPM_CREATE_INIT_ACTIVITY;
		p_cmd->own_addr_type                   = GAPM_STATIC_ADDR;
	
	  ke_msg_send(p_cmd);
			
	  voice_device_env.connection_state = APP_VOCIE_DEVICE_STATE_CREATING;
	
	
}




void app_voice_device_connection_start(struct gap_bdaddr peer_addr)
{
		if(voice_device_env.connection_state == APP_VOCIE_DEVICE_STATE_CREATED)
		{
				if(!app_voice_device_is_connected())
				{
					app_voice_device_set_connected();
					
					struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_VOICE_DEVICE,
																																								 gapm_activity_start_cmd);

				
				  p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
					p_start_cmd->actv_idx                                   = voice_device_env.act_id;
					p_start_cmd->u_param.init_param.type                    = GAPM_INIT_TYPE_DIRECT_CONN_EST;
					p_start_cmd->u_param.init_param.prop                    = GAPM_INIT_PROP_1M_BIT ;
					p_start_cmd->u_param.init_param.conn_to            			= 0;
				
					p_start_cmd->u_param.init_param.scan_param_1m.scan_intv = voice_device_env.interval;
					p_start_cmd->u_param.init_param.scan_param_1m.scan_wd   = voice_device_env.interval;
					
				
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 15*voice_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 15*voice_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //100*connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_min = voice_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_max = voice_device_env.interval;
					p_start_cmd->u_param.init_param.peer_addr                		 = peer_addr;
					
					// Send message
					ke_msg_send(p_start_cmd);
				}
		}
	
}

void app_voice_device_exchange_mtu(void)
{
	  struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                  KE_BUILD_ID(TASK_GATTC, voice_device_env.conidx), TASK_VOICE_DEVICE,
                                                  gattc_exc_mtu_cmd);
		cmd->operation = GATTC_MTU_EXCH;
		
		ke_msg_send(cmd);
}

//添加主设备的 profile

void app_voice_device_add_profile(void)
{

		struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_VOICE_DEVICE,
                                                  gapm_profile_task_add_cmd);
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_VOICE_MASTER;
    req->app_task = TASK_VOICE_DEVICE;
    req->start_hdl = 0;
    // Send the message
    ke_msg_send(req);

}

//发现对方设备的 服务
void app_voice_device_find_service(void)
{
	
			// Allocate the message
    struct voice_master_enable_req *req = KE_MSG_ALLOC(VOICE_MASTER_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_VOICE_MASTER),
                                                TASK_VOICE_DEVICE,
                                                voice_master_enable_req);
		
		req->con_type = PRF_CON_DISCOVERY;
		req->connection_index = voice_device_env.conidx;


    // Send the message
    ke_msg_send(req);
			
}

//让 语音采集设备 可以上报数据
void app_voice_device_enable_report(unsigned int connection_index)
{
		struct voice_master_cfg_indntf_req *req = KE_MSG_ALLOC(VOICE_MASTER_CFG_INDNTF_REQ,
                                                prf_get_task_from_id(TASK_ID_VOICE_MASTER),
                                                TASK_VOICE_DEVICE,
                                                voice_master_cfg_indntf_req);
		req->cfg_val = PRF_CLI_START_IND;
		req->char_code = VOICE_MASTER_BEGIN_CHAR;
		req->connection_index = connection_index;
		ke_msg_send(req);
		
		struct voice_master_cfg_indntf_req *req1 = KE_MSG_ALLOC(VOICE_MASTER_CFG_INDNTF_REQ,
                                                prf_get_task_from_id(TASK_ID_VOICE_MASTER),
                                                TASK_VOICE_DEVICE,
                                                voice_master_cfg_indntf_req);
		req1->cfg_val = PRF_CLI_START_IND;
		req1->char_code = VOICE_MASTER_DATA_CHAR;
		req1->connection_index = connection_index;
		ke_msg_send(req1);
		
		
}



unsigned int app_voice_device_is_connected(void)
{
		return voice_device_is_connected;	
}

void app_voice_device_set_connected(void)
{
		voice_device_is_connected = 1;
}

void app_voice_device_reconnect(void)
{	
//	  app_device_enable_scan_duanluqi1_address(0);   //需要继续扫描
	
	  voice_device_is_connected = 0;
	
//	  app_device_set_duanluqi_1_connected_flag(0);
}


#if 0
//发现对方设备的 服务
void app_device1_connection_find_service(void)
{
	
			// Allocate the message
    struct device1_client_uart_enable_req *req = KE_MSG_ALLOC(DEVICE1_CLIENT_UART_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_DEVICE1_COLLECTOR_UART),
                                                TASK_CONNECTION_DEVICE1,
                                                device1_client_uart_enable_req);
		
		req->con_type = PRF_CON_DISCOVERY;
		req->connection_index = connection_devive1_env.conidx;


    // Send the message
    ke_msg_send(req);
			
}

//添加主设备的 profile

void app_device1_connection_add_profile(void)
{


	
  struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_CONNECTION_DEVICE1,
                                                  gapm_profile_task_add_cmd);
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_DEVICE1_COLLECTOR_UART;
    req->app_task = TASK_CONNECTION_DEVICE1;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
		

		
	
}

//2019/11/29 交换 MTU

void app_device1_exchange_mtu(void)
{
	  struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                  KE_BUILD_ID(TASK_GATTC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
                                                  gattc_exc_mtu_cmd);
		cmd->operation = GATTC_MTU_EXCH;
		
		ke_msg_send(cmd);
}

//让 server 可以上报数据
void app_device1_enable_uart_report(unsigned int connection_index)
{
		struct uart_client_cfg_indntf_req *req = KE_MSG_ALLOC(UART_CFG_INDNTF_REQ,
                                                prf_get_task_from_id(TASK_ID_DEVICE1_COLLECTOR_UART),
                                                TASK_CONNECTION_DEVICE1,
                                                uart_client_cfg_indntf_req);
		req->cfg_val = PRF_CLI_START_NTF;
		req->char_code = DEVICE1_UART_TX_CHAR;
		req->connection_index = connection_index;
		ke_msg_send(req);
		
		app_device_set_duanluqi_1_connected_flag(1);
		
		iris_dianbiao_slave_number_rsp();
		
}



//发送数据到  server 串口
void app_device1_send_data_to_uart_server(unsigned char length, unsigned char *p_buffer)
{
		uint8_t i;
		struct uart_client_tx_data_req *req = KE_MSG_ALLOC(UART_CLIENT_TX_DATA_REQ,
                                                prf_get_task_from_id(TASK_ID_DEVICE1_COLLECTOR_UART),
                                                TASK_CONNECTION_DEVICE1,
                                                uart_client_tx_data_req);
	//	printf("app_device1_send_data_to_uart_server....\n");

		req->length = length;
		memcpy(req->value, p_buffer, length);
		req->connection_index = connection_devive1_env.conidx;
		
		ke_msg_send(req);
	
}

void app_device1_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, connection_devive1_env.conidx), TASK_CONNECTION_DEVICE1,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}

#endif


#endif
