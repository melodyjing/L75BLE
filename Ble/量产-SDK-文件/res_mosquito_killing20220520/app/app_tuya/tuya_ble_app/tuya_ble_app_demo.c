/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_ble_app_demo.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/19

	Description :   
	      移植到涂鸦 SDK 

	Changed Log	:

		Liujian  2021/5/19			- Creation
*****************************************************************************/

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "tuya_ble_app_demo.h"
//#include "tuya_ble_demo_version.h"
#include "tuya_ble_log.h"
#include "rdw_mcu.h"
#include "cobra_uart.h"
#include "cobra_pinmux.h"
#include "rdw_gpio.h"

tuya_ble_device_param_t device_param = {0};

static const char auth_key_test[] = "GvkGP46M0uBXdCpypU3JbDicJF3a4hVk";
static const char device_id_test[] = "tuya2b6b9b3dd8cf";
//DC 23 4D 43 A9 81
static const uint8_t mac_test[6] = {0x78,0xA9,0x43,0x4D,0x23,0xDC}; //The actual MAC address is : 66:55:44:33:22:11

static uint8_t dp_data_array[255+3];
static uint16_t dp_data_len = 0;
static uint16_t sn = 0;
static uint32_t time_stamp = 1587795793;

#if (COBRA_LIGHT_WS2812B_ENABLE == 1)
void app_light_ws2812b_init(void);
void app_light_ws2812b_light_data(uint32_t *p_buffer, uint32_t length);
uint32_t   app_test_array[16];
#endif

static void tuya_cb_handler(tuya_ble_cb_evt_param_t* event)
{
    int16_t result = 0;
    switch (event->evt)
    {
    case TUYA_BLE_CB_EVT_CONNECTE_STATUS:
        TUYA_APP_LOG_INFO("received tuya ble conncet status update event,current connect status = %d",event->connect_status);
        break;
    case TUYA_BLE_CB_EVT_DP_WRITE:
        dp_data_len = event->dp_write_data.data_len;
        memset(dp_data_array,0,sizeof(dp_data_array));
        memcpy(dp_data_array,event->dp_write_data.p_data,dp_data_len);        
        TUYA_APP_LOG_HEXDUMP_DEBUG("received dp write data :",dp_data_array,dp_data_len);
        sn = 0;
        tuya_ble_dp_data_report(dp_data_array,dp_data_len);//1

        break;
    case TUYA_BLE_CB_EVT_DP_DATA_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data report response result code ken = %d",event->dp_response_data.status);
		    {
			        //测试灯带反应时间				
					   app_light_ws2812b_light_data(app_test_array, 16);
		    }

        break;
    case TUYA_BLE_CB_EVT_DP_DATA_WTTH_TIME_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data report response result code =%d",event->dp_response_data.status);
        break;
        case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data with flag report response sn = %d , flag = %d , result code =%d",event->dp_with_flag_response_data.sn,event->dp_with_flag_response_data.mode
    ,event->dp_with_flag_response_data.status);

        break;
    case TUYA_BLE_CB_EVT_DP_DATA_WITH_FLAG_AND_TIME_REPORT_RESPONSE:
        TUYA_APP_LOG_INFO("received dp data with flag and time report response sn = %d , flag = %d , result code =%d",event->dp_with_flag_and_time_response_data.sn,
    event->dp_with_flag_and_time_response_data.mode,event->dp_with_flag_and_time_response_data.status);
       
        break;
    case TUYA_BLE_CB_EVT_UNBOUND:
        
        TUYA_APP_LOG_INFO("received unbound req");

        break;
    case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND:
        
        TUYA_APP_LOG_INFO("received anomaly unbound req");

        break;
    case TUYA_BLE_CB_EVT_DEVICE_RESET:
        
        TUYA_APP_LOG_INFO("received device reset req");

        break;
    case TUYA_BLE_CB_EVT_DP_QUERY:
        TUYA_APP_LOG_INFO("received TUYA_BLE_CB_EVT_DP_QUERY event");
        if(dp_data_len>0)
            tuya_ble_dp_data_report(dp_data_array,dp_data_len);
        break;
    case TUYA_BLE_CB_EVT_OTA_DATA:
        //tuya_ota_proc(event->ota_data.type,event->ota_data.p_data,event->ota_data.data_len);
        break;
    case TUYA_BLE_CB_EVT_NETWORK_INFO:
        TUYA_APP_LOG_INFO("received net info : %s",event->network_data.p_data);
        tuya_ble_net_config_response(result);
        break;
    case TUYA_BLE_CB_EVT_WIFI_SSID:

        break;
    case TUYA_BLE_CB_EVT_TIME_STAMP:
        TUYA_APP_LOG_INFO("received unix timestamp : %s ,time_zone : %d",event->timestamp_data.timestamp_string,event->timestamp_data.time_zone);
        break;
    case TUYA_BLE_CB_EVT_TIME_NORMAL:

        break;
    case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH:
        TUYA_APP_LOG_HEXDUMP_DEBUG("received ble passthrough data :",event->ble_passthrough_data.p_data,event->ble_passthrough_data.data_len);
        tuya_ble_data_passthrough(event->ble_passthrough_data.p_data,event->ble_passthrough_data.data_len);
        break;
    default:
        TUYA_APP_LOG_WARNING("app_tuya_cb_queue msg: unknown event type 0x%04x",event->evt);
        break;
    }
}

void tuya_ble_app_uart_receive_process(uint8_t rx_char)
{
    tuya_ble_common_uart_receive_data(&rx_char, 1);
}



void tuya_ble_app_init(void)
{
    device_param.device_id_len = 16;    //If use the license stored by the SDK,initialized to 0, Otherwise 16 or 20.
    
    if(device_param.device_id_len==16)
    {
        memcpy(device_param.auth_key,(void *)auth_key_test,AUTH_KEY_LEN);
        memcpy(device_param.device_id,(void *)device_id_test,DEVICE_ID_LEN);
        memcpy(device_param.mac_addr.addr,mac_test,6);
        device_param.mac_addr.addr_type = TUYA_BLE_ADDRESS_TYPE_RANDOM;
    }
    device_param.p_type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    device_param.product_id_len = 8;
    memcpy(device_param.product_id,APP_PRODUCT_ID,8);
    device_param.firmware_version = TY_APP_VER_NUM;
    device_param.hardware_version = TY_HARD_VER_NUM;

    tuya_ble_sdk_init(&device_param);
    tuya_ble_callback_queue_register(tuya_cb_handler);
		
		cobra4_uart_set_receive_callback(tuya_ble_app_uart_receive_process);

//    tuya_ota_init();
#if (COBRA_LIGHT_WS2812B_ENABLE == 1)
    app_light_ws2812b_init();
#endif	
    TUYA_APP_LOG_INFO("demo project version : " TUYA_BLE_DEMO_VERSION_STR);
    TUYA_APP_LOG_INFO("app version : " TY_APP_VER_STR);
      
}

#include "app.h"
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "co_bt.h"

extern struct app_env_tag app_env;

uint8_t tuya_ble_adv_data[ADV_DATA_LEN];
uint8_t tuya_ble_adv_length;

uint8_t tuya_ble_rsp_data[SCAN_RSP_DATA_LEN];
uint8_t tuya_ble_rsp_length;

void tuya_ble_build_adv_data(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf)
{
	   memcpy(p_buf, tuya_ble_adv_data+3, tuya_ble_adv_length-3);
	   *p_length = tuya_ble_adv_length-3;
	
}

void tuya_ble_build_scan_rsp_data(uint16_t *p_length, uint8_t *p_buf)
{
	   memcpy(p_buf, tuya_ble_rsp_data, tuya_ble_rsp_length);
	   *p_length = tuya_ble_rsp_length;
}


void update_adv_data(uint8_t const* p_ad_data, uint8_t ad_len)
{
   
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_GAPM, TASK_APP,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;

    memcpy(tuya_ble_adv_data, p_ad_data, ad_len);
		tuya_ble_adv_length = ad_len;
		
    p_cmd->length = ad_len;
		
		memcpy(&p_cmd->data[0], p_ad_data, p_cmd->length);
		
    ke_msg_send(p_cmd);
		
		TUYA_APP_LOG_INFO("update_adv_data.....\n");

}

void update_scan_rsp_data(uint8_t const *p_sr_data, uint8_t sr_len)
{
	   
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_GAPM, TASK_APP,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_SCAN_RSP_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;
		
		p_cmd->length = sr_len;
		
		memcpy(&p_cmd->data[0], p_sr_data, p_cmd->length);
		
    memcpy(tuya_ble_rsp_data, p_sr_data, sr_len);
	  
		tuya_ble_rsp_length = sr_len;
		
    // Send the message
    ke_msg_send(p_cmd);

	  TUYA_APP_LOG_INFO("update_scan_rsp_data.....\n");
}


////////////
#if (COBRA_LIGHT_WS2812B_ENABLE == 1)

void app_light_ws2812b_init(void)
{
	  int i;
	
	  app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_OUTPUT);
	
	  for(i = 0; i < 16; i++)
	  {
			  app_test_array[i] = rand();
		}
}


void app_light_ws2812b_light_data(uint32_t *p_buffer, uint32_t length)
{
	  uint32_t i ;
	  uint32_t temp;
	  for(i = 0; i < length; i++)
	  {
			  app_light_ws2812b_light_display(p_buffer[i]);			  
		}	
}



#endif

