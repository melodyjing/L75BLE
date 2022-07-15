/******************************************************************************
	File Name	:	  master_scan_task.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/08

	Description : 
      蓝牙模组, 可以通过 AT COMMAND 来控制


	Changed Log	:
	
		Liujian  2021/03/08	 - Creation

*****************************************************************************/
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan_task.h"
#include "master_scan.h"
#include "module_report_adv.h"
#include "slave_device1_connection.h"
#include <ke_task.h>
#include <gapm_task.h>
#include <co_utils.h>
#include <string.h>
#include <stdio.h>
#if (COBRA_LOGGER_ENABLE == 1)	
#include <elog.h>
#endif

extern struct scan_device_env_tag  master_scan_device_env;

static int master_scan_device_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
		
		if (master_scan_device_env.scan_state == APP_DEVICE_SCAN_STATE_CREATING)
    {
        // Store the advertising activity index
        master_scan_device_env.act_id = p_param->actv_idx;
    }

    return (KE_MSG_CONSUMED);
	
}


static int master_scan_device_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
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
								master_scan_set_device_config();							
						}
					}
					break;
					case GAPM_SET_DEV_CONFIG:
					{
							master_scan_scan_create_activity();		
					}
					break;
					
					case GAPM_CREATE_SCAN_ACTIVITY:   //已经建立了一个扫描
					{
								master_scan_start_scan();					
					}
					break;
					default:
						break;
			}
			
			return (KE_MSG_CONSUMED);
}

int master_scan_device_gapm_ext_adv_report_ind_handler(ke_msg_id_t const msg_id, struct gapm_ext_adv_report_ind* p_param,
                                    ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	  
		unsigned int i;
		unsigned int temp_pin_code;

	  if(master_scan_get_state())
		{
		    app_adv_report_process(p_param);
		}
		
		if (p_param->info == (GAPM_REPORT_TYPE_ADV_LEG | GAPM_REPORT_INFO_SCAN_ADV_BIT| GAPM_REPORT_INFO_COMPLETE_BIT | GAPM_REPORT_INFO_CONN_ADV_BIT ))
    {

		
		//这个是为了保证每次在连接对方断路器的时候, 保证不会多个连接, 要一个连接后才开始
		//下一个连接
		if(app_device1_scan_get_global_flag()) return (KE_MSG_CONSUMED);
		
#if 1	
        if(!app_device_continue_scan_duanluqi_address1()&&app_slave_device1_task_is_ready())
				{		
						if(app_device_connect_to_duanluqi_1())
						{
								if(app_device_compare_duanluqi1_address((p_param->trans_addr.addr.addr)))
								{
										app_device_enable_scan_duanluqi1_address(1);	
#if (COBRA_LOGGER_ENABLE == 1)									
										log_i("+GATTSTA=0,2");
#endif									
									
										app_device1_scan_set_global_flag(SERVER_DEVICE1_INDEX);
										app_slave_device1_connection_start(p_param->trans_addr);
									
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
KE_MSG_HANDLER_TAB(master_scan_device)
{

      {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)master_scan_device_gapm_activity_created_ind_handler},
						
      {GAPM_CMP_EVT,              (ke_msg_func_t)master_scan_device_gapm_cmp_evt_handler},
			
			{GAPM_EXT_ADV_REPORT_IND,   (ke_msg_func_t)master_scan_device_gapm_ext_adv_report_ind_handler   },

			   
};


ke_state_t master_scan_device_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_MASTER_SCAN = {master_scan_device_msg_handler_tab, master_scan_device_state, 1, ARRAY_LEN(master_scan_device_msg_handler_tab)};


#endif

