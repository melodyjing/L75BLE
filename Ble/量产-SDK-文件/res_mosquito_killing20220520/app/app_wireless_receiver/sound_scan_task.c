/******************************************************************************
	File Name	:	sound_scan_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/06

	Description :   

	Changed Log	:
		Liujian  2020/03/06		- Creation

*****************************************************************************/

#include "sound_scan_task.h"
#include "voice_device.h"

#if BLE_APP_RECEIVE_VOICE

#define  BLE_AUDIO_SERV_UUID               0xFFF0

extern struct sound_scan_device_env_tag  sound_scan_env;

static int app_sound_scan_gapm_activity_created_ind_handler(ke_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
//		printf("app_sound_scan_gapm_activity_created_ind_handler idx = %d \n", p_param->actv_idx);
		if (sound_scan_env.scan_state == APP_SOUND_SCAN_STATE_CREATING)
    {
        // Store the advertising activity index
        sound_scan_env.act_id = p_param->actv_idx;
				
    }

    return (KE_MSG_CONSUMED);
	
}


static int app_sound_scan_gapm_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
//			printf("app_sound device operation = %d \n", param->operation);
	   
		  switch(param->operation)
			{
					case (GAPM_RESET):
					{
            if(param->status == GAP_ERR_NO_ERROR)
            {
								app_sound_scan_set_device_config();								
						}
					}
					break;
					case GAPM_SET_DEV_CONFIG:
					{
							app_sound_scan_create_activity();
									
					}
					break;
					
					case GAPM_CREATE_SCAN_ACTIVITY:   //已经建立了一个扫描
					{
								app_sound_scan_start_scan();					
					}
					break;

					default:
						break;
			}
			
			return (KE_MSG_CONSUMED);
}

int app_sound_scan_gapm_ext_adv_report_ind_handler(ke_msg_id_t const msg_id, struct gapm_ext_adv_report_ind* p_param,
                                    ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	  
		unsigned int i;
	
//		printf("123333333333333333333333\n");
//	   for(i = 0; i < p_param->length; i++)
//	   {
//			   printf("0x%02x ", p_param->data[i]);
//		 }
//		 printf("\n------------------------------------------------------------\n");
		if (p_param->info == (GAPM_REPORT_TYPE_ADV_LEG | GAPM_REPORT_INFO_SCAN_ADV_BIT| GAPM_REPORT_INFO_COMPLETE_BIT | GAPM_REPORT_INFO_CONN_ADV_BIT ))
    {
			if(app_sound_scan_find_svc_uuid(BLE_AUDIO_SERV_UUID, p_param->data, p_param->length))
			{
//						printf("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", 
//														p_param->trans_addr.addr.addr[0], p_param->trans_addr.addr.addr[1],
//														p_param->trans_addr.addr.addr[2], p_param->trans_addr.addr.addr[3],
//														p_param->trans_addr.addr.addr[4], p_param->trans_addr.addr.addr[5]);
				
					app_voice_device_connection_start(p_param->trans_addr);
			}
    }

    return (KE_MSG_CONSUMED);
}



/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(app_sound_scan)
{

      {GAPM_ACTIVITY_CREATED_IND, (ke_msg_func_t)app_sound_scan_gapm_activity_created_ind_handler},
						
      {GAPM_CMP_EVT,              (ke_msg_func_t)app_sound_scan_gapm_cmp_evt_handler},
			
			{GAPM_EXT_ADV_REPORT_IND,   (ke_msg_func_t)app_sound_scan_gapm_ext_adv_report_ind_handler   },

			   
};




ke_state_t app_sound_scan_state[1];

// Application task descriptor
const struct ke_task_desc TASK_DESC_SOUND_RECEIVER = {app_sound_scan_msg_handler_tab, app_sound_scan_state, 1, ARRAY_LEN(app_sound_scan_msg_handler_tab)};

#endif

