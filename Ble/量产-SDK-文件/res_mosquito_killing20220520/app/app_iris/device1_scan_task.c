/******************************************************************************
	File Name	:	device1_scan_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/21

	Description :   

	Changed Log	:
		Liujian  2019/11/21		- Creation

*****************************************************************************/
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"
#include "cobra4_common.h"

#include "device1_scan.h"
#include "device1_scan_task.h"
#include "app_autoconnect.h"
#include "app_report_adv.h"
#include <string.h>
#include <stdio.h>

#if((APP_SCAN_DEVICE_ENABLE  == 1) && (DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1))

extern struct scan_device_env_tag  scan_device1_env;



static int app_scan_device1_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
		//printf("app_scan_device1_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (scan_device1_env.scan_state == APP_DEVICE_SCAN_STATE_CREATING)
    {
        // Store the advertising activity index
        scan_device1_env.act_id = p_param->actv_idx;
    }

    return (KE_MSG_CONSUMED);
	
}


static int app_scan_device1_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
			//printf("scan device operation = %d \n", param->operation);
	   
		  switch(param->operation)
			{
					case (GAPM_RESET):
					{
            if(param->status == GAP_ERR_NO_ERROR)
            {
								app_device1_set_device_config();
							
								
						}
					}
					break;
					case GAPM_SET_DEV_CONFIG:
					{
						  //app_device1_create_watchdog_timer();
							app_device1_scan_create_activity();		
					}
					break;
					
					case GAPM_CREATE_SCAN_ACTIVITY:   //已经建立了一个扫描
					{
								app_device1_start_scan();					
					}
					break;
					default:
						break;
			}
			
			return (KE_MSG_CONSUMED);
}

int app_scan_device1_gapm_ext_adv_report_ind_handler(ke_msg_id_t const msg_id, struct gapm_ext_adv_report_ind* p_param,
                                    ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	  
		unsigned int i;
		unsigned int temp_pin_code;
#if (APP_METER_DEBUG_ENABLE)	
	   app_debug_led();
#endif
	
		app_adv_report_process(p_param);
	
		if (p_param->info == (GAPM_REPORT_TYPE_ADV_LEG | GAPM_REPORT_INFO_SCAN_ADV_BIT| GAPM_REPORT_INFO_COMPLETE_BIT | GAPM_REPORT_INFO_CONN_ADV_BIT ))
    {
#if DIANBIAO_AUTO_ENABLE 	
			//2020/2/29 liujian 添加自动连接功能
		if(app_iris_get_scan_mode())
		{
			app_iris_process_adv_data(p_param);
		}
		
		if(app_iris_autoconnect_get_report_flag())
		{
			app_iris_recode_adv_data(p_param);
		}
#endif
		
		//这个是为了保证每次在连接对方断路器的时候, 保证不会多个连接, 要一个连接后才开始
		//下一个连接
		if(app_device1_scan_get_global_flag()) return (KE_MSG_CONSUMED);
		
#if BLE_DEVICE1_UART_CLIENT_ENABLE					
				if(!app_device_continue_scan_duanluqi_address1()&&app_device1_task_is_ready())   //如果断路器没有被连接, 就要察看是否需要连接
				{
						if(app_device_connect_to_duanluqi_1())
						{
							if(app_device_compare_duanluqi1_address((p_param->trans_addr.addr.addr)))
							{
									app_device_enable_scan_duanluqi1_address(1);	
								
//									if(app_device1_find_adv_pin_code(p_param, &temp_pin_code))
//									{
//										app_device1_set_pincode(temp_pin_code);
//										app_device1_set_pair_flag(1);
//									}
//									else
//									{
//										 app_device1_set_pair_flag(0);
//									}
								
									printf("\n begin connection 断路器 1\n");
								
									app_device1_scan_set_global_flag(SERVER_DEVICE1_INDEX);
									app_device1_connection_start(p_param->trans_addr);
								
									return (KE_MSG_CONSUMED);
							}
						}
				}
#endif

#if BLE_DEVICE2_UART_CLIENT_ENABLE					
				if(!app_device_continue_scan_duanluqi_address2()&&app_device2_task_is_ready())
				{		
						if(app_device_connect_to_duanluqi_2())
						{
								if(app_device_compare_duanluqi2_address((p_param->trans_addr.addr.addr)))
								{
										app_device_enable_scan_duanluqi2_address(1);	
									
										printf("\n begin connection 断路器 2\n");
									
										app_device1_scan_set_global_flag(SERVER_DEVICE2_INDEX);
										app_device2_connection_start(p_param->trans_addr);
									
										return (KE_MSG_CONSUMED);
								}
						}
				}
#endif

#if BLE_DEVICE3_UART_CLIENT_ENABLE					
				if(!app_device_continue_scan_duanluqi_address3()&&app_device3_task_is_ready())
				{		
						if(app_device_connect_to_duanluqi_3())
						{
								if(app_device_compare_duanluqi3_address((p_param->trans_addr.addr.addr)))
								{
										app_device_enable_scan_duanluqi3_address(1);	
									
										printf("\n begin connection 断路器 3\n");
									
									  app_device1_scan_set_global_flag(SERVER_DEVICE3_INDEX);
										app_device3_connection_start(p_param->trans_addr);
									
										return (KE_MSG_CONSUMED);
								}
						}
				}
#endif
				
        	 
    }

    return (KE_MSG_CONSUMED);
}



/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_scan_dev1)
{

      {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_scan_device1_gapm_activity_created_ind_handler},
						
      {GAPM_CMP_EVT,              (ke_msg_func_t)app_scan_device1_gapm_cmp_evt_handler},
			
			{GAPM_EXT_ADV_REPORT_IND,   (ke_msg_func_t)app_scan_device1_gapm_ext_adv_report_ind_handler   },

			   
};




ke_state_t app_scan_dev1_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_SCAN_DEV1 = {app_scan_dev1_msg_handler_tab, app_scan_dev1_state, 1, ARRAY_LEN(app_scan_dev1_msg_handler_tab)};




#endif
