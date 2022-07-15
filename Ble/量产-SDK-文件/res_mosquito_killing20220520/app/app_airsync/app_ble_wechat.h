/**@file            app_ble_wechat.c
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/

#ifndef __APP_BLE_WECHAT_H__
#define __APP_BLE_WECHAT_H__

struct app_wechat_env_tag
{
    unsigned char ind_enable;
};


/// Database Feature Configuration Flags
enum uart_features
{
    /// All Features supported
    WECHAT_ALL_FEAT_SUP = 0xFF,
};

void app_wechat_add_service(void);
void app_wechat_init(void);
void app_wechat_disconnection(void);
void app_wechat_send_data(unsigned char *p_buffer, unsigned char length);

extern const struct app_subtask_handlers app_wechat_handlers;

#endif
