//liujian 
//2018/6/12
//ver1.0


#include "app_rdw_voice.h"     // SW configuration
#include "app_pdm_record.h"
#include "app_i2s_record.h"

#if (BLE_APP_RDW_VOICE)


struct app_voice_env_tag   app_voice_env;

typedef void (*profile_callback) (void*, uint8_t);


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void app_voice_init(void)
{
    // Reset the environment
    memset(&app_voice_env, 0, sizeof(app_voice_env));
    // Key sample time
    app_voice_env.key_sample_time   = 6;
		app_voice_env.timer_enable = 0;
		app_voice_env.sound_begin = 0;
		app_voice_env.device_ready = 0;
	

	
	
}

//ֹͣ����ɨ��
//Ҳ����Ա��ͨ��Ӧ�ò㷢�Ͱ�����Ϣ���������,����ֻ�Ƿ��������������һ��ʱ������ʱ���
//����
void app_voice_stop_timer (void)
{
    // Stop the timer used for the detect key 
    if (app_voice_env.timer_enable)
    {
        ke_timer_clear(APP_VOICE_KEY_TIMER, TASK_APP);
        app_voice_env.timer_enable = false;
    }
//���������Ϊ�����豸�������˶Ͽ�֮������������ĵط��������ͷ��ڴ��		
		app_voice_env.device_ready = 0;
		app_voice_env.sound_begin = 0;

#if USE_WM8976_SAMPLE
//		rdw_i2s_rx_disable(1);    //�������԰�WM8976 �ص�ʡ�� ??????
			cobra_i2s_rx_disable(I2S0, 1);
#endif

#if USE_PDM_SAMPLE		
//		rdw_pdm_disable();
		app_pdm_record_uninit();
	
#endif
		
		
}

void app_voice_add_profile(void)
{
    struct voice_db_cfg* db_cfg;
  
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct voice_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_128);  //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_VOICE;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct voice_db_cfg* ) req->param;
    // All features are supported
    db_cfg->features = VOICE_ALL_FEAT_SUP;

    // Send the message
    ke_msg_send(req);
		
	
		

}

void app_voice_enable_prf(uint8_t conidx)
{
    // Allocate the message
	
	  struct gapc_conn_param conn_param;
	
    struct voice_enable_req * req = KE_MSG_ALLOC(VOICE_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_VOICE),
                                                TASK_APP,
                                                voice_enable_req);
    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;


    ke_msg_send(req);

#if USE_WM8976_SAMPLE
//��ʾֻ�����ӵ�����֮�󣬲ſ��Դ�WM8976
		app_i2s_record_init();		
#endif

#if USE_PDM_SAMPLE
//ʹ��PDM ������
		app_pdm_record_init_dma();
		
#endif
		app_voice_env.device_ready = 1;
		
		printf("app_voice_env.device_ready \n");
}


static int app_voice_enable_rsp_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	  struct gapc_conn_param conn_param;
	
	  conn_param.intv_min = 16;
    conn_param.intv_max = 16;
    conn_param.latency  = 0;
    conn_param.time_out = 1000;
	
    host_appm_update_param(&conn_param, TASK_APP);
	
//		host_appm_update_param(&conn_param, TASK_APP);
	
	  printf("app_voice_enable_rsp_handler Update para ... \n");
	  
    // Do nothing
    return (KE_MSG_CONSUMED);
}

void app_voice_update_param_again(void)
{
		 struct gapc_conn_param conn_param;
	
	   conn_param.intv_min = 64;
		 conn_param.intv_max = 64;
     conn_param.latency  = 0;
     conn_param.time_out = 1000;

     host_appm_update_param(&conn_param, TASK_APP);
	
	   printf("update again ..... \n");
	
}
	
	


//���Ժ���
static void rdw_voice_enable(int En)
{
		
		struct voice_start_req * req = KE_MSG_ALLOC(VOICE_AUDIO_START_REQ,
                                                    prf_get_task_from_id(TASK_ID_VOICE),
                                                    TASK_APP,
                                                    voice_start_req);
		if(En)
		{
				//rdw_void_init_i2s_dma();	
				req->start_state = 1;
		}
		else
		{
				req->start_state = 0;
		}
		
		ke_msg_send(req);
}




static int app_voice_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}



static int app_key_detect_handler(ke_msg_id_t const msgid,
                                          void const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
		
		if(app_voice_env.sound_begin == 0)
		{
		//��ʼ�ɼ���������
				app_voice_env.sound_begin = 1;
			
				app_adpcm_init();
			
#if USE_WM8976_SAMPLE
			//	rdw_i2s_rx_enable(1);
				cobra_i2s_rx_enable(I2S0, 1);
#endif

#if   USE_PDM_SAMPLE	
				app_pdm_record_init();
#endif
			
				printf("Please speak ........... \n");

		}
		else
		{
			//ֹͣ�ɼ���������
				app_voice_env.sound_begin = 0;
			
#if USE_WM8976_SAMPLE
	//			rdw_i2s_rx_disable(1);
			cobra_i2s_rx_disable(I2S0, 1);
#endif
			
#if   USE_PDM_SAMPLE	
			
			app_pdm_record_uninit();

#endif

		}
		
		rdw_voice_enable(app_voice_env.sound_begin);

    return (KE_MSG_CONSUMED);
}

static int app_enable_key_handler(ke_msg_id_t const msgid,
                                        struct voice_cfg_indntf_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{

    return (KE_MSG_CONSUMED);
}


static int app_audio_start_rsp_key_handler(ke_msg_id_t const msgid,
                                        struct voice_start_rsp const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
			 if (app_voice_env.timer_enable)
			 {					 
						ke_timer_set(APP_VOICE_KEY_TIMER, TASK_APP, app_voice_env.key_sample_time*100);
			 }			

			return (KE_MSG_CONSUMED);				 
}

void app_audio_record_begin()
{
	  if(app_voice_env.device_ready)
		{
			ke_timer_set(APP_VOICE_KEY_TIMER, TASK_APP, app_voice_env.key_sample_time);
		}
}




// ��Ҫ��ӵ�app_task.c �ļ���
// appm_msg_handler ������
/// Default State handlers definition
const struct ke_msg_handler app_voice_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_voice_msg_handler},
	
	{APP_VOICE_KEY_TIMER,        	(ke_msg_func_t)app_key_detect_handler},
	
	{VOICE_CFG_INDNTF_IND,			(ke_msg_func_t)app_enable_key_handler},
	
	{VOICE_AUDIO_START_RSP,     (ke_msg_func_t)app_audio_start_rsp_key_handler},
	
	{VOICE_ENABLE_RSP, 					(ke_msg_func_t)app_voice_enable_rsp_handler},
	
};

const struct app_subtask_handlers app_voice_handlers = APP_HANDLERS(app_voice);

#endif 


