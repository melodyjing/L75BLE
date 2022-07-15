/**@file            app_heart_rate.h
* @brief            Heart Rate Application entry point
* @details          
* @author      			
* @date             2019/6/23
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/6/23  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __APP_HEART_RATE_H__
#define __APP_HEART_RATE_H__

#if (BLE_APP_HEART_RATE)
#include "rwip_config.h"     // SW configuration
#include "hrp_common.h"

struct app_hrpt_env_tag
{
   uint16_t hrp_meas_intv;
	
   uint8_t  body_sensor_loc;

   uint16_t heart_rate;
	
    /// Energy Expended
   uint16_t energy_expended;

   uint16_t cfg_val;

   uint8_t  timer_enable;
	
};

/// Table of message handlers
extern const struct app_subtask_handlers app_heart_rate_handlers;


void app_heart_rate_init(void);

void app_heart_rate_add_hprs(void);

void app_heart_rate_enable_prf(uint8_t conidx);

void app_heart_rate_stop_timer(void);

#endif

#endif
