/******************************************************************************
	File Name	  :	  ota_device.c
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/26

	Description :  
    用于和OTA 设备建立连接	

	Changed Log	:
		Liujian  2020/08/26	   - Creation

*****************************************************************************/

#include "ota_device.h"

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))

#define OTA_DEVICE_CONNETION_INTERVAL                  (1*5)

extern const struct ke_task_desc TASK_DESC_OTA_DEVICE;

struct ota_device_env_tag  ota_device_env;

static unsigned int ota_device_is_connected ;

static unsigned char ota_device_ready = 0;   //说明OTA 设备已经连接好了

static unsigned char ota_send_data_flag = 0;

static struct gap_bdaddr ask_connected_address;
void app_ota_device_init(void)
{
	
		memset(&ota_device_env, 0, sizeof(ota_device_env));
	
		ke_task_create(TASK_OTA_DEVICE, &TASK_DESC_OTA_DEVICE);
	
		ota_device_env.interval = OTA_DEVICE_CONNETION_INTERVAL;
		ota_device_env.connection_state = APP_OTA_DEVICE_STATE_IDLE;
		ota_device_is_connected = 0;
	  app_ota_device_set_state(0);
}

void app_ota_device_config(void)
{
		// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD, TASK_GAPM, TASK_OTA_DEVICE, gapm_set_dev_config_cmd);
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

void app_ota_device_create(void)
{
		struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM, TASK_OTA_DEVICE, gapm_activity_create_cmd);
	
		p_cmd->operation                       = GAPM_CREATE_INIT_ACTIVITY;
		p_cmd->own_addr_type                   = GAPM_STATIC_ADDR;
	
	  ke_msg_send(p_cmd);
			
	  ota_device_env.connection_state = APP_OTA_DEVICE_STATE_CREATING;
	
	
}

unsigned int app_ota_device_comp_addr(struct gap_bdaddr peer_addr)
{
	   if(!strcmp((const char *)&ask_connected_address, (const char *)&peer_addr))
			 return 1;
		 else
			 return 0;
	
}

void app_ota_set_connect_addr(struct gap_bdaddr addr)
{
	   ask_connected_address = addr;
}



void app_ota_device_connection_start(struct gap_bdaddr peer_addr)
{
		if(ota_device_env.connection_state == APP_OTA_DEVICE_STATE_CREATED)
		{
				if(!app_ota_device_is_connected())
				{
					app_ota_device_set_connected();
					
					struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_OTA_DEVICE,
																																								 gapm_activity_start_cmd);

				
				  p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
					p_start_cmd->actv_idx                                   = ota_device_env.act_id;
					p_start_cmd->u_param.init_param.type                    = GAPM_INIT_TYPE_DIRECT_CONN_EST;
					p_start_cmd->u_param.init_param.prop                    = GAPM_INIT_PROP_1M_BIT ;
					p_start_cmd->u_param.init_param.conn_to            			= 0;
				
					p_start_cmd->u_param.init_param.scan_param_1m.scan_intv = ota_device_env.interval;
					p_start_cmd->u_param.init_param.scan_param_1m.scan_wd   = ota_device_env.interval;
					
				
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_max = 16; //15*ota_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_intv_min = 16; //15*ota_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.supervision_to = 1000; //100*connection_devive1_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.conn_latency   = 3;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_min = ota_device_env.interval;
					p_start_cmd->u_param.init_param.conn_param_1m.ce_len_max = ota_device_env.interval;
					p_start_cmd->u_param.init_param.peer_addr                		 = peer_addr;
					
					// Send message
					ke_msg_send(p_start_cmd);
				}
		}
	
}

void app_ota_device_exchange_mtu(void)
{
	  struct gattc_exc_mtu_cmd *cmd = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                  KE_BUILD_ID(TASK_GATTC, ota_device_env.conidx), TASK_OTA_DEVICE,
                                                  gattc_exc_mtu_cmd);
		cmd->operation = GATTC_MTU_EXCH;
		
		ke_msg_send(cmd);
}

//添加主设备的 profile

void app_ota_device_add_profile(void)
{

		struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_OTA_DEVICE,
                                                  gapm_profile_task_add_cmd);
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_OTA_MASTER;
    req->app_task = TASK_OTA_DEVICE;
    req->start_hdl = 0;
    // Send the message
    ke_msg_send(req);

}

//发现对方设备的 服务
void app_ota_device_find_service(void)
{
	
			// Allocate the message
    struct ota_master_enable_req *req = KE_MSG_ALLOC(OTA_MASTER_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_enable_req);
		
		req->con_type = PRF_CON_DISCOVERY;
		req->connection_index = ota_device_env.conidx;


    // Send the message
    ke_msg_send(req);
			
}


//设置OTA 设备的存储信息
void app_ota_device_set_memory_information(void)
{
	  struct ota_master_write_cmd *req = KE_MSG_ALLOC(OTA_MASTER_WRITE_CMD,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_write_cmd);
		req->write_code =OTA_WD_MEM_DEV;
		//EFLASH + mem_base_add
		req->value.ota_mem_dev_info = (0x14 << 24 ) +  0x00 ;
		ke_msg_send(req);
	
}

void app_ota_device_ota_cmd(unsigned int cmd)
{
	 struct ota_master_write_cmd *req = KE_MSG_ALLOC(OTA_MASTER_WRITE_CMD,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_write_cmd);
		req->write_code =OTA_WD_MEM_DEV;
		//EFLASH + mem_base_add
		req->value.ota_mem_dev_info = (cmd << 24 ) +  0x00 ;
		ke_msg_send(req);
	
}

//设置每次传送patch 的长度

void app_ota_device_patch_len(uint16_t len )
{
	  struct ota_master_write_cmd *req = KE_MSG_ALLOC(OTA_MASTER_WRITE_CMD,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_write_cmd);
		req->write_code =OTA_WD_PATCH_LEN;
    req->value.ota_patch_len = len;
		ke_msg_send(req);
	
}

void app_ota_device_patch_data(unsigned char *p_buffer, uint16_t length)
{
		struct ota_master_write_cmd *req = KE_MSG_ALLOC(OTA_MASTER_WRITE_CMD,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_write_cmd);
		req->write_code =OTA_WD_PATCH_DATA;
		req->data_length = length;
    memcpy(req->value.ota_patch_data, p_buffer, length);
		ke_msg_send(req);	
		
		ota_send_data_flag = 1;
}	


void app_ota_device_patch_data_send_ok(void)
{
		ota_send_data_flag = 0;
}

unsigned char app_ota_device_patch_data_send_state(void)
{
		return ota_send_data_flag;
}

//
void app_ota_device_enable_report(unsigned int connection_index)
{
		struct ota_master_write_cmd *req = KE_MSG_ALLOC(OTA_MASTER_WRITE_CMD,
                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
                                                TASK_OTA_DEVICE,
                                                ota_master_write_cmd);
		req->write_code =OTA_WD_MEM_DEV;
		req->value.ota_mem_dev_info = 0x0400;
		ke_msg_send(req);
		
//		struct voice_master_cfg_indntf_req *req1 = KE_MSG_ALLOC(VOICE_MASTER_CFG_INDNTF_REQ,
//                                                prf_get_task_from_id(TASK_ID_OTA_MASTER),
//                                                TASK_OTA_DEVICE,
//                                                voice_master_cfg_indntf_req);
//		req1->cfg_val = PRF_CLI_START_IND;
//		req1->char_code = VOICE_MASTER_DATA_CHAR;
//		req1->connection_index = connection_index;
//		ke_msg_send(req1);
		
		
}

void app_ota_device_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, ota_device_env.conidx), TASK_OTA_DEVICE,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}




unsigned int app_ota_device_is_connected(void)
{
		return ota_device_is_connected;	
}

void app_ota_device_set_connected(void)
{
		ota_device_is_connected = 1;
}

void app_ota_device_reconnect(void)
{	
//	  app_device_enable_scan_duanluqi1_address(0);   //需要继续扫描
	
	  ota_device_is_connected = 0;
	
//	  app_device_set_duanluqi_1_connected_flag(0);
}

unsigned char app_ota_device_ready(void)
{
		return ota_device_ready;
}

void app_ota_device_set_state(unsigned char state)
{
		ota_device_ready = state;
}



#endif

