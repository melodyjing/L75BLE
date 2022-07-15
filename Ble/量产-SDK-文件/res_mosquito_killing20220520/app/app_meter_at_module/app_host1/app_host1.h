#ifndef __AAPP_HOST1_H__
#define __AAPP_HOST1_H__
#include "rwip_config.h"     // SW configuration

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
                
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
           
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"	
#include "co_utils.h"
#include "co_math.h"
#include <string.h>

void app_host1_uart_init(void);

void app_host1_uart_add_uarts(void);


void app_host1_uart_enable_prf_app(uint8_t conidx);

extern const struct app_subtask_handlers app_host1_uart_handlers;

void app_meter_send_data_to_master(uint8_t length, uint8_t *p_buffer);

#endif

#endif
