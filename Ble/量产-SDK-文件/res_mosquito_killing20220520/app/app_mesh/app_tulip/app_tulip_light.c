/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Application for Tulips

	File Name	:	app_tulip_light.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/2/19
	
*******************************************************************************/

#include "app_tulip_light.h"
#include "rwip_config.h"  
#include "m_api.h"
#include "m_tb.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "mm_genc.h"
#include "app_mesh_tscns.h"
#include "app_mesh_util.h"
#include "tulip_vendor_module.h"
#include "cobra_light.h"
#include "app_tulip_comm.h"
#include "mm_gens_int.h"

#if(COBRA_SCENE_VERSION2_ENABLE == 1)
#include "app_tulip_scene.h"
#endif

#if APP_FLASH_LIGHT_NUMBERS	
#include "app_light_breath.h"
#endif

#if(APP_TULIP_BATTER_DETECT_VERSION2 == 1)
#include "app_tulip_battery.h" 
#endif
 
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))

#if (COBRA_POWER_DOWN_SAVE_ENABLE==1)
void save_data_before_ota(void);
#endif


static uint32_t flash_is_dirty ;
static uint32_t device_state;    //prov or node
typedef struct {
	   uint8_t   elmt_idx;       //注册时用的 element index
	   m_lid_t   mdl_lid[28];    //最多一个element 下有 10 个 model
	   uint32_t  model_id[28];
	   uint8_t   total_model;
}model_idx_element;

static model_idx_element  light_model;

static uint32_t  led_light_count;     //没有入网前， 蓝灯闪三次
#if APP_FLASH_LIGHT_NUMBERS
bool flash_light_enable;
#endif
static uint32_t  prov_timer_id;
static uint32_t  mesh_save_timer_id;
static uint32_t  schdule_time_id;
static uint8_t   light_app_keyid;
static uint8_t   light_node_reboot_flag;

static uint8_t led_flag = 0;
static m_lid_t  light_model_lid;
static uint8_t  light_elmt_idx;

void app_mesh_set_g_tai_seconds(uint32_t seconds);
static void app_mesh_led_callback(uint32_t param);

void app_mesh_task_enable_proxy(uint32_t enable);

static uint32_t  mesh_save_timer_delay_id;

#if APP_FLASH_LIGHT_NUMBERS	
static uint8_t  light_flash_number=0;
uint8_t get_light_flash_number(void)
{
	return light_flash_number;
}
void set_flash_light_number(uint8_t number)
{
	light_flash_number = number;
}

uint8_t get_led_light_count(void)
{
	return led_light_count;
}
#endif

static void  app_mesh_save_delay_information(uint32_t param)
{
	  printf("save delay .... \n");
	  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
	  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
}

static void  app_mesh_save_information(uint32_t param)
{
	  if(cobra_system_nvds_is_modified())
		{
			// app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_STOP);
			 cobra_app_set_mesh_nvds_task();
			// cobra_soft_timer_start(mesh_save_timer_delay_id, COBRA_SOFT_TIMER_TICK*2, 0);
		}	
}

void app_mesh_save_init_time(unsigned int period, unsigned int repeat)
{
		  mesh_save_timer_id = cobra_soft_timer_create(app_mesh_save_information);
	    cobra_soft_timer_start(mesh_save_timer_id, period, repeat);
      mesh_save_timer_delay_id = cobra_soft_timer_create(app_mesh_save_delay_information);
}

void app_mesh_save_information_event(void)
{	
		
	  printf("save begin .... \n");
	
	  if(device_state == 0)
		{
	     cobra_system_nvds_save_to_eflash();
			 device_state = 1;
		}
	  flash_is_dirty = 1;

}

void app_mesh_prov_init_timer(void)
{
	   prov_timer_id = cobra_soft_timer_create(app_mesh_led_callback);
#if (APP_FLASH_LIGHT_NUMBERS==0)	
	   cobra_soft_timer_start(prov_timer_id, COBRA_SOFT_TIMER_TICK, 1);
	   led_light_count = 0;
#endif	
}
#if (APP_FLASH_LIGHT_NUMBERS)
void app_mesh_prov_start_timer(void)
{
	printf("flash_light_enable=%x",flash_light_enable);
	   if(flash_light_enable == false)
		 {
	   cobra_soft_timer_start(prov_timer_id, COBRA_SOFT_TIMER_TICK/2, 1);
	   led_light_count = 0;
#if APP_FLASH_LIGHT_NUMBERS
     flash_light_enable = true;
#endif	
		 }
}
#endif

void app_mesh_prov_uninit_timer(void)
{
		cobra_soft_timer_stop(prov_timer_id);
	  led_flag = 0;		
}
#if APP_FLASH_LIGHT_NUMBERS
void app_mesh_led_first_show(void)
{
//			app_cobra_light_set_blue_duty(0);
//			app_cobra_light_set_green_duty(0);
//			app_cobra_light_set_red_duty(255);
			app_cobra_light_set_lum(15);
			app_ctl_set_warm_duty(0);	
}
#endif
/*
如果是没有加入网络, LED 闪烁
*/
void app_mesh_led_callback(uint32_t param)
{
	 if(led_flag)
	 {
			led_flag = 0;
	 }
	 else
	 {
			led_flag = 1;
	 }	
	 
   led_light_count++;
	 
#if APP_FLASH_LIGHT_NUMBERS	 
	 if(led_light_count < 2 * get_light_flash_number())
#else	 
   if(led_light_count < 6)
#endif	 
	 {		 
	    app_mesh_led_all_light_on_off(led_flag);
	 }
	 else
	 {
		 printf("led_light_count=%x\r\n",led_light_count);
//		  app_mesh_led_init_state();
 //     app_cobra_light_set_cool_duty(50);
		 #if APP_FLASH_LIGHT_NUMBERS
    // app_mesh_led_first_show();
		 app_mesh_prov_uninit_timer();
		 flash_light_enable = false;
		 app_light_tulipi_onff(1);	 

     
	 
		 #endif
	 }
}


void app_mesh_task_init(void)
{

//	app_mesh_gpio_init();
	  
	  light_model_lid = -1;   //灯的模型还没有加上去
	
	  light_elmt_idx = 0;     //灯放在第一个位置, 最多 MESH_MODEL_NB_MAX
	
	  light_node_reboot_flag = 0;
	
//	light_on_off_state = 0;
	
//  light_pwm_init();
	
	  flash_is_dirty = 0;
}

/*
在任务中 添加事件, 主要是用于开始 Mesh 的工作
*/
void app_mesh_task_begin(void)
{

	      light_model.elmt_idx = 0;
	      light_model.total_model = 0;
	      m_lid_t ln_mdl_lid, ctl_mdl_lid, ctlt_mdl_lid;


        mm_api_register_server(0, MM_CFG_IDX_LIGHTS_HSL, 1);   //这个是调色灯
	      mm_api_register_server(0, MM_CFG_IDX_TSCNS_TIME, 1);
	      mm_api_register_server(0, MM_CFG_IDX_TSCNS_SCENCE, 1);
	      mm_api_register_server(0, MM_CFG_IDX_TSCNS_SCHDULE, 1);
	      mm_api_register_server(0, MM_CFG_IDX_GENS_DFT_TRANS_TIME, 1);
	
	    //  mm_api_register_server(3, MM_CFG_IDX_LIGHTS_CTL, 1);
	
	      app_tulip_add_module_ctl(2, 1);
	
	      m_tulipi_vendor_register_server(0, 1);
	        
			  app_mesh_task_load_info();	
	    

        app_cobra_light_init();
		    app_cobra_light_enable();
		    app_mesh_led_init_state();
		
	      app_scene_set_change_light_init();
	 			#if APP_LIGHT_BREATH_FUNCTION
				app_scene_set_breath_light_init();
				app_light_ctrl_breathing_timer_init();
				#endif				
        led_resume_light_handle();
				
//#if(COBRA_SCENE_VERSION2_ENABLE == 1)
        //app_tulipe_scene_color_init();
//#endif
				
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

static void app_mesh_set_onoff_publish(uint16_t app_key_lid)
{
	   uint32_t i;
	
		 for(i = 0; i < light_model.total_model; i++)
	   {
		    if(light_model.model_id[i] == MM_ID_GENS_OO)  
				  break;
	   }
		 
		 m_tb_mio_set_publi_param(light_model.mdl_lid[i], 0x0001, 0, app_key_lid,  10,  2,  0, 0);
	
}

//自动绑定 APP key
void app_mesh_task_bind_app_key(uint16_t app_key_lid)
{

     uint16_t status;
	
	   uint32_t i;

		 m_lid_t model_lid;
	

	   uint32_t  flag = 0;
	
     for(i = 0; i < light_model.total_model; i++)
		 {
			      model_lid = light_model.mdl_lid[i];
			     
			      if(flag && (light_model.model_id[i] == MM_ID_GENS_OO)) continue;

//使用公版  APP 一直挥发送 配网信息			 
#if 0			 
     			  status = m_tb_key_model_bind(app_key_lid, model_lid);
			      
			      if(status == MESH_ERR_NO_ERROR)
            {
                // Inform the Model IO manager
                m_tb_mio_bind(model_lid);
            }
#endif
					 // status = m_tb_mio_add_subscription(model_lid, 0xc000); 
				    status = m_tb_mio_add_subscription(model_lid, 0xffff);
			      
			      if(light_model.model_id[i] == MM_ID_GENS_OO)
				    {
					      flag = 1;
				    }

	   }

		 
		 m_tb_state_set_relay_state(1, 1);
		
     app_mesh_set_on_off_state_main();
		 
 //    app_mesh_set_onoff_publish(app_key_lid);
		 
}

void app_mesh_task_enable_profile(void)
{
    // Allocate the message
    struct m_api_cmd * req = KE_MSG_ALLOC(MESH_API_CMD, prf_get_task_from_id(TASK_ID_MESH), TASK_APP, m_api_cmd);
		
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
		req->enable = enable;  
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

void app_tulipx_save_param(void)
{
	   if(flash_is_dirty)
		 {
			   flash_is_dirty = 0;
			   cobra_system_nvds_save_to_eflash();
		 }
}

//当对方连接的设备退出时
void app_mesh_task_disconnection(void)
{
	    printf("app tulipx mesh disconnection .... \n");
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

		db_cfg->prf_cfg.cid = MESH_AILIBABA_ID;

		db_cfg->prf_cfg.pid = 0x0005;
		db_cfg->prf_cfg.vid = 0x0002;
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
#if APP_LIGHT_BREATH_FUNCTION
bool same_command_scene_edit(scene_edit_cmd *p_cmd)
{
	static scene_edit_cmd pre_cmd;
	uint8_t *p=&(p_cmd->data[0].color_type);
	uint8_t *q=&(pre_cmd.data[0].color_type);
	if(p_cmd->len == pre_cmd.len && p_cmd->scene_index == pre_cmd.scene_index && p_cmd->mode_flag == pre_cmd.mode_flag)
	{
		uint16_t i;
		for(i=0;i<pre_cmd.len;i++)
		{
			if(*p != *q)
			{
					pre_cmd = *p_cmd;
					return false;
			}
			p++;
      q++;			
		}
		if(i >= pre_cmd.len)
			return true;
	}
	else
	{
		pre_cmd = *p_cmd;
		return false;
	}
}
#endif

static int app_mesh_task_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{

	  int32_t i;
      uint8_t   temp;
	  unsigned int onoff;
	  switch(msgid)
	  {

		   case MESH_API_PROV_PARAM_REQ_IND:
		   {
			     
					  struct m_api_prov_param_cfm * req = KE_MSG_ALLOC(MESH_API_PROV_PARAM_CFM,
																											prf_get_task_from_id(TASK_ID_MESH),
																											TASK_APP,
																											m_api_prov_param_cfm);
				
						unsigned char *p_uuid = cobra_nvds_get_prov_uuid();
					  for(i = 0; i < MESH_DEV_UUID_LEN; i++)
					  {
							    req->dev_uuid[i] = p_uuid[i];
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
								app_mesh_prov_uninit_timer();
						}
						 #if APP_FLASH_LIGHT_NUMBERS
						    set_flash_light_number(4);
						    app_mesh_prov_start_timer();
						 #endif						
			 }
		   break;
			 
			 case MESH_API_PROXY_ADV_UPDATE_IND:
			 {
					m_api_proxy_adv_update_ind_t *p_ind = (m_api_proxy_adv_update_ind_t *)param;
       	  printf("PROXY_ADV state = %d, reason = %d \n", p_ind->state, p_ind->reason);
				  if(MESH_PROXY_ADV_NET_STOP == p_ind->state)
					{
						  if(MESH_PROXY_ADV_UPD_REASON_TIMEOUT == p_ind->reason)
							{
								  if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_PROV)
								  {									
									    app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
								  }
						      else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_BEING_PROV)
								  {
										 app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
								  }
							}
					} 
					else if(MESH_PROXY_ADV_NODE_STOP == p_ind->state)
					{
						  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
					}
			 }
			 break;
			 
			 case MESH_API_GROUP_UPDATE_IND:
			 {
				   uint8_t appkey_flag;
					 m_api_group_update_ind_t *p_ind = (m_api_group_update_ind_t *)param;
				   if(p_ind->added)
					 {
						    printf("APP KEY %d %d \n", p_ind->appkey_id, p_ind->app_key_lid);
						 
						    light_app_keyid = p_ind->app_key_lid;
						    app_mesh_task_bind_app_key(p_ind->app_key_lid);
						 
						    temp = MESH_APPKEY_OK_LENGTH;
						    if(nvds_get(MESH_APPKEY_OK_FLAG, (uint8_t *)&temp, &appkey_flag) != NVDS_OK)
								{
									  appkey_flag = 0;
								}
								
								if(appkey_flag == 0)
								{
									  printf("save nvds ...... \n");
									
									  appkey_flag = 1;
									  nvds_put(MESH_APPKEY_OK_FLAG, MESH_APPKEY_OK_LENGTH, &appkey_flag);
									#if (COBRA_FLASH_POWERON_CRASE == 0)
									  app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*40, 0);
									#endif
									  light_node_reboot_flag = 1;
								
									  if(0x88 != read_poweron_timeout())
								    cobra_app_set_mesh_erase_flag_task();	//  配网成功后，开关机计数从新开始。							
								}
								
                onoff = get_light_onoff();
								

                app_mesh_key_response_on_off_state(onoff);
								
					 }
			 }
			 break;
			 
			 
			 case MESH_MDL_API_IND:
			 {
					mm_api_ind_t *p_ind = (mm_api_ind_t *)param;
				 		 
				  if(p_ind->ind_code == MM_API_REGISTER_IND)
					{
						      mm_api_register_ind_t *p_ind = (mm_api_register_ind_t *)param;	
							    light_model.mdl_lid[light_model.total_model] = p_ind->mdl_lid;
							    light_model.model_id[light_model.total_model] = p_ind->model_id;
							    light_model.total_model++;
						
						      if(p_ind->model_id == MM_ID_GENS_OO)
						      {
							        light_model_lid = p_ind->mdl_lid;   //得到灯的 model_id
						      }
					}
					
					else if(p_ind->ind_code == MM_API_SRV_STATE_UPD_IND)
					{
						  
							mm_api_srv_state_upd_ind_t *p_ind = (mm_api_srv_state_upd_ind_t *)param;
						
						  if(p_ind->state_id == MM_STATE_GEN_ONOFF)
							{
								 printf("on off %x\n",p_ind->state);
								 app_light_tulipi_onff(p_ind->state);
							}

							else if(p_ind->state_id == MM_STATE_TSCNS_TAI_SECONDS)
							{

								  app_mesh_tscns_set_tai_seconds(p_ind->state);
								
								  app_mesh_tscns_set_seconds_callback(app_mesh_set_g_tai_seconds);
								
							}
							else if(p_ind->state_id == MM_STATE_TSCNS_ZONE_OFFSET)
							{
								 app_mesh_tscns_set_zone_offset(p_ind->state);
							}
              else if(p_ind->state_id == MM_STATE_TSCNS_ACTION_ON)
              {
								    printf("on ...\n");
								    app_tulip_time_schedule_action_on();
	
              }
              else if(p_ind->state_id == MM_STATE_TSCNS_ACTION_OFF)
              {
								    printf("off ...\n");
								    app_tulip_time_schedule_action_off();
              }
              else if( MM_STATE_LIGHT_HSL_LN == p_ind->state_id)   //亮度 Light HSL Lightness
							{
                   printf("Light  Lightness = %d \n",   p_ind->state);
                   if(p_ind->state != 0)
                   {
                   			app_light_tulipi_set_hsl_lightness(p_ind->state);
                        app_mesh_lightness_process();
                   }
                   					
							}	
							else if( MM_STATE_LIGHT_HSL_HUE == p_ind->state_id)   //Light HSL Hue
							{
                   printf("Light HSL Hue = %d \n",   p_ind->state);  
                   app_light_tulipi_set_hsl_hue(p_ind->state);	
								   app_tulip_set_screen_index(SCREEN_IN_COLOR_LIGHT);
                   app_mesh_led_control();								
							}				
							else if( MM_STATE_LIGHT_HSL_SAT == p_ind->state_id)   //Light HSL Saturation
							{
                   printf("Light HSL Saturation = %d \n",   p_ind->state);
                   app_light_tulipi_set_hsl_sat(p_ind->state);
								   app_tulip_set_screen_index(SCREEN_IN_COLOR_LIGHT);
                   app_mesh_led_control();								
							}				
							else if( MM_STATE_LIGHT_CTL_TEMP == p_ind->state_id)   //Light CTL Temperature
							{
								   app_tulip_set_screen_index(SCREEN_IN_WARM_COOL_LIGHT);	
								
								   if(!get_ctl_onoff()) //  开关状态对不上时，使两者（手机和目标板）一致
									 {
										 app_mesh_key_response_on_off_state(1);
									 }
                      
									 app_light_tulipi_set_ctl_temp(p_ind->state);	                     
                   app_cobra_light_control_light();										
							}	
									
              else if(MM_STATE_SCREEN_CTL == p_ind->state_id)
							{
                   printf("now is CTL screen \n");
								   app_tulip_set_screen_index(SCREEN_IN_WARM_COOL_LIGHT);
                   if(!get_ctl_onoff())  //  开关状态对不上时，使两者（手机和目标板）一致
									 {
										 app_mesh_key_response_on_off_state(1);
									 }
								   app_cobra_light_control_light();    //从场景返回
							}			
              else if(MM_STATE_SCREEN_HSL == p_ind->state_id)
							{
                   printf("now is HSL screen \n");
								   if(!get_hsl_onoff()) //  开关状态对不上时，使两者（手机和目标板）一致
								   {
										   app_mesh_key_response_on_off_state(1);
								   }
								   app_tulip_set_screen_index(SCREEN_IN_COLOR_LIGHT);
								   app_mesh_led_control();	    //从场景返回
							}	
              else if(MM_STATE_TSCNS_SCENE_RECALL == p_ind->state_id)
							{
                    printf("recall scene %d %x\n", p_ind->state,p_ind->trans_time_ms);
								    app_tulip_set_screen_index(SCREEN_IN_SCENE);
								    app_tulip_process_scene_recall(p_ind->state);
							}								
							
              else if(MM_STATE_TULIPXI_VENDOR_OTA == p_ind->state_id)
							{
								   printf("OTA BEGIN %s ...................\n", __TIMESTAMP__);
#ifdef COBRA_OTA_ENABLE								  
								    unsigned int temp_ota_magic = 0x12345678;
								    
								    #if (COBRA_POWER_DOWN_SAVE_ENABLE==1)
                     save_data_before_ota();
	                  #endif
								
                    cobra_flash_write(M4_EFLASH_OTA_ADDRESS, sizeof(temp_ota_magic),	(unsigned char *)&temp_ota_magic);	
								
	                  cobra4_aon_pd_top_protection_en(1);
	                  cobra4_aon_pd_top_protection_en(0);
								    ota_platform_reset(0x00);
#endif
							}								
							
					}
					//2021/4/14 lichun
          else if(MM_API_VENDOR_MSG_IND  == p_ind->ind_code)
					{
							mm_api_vendor_msg_upd_ind_t *p_ind = (mm_api_vendor_msg_upd_ind_t *)param;
						
							printf("save scene %d \n", p_ind->state_id);
						
						  if(MM_STATE_TULIPXI_VENDOR_SCENS_SAVE == p_ind->state_id)
							{	
								#if APP_LIGHT_BREATH_FUNCTION  
								if(same_command_scene_edit((scene_edit_cmd *)p_ind->msg))  // 消除连续发送同一命令，导致呼吸抖动问题
									 break;
								#endif
							   app_tulip_set_screen_index(SCREEN_IN_SCENE);
							
								#if(COBRA_SCENE_VERSION2_ENABLE == 1)       
                  app_tulipe_scene_process((scene_edit_cmd *)p_ind->msg);
								#else                   
							    app_light_tulipi_tscns_save(p_ind->msg);
							    if((p_ind->msg[2]&0x0f) == 1)
                  {
                      app_tulip_process_scene_recall(p_ind->msg[1]);
                  }
                #endif
							}
					}								
				 
			 }
			 break;
			 
			 case MESH_API_NODE_RESET_IND:
			 {
				    //reset the node
				    printf("reset the node ......\n");
						#if APP_FLASH_LIGHT_NUMBERS	
	          uint8_t number = 8;

				    app_scene_close_change_light();
				    app_light_shade_ctrl_disable();

						#endif
				    //cobra_system_nvds_reset();
						cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, 4096); //del scene and save data
				    cobra_flash_erase(M4_EFLASH_PARAM_ADDRESS, 4096);
            cobra_flash_erase(APP_MESH_TULIPX_SCENS_DATA, 4096); //del scene and save data
				 
				    #if APP_FLASH_LIGHT_NUMBERS
				 		cobra_flash_write(M4_EFLASH_FLASH_NUMBERS_ADDRESS, sizeof(number),  (unsigned char *)&number);
						#endif
				 
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
									  printf("always in the network  address = 0x%x \n", m_tb_mio_get_prim_addr());
									
									  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
	//								  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
									  device_state = 1;
									
					          // 在正常使用 过程 中 保存数据 
									  // app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*40, 1);   //记录那个重复 replay attack								
								}
						    else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_BEING_PROV)
								{
									  printf("PROVING...................... \n");
									
										app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);

										#if (APP_FLASH_LIGHT_NUMBERS == 1)
									  #if (COBRA_FLASH_POWERON_CRASE == 1)
									  uint8_t number = get_mesh_flash_light_number();
									  #else
										uint8_t *p_buffer = (uint8_t * )M4_EFLASH_FLASH_NUMBERS_ADDRESS;
	                  uint8_t number = *p_buffer;
									  #endif
									  printf("number=%x \n",number);
									  if(number>0&&number!=0xFF)
										{
										set_flash_light_number(number);
										app_mesh_prov_start_timer();
										number = 0;
										//cobra_flash_erase(M4_EFLASH_FLASH_NUMBERS_ADDRESS, sizeof(uint8_t));	
										cobra_flash_write(M4_EFLASH_FLASH_NUMBERS_ADDRESS, sizeof(number),  (unsigned char *)&number);												
										}
										else	
										{
											  app_mesh_led_first_show();
										}
										
	                  #else
									      app_mesh_prov_init_timer();
									  #endif
										
									  device_state = 0;
								}
								else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_UNPROV)
								{
									   printf("UNPROV .....................\n");
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
                printf("save data .......................................................\n");		 
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


/// Default State handlers definition
const struct ke_msg_handler app_mesh_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_mesh_task_msg_handler},


};

const struct app_subtask_handlers app_mesh_handlers = APP_HANDLERS(app_mesh);



void app_mesh_set_g_tai_seconds(uint32_t seconds)
{
		 uint16_t i;
	
	   for(i = 0; i < light_model.total_model; i++)
	   {
		     if(light_model.model_id[i] == MM_ID_TSCNS_SCH)
				     break;
	   }
		 
		 if(i < light_model.total_model)
		 {	
	
	      struct mm_api_srv_set_cmd * req = KE_MSG_ALLOC(MESH_MDL_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                mm_api_srv_set_cmd);
		    
				req->cmd_code = MM_API_SRV_SET;
		    req->mdl_lid = light_model.mdl_lid[i];
        req->state_id = MM_STATE_TSCNS_TAI_SECONDS;
        req->state    = seconds;
        ke_msg_send(req);				
	 }
		 
}



void app_mesh_key_response_on_off_state(unsigned int on_off)
{

		 uint16_t i;
	
	   for(i = 0; i < light_model.total_model; i++)
	   {
		     if(light_model.model_id[i] == MM_ID_GENS_OO)  
			   {
				    break;
			   }
	   }
		 
		 if(i < light_model.total_model)
		 {	
			 
			  mm_gens_oo_cb_set_state(light_model.mdl_lid[i], MM_STATE_TYPE_CURRENT, on_off);
			  mm_gens_oo_cb_set_state(light_model.mdl_lid[i], MM_STATE_TYPE_TARGET, on_off);
			  printf("onoff state:%d ..............\n", on_off);
	 }		 

}

uint32_t get_device_state(void)
{
	return device_state;
}
#endif

