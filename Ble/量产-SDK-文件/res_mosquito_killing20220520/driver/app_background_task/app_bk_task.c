/*
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
//liujian
//2019/5/8 add for background task
*/
#include "rdw_mcu.h"
#include "cobra_config.h"
#include "app_bk_task.h"
#include "rdw_dma.h"
#include "cobra_udma.h"
#include "cobra_pcrm.h"
#include "rwip_config.h"  
#include "ke_event.h"
#include "cobra_irda.h"
#if COBRA_TRIM_EN
#include "app_system_trim.h"
#endif

#if(CFG_NRF24L01_DEMO)
#include "app_nrf24l01.h"
#endif

#if APP_SCAN_DEVICE_ENABLE
#include "device1_scan.h"
#endif
#include "cobra_system.h"
#if (APP_UART_TASK_ENABLE)
#include "rdw_uart_tx.h"
#endif

#if (APP_OTA_MATSER_ENABLE == 1)
#include "ota_shell.h"
#endif


#if (BLE_APP_MESH)
#if (BLE_APP_MESH_TIANMAO_LIGHT == 1)
#include "app_mesh_task.h"
#endif

#if (BLE_APP_MESH_CLIENT_LIGHT == 1)
#include "app_mesh_client_light.h"
#endif

#if (BLE_APP_MESH_SERVER_LIGHT == 1)
#include "app_mesh_server_light.h"
#endif

#if (BLE_APP_MESH_CLIENT_GATEWAY == 1)
#include "app_mesh_client_gateway.h"
#endif

#if (BLE_APP_MESH_SERVER_SWITCH == 1)
#include "app_mesh_server_switch.h"
#endif

#if (BLE_APP_MESH_TULIPE_LIGHT == 1)
#include "app_tulip_light.h"
#include "app_mesh_util.h"
#endif
#if (APP_MESH_CWHSL_LIGHT_FUNTION == 1)
#include "app_mesh_cwhsv_util.h"
#endif
#endif

#if (APP_ADC_SAMPLE_ENABLE == 1)
#include "app_adc_sample.h"
#endif

#if (APP_MOUSE_ENABLE == 1)
#include "app_mouse.h"
#endif


#if (APP_TULIP_BATTER_DETECT_VERSION2 == 1)
#include "app_tulip_battery.h"
#endif

typedef enum {
		TASK_NULL,
		TASK_IRDA,
		TASK_TRIM,
	  TASK_NRF24L01,     //nRF24L01 收到中断
	  TASK_REBOOT,
	  TASK_MESH_NVDS_SAVE,
#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
	  TASK_RESET_REPORT,
#endif
#if (APP_OTA_MATSER_ENABLE == 1)
    TASK_OTA_SHELL,
#endif
	
#if (BLE_APP_MESH == 1)
	  TASK_WRITE_MESH_FLAG,
	  TASK_ERASE_MESH_FLAG,
#endif	
	
#if (APP_ADC_SAMPLE_ENABLE == 1||APP_ADC_SAMPLE_COMMON_ENABLE == 1)
	  TASK_ADC_SAMPLE_FLAG,
#endif	

#if (APP_MOUSE_ENABLE == 1)
	  TASK_MOUSE_ADC_SAMPLE_FLAG,
#endif

#if (APP_TULIP_BATTER_DETECT_VERSION2 == 1)
   TASK_TULIP_POWER_DOWN_KEY_FLAG,        //电源开关 按下的信息
#endif

}COBRA_TASK_NUM;

#if (COBRA_POWER_DOWN_ENABLE)
static COBRA_TASK_NUM task_number		__attribute__((section("retention_area"),zero_init)); 
#else
static COBRA_TASK_NUM task_number;
#endif


static void cobra_background_task_callback(void)
{
	
    ke_event_clear(KE_EVENT_BACKGROUND);
		switch(task_number)
		{
#if COBRA_IRDA_EN			
			case TASK_IRDA:
			{
				cobra_app_irda_task_loop();
			}
			break;
#endif	
			
#if COBRA_TRIM_EN				
			case TASK_TRIM:
			{
			   cobra_app_trim_task_loop();
			}
			break;
#endif	
			
#if CFG_NRF24L01_DEMO			
			case 	TASK_NRF24L01:
      {
			  cobra_app_nrf24l01_task_loop();	
			}
			break;
#endif					
			
			case TASK_REBOOT:
			{
			//	app_device1_watchdog_reboot();	
        platform_reset(0);				
				break;
			}
#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
			case TASK_RESET_REPORT:
			{
			  report_up_reset();
				break;
			}

#endif		
	    case TASK_MESH_NVDS_SAVE:
	   {
#if ((BLE_APP_MESH) && (BLE_APP_MESH_TULIPE_LIGHT | BLE_APP_MESH_TIANMAO_LIGHT | BLE_APP_MESH_CLIENT_LIGHT | BLE_APP_MESH_SERVER_LIGHT | BLE_APP_MESH_CLIENT_GATEWAY | BLE_APP_MESH_SERVER_SWITCH))
       app_mesh_save_information_event();		
#endif		
		   break;
	   }
#if (APP_OTA_MATSER_ENABLE == 1)
	 case TASK_OTA_SHELL:
	 {
      app_ota_shell_task_loop();	
      break;		 
	 }
#endif	
	 
#if (BLE_APP_MESH_TULIPE_LIGHT == 1 || APP_MESH_CWHSL_LIGHT_FUNTION == 1)
	 case TASK_WRITE_MESH_FLAG:
	 {
      app_mesh_util_write_flag();	
      break;		 
	 } 
	 case TASK_ERASE_MESH_FLAG:
	 {
      app_mesh_util_erase_flag();	
      break;		 
	 }
#endif

#if (APP_ADC_SAMPLE_ENABLE == 1||APP_ADC_SAMPLE_COMMON_ENABLE == 1)
	 case TASK_ADC_SAMPLE_FLAG:
	 {
		   app_adc_sample_task();
		   break;
	 }
#endif

#if (APP_MOUSE_ENABLE  == 1)
	 case TASK_MOUSE_ADC_SAMPLE_FLAG:
	 {
		   app_mouse_adc_sample_task();
		   break;
	 }
#endif
	 
#if (APP_TULIP_BATTER_DETECT_VERSION2 == 1)
   case TASK_TULIP_POWER_DOWN_KEY_FLAG:
	 {
		   app_tulip_power_down_save_task();
		   break;
	 }
#endif	 
			default:
				break;
			
		}
}


void cobra_app_background_task_init(void)
{
    ke_event_callback_set(KE_EVENT_BACKGROUND, &cobra_background_task_callback);
}
#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
void cobra_app_set_reset_report_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
		task_number = TASK_RESET_REPORT;
}
#endif		


void cobra_app_set_irda_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
		task_number = TASK_IRDA;
}

void cobra_app_set_trim_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
		task_number = TASK_TRIM;
}

void cobra_app_set_nrf24l01_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
		task_number = TASK_NRF24L01;
}

void cobra_app_reboot_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
		task_number = TASK_REBOOT;
}

void cobra_app_set_mesh_nvds_task(void)
{
	ke_event_set(KE_EVENT_BACKGROUND);
	task_number = TASK_MESH_NVDS_SAVE;
}

#if (APP_OTA_MATSER_ENABLE == 1)
void cobra_app_set_ota_shell_task(void)
{
		ke_event_set(KE_EVENT_BACKGROUND);
	  task_number = TASK_OTA_SHELL;
}
#endif

#if (BLE_APP_MESH == 1)

void cobra_app_set_mesh_write_flag_task(void)
{
	ke_event_set(KE_EVENT_BACKGROUND);
	task_number = TASK_WRITE_MESH_FLAG;
}

void cobra_app_set_mesh_erase_flag_task(void)
{
	ke_event_set(KE_EVENT_BACKGROUND);
	task_number = TASK_ERASE_MESH_FLAG;
}


#endif

#if (APP_ADC_SAMPLE_ENABLE == 1||APP_ADC_SAMPLE_COMMON_ENABLE == 1)
void cobra_app_set_sample_adc_flag_task(void)
{
	  ke_event_set(KE_EVENT_BACKGROUND);
	  task_number = TASK_ADC_SAMPLE_FLAG;
}
#endif

#if (APP_MOUSE_ENABLE  == 1)
void cobra_app_set_mouse_sample_adc_flag_task(void)
{
	  ke_event_set(KE_EVENT_BACKGROUND);
	  task_number = TASK_MOUSE_ADC_SAMPLE_FLAG;
}
#endif


#if (APP_TULIP_BATTER_DETECT_VERSION2 == 1)
void cobra_app_set_tulip_power_down_flag_task(void)
{
	  ke_event_set(KE_EVENT_BACKGROUND);
	  task_number = TASK_TULIP_POWER_DOWN_KEY_FLAG;
}

#endif



