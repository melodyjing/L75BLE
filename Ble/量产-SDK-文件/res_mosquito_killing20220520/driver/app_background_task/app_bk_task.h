//liujian add 
//2019/5/8

#ifndef __APP_BK_TASK_H__
#define __APP_BK_TASK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

void cobra_app_background_task_init(void);
	
void cobra_app_set_irda_task(void);
	
void cobra_app_set_trim_task(void);	
	
void cobra_app_set_nrf24l01_task(void);

void cobra_app_reboot_task(void);	
	
void cobra_app_set_mesh_nvds_task(void);
	
void cobra_app_set_ota_shell_task(void);

#if (BLE_APP_MESH == 1)

void cobra_app_set_mesh_write_flag_task(void);

void cobra_app_set_mesh_erase_flag_task(void);	

#endif

#if (APP_ADC_SAMPLE_ENABLE == 1||APP_ADC_SAMPLE_COMMON_ENABLE == 1)
void cobra_app_set_sample_adc_flag_task(void);
#endif

#if (APP_MOUSE_ENABLE == 1)
void cobra_app_set_mouse_sample_adc_flag_task(void);
#endif

#if (APP_TULIP_BATTER_DETECT_VERSION2 == 1)
void cobra_app_set_tulip_power_down_flag_task(void);
#endif

#ifdef __cplusplus
}
#endif

#endif 
