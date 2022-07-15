/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description: 使用　client profile, 应用 device1_client_uart profile  

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/
#include "app_realtek_device.h"
#include "device1_client_uart_task.h"
#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"

extern struct connection_device_env_tag  connection_devive1_env;
extern struct connection_device_env_tag  connection_devive2_env;
extern struct connection_device_env_tag  connection_devive3_env;
//添加主设备的 profile
LOCAL INT32 iris_msg_number;  //2020/9/8

void app_iris_increase_msg(void)
{
	   iris_msg_number++;	
}

void app_iris_decrease_msg(void)
{
	  iris_msg_number--;	
}

INT32 app_iris_get_msg(void)
{
	  return iris_msg_number;
}


void app_iris_device_add_profile(void)
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
void app_iris_device_enable_report(UINT8 conidx)
{
		struct uart_client_cfg_indntf_req *req = KE_MSG_ALLOC(UART_CFG_INDNTF_REQ,
                                                prf_get_task_from_id(KE_BUILD_ID(TASK_ID_DEVICE1_COLLECTOR_UART, conidx)),
                                                TASK_APP,
                                                uart_client_cfg_indntf_req);
		req->cfg_val = PRF_CLI_START_NTF;
		req->char_code = DEVICE1_UART_TX_CHAR;

		ke_msg_send(req);
		
		iris_dianbiao_slave_number_rsp();
		
}

//发现对方设备的 服务
void app_iris_device_find_service(UINT8 conidx)
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




LOCAL INT app_iris_device_msg_handler(ke_msg_id_t CONST msgid,
                              void CONST *param,
                              ke_task_id_t CONST dest_id,
                              ke_task_id_t CONST src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

LOCAL INT app_iris_device_connection_handler(ke_msg_id_t CONST msgid,
                                      struct device1_client_uart_enable_rsp CONST *param,
                                      ke_task_id_t CONST dest_id,
                                      ke_task_id_t CONST src_id)
{
		UINT8 conidx = KE_IDX_GET(src_id);
	
		if(param->status == GAP_ERR_NO_ERROR)
		{
				printf("connect OK dest_id = %d, src_id = %d ...... \n", dest_id, src_id);

		    if(conidx == connection_devive1_env.conidx)
				{
							connection_devive1_env.src_id = src_id;
							app_device_set_duanluqi_1_connected_flag(1);
					    app_device1_scan_clear_global_flag(SERVER_DEVICE1_INDEX);
					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL1, CONNECT_OK);
					
				}
        else if(conidx == connection_devive2_env.conidx)
				{
							connection_devive2_env.src_id = src_id;
							app_device_set_duanluqi_2_connected_flag(1);
					    app_device1_scan_clear_global_flag(SERVER_DEVICE2_INDEX);
					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL2, CONNECT_OK);
					
				}
				else if(conidx == connection_devive3_env.conidx)
				{
							connection_devive3_env.src_id = src_id;
							app_device_set_duanluqi_3_connected_flag(1);
							app_device1_scan_clear_global_flag(SERVER_DEVICE3_INDEX);
					    //app_iris_attconnect_report_try_state(CONNECT_CHANNEL3, CONNECT_OK);
				}
				
			  //使能 server, 让断路器可以上报数据
			  app_iris_device_enable_report(conidx);

		}
		return (KE_MSG_CONSUMED);
	
}

LOCAL INT app_iris_device_rx_handler(ke_msg_id_t CONST msgid,
                                        struct uart_client_rx_data_ind  CONST *param,
                                        ke_task_id_t CONST dest_id,
                                        ke_task_id_t CONST src_id)
{
//收到断路器的发送过来的蓝牙数据
//2020/2/14
	UINT8 conidx = KE_IDX_GET(src_id);
	printf("app_iris_device_rx_handler %x %x %x %x",conidx,connection_devive1_env.conidx,connection_devive2_env.conidx,connection_devive3_env.conidx);
	#if (BLE_DATA_RECEIVE_MODE)
	if(app_get_ble_data_rece_mode()==1)
	{
		  realtek_client_rx_data_handle(conidx,param);
		  return (KE_MSG_CONSUMED);
	}
	#endif
	if(conidx == connection_devive1_env.conidx)
	{
			app_connetion_device1_rx_data(param);
	}
  else if(conidx == connection_devive2_env.conidx)
	{
			app_connetion_device2_rx_data(param);			
	}
	else if(conidx == connection_devive3_env.conidx)
	{
			app_connetion_device3_rx_data(param);			
	}
	
	return (KE_MSG_CONSUMED);
}
	#if (BLE_DATA_RECEIVE_MODE)
void realtek_client_handle_data(struct sch_realtek_rece_elt_tag * elt)
{
	struct uart_client_rx_data_ind param;
	param.length = elt->length;
	memcpy(param.value, elt->rx_data, param.length);
	if(elt->conidx == connection_devive1_env.conidx)
	{
			app_connetion_device1_rx_data(&param);
	}
  else if(elt->conidx == connection_devive2_env.conidx)
	{
			app_connetion_device2_rx_data(&param);			
	}
	else if(elt->conidx == connection_devive3_env.conidx)
	{
			app_connetion_device3_rx_data(&param);			
	}
	
}
	#endif
LOCAL INT app_iris_device_cmp_handler(ke_msg_id_t CONST msgid,
                                      struct uart_client_cmp_evt CONST *param,
                                      ke_task_id_t CONST dest_id,
                                      ke_task_id_t CONST src_id)
{
	  UINT8 conidx = KE_IDX_GET(src_id);
	
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
				}
		}			
	  
	
	  return (KE_MSG_CONSUMED);
}



/// Default State handlers definition
CONST struct ke_msg_handler app_iris_device_msg_handler_list[] =
{
		// Note: first message is latest message checked by kernel so default is put on top.
		{KE_MSG_DEFAULT_HANDLER,               (ke_msg_func_t)app_iris_device_msg_handler},
		
		{DEVICE1_CLIENT_ENABLE_RSP,            (ke_msg_func_t)app_iris_device_connection_handler},

		{UART_CLIENT_RX_DATA_IND,              (ke_msg_func_t)app_iris_device_rx_handler},  
		
		{UART_CLIENT_CMP_EVT,                  (ke_msg_func_t)app_iris_device_cmp_handler},  

};

CONST struct app_subtask_handlers app_iris_device_handlers = APP_HANDLERS(app_iris_device);
#endif
