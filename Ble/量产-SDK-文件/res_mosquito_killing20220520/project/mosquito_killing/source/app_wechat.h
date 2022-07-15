/**@file            app_wechat.h
* @brief            
* @details          
* @author      			Liujian
* @date             2021-10-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/

#ifndef __APP_WECHAT_H__
#define __APP_WECHAT_H__

#if(APP_WECHAT_ENABLE == 1)

void app_wechat_ble_init(void);
void app_wechat_ble_loop(void);

#endif

#endif
