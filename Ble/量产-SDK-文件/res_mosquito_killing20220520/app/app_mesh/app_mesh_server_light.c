/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_server_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/7/31

	Description :   mesh task
	说明一下: MESH IP 里面要将 下面的定义定义为如下:  这样 NVDS 的更改
	变为 10s, 这样避免演示的时候, 刚刚连接到天猫精灵, 然后复位 IC ,这样
	很快就丢失了信息, 快一点更好
	
/// Default NVDS update duration (in seconds, 10 minutes)
#define M_TB_STORE_DEFAULT_UPD_DUR                  (10 * 1)

#define MESH_BUF_BLOCK_NB_MAX                (10)    //2020/7/30 Liujian

#define M_STORE_SEQ_OFFSET                          (1)    //这个是控制灯的define, 必须改为 1, 否者 灯复位后, 不能控制
                                                           //原因是要保存SEQ 和IV , 要快速保存这些参数到NVDS
	Changed Log	:

		Liujian  2020/7/31			- Creation

*****************************************************************************/

#include "rwip_config.h"  

#include "app_mesh_server_light.h"
#include "app_mesh_sha256.h"
#include "m_api.h"
#include "m_tb.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"

#if(COBRA_APP_BUTTON_EN)

#include "app_button.h"

#endif

#include "mm_genc.h"
#include "app_mesh_tscns.h"
//#include "cobra4A_linked_softtimer.h"
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_SERVER_LIGHT == 1))

#define SERVER_IS_COLOR_CONTROL   1    //是三色灯

#if(SERVER_IS_COLOR_CONTROL == 1)

typedef struct {
	   uint8_t   elmt_idx;       //注册时用的 element index
	   m_lid_t   mdl_lid[28];    //最多一个element 下有 10 个 model
	   uint32_t  model_id[28];
	   uint8_t   total_model;
}model_idx_element;

static model_idx_element  light_model;
#endif
//static uint32_t g_tai_seconds;

//static COBRA_SOFTWARE_TIMER * prov_timer_id;
//static COBRA_SOFTWARE_TIMER * mesh_save_timer_id;
//static COBRA_SOFTWARE_TIMER * mesh_test_key_id;

static uint32_t prov_timer_id;
static uint32_t mesh_save_timer_id;
static uint32_t schdule_time_id;

static uint32_t mesh_state_timer_id;

static void  app_mesh_sw9_key_long_press(void * param);
static void  app_mesh_sw9_key_down_press(void * param);

static void  app_mesh_save_information(uint32_t param);
static void  app_mesh_led_callback(uint32_t param);
static void  app_mesh_state_callback(uint32_t param);
static void  app_mesh_schdule_callback(uint32_t param);

//static void  app_mesh_test_send_key(COBRA_SOFTWARE_TIMER * param);

static void  app_mesh_led_all_light_on_off(unsigned int light_flag);
static void  app_mesh_gpio_init(void);

static uint8_t led_flag = 0;
static unsigned int  test_press_key = 0;

static m_lid_t  light_model_lid;
static uint8_t  light_elmt_idx;

static uint8_t  light_app_keyid;

static uint8_t   light_node_reboot_flag;
static uint8_t   light_on_off_state;
#define CLIENT_NODE_UUID {0xA9, 0x05, 0x71, 0x0C, 0x2F, 0x00, 0x00, 0xCA, 0xF6, 0x21, 0x7A, 0xFA, 0x12, 0x34, 0x56, 0x78}

uint8_t  rdw_uuid[MESH_DEV_UUID_LEN] = CLIENT_NODE_UUID;

void app_mesh_schdule_timer_init(void)
{
	   schdule_time_id = cobra_soft_timer_create(app_mesh_schdule_callback);
	   cobra_soft_timer_start(schdule_time_id, COBRA_SOFT_TIMER_TICK, 1);	
}


void app_mesh_prov_init_timer(void)
{
	   prov_timer_id = cobra_soft_timer_create(app_mesh_led_callback);
	   cobra_soft_timer_start(prov_timer_id, COBRA_SOFT_TIMER_TICK/2, 1);
}

void app_mesh_save_init_time(unsigned int period)
{
		  mesh_save_timer_id = cobra_soft_timer_create(app_mesh_save_information);
	    cobra_soft_timer_start(mesh_save_timer_id, period, 1);	 
}

void app_mesh_save_time_stop(void)
{
	   cobra_soft_timer_stop(mesh_save_timer_id);
}


void app_mesh_state_time(unsigned int period)
{
		  mesh_state_timer_id = cobra_soft_timer_create(app_mesh_state_callback);
	    cobra_soft_timer_start(mesh_state_timer_id, period, 1);	  
}



//void app_mesh_test_key_init_timer(void)
//{
////		   mesh_test_key_id = cobra_linked_softtimer_create(NULL, app_mesh_test_send_key, 5000);
////	     cobra_linked_softtimer_start(mesh_test_key_id);	
//}


void app_mesh_prov_uninit_timer(void)
{
		cobra_soft_timer_stop(prov_timer_id);
	  led_flag = 0;	
		app_mesh_led_all_light_on_off(0);		
}

#if COBRA_APP_BUTTON_EN

static void  app_mesh_sw9_key_long_press(void * param)
{
#if ((COBRA4C_INSIDE_SPI_FLASH_ENABLE) && (APP_RUN_FROM_SPI_FLASH_ENABLE == 0))  // 只有程序在 EFLASH 运行时,才执行这个
	  cobra_system_nvds_reset();
	  platform_reset(0);
	#endif
	
}

static void  app_mesh_sw9_key_down_press(void * param)
{
	
}

#endif

void app_mesh_task_init(void)
{
#if COBRA_APP_BUTTON_EN
		app_button_init();
		app_button_sw9_attach_event(PRESS_DOWN, app_mesh_sw9_key_down_press);	
	  app_button_sw9_attach_event(LONG_PRESS_HOLD, app_mesh_sw9_key_long_press);	
		app_button_start();
#endif
	 // app_mesh_auth_init();
	  app_mesh_gpio_init();
	  
	  light_model_lid = -1;   //灯的模型还没有加上去
	  light_elmt_idx = 0;     //灯放在第一个位置, 最多 MESH_MODEL_NB_MAX
	
	  light_node_reboot_flag = 0;
	  light_on_off_state = 0;
}

#if(SERVER_IS_COLOR_CONTROL == 1)
/*
每次上电设置 灯为   default 亮度为 4000
*/
void app_mesh_set_light_state(unsigned int value)
{
		 uint16_t i;
	   for(i = 0; i < light_model.total_model; i++)
	   {
		   if(light_model.model_id[i] == MM_ID_LIGHTS_LN)
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
            req->state_id = MM_STATE_LIGHT_LN_DFLT;
            req->state    = value;
            ke_msg_send(req);
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
		 
//		 if(i < light_model.total_model)
//		 {	
//		      p_env = mm_tb_state_get_env(light_model.mdl_lid[i]);
//			    SETB(p_env->info, MM_TB_STATE_MDL_INFO_MAIN, 1);
//		 }
		 
		
		 
	
}


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



void app_mesh_set_light_on_off_state(unsigned int on_off)
{
#if 1	
		 uint16_t i;
	   for(i = 0; i < light_model.total_model; i++)
	   {
		   if(light_model.model_id[i] == MM_ID_GENS_OO)  //MM_ID_GENS_OO)
				  break;
	   }
		 if(i < light_model.total_model)
		 {	
			 
			  mm_gens_oo_cb_set_state(light_model.mdl_lid[i], MM_STATE_TYPE_CURRENT, on_off);
			  mm_gens_oo_cb_set_state(light_model.mdl_lid[i], MM_STATE_TYPE_TARGET, on_off);
	
	
//	      struct mm_api_srv_set_cmd * req = KE_MSG_ALLOC(MESH_MDL_API_CMD,
//                                                prf_get_task_from_id(TASK_ID_MESH),
//                                                TASK_APP,
//                                                mm_api_srv_set_cmd);
//		        req->cmd_code = MM_API_SRV_SET;
//		        req->mdl_lid = light_model.mdl_lid[i];
//            req->state_id = MM_STATE_LIGHT_LN;
//            req->state    = on_off;
//            ke_msg_send(req);
	 }		 
#else
	   uint16_t i;
	   for(i = 0; i < light_model.total_model; i++)
	   {
		   if(light_model.model_id[i] == MM_ID_LIGHTS_LN)  //MM_ID_GENS_OO)
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
            req->state_id = MM_STATE_LIGHT_LN;
            req->state    = on_off;
            ke_msg_send(req);
	 }	

     for(i = 0; i < light_model.total_model; i++)
	   {
		   if(light_model.model_id[i] == MM_ID_GENS_OO)  //MM_ID_GENS_OO)
				  break;
	   }
		 
		 struct mm_api_srv_set_cmd * req2 = KE_MSG_ALLOC(MESH_MDL_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                mm_api_srv_set_cmd);
		 req2->cmd_code = MM_API_SRV_SET;
		 req2->mdl_lid = light_model.mdl_lid[i];
     req2->state_id = MM_STATE_LIGHT_LN;
//     req2->state    = on_off;
		 
		 if(on_off > 0)
		 {
			   req2->state    = 1;
		 }
		 else
		 {
			   req2->state    = 0;
		 }
     ke_msg_send(req2);
		 
		 
	 
#endif
}

#endif


void app_mesh_key_public_key_state(unsigned int key_state)
{
	  if(light_model_lid < 0) return;
		
	   // Allocate the message
    struct m_api_model_publish_cmd * req = KE_MSG_ALLOC_DYN(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_model_publish_cmd, 2);
		req->cmd_code = M_API_MODEL_PUBLISH;
		req->model_lid = light_model_lid;
		req->tx_hdl = 0;
		req->opcode = MM_MSG_GEN_OO_SET;
		
		req->len = 2;
		req->msg[MM_GEN_OO_SET_OO_POS] = key_state;
		req->msg[MM_GEN_OO_SET_TID_POS] = rand();
		
		
    // Send the message
    ke_msg_send(req);

}	



void app_mesh_key_response_key_state(unsigned int key_state)
{
//	  return;   //测试
	
	  if(light_model_lid < 0) return;
	
	   uint16_t i;
	   for(i = 0; i < light_model.total_model; i++)
	   {
		     if(light_model.model_id[i] == MM_ID_GENS_OO)  //MM_ID_GENS_OO)
				     break;
	   }
		 if(i < light_model.total_model)
		 {	
			     // Allocate the message
          struct m_api_model_rsp_send_cmd * req = KE_MSG_ALLOC_DYN(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_model_rsp_send_cmd, MM_GEN_OO_STATUS_LEN);
					
		      req->cmd_code = M_API_MODEL_RSP_SEND;
		      req->model_lid = light_model.mdl_lid[i];
		      req->tx_hdl = 0;
		      req->opcode = MM_MSG_GEN_OO_STATUS;
		      req->key_lid = light_app_keyid;
		
		      req->len = MM_GEN_OO_STATUS_LEN;
		      req->dst = 0x0001;
					req->not_relay = 0;
		      req->msg[MM_GEN_OO_STATUS_OO_POS] = key_state;
	        req->msg[MM_GEN_OO_STATUS_TGT_OO_POS] = key_state;
		      req->msg[MM_GEN_OO_STATUS_REM_TIME_POS] = 0;
		
          // Send the message
          ke_msg_send(req);

		 }
		 
	   
}	


/*
初始化 LED IO 口

*/
static void app_mesh_gpio_init(void)
{


  app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_OUTPUT);
	app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_OUTPUT);


	app_mesh_led_all_light_on_off(0);
	led_flag = 0;

}

static void app_mesh_led_all_light_on_off(unsigned int light_flag)
{
		if(light_flag)
		{
	
			app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_LOW);
			app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_LOW);			
		}
		else
		{
			app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_HIGH);
			app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_HIGH);
		}
	
}

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
	 app_mesh_led_all_light_on_off(led_flag);
}


static void  app_mesh_state_callback(uint32_t param)
{
	 app_mesh_key_response_key_state(light_on_off_state);
}

void app_mesh_save_information_event(void)
{	
	
	  
		cobra_system_nvds_save_to_eflash();

	  printf("save .................. \n");
	  

	  if(light_node_reboot_flag)
		{
	//		 platform_reset(0x00);
		}
}

static void  app_mesh_save_information(uint32_t param)
{
	  if(cobra_system_nvds_is_modified())
		{

			 cobra_app_set_mesh_nvds_task();

		}	
}

//自动绑定 APP key
void app_mesh_task_bind_app_key(uint16_t app_key_lid)
{
     // Status
	//  return;
#if 1
     uint16_t status;
	
	   uint32_t i;

		 m_lid_t model_lid;
	
	   uint32_t  flag = 0;
	
     for(i = 0; i < light_model.total_model; i++)
		 {
			      model_lid = light_model.mdl_lid[i];
			     
			      if(flag && (light_model.model_id[i] == MM_ID_GENS_OO)) continue;
				   // status = m_tb_key_model_bind(app_key_lid, model_lid);

					  status = m_tb_mio_add_subscription(model_lid, 0xc000); 
				  //  status = m_tb_mio_add_subscription(model_lid, 0xffff);
			      
			      if(light_model.model_id[i] == MM_ID_GENS_OO)
				    {
					      flag = 1;
				    }

	   }
		 m_tb_state_set_relay_state(1, 1);
		 
//		 app_mesh_set_light_state(0xffff);
		 
//		 app_mesh_set_light_on_off_state(0);
		 
     app_mesh_set_on_off_state_main();
		 
//	   for(i = 0; i < light_model.total_model; i++)
//	   {
//		   if(light_model.model_id[i] == MM_ID_GENS_OO)  
//				  break;
//	   }
//		 
//		 m_tb_mio_set_publi_param(light_model.mdl_lid[i], 0x0001, 0, app_key_lid,  10,  2,  0, 0);
		 
#else	
     uint16_t status;
	
     if(light_model_lid < 0) return;
	
	//	 status = m_tb_key_model_bind(app_key_lid, light_model_lid);

		 if (status == MESH_ERR_NO_ERROR)
		 {
						// Inform the Model IO manager
			//			m_tb_mio_bind(light_model_lid);
						printf("OK .......\n");
		 }
		 
		 status = m_tb_mio_add_subscription(light_model_lid, 0xffff); 
		 
	//	 m_tb_mio_set_publi_param(
		
  //   m_tb_state_set_relay_state(1, 1);
		 
		 printf("status = %d \n", status);
#endif
}

static void  app_mesh_schdule_callback(uint32_t param)
{
	
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
在任务中 添加事件, 主要是用于开始 Mesh 的工作
*/
void app_mesh_task_begin(void)
{
#if(SERVER_IS_COLOR_CONTROL == 1)
	      light_model.elmt_idx = 0;
	      light_model.total_model = 0;
	      mm_api_register_server(0, MM_CFG_IDX_GENS_ONOFF, 1);	
        mm_api_register_server(0, MM_CFG_IDX_TSCNS_TIME, 1);
	      mm_api_register_server(0, MM_CFG_IDX_TSCNS_SCENCE, 1);
	      mm_api_register_server(0, MM_CFG_IDX_TSCNS_SCHDULE, 1);
	
#endif
	
	      app_mesh_task_load_info();	
#if(APP_RUN_FROM_SPI_FLASH_ENABLE)	
	    printf("mesh light run from SPI Flash V1.0\n");
#else
      printf("mesh light run from EFLASH V2.0\n");
#endif	
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

//当对方连接的设备退出时
void app_mesh_task_disconnection(void)
{
	  printf("app_mesh_task_disconnection .... \n");
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

		db_cfg->prf_cfg.pid = 0x0071;
		db_cfg->prf_cfg.vid = 0x0001;
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


static int app_mesh_task_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{

	  int32_t i;
    uint8_t   temp;

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
									  app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*20);
									  light_node_reboot_flag = 1;
								}
					 }
			 }
			 break;
			 
			 
			 case MESH_MDL_API_IND:
			 {
					mm_api_ind_t *p_ind = (mm_api_ind_t *)param;
				 
			//	  printf("API_IND = %x \n", p_ind->ind_code);
				 
				  if(p_ind->ind_code == MM_API_REGISTER_IND)
					{
						 #if(SERVER_IS_COLOR_CONTROL == 1)
						      mm_api_register_ind_t *p_ind = (mm_api_register_ind_t *)param;	
							    light_model.mdl_lid[light_model.total_model] = p_ind->mdl_lid;
							    light_model.model_id[light_model.total_model] = p_ind->model_id;
							    light_model.total_model++;
						   //   printf("elmt_idx = %d  mdl_lid = %x  model_id = %x \n",  p_ind->elmt_idx, p_ind->mdl_lid, p_ind->model_id);	
						 #else
						 mm_api_register_ind_t *p_ind = (mm_api_register_ind_t *)param;
						 if((p_ind->model_id == MM_ID_GENS_OO)&&(p_ind->elmt_idx == light_elmt_idx))
						 {
							    light_model_lid = p_ind->mdl_lid;   //得到灯的 model_id
						 }
						// printf("elmt_idx = %d  mdl_lid = %x  model_id = %x \n",  p_ind->elmt_idx, p_ind->mdl_lid, p_ind->model_id);	
             #endif	
						 
					}
					
					else if(p_ind->ind_code == MM_API_SRV_STATE_UPD_IND)
					{
						  
							mm_api_srv_state_upd_ind_t *p_ind = (mm_api_srv_state_upd_ind_t *)param;
						
						  printf("state_id = %d\n", p_ind->state_id);
						
						  if(p_ind->state_id == MM_STATE_GEN_ONOFF)
							{
								 if(p_ind->state)
								 {
									  printf("ON ............................ \n");
									  app_mesh_led_all_light_on_off(1);
								 }
								 else
								 {
									 printf("OFF ............................ \n");
									 app_mesh_led_all_light_on_off(0);
								 }
							}
							else if(p_ind->state_id == MM_STATE_LIGHT_HSL_LN)
							{
								  printf("light lns lightless = %d \n", p_ind->state);
							//	  app_mesh_set_light_on_off_state(p_ind->state);
								  if(p_ind->state > 0)
									{
										  app_mesh_set_light_on_off_state(1);
										  app_mesh_led_all_light_on_off(1);
										  app_mesh_key_response_key_state(1);
										  light_on_off_state = 1;
									}
									else
									{
										 app_mesh_set_light_on_off_state(0);
										 app_mesh_led_all_light_on_off(0);
										 app_mesh_key_response_key_state(0);
										 light_on_off_state = 0;
									}
							}
							else if(p_ind->state_id == MM_STATE_TSCNS_TAI_SECONDS)
							{
								  printf("tai seconds = %d \n", p_ind->state);
								  app_mesh_tscns_set_tai_seconds(p_ind->state);
								
								  app_mesh_tscns_set_seconds_callback(app_mesh_set_g_tai_seconds);
								
							}
							else if(p_ind->state_id == MM_STATE_TSCNS_ZONE_OFFSET)
							{
								 printf("zone offset = %d \n", p_ind->state);
								 app_mesh_tscns_set_zone_offset(p_ind->state);
							}
              else if(p_ind->state_id == MM_STATE_TSCNS_ACTION_ON)
              {
								  app_mesh_led_all_light_on_off(1);
              }
              else if(p_ind->state_id == MM_STATE_TSCNS_ACTION_OFF)
              {
								  app_mesh_led_all_light_on_off(0);
              }							
					}
				 
			 }
			 break;
			 
			 case MESH_API_NODE_RESET_IND:
			 {
				    //reset the node
				    printf("reset the node ......\n");
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
									  printf("always in the network \n");
		
									  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
					
									  //app_mesh_state_time(COBRA_SOFT_TIMER_TICK*3);
									
									  app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*20);   //记录那个重复 replay attack
									
									  //app_mesh_test_key_init_timer();
								}
						    else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_BEING_PROV)
								{
									  printf("PROVING...................... \n");
									 // m_tb_state_set_gatt_proxy_state(M_CONF_GATT_PROXY_STATE_ENABLED);
										app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
									  app_mesh_prov_init_timer();
									 // cobra_system_nvds_reset();
									  //app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*40);
									  //appm_update_adv_state(true);   //如果没有进入配网状态, 不要广播其他信息
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

#endif
