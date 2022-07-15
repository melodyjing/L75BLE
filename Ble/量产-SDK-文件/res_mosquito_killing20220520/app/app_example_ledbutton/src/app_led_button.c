/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_led_button.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   use Led and button service 
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/
#if (BLE_APP_LED_BUTTON)
#include "app_led_button.h"
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "led_button.h"
#include "led_button_task.h"
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "rdw_mcu.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "app_button.h"

static struct app_led_button_env_tag    app_led_button_env;

#define BLE_BUTTON_KEY_NUMBER			GPIO_PIN_29
#define BLE_BUTTON_PIN_INDEX			PIN29_INDEX

static struct Button cobra_ble_key_SW9;

//按键的处理
static void app_led_button_key_up_press(void * param);
static void app_led_button_key_down_press(void * param);

static void app_led_button_key_repeat(void * param);
static void app_led_button_key_single_click(void * param);
static void app_led_button_key_double_click(void * param);
static void app_led_button_key_long_press_start(void * param);
static void app_led_button_key_long_press_hold(void * param);

static unsigned char app_ble_read_cobra_key_SW9() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(BLE_BUTTON_KEY_NUMBER);
	printf("key_value = 0x%x \n", key_value);
	return key_value;
}

static void app_ble_button_init(void)
{
	
		 app_m3_gpio_set_direction(BLE_BUTTON_KEY_NUMBER, GPIO_INPUT);
	
		 cobra_pin_pullup_enable(BLE_BUTTON_PIN_INDEX, 1);
	
		 button_init(&cobra_ble_key_SW9, app_ble_read_cobra_key_SW9, GPIO_LOW);
		
}

static void app_ble_button_start(void)
{
			button_start(&cobra_ble_key_SW9);
}

static void app_ble_button_stop(void)
{
			button_stop(&cobra_ble_key_SW9);
}

void app_ble_button_sw9_attach_event(PressEvent event, BtnCallback cb)
{
		 button_attach(&cobra_ble_key_SW9, event, cb);
}

void app_led_button_button_init(void)
{
		app_led_button_env.button_value = 0;
	  app_ble_button_init();
	  app_ble_button_sw9_attach_event(PRESS_DOWN,  app_led_button_key_down_press);
		app_ble_button_sw9_attach_event(PRESS_UP,    app_led_button_key_up_press);
	
	  app_ble_button_sw9_attach_event(PRESS_REPEAT,   app_led_button_key_repeat);
		app_ble_button_sw9_attach_event(SINGLE_CLICK,   app_led_button_key_single_click);
	  app_ble_button_sw9_attach_event(DOUBLE_CLICK,      app_led_button_key_double_click);
		app_ble_button_sw9_attach_event(LONG_RRESS_START,   app_led_button_key_long_press_start);
	  app_ble_button_sw9_attach_event(LONG_PRESS_HOLD,   app_led_button_key_long_press_hold);
}

//按键开始工作
void app_led_button_begin(void)
{	  
  	app_ble_button_start();	
}

//按键停止工作
void app_led_button_off(void)
{
	 app_ble_button_stop();
}

static void app_led_button_key_down_press(void * param)
{
	  printf("off ...\n");
    app_led_button_send_button_value(0x01);
}

static void app_led_button_key_up_press(void * param)
{
	  printf("up ...\n");
    app_led_button_send_button_value(0x00);
}

static void app_led_button_key_repeat(void * param)
{
	  printf("repeat ...\n");
    app_led_button_send_button_value(0x02);
}

static void app_led_button_key_single_click(void * param)
{
	  printf("single_click ...\n");
    app_led_button_send_button_value(0x03);
}
static void app_led_button_key_double_click(void * param)
{
	  printf("double_click ...\n");
    app_led_button_send_button_value(0x04);
}
static void app_led_button_key_long_press_start(void * param)
{
	  printf("long_press_start ...\n");
    app_led_button_send_button_value(0x05);
}
static void app_led_button_key_long_press_hold(void * param)
{
	  printf("ong_press_hold ...\n");
    app_led_button_send_button_value(0x06);
}


/////////////////////////////////////////////////////////////////////////////
//LED 的处理
void app_led_button_led_init(void)
{
	  app_m3_gpio_set_direction(GPIO_PIN_23, GPIO_OUTPUT);
	
	  app_m3_gpio_write_pin(GPIO_PIN_23, GPIO_HIGH);
	
	  cobra_pin_pullup_enable(PIN23_INDEX, 1);	
	
}
//LED 灯亮
void app_led_button_led_on(void)
{
	 app_m3_gpio_write_pin(GPIO_PIN_23, GPIO_LOW);
}
//LED 灯灭
void app_led_button_led_off(void)
{
	 app_m3_gpio_write_pin(GPIO_PIN_23, GPIO_HIGH);
}

///////////////////////////////////////////////////////////////////////////////////////
//初始化
void app_led_button_init(void)
{
    // Reset the environment
    memset(&app_led_button_env, 0, sizeof(app_led_button_env));
	
	  app_led_button_led_init();
	
    app_led_button_button_init();	
}

//将 LED BUTTON Profile 添加到程序中去
void app_led_button_add_service(void)
{
	  struct led_button_db_cfg* db_cfg;
 
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct led_button_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_128); ;
    req->prf_task_id = TASK_ID_LED_BUTTON;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct led_button_db_cfg* )req->param;
	  db_cfg->features = LED_BUTTON_ALL_FEAT_SUP;
	  
    // Send the message
    ke_msg_send(req);
	
}

void app_led_button_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct led_button_enable_req * req = KE_MSG_ALLOC(LED_BUTTON_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_LED_BUTTON),
                                                TASK_APP,
                                                led_button_enable_req);

    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;
		
		app_led_button_led_on();
		
		req->led_value = 0x01;   //代表灯亮
		
    // Send the message
    ke_msg_send(req);
		
}


void app_led_button_send_button_value(uint8_t button_value)
{

    struct led_button_button_send_req * req = KE_MSG_ALLOC(LED_BUTTON_BUTTON_SEND_REQ,
                                                    prf_get_task_from_id(TASK_ID_LED_BUTTON),
                                                    TASK_APP,
                                                    led_button_button_send_req);

    req->key_value =  button_value;

		
		ke_msg_send(req);
}


static int app_led_button_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}


static int app_led_led_value_handler(ke_msg_id_t const msgid,
                              struct led_button_led_req_ind *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    if(param->led_value)
		{
			  app_led_button_led_on();
		}
		else
		{
			 app_led_button_led_off();
		}
    return (KE_MSG_CONSUMED);
}

static int app_led_button_cfg_handler(ke_msg_id_t const msgid,
                              struct led_button_cfg_indntf_ind *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	 
	  if(param->ntf_ind_cfg & LED_BUTTON_CFG_BUTTON_NTF)
		{
				app_led_button_begin();	
		}
		else
		{
			  app_led_button_off();
		}
	  return (KE_MSG_CONSUMED);
}


const struct ke_msg_handler app_led_button_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_led_button_msg_handler},
		{LED_BUTTON_LED_REQ_IND, 				(ke_msg_func_t)app_led_led_value_handler},
		{LED_BUTTON_CFG_INDNTF_IND,     (ke_msg_func_t)app_led_button_cfg_handler},
 
};

const struct app_subtask_handlers app_led_button_handlers = APP_HANDLERS(app_led_button);

#endif