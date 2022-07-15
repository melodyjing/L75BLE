#ifndef __APP_HOST2_UART_H__
#define __APP_HOST2_UART_H__
#include "rwip_config.h"     // SW configuration

#if (BLE_HOST2_UART)
                
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

#include "def_self.h"
//struct app_host2_uart_env_tag
//{
//    /// Measurement interval
//    uint16_t uart_meas_intv;



//    /// Measurement interval timer enable
//    bool timer_enable;
//	
//		UINT8  rx_data[MAX_UART_SIZE];
//	
//	  
//		UINT8  receive_data[MAX_RX_UART];
//	  UINT8  receive_length;
//	
//		UINT8  connection_index;
//};




void app_host2_uart_init(void);


void app_host2_uart_add_uarts(void);



void app_host2_uart_enable_prf_app(UINT8 conidx);

extern CONST struct app_subtask_handlers app_host2_uart_handlers;

void app_meter_send_data_to_collection1(UCHAR length, UCHAR *p_buffer);
#endif

#endif
