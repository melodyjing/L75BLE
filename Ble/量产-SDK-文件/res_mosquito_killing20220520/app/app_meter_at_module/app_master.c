/******************************************************************************
	File Name	:	app_iris_device.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/06/10

	Description : 
    使用　master profile, 应用 device1_client_uart profile 	

	Changed Log	:
		Liujian  2020/06/10			- Creation

*****************************************************************************/
#include "app_master.h"
#include "master_com_task.h"
#include "master_scan.h"
#include "slave_device1_connection.h"
#if (COBRA_LOGGER_ENABLE == 1)	
#include <elog.h>
#endif
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

extern struct connection_device_env_tag  connection_devive1_env;
//extern struct connection_device_env_tag  connection_devive2_env;
//extern struct connection_device_env_tag  connection_devive3_env;
//添加主设备的 profile
static int32_t iris_msg_number;  //2020/9/8

void app_master_increase_msg(void)
{
	   iris_msg_number++;	
}

void app_master_decrease_msg(void)
{
	  iris_msg_number--;	
}

int32_t app_master_get_msg(void)
{
	  return iris_msg_number;
}


void app_master_device_add_profile(void)
{

  struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd);
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //UNAUTH) ; //NO_AUTH);
    req->prf_task_id = TASK_ID_DEVICE1_COLLECTOR_UART;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
	
	  iris_msg_number = 0;
		
}

//让 server 可以上报数据
void app_master_device_enable_report(uint8_t conidx)
{
		struct uart_client_cfg_indntf_req *req = KE_MSG_ALLOC(UART_CFG_INDNTF_REQ,
                                                prf_get_task_from_id(KE_BUILD_ID(TASK_ID_DEVICE1_COLLECTOR_UART, conidx)),
                                                TASK_APP,
                                                uart_client_cfg_indntf_req);
		req->cfg_val = PRF_CLI_START_NTF;
		req->char_code = DEVICE1_UART_TX_CHAR;

		ke_msg_send(req);
			
}

//发现对方设备的 服务
void app_master_device_find_service(uint8_t conidx)
{

			// Allocate the message
    struct device1_client_uart_enable_req *req = KE_MSG_ALLOC(DEVICE1_CLIENT_UART_ENABLE_REQ,
																								 prf_get_task_from_id(KE_BUILD_ID(TASK_ID_DEVICE1_COLLECTOR_UART, conidx)),
                                                 TASK_APP,
                                                 device1_client_uart_enable_req);
	
		req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    ke_msg_send(req);
	
		
}




static int app_master_device_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

static int app_master_device_connection_handler(ke_msg_id_t const msgid,
                                      struct device1_client_uart_enable_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
		uint8_t conidx = KE_IDX_GET(src_id);
	
		if(param->status == GAP_ERR_NO_ERROR)
		{
				//printf("connect OK dest_id = %d, src_id = %d ...... \n", dest_id, src_id);
			
		    if(conidx == connection_devive1_env.conidx)
				{
							connection_devive1_env.src_id = src_id;
							app_device_set_duanluqi_1_connected_flag(1);
					    app_device1_scan_clear_global_flag(SERVER_DEVICE1_INDEX);
					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL1, CONNECT_OK);
#if (COBRA_LOGGER_ENABLE == 1)			
             log_i("+GATTSTA=0,3");
#endif
					
				}
//        else if(conidx == connection_devive2_env.conidx)
//				{
//							connection_devive2_env.src_id = src_id;
//							app_device_set_duanluqi_2_connected_flag(1);
//					    app_device1_scan_clear_global_flag(SERVER_DEVICE2_INDEX);
//					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL2, CONNECT_OK);
//					
//				}
//				else if(conidx == connection_devive3_env.conidx)
//				{
//							connection_devive3_env.src_id = src_id;
//							app_device_set_duanluqi_3_connected_flag(1);
//							app_device1_scan_clear_global_flag(SERVER_DEVICE3_INDEX);
//					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL3, CONNECT_OK);
//				}
				
			  //使能 server, 让断路器可以上报数据
			  app_master_device_enable_report(conidx);

		}
		return (KE_MSG_CONSUMED);
	
}

static int app_master_device_rx_handler(ke_msg_id_t const msgid,
                                        struct uart_client_rx_data_ind  const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
//收到断路器的发送过来的蓝牙数据
//2020/2/14
	uint8_t conidx = KE_IDX_GET(src_id);
	
//	printf("app_iris_device_rx_handler connection id = %d \n", conidx);
	if(conidx == connection_devive1_env.conidx)
	{
			app_slave_connetion_device1_rx_data(param);
	}
//  else if(conidx == connection_devive2_env.conidx)
//	{
//			app_connetion_device2_rx_data(param);			
//	}
//	else if(conidx == connection_devive3_env.conidx)
//	{
//			app_connetion_device3_rx_data(param);			
//	}
	
	return (KE_MSG_CONSUMED);
}

static int app_master_device_cmp_handler(ke_msg_id_t const msgid,
                                      struct uart_client_cmp_evt const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	  uint8_t conidx = KE_IDX_GET(src_id);
	
	  //printf("app_iris_device_cmp_handler conidx = %d  operation = %d ,status = %d ....... \n", conidx, param->operation ,param->status);
    
	  if(param->operation == UART_TX_DATA_CMD_OP_CODE)
		{
				if(param->status == GAP_ERR_NO_ERROR)
				{

				}
		}	
    else if(UART_CFG_INDNTF_CMD_OP_CODE == param->operation)
		{
				if(param->status == GAP_ERR_NO_ERROR)
				{
#if(DIANBIAO_AUTO_ENABLE == 1)
		      		if(conidx == connection_devive1_env.conidx)
							{
									app_iris_attconnect_report_try_state(CONNECT_CHANNEL1, CONNECT_OK);
							}	
              else if(conidx == connection_devive2_env.conidx)
							{
									app_iris_attconnect_report_try_state(CONNECT_CHANNEL2, CONNECT_OK);
							}
							else if(conidx == connection_devive3_env.conidx)
							{
									app_iris_attconnect_report_try_state(CONNECT_CHANNEL3, CONNECT_OK);
							}	
#endif							
				}
		}			
	  
	
	  return (KE_MSG_CONSUMED);
}



/// Default State handlers definition
const struct ke_msg_handler app_master_device_msg_handler_list[] =
{
		// Note: first message is latest message checked by kernel so default is put on top.
		{KE_MSG_DEFAULT_HANDLER,               (ke_msg_func_t)app_master_device_msg_handler},
		
		{DEVICE1_CLIENT_ENABLE_RSP,            (ke_msg_func_t)app_master_device_connection_handler},

		{UART_CLIENT_RX_DATA_IND,              (ke_msg_func_t)app_master_device_rx_handler},  
		
		{UART_CLIENT_CMP_EVT,                  (ke_msg_func_t)app_master_device_cmp_handler},  

};

const struct app_subtask_handlers app_master_device_handlers = APP_HANDLERS(app_master_device);

#endif


