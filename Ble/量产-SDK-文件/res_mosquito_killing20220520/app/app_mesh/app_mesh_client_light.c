/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_client_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/7/30

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

		Liujian  2020/7/30			- Creation

*****************************************************************************/

#include "rwip_config.h"  

#include "app_mesh_client_light.h"
#include "app_mesh_sha256.h"
#include "m_api.h"
#include "m_tb.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "app_button.h"
#include "mm_genc.h"


#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_CLIENT_LIGHT == 1))

//static COBRA_SOFTWARE_TIMER * prov_timer_id;
//static COBRA_SOFTWARE_TIMER * mesh_save_timer_id;
//static COBRA_SOFTWARE_TIMER * mesh_test_key_id;


static uint32_t prov_timer_id;
static uint32_t mesh_save_timer_id;
static uint32_t mesh_test_key_id;

static void  app_mesh_sw9_key_long_press(void * param);
static void  app_mesh_sw9_key_down_press(void * param);
static void  app_mesh_save_information(uint32_t  temp);
static void  app_mesh_led_callback(uint32_t  param);
static void  app_mesh_test_send_key(uint32_t  param);
static void  app_mesh_led_all_light_on_off(unsigned int light_flag);
static void  app_mesh_gpio_init(void);

static uint8_t led_flag = 0;
static unsigned int  test_press_key = 0;

static m_lid_t  light_ctrl_model_lid;    //开关的控制 ID, 注册后可以得到

#define CLIENT_NODE_UUID {0xA9, 0x01, 0x71, 0x0C, 0x2F, 0x00, 0x00, 0xCA, 0xF6, 0x21, 0x7A, 0xFA, 0x28, 0x55, 0x55, 0x02}

uint8_t  rdw_uuid[MESH_DEV_UUID_LEN] = CLIENT_NODE_UUID;

void app_mesh_prov_init_timer(void)
{
//	   prov_timer_id = cobra_linked_softtimer_create(NULL, app_mesh_led_callback, 1000);
//	   cobra_linked_softtimer_start(prov_timer_id);	
	   prov_timer_id = cobra_soft_timer_create(app_mesh_led_callback);
	   cobra_soft_timer_start(prov_timer_id, COBRA_SOFT_TIMER_TICK/2, 1);
	
}

void app_mesh_save_init_time(unsigned int period)
{
//	   mesh_save_timer_id = cobra_linked_softtimer_create(NULL, app_mesh_save_information, 10000);
//	   cobra_linked_softtimer_start(mesh_save_timer_id);	
	   mesh_save_timer_id = cobra_soft_timer_create(app_mesh_save_information);
	    cobra_soft_timer_start(mesh_save_timer_id, period, 1);	 
}

void app_mesh_test_key_init_timer(unsigned int period)
{
//	   mesh_test_key_id = cobra_linked_softtimer_create(NULL, app_mesh_test_send_key, 5000);
//	   cobra_linked_softtimer_start(mesh_test_key_id);	
	    mesh_test_key_id = cobra_soft_timer_create(app_mesh_test_send_key);
	    cobra_soft_timer_start(mesh_test_key_id, period, 1);	  
}


void app_mesh_prov_uninit_timer(void)
{
		cobra_soft_timer_stop(prov_timer_id);
	    led_flag = 0;	
		app_mesh_led_all_light_on_off(0);		
}

void app_mesh_button_start(void)
{
#if COBRA_APP_BUTTON_EN	
		app_button_start();
#endif	
}

void app_mesh_task_init(void)
{
#if COBRA_APP_BUTTON_EN
		app_button_init();
		app_button_sw9_attach_event(PRESS_DOWN, app_mesh_sw9_key_down_press);	
	  app_button_sw9_attach_event(LONG_PRESS_HOLD, app_mesh_sw9_key_long_press);	
		//app_button_start();
#endif
	 // app_mesh_auth_init();
	  app_mesh_gpio_init();
	  
	  light_ctrl_model_lid = -1;

}

void app_mesh_key_public_key_state(m_lid_t model_lid, unsigned int key_state)
{
	  if(light_ctrl_model_lid < 0) return;
	   // Allocate the message
    struct m_api_model_publish_cmd * req = KE_MSG_ALLOC_DYN(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_model_publish_cmd, 2);
		req->cmd_code = M_API_MODEL_PUBLISH;
		req->model_lid = model_lid;
		req->tx_hdl = 0;
		req->opcode = MM_MSG_GEN_OO_SET;
		
		req->len = 2;
		req->msg[MM_GEN_OO_SET_OO_POS] = key_state;
		req->msg[MM_GEN_OO_SET_TID_POS] = rand();
		
		
    // Send the message
    ke_msg_send(req);

}	


//保存数据然后复位
static void  app_mesh_sw9_key_long_press(void * param)
{	
	  
	  if(cobra_system_nvds_is_modified())
		{
			 printf("save data ........................\n");
			 app_mesh_led_all_light_on_off(1);		
	     GLOBAL_INT_DISABLE();
		   cobra_system_nvds_save_to_eflash();
	     GLOBAL_INT_RESTORE();
	     app_mesh_led_all_light_on_off(0);
		}	
		
	
    //platform_reset(0x00);	
}

//按键清除邦定信息
void app_mesh_sw9_key_down_press(void * param)
{
//	  printf("erase nvds ....\n");
//	  app_timer_pause();
//		cobra_system_nvds_reset();
//    app_timer_resume();	
	
	    if(light_ctrl_model_lid < 0) return;
	
			if(test_press_key)
			 {
						//app_mesh_key_press(1);
				    app_mesh_key_public_key_state(light_ctrl_model_lid, 1);
						test_press_key = 0;
			 }
			 else
			 {
						//app_mesh_key_press(0);
				    app_mesh_key_public_key_state(light_ctrl_model_lid, 0);
						test_press_key = 1;
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
void app_mesh_led_callback(uint32_t  param)
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

static void  app_mesh_test_send_key(uint32_t  param)
{
	     if(light_ctrl_model_lid < 0) return;
	
	     if(test_press_key)
			 {
						//app_mesh_key_press(1);
				    app_mesh_key_public_key_state(light_ctrl_model_lid, 1);
						test_press_key = 0;
			 }
			 else
			 {
						//app_mesh_key_press(0);
				    app_mesh_key_public_key_state(light_ctrl_model_lid, 0);
						test_press_key = 1;
			 }
	
}

void app_mesh_save_information_event(void)
{	
    cobra_system_nvds_save_to_eflash();
	//  if(light_node_reboot_flag)
		{
			 platform_reset(0x00);
		}
}

static void  app_mesh_save_information(uint32_t  temp)
{ 
	  if(cobra_system_nvds_is_modified())
		{
			 printf("NVDS ........................\n");
			 cobra_app_set_mesh_nvds_task();
		}	
}

//自动绑定 APP key
void app_mesh_task_bind_app_key(uint16_t app_key_lid, m_lid_t model_lid)
{
    // Status
     uint16_t status;

		 status = m_tb_key_model_bind(app_key_lid, model_lid);

		 if (status == MESH_ERR_NO_ERROR)
		 {
						// Inform the Model IO manager
						m_tb_mio_bind(model_lid);
				
   			    printf("OK .......\n");
			 
			      status = m_tb_mio_set_publi_param(model_lid, 0xc000, NULL, app_key_lid, 7, 0, 0, 0);
		 
		        printf("status = %d \n", status);
		 }
		 
		 
		 //status = m_tb_mio_add_subscription(light_ctrl_model_lid, 0xc000); 
			 
		

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
在任务中 添加事件, 主要是用于开始 Mesh 的工作
*/
void app_mesh_task_begin(void)
{
	    mm_api_register_client(MM_CMDL_IDX_GENC_ONOFF);  //注册开关 client
	
	    app_mesh_task_load_info();	
	
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
	  printf("msgid = 0x%x \n", msgid);
	  switch(msgid)
	  {

		   case MESH_API_PROV_PARAM_REQ_IND:
		   {
			
					  struct m_api_prov_param_cfm * req = KE_MSG_ALLOC(MESH_API_PROV_PARAM_CFM,
																											prf_get_task_from_id(TASK_ID_MESH),
																											TASK_APP,
																											m_api_prov_param_cfm);
				
					  for(i = 0; i < MESH_DEV_UUID_LEN; i++)
					  {
							    req->dev_uuid[i] = rdw_uuid[i];
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
       	 // printf("PROXY_ADV state = %d, reason = %d \n", p_ind->state, p_ind->reason);
			 }
			 break;
			 
			 case MESH_API_GROUP_UPDATE_IND:
			 {
				  uint8_t appkey_flag;
					 m_api_group_update_ind_t *p_ind = (m_api_group_update_ind_t *)param;
				   if(p_ind->added)
					 {
						    printf("APP KEY %d %d \n", p_ind->appkey_id, p_ind->app_key_lid);
						    if(light_ctrl_model_lid > 0)
								{
										app_mesh_task_bind_app_key(p_ind->app_key_lid, light_ctrl_model_lid);
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
									  app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*40);
									//  light_node_reboot_flag = 1;
								}
								
								}
								//app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*40);
					 }
			 }
			 break;
			 
			 
			 case MESH_MDL_API_IND:
			 {
					mm_api_ind_t *p_ind = (mm_api_ind_t *)param;
				 
				  printf("API_IND = %x \n", p_ind->ind_code);
				 
				  if(p_ind->ind_code == MM_API_REGISTER_IND)
					{
						 mm_api_register_ind_t *p_ind = (mm_api_register_ind_t *)param;
						 if(p_ind->model_id == MM_ID_GENC_OO)
						 {
								 light_ctrl_model_lid = p_ind->mdl_lid;
						 }
						 printf("elmt_idx = %d  mdl_lid = %d  model_id = %x \n",  p_ind->elmt_idx, p_ind->mdl_lid, p_ind->model_id);						
					}
					
					else if(p_ind->ind_code == MM_API_SRV_STATE_UPD_IND)
					{
						  
							mm_api_srv_state_upd_ind_t *p_ind = (mm_api_srv_state_upd_ind_t *)param;
						
						  printf("state_id = %d\n", p_ind->state_id);
						
						  if(p_ind->state_id == MM_STATE_GEN_ONOFF)
							{
								 if(p_ind->state)
								 {
									  app_mesh_led_all_light_on_off(1);
								 }
								 else
								 {
									 app_mesh_led_all_light_on_off(0);
								 }
							}
					}
				 
			 }
			 break;
			 
			 case MESH_API_NODE_RESET_IND:
			 {
				    //reset the node
				//    printf("reset the node ......\n");
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
		
									  app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NET);
					
									//  app_mesh_save_init_time(COBRA_SOFT_TIMER_TICK*60);
									  app_mesh_button_start();
									
									 // app_mesh_test_key_init_timer();    //测试发送开关信息到 组0xc000 
								}
						    else if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_BEING_PROV)
								{
									  printf("M_TB_STATE_PROV_STATE_BEING_PROV \n");
										app_mesh_task_enable_proxy(MESH_PROXY_ADV_CTL_START_NODE);
									  app_mesh_prov_init_timer();      //指示在配置,灯闪烁
									  app_mesh_button_start();
									
									 // app_mesh_save_init_time();
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


/// Default State handlers definition
const struct ke_msg_handler app_mesh_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_mesh_task_msg_handler},


};

const struct app_subtask_handlers app_mesh_handlers = APP_HANDLERS(app_mesh);

#endif
