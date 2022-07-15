/**@file            app_blood.h
* @brief            Blood Application entry point
* @details          
* @author      			
* @date             2020/8/14
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/8/14   <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/
// liujian 
//2018/4/9
//Ver 1.0

#ifndef APP_BLOOD_H__
#define APP_BLOOD_H__



#include "rwip_config.h"     // SW configuration

#if (BLE_APP_BLOOD)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition
#include "blps.h"


struct app_blood_env_tag
{
    uint8_t conidx;
	uint16_t  measure_time;
	uint16_t	  timer_enable;
	
	 prf_sfloat systolic;
    /// Diastolic (mmHg/kPa)
   prf_sfloat diastolic;
    /// Mean Arterial Pressure (mmHg/kPa)
    prf_sfloat mean_arterial_pressure;
    /// Pulse Rate
    prf_sfloat pulse_rate;
	
		 /// year time element
    uint16_t year;
    /// month time element
    uint8_t month;
    /// day time element
    uint8_t day;
    /// hour time element
    uint8_t hour;
    /// minute time element
    uint8_t min;
    /// second time element
    uint8_t sec;
	
	
};



extern struct app_blood_env_tag app_blood_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_blood_handlers;


void app_blood_init(void);


void app_blood_add_server(void);

void app_blood_enable_prf(uint8_t);

void app_blood_stop_timer (void);

void app_blood_disable_profile (void);
#endif //(BLE_APP_BLOOD)



#endif // BLE_APP_BLOOD
