/******************************************************************************
	File Name	  :	  app_ota_scan_task.c
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/26

	Description :   

	Changed Log	:
		Liujian  2020/08/26	   - Creation

*****************************************************************************/

#include "app_ota_scan_task.h"
#include "app_ota_scan.h"
#include "ota_device.h"

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))

#define  BLE_OTA_SERV_UUID               0xFEF5

extern struct ota_scan_device_env_tag  ota_scan_env;

static int app_ota_scan_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
//		printf("app_ota_scan_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (ota_scan_env.scan_state == APP_OTA_SCAN_STATE_CREATING)
    {
        // Store the advertising activity index
        ota_scan_env.act_id = p_param->actv_idx;
				
    }

    return (KE_MSG_CONSUMED);
	
}


static int app_ota_scan_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
			//printf("app_ota scan  task operation = %d \n", param->operation);
	   
		  switch(param->operation)
			{
					case (GAPM_RESET):
					{
            if(param->status == GAP_ERR_NO_ERROR)
            {
								app_ota_scan_set_device_config();								
						}
					}
					break;
					case GAPM_SET_DEV_CONFIG:
					{
							app_ota_scan_create_activity();
									
					}
					break;
					
					case GAPM_CREATE_SCAN_ACTIVITY:   //已经建立了一个扫描
					{
								app_ota_scan_start_scan();					
					}
					break;

					default:
						break;
			}
			
			return (KE_MSG_CONSUMED);
}

int app_ota_scan_gapm_ext_adv_report_ind_handler(ke_msg_id_t const msg_id, struct gapm_ext_adv_report_ind* p_param,
                                    ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	  
		unsigned int i;
	
		if ((p_param->info == (GAPM_REPORT_TYPE_ADV_LEG | GAPM_REPORT_INFO_SCAN_ADV_BIT| GAPM_REPORT_INFO_COMPLETE_BIT | GAPM_REPORT_INFO_CONN_ADV_BIT )))
    {
        if((p_param->trans_addr.addr.addr[5] == 0xC0) && (p_param->trans_addr.addr.addr[4] == 0x55))
	//		if(app_ota_scan_find_svc_uuid(BLE_OTA_SERV_UUID, p_param->data, p_param->length))
			{
				     app_ota_process_avd_data(p_param);
				     if(app_ota_device_comp_addr(p_param->trans_addr))
						 {
					       app_ota_device_connection_start(p_param->trans_addr);
						 }
			}
    }

    return (KE_MSG_CONSUMED);
}


/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_ota_scan)
{

      {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_ota_scan_gapm_activity_created_ind_handler},
						
      {GAPM_CMP_EVT,              (ke_msg_func_t)app_ota_scan_gapm_cmp_evt_handler},
			
			{GAPM_EXT_ADV_REPORT_IND,   (ke_msg_func_t)app_ota_scan_gapm_ext_adv_report_ind_handler   },

			   
};




ke_state_t app_ota_scan_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_OTA_SCAN = {app_ota_scan_msg_handler_tab, app_ota_scan_state, 1, ARRAY_LEN(app_ota_scan_msg_handler_tab)};

#endif
