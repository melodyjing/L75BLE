#ifndef __APP_HOST2_UART_H__
#define __APP_HOST2_UART_H__
#include "rwip_config.h"     // SW configuration

#if (BLE_HOST2_UART)
                
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "device2_uart_task.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"	
#include "co_utils.h"
#include "co_math.h"
#include <string.h>


struct app_host2_uart_env_tag
{
    /// Measurement interval
    uint16_t uart_meas_intv;



    /// Measurement interval timer enable
    bool timer_enable;
	
		uint8_t  rx_data[MAX_UART_SIZE];
	
	  
		uint8_t  receive_data[MAX_RX_UART];
	  uint8_t  receive_length;
	
		uint8_t  connection_index;
};




void app_host2_uart_init(void);


void app_host2_uart_add_uarts(void);



void app_host2_uart_enable_prf_app(uint8_t conidx);

extern const struct app_subtask_handlers app_host2_uart_handlers;


#endif

#endif
