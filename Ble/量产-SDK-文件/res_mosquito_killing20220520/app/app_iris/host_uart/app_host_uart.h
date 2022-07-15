#ifndef __APP_HOST_UART_H__
#define __APP_HOST_UART_H__
#include "rwip_config.h"     // SW configuration

                
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "device1_uart_task.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"	
#include "co_utils.h"
#include "co_math.h"
#include <string.h>
#include "app_iris_console.h"




#if (BLE_HOST_UART)

void app_host_uart_init(void);


void app_host_uart_add_uarts(void);



void app_host_uart_enable_prf_app(uint8_t conidx);

extern const struct app_subtask_handlers app_host_uart_handlers;

void app_meter_send_data_to_collection2(unsigned char length, unsigned char *p_buffer);
#endif

#endif
