/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_tuya_mesh_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/5/27

	Description :   tuya mesh task


	Changed Log	:

		Liujian   2021/5/27			- Creation

*****************************************************************************/

#include "rwip_config.h"  

#include "app_tuya_mesh_task.h"
#include "m_api.h"
#include "m_tb.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "app_button.h"
#include "cobra4A_spwm.h"
#include "alibaba_vendor.h"
#include "cobra4A_linked_softtimer.h"
#include "app_mesh_util.h"

#if ((BLE_APP_MESH == 1) && (BLE_TUYA_MESH_SDK_ENABLE == 1))

#include "tuya_node_init.h"


//nordic  used 
//#define RDW_AUTH_DATA_LEN     16


//#define CLIENT_NODE_UUID {0xA8, 0x01, 0x71, 0x0C, 0x2F, 0x00, 0x11, 0x10, 0x70, 0x61, 0x79, 0x65, 0x6C, 0x73, 0x76, 0x67}


//uint8_t  rdw_uuid[MESH_DEV_UUID_LEN] = CLIENT_NODE_UUID;

//uint8_t mIAuthValue[32] = {0};

uint8_t led_flag = 0;

static COBRA_SOFTWARE_TIMER * prov_timer_id;
static COBRA_SOFTWARE_TIMER * mesh_save_timer_id;
static COBRA_SOFTWARE_TIMER * mesh_power_on_id;

static model_idx_element  light_model;
//static model_idx_element  alibaba_model;
static uint8_t            alibaba_tid;
//static int32_t prov_timer_index;

//自动绑定 APP key
void app_mesh_task_bind_app_key(uint16_t app_key_lid);

//static unsigned int app_mesh_save_timer_index;   //定时保存配置信息
static void  app_mesh_save_information(COBRA_SOFTWARE_TIMER * temp);
static void app_mesh_led_all_light_on_off(unsigned int light_flag);
//按键清除邦定信息
void app_mesh_sw9_key_down_press(void * param)
{
	  printf("erase nvds ....\n");
    cobra_system_nvds_reset();
}


/*
初始化 LED IO 口

*/
static void app_mesh_gpio_init(void)
{

#if COBRA_SPWM_EN	
	cobra_map_pinmodeset(PIN28_INDEX, PIN28_SPWM_OUT_N);
	cobra_map_pinmodeset(PIN27_INDEX, PIN27_SPWM_OUT);
	
	cobra_spwm_pwm_b_enable(1);
	cobra_spwm_pwm_a_enable(1);
	
	cobra_spwm_pola_a_set(PASSIVE_HIGH_ACTIVE_LOW);
	cobra_spwm_pola_b_set(PASSIVE_LOW_ACTIVE_HIGH);
	
	app_cobra_spwm_set(1000, 2);
	
	cobra_spwm_pwm_en(1);
#else
    app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_OUTPUT);
    app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_OUTPUT);
#endif

	app_mesh_led_all_light_on_off(0);
	led_flag = 0;

}

static void app_mesh_led_all_light_on_off(unsigned int light_flag)
{
		if(light_flag)
		{
#if (COBRA_SPWM_EN)
			app_cobra_spwm_set(1000, 99);
#else			
			app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_LOW);
			app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_LOW);
#endif	
			
		}
		else
		{
#if (COBRA_SPWM_EN)
			app_cobra_spwm_set(1000, 0);
#else	
			app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_HIGH);
			app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_HIGH);
#endif
		}
	
}

void app_mesh_public_vendor_status_message(m_lid_t model_lid, uint16_t  attr_name, void * p_attr_parameter)
{
	    uint16_t param_size = 0;
	 
	    if(attr_name == 0xF01f)   
		{
			  param_size = 4;
		}
		else if(attr_name == 0xF01E)
		{
			 param_size = 1;
		}
		else
		{
			return;
		}
		
	   struct m_api_model_publish_cmd * req = KE_MSG_ALLOC_DYN(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_model_publish_cmd, (3 + param_size));
		req->cmd_code = M_API_MODEL_PUBLISH;
		req->model_lid = model_lid;
		req->tx_hdl = 0;
		req->opcode = 0x01A8D3;
	    
	    req->len = (3 + param_size);
		req->msg[ALIBABA_SET_TID_POS] = alibaba_tid;
		req->msg[ALIBABA_SET_ATTR_NAME_POS] = attr_name & 0xff;
		req->msg[ALIBABA_SET_ATTR_NAME_POS + 1] = (attr_name >> 8) & 0xff;
	   
        if(attr_name == 0xF01F)   
		{
              req->msg[ALIBABA_SET_PARAMETER_POS]     =   *((uint8_t *)p_attr_parameter);
              req->msg[ALIBABA_SET_PARAMETER_POS + 1] =   *((uint8_t *)p_attr_parameter + 1);
              req->msg[ALIBABA_SET_PARAMETER_POS + 2] =   *((uint8_t *)p_attr_parameter + 2);
              req->msg[ALIBABA_SET_PARAMETER_POS + 3] =   *((uint8_t *)p_attr_parameter + 3);			
		}
		else if(attr_name == 0xF01E)
		{
			  req->msg[ALIBABA_SET_PARAMETER_POS]     =   *((uint8_t *)p_attr_parameter);
		}
		
		alibaba_tid++;
	    ke_msg_send(req);
}



/*
如果是没有加入网络, LED 闪烁

*/

void app_mesh_led_callback(COBRA_SOFTWARE_TIMER * param)
{
	 if(led_flag)
	 {
			led_flag = 0;
	 }
	 else
	 {
			led_flag = 1;
	 }
	 
	 app_mesh_led_all_light_on_off(led_flag);
}

void app_mesh_init_timer(void)
{

	   mesh_save_timer_id = cobra_linked_softtimer_create(NULL, app_mesh_save_information, 10000);
	   cobra_linked_softtimer_start(mesh_save_timer_id);	
	
	   prov_timer_id = cobra_linked_softtimer_create(NULL, app_mesh_led_callback, 1000);
	   cobra_linked_softtimer_start(prov_timer_id);	
	
}

void app_mesh_power_on_timer_init(void)
{

//	   mesh_power_on_id = cobra_linked_softtimer_create(NULL, app_mesh_power_on_state, 7000);
//	   cobra_linked_softtimer_start(mesh_power_on_id);	
	
}

void app_mesh_uninit_timer(void)
{
      cobra_linked_softtimer_stop(prov_timer_id);
	   led_flag = 0;	
      app_mesh_led_all_light_on_off(0);		
}




void app_mesh_task_init(void)
{
	//  app_mesh_auth_init();
	  app_mesh_gpio_init();
	  alibaba_tid = 200;
	  tuya_mesh_para_init();
	
}


void app_mesh_save_information_event(void)
{	
	cobra_system_nvds_save_to_eflash();
}

static void  app_mesh_save_information(COBRA_SOFTWARE_TIMER * temp)
{
 
	  if(cobra_system_nvds_is_modified())
		{
			 printf("NVDS ........................\n");
			// cobra_app_set_mesh_nvds_task();    //
			 //cobra_system_nvds_save_to_eflash();
		}
	
}
void app_mesh_task_add_mesh(void)
{
    struct mesh_cfg * db_cfg;
    // Allocate the BASS_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct mesh_cfg));
    // Fill message
    req->operation   = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl     = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_MESH;
    req->app_task    = TASK_APP;
    req->start_hdl   = 0;

    // Set parameters
    db_cfg = (struct mesh_cfg* ) req->param;
		
		
		db_cfg->prf_cfg.features = M_FEAT_MSG_API_SUP|M_FEAT_PB_GATT_SUP|M_FEAT_PROXY_NODE_SUP|M_FEAT_RELAY_NODE_SUP|M_FEAT_DYN_BCN_INTV_SUP;

		db_cfg->prf_cfg.cid = 0x07D0;
		db_cfg->prf_cfg.pid = 0x0005;       //高 BYTE 定义了设备注册方式 
		db_cfg->prf_cfg.vid = 0x3130;
		db_cfg->prf_cfg.loc = 0x0100;
		db_cfg->prf_cfg.nb_addr_replay = 0x0000;
		db_cfg->prf_cfg.nb_cdata_page = 0x01;
		db_cfg->prf_cfg.frd_rx_window_ms = 0x00;
		db_cfg->prf_cfg.frd_queue_size=0x00;
		db_cfg->prf_cfg.frd_subs_list_size=0x00;
		db_cfg->model_cfg.nb_replay = 2;
   
    // Send the message
    ke_msg_send(req);
		
}

void app_tuya_mesh_add_module_ctl(uint8_t elmt_idx, bool main)
{
	 // Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;


    {
        do
        {
            // Model local index for Light CTL Server model and for Light CTL Tempereture Server model
            // and for Light Lightness Server model
            m_lid_t ln_mdl_lid, ctl_mdl_lid, ctlt_mdl_lid;


            // Register Generic Level Server model
            status = mm_api_register_server(elmt_idx + 1, MM_CFG_IDX_GENS_LEVEL, 0);

            if (status != MESH_ERR_NO_ERROR)
            {
                break;
            }

            // Register the Light CTL Server model and associated models
            status = mm_lights_ctl_register(elmt_idx, &ctl_mdl_lid, &ctlt_mdl_lid);

            if (main && (status == MESH_ERR_NO_ERROR))
            {
                // Group local index
                m_lid_t grp_lid;

                // Create group and set Light CTL Server model as main model
                mm_tb_bind_add_group(3, elmt_idx, &grp_lid, ctl_mdl_lid,
                                     mm_lights_ctl_cb_grp_event, mm_lights_ctl_cb_trans_req);

                // Add Generic Level Server model to the group
                mm_api_grp_add_local(elmt_idx, MM_ID_GENS_LVL, grp_lid);
                // Add Generic OnOff Server model to the group
                mm_api_grp_add_local(elmt_idx, MM_ID_GENS_OO, grp_lid);
                // Add Light Lightness Server model to the group
                mm_tb_bind_group_add_mdl(grp_lid, ln_mdl_lid, MM_ID_LIGHTS_LN,
                                         mm_lights_ln_cb_grp_event, mm_lights_ln_cb_set_state);

                // Create group and set Light CTL Temperature Server model as main model
                mm_tb_bind_add_group(1, elmt_idx + 1, &grp_lid, ctlt_mdl_lid,
                                     mm_lights_ctl_cb_grp_event_temp, mm_lights_ctl_cb_trans_req_temp);

                // Add Generic Level Server model to the group
                mm_api_grp_add_local(elmt_idx + 1, MM_ID_GENS_LVL, grp_lid);
            }
        } while (0);
    }

}


void app_mesh_set_on_off_state_main(void)
{
	   uint16_t i;
	   mm_tb_state_mdl_env_t *p_env;
	
	   for(i = 0; i < light_model.total_model; i++)
	   {
		     if(light_model.model_id[i] == MM_ID_GENS_OO)
				 {
					   printf("main  %d.............................................\n", i);
					   p_env = mm_tb_state_get_env(light_model.mdl_lid[i]);
			       SETB(p_env->info, MM_TB_STATE_MDL_INFO_MAIN, 1);
				 }
	   }
}

/*
在任务中 添加事件, 主要是用于开始 Mesh 的工作
*/
void app_mesh_task_begin(void)
{
	      light_model.elmt_idx = 0;
	      light_model.total_model = 0;
	
//	      mm_api_register_server(light_model.elmt_idx, MM_CFG_IDX_GENS_ONOFF, 1); 
	
	        mm_api_register_server(light_model.elmt_idx, MM_CFG_IDX_LIGHTS_CTL, 1); 
	
	       mm_api_register_server(2, MM_CFG_IDX_LIGHTS_HSL, 1); 
	
//	//     mm_api_register_server(elmt_idx, MM_CFG_IDX_GENS_POWER_ONOFF, 0);

//	 //      mm_api_register_server(0, MM_CFG_IDX_LIGHTS_CTL, 1); 
//   	     
//	       app_tuya_mesh_add_module_ctl(0, 1); 
	
	       app_mesh_task_load_info();	
}


void app_mesh_task_enable_profile(void)
{
    // Allocate the message
    struct m_api_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_cmd);
	req->cmd_code = M_API_ENABLE;
		
    // Send the message
    ke_msg_send(req);
				
}


void app_mesh_task_disbale_profile(void)
{
    // Allocate the message
    struct m_api_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_cmd);
	req->cmd_code = M_API_DISABLE;
		
    // Send the message
    ke_msg_send(req);	
}

void app_mesh_task_enable_proxy(uint32_t enable)
{
	  // Allocate the message
    struct m_api_proxy_ctl_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_proxy_ctl_cmd);
		req->enable = enable;  //M_LAY_PROXY_CTL_ADV_START_NODE;
		req->cmd_code = M_API_PROXY_CTL;
		
    // Send the message
    ke_msg_send(req);	
}

void app_mesh_task_load_info(void)
{		
		// Allocate the message
    struct m_api_storage_load_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_storage_load_cmd);
		req->cmd_code = M_API_STORAGE_LOAD;
		
    // Send the message
    ke_msg_send(req);	
}

//当对方连接的设备退出时
void app_mesh_task_disconnection(void)
{
//		app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_STOP);
//	  app_mesh_task_load_info();
}

void app_mesh_task_save_info(void)
{
		 // Allocate the message
    struct m_api_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_cmd);
	req->cmd_code = M_API_STORAGE_SAVE;
		

    ke_msg_send(req);
}



static int app_mesh_task_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{

	  int32_t i;

	  printf("msgid = %x \n", msgid);
	
	  switch(msgid)
	  {
		   case MESH_API_PROV_PARAM_REQ_IND:
		   {
			      uint8_t * p_device_id = tuya_get_device_uuid();
				 
				    if(p_device_id == NULL) break;
				 
					  struct m_api_prov_param_cfm * req = KE_MSG_ALLOC(MESH_API_PROV_PARAM_CFM, prf_get_task_from_id(TASK_ID_MESH),
																											TASK_APP,
																											m_api_prov_param_cfm);
				
					  for(i = 0; i < MESH_DEV_UUID_LEN; i++)
					  {
							  req->dev_uuid[i] = p_device_id[i];
					  }
						
						req->uri_hash = 0x00;
						req->oob_info = 0x00;
						req->pub_key_oob = 0x00;
						req->static_oob = 0x00;                
						req->out_oob_size = 0x00;
						req->in_oob_size = 0x00;
						req->out_oob_action = 0x00;
						req->in_oob_action = 0x00;
						req->info = 0x00;
					
					  // Send the message
					  ke_msg_send(req);
						
						break;
		   }
		 
		
		   case MESH_API_PROV_STATE_IND:     //prov 完成后,需要保存到NVDS 
			 {
						m_api_prov_state_ind_t *p_ind = (m_api_prov_state_ind_t *)param;
				    if(p_ind->state == M_PROV_SUCCEED)
						{
								app_mesh_uninit_timer();
						}
			 }
		   break;
			 
			 case MESH_API_PROXY_ADV_UPDATE_IND:
			 {
					m_api_proxy_adv_update_ind_t *p_ind = (m_api_proxy_adv_update_ind_t *)param;
			 }
			 break;
			 
			 case MESH_API_GROUP_UPDATE_IND:
			 {
					 m_api_group_update_ind_t *p_ind = (m_api_group_update_ind_t *)param;
				   if(p_ind->added)
					 {
								
						    printf("APP KEY %d %d \n", p_ind->appkey_id, p_ind->app_key_lid);
								app_mesh_task_bind_app_key(p_ind->app_key_lid);
					 }
			 }
			 break;
			 
			 
			 case MESH_MDL_API_IND:
			 {
				  mm_api_ind_t *p_ind = (mm_api_ind_t *)param;
				 
				  printf("API_IND = 0x%x \n", p_ind->ind_code);
				 
					if(p_ind->ind_code == MM_API_REGISTER_IND)
					{
						      mm_api_register_ind_t *p_ind = (mm_api_register_ind_t *)param;
	
							    light_model.mdl_lid[light_model.total_model] = p_ind->mdl_lid;
							    light_model.model_id[light_model.total_model] = p_ind->model_id;
							    light_model.total_model++;

						       printf("elmt_idx = %d  mdl_lid = %x  model_id = %x \n",  p_ind->elmt_idx, p_ind->mdl_lid, p_ind->model_id);						
					}
					
					else if(p_ind->ind_code == MM_API_SRV_STATE_UPD_IND)
					{
						  
							mm_api_srv_state_upd_ind_t *p_ind = (mm_api_srv_state_upd_ind_t *)param;
						
						  printf("state_id = %d\n", p_ind->state_id);
						
						  if(p_ind->state_id == MM_STATE_GEN_ONOFF)
							{
								 if(p_ind->state)
								 {
									//  printf("ON ............................ \n");
									  app_mesh_led_all_light_on_off(1);
								 }
								 else
								 {
									// printf("OFF ............................ \n");
									 app_mesh_led_all_light_on_off(0);
								 }
							}
							else if(p_ind->state_id == MM_STATE_LIGHT_LN)
							{
								  printf("p_ind->state = %d \n", p_ind->state);
#if (COBRA_SPWM_EN)								
								  app_cobra_spwm_set(1000, (p_ind->state)/655);
#endif								
							}
					}
				 
			 }
			 break;
			 
			 case MESH_API_NODE_RESET_IND:
			 {
				    //reset the node
				    cobra_system_nvds_reset();
				    platform_reset(0);
				 
			 }
			 break;
		 
		   case MESH_API_CMP_EVT:
		   {
				    struct m_api_cmp_evt * p = (struct m_api_cmp_evt *)param;
				    				 
					  switch(p->cmd_code)
					  {
						 case M_API_ENABLE:
						 {
								app_mesh_task_storage_config();	
								if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_PROV)
								{
									printf("M_TB_STATE_PROV_STATE_PROV \n");
									#if BLE_MESH_GATT_PROXY			
										app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
									#endif
									  app_mesh_uninit_timer();
									 // app_mesh_power_on_timer_init();   //发送上电信息给 天猫精灵
									
								}
						    else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_BEING_PROV)
								{
									  printf("M_TB_STATE_PROV_STATE_BEING_PROV \n");
										app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
								//	  app_mesh_init_timer();
								}
								else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_UNPROV)
								{
									printf("M_TB_STATE_PROV_STATE_UNPROV \n");
								}
						 }
					
						 break;
						 case M_API_STORAGE_LOAD:
						 {
								app_mesh_task_enable_profile();  //load info 之后, 开始使能 API CMD
						    break;
						 }
						 case M_API_DISABLE:
						 {
								app_mesh_task_enable_profile();
						    break;
						 }
						 case M_API_STORAGE_SAVE:
						 {						 
								cobra_system_nvds_save_to_eflash();							 
						 }
						 break;
	
						 default:
							 break;
					 }
					break;	
		 }
		 
		 default:
			  break;
	 }

    return (KE_MSG_CONSUMED);
}

//自动绑定 APP key
void app_mesh_task_bind_app_key(uint16_t app_key_lid)
{

     uint16_t status;
	
	   uint32_t i;

		 m_lid_t model_lid;
	
//     for(i = 0; i < light_model.total_model; i++)
//		 {
//			   model_lid = light_model.mdl_lid[i];
//			 
//				 status = m_tb_key_model_bind(app_key_lid, model_lid);

//				 if (status == MESH_ERR_NO_ERROR)
//				 {
//						// Inform the Model IO manager
//						m_tb_mio_bind(model_lid);
//						printf("OK .......\n");
//					 
//					  status = m_tb_mio_add_subscription(model_lid, 0xc000); 
//				    status = m_tb_mio_add_subscription(model_lid, 0xcfff);
//				 
//				    status = m_tb_mio_set_publi_param(model_lid, 0xf000, NULL, app_key_lid, 0, 10, 0, 0);
//				    printf("status = %d \n", status);
//				 }
//	   }
//		 
		 
//		 app_mesh_set_light_state();
//		 app_mesh_set_light_onoff_state();
//		 app_mesh_set_light_on_state();

      app_mesh_set_on_off_state_main();
}



/// Default State handlers definition
const struct ke_msg_handler app_mesh_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_mesh_task_msg_handler},


};

const struct app_subtask_handlers app_mesh_handlers = APP_HANDLERS(app_mesh);

#endif

