/******************************************************************************
	File Name	:	slave_device1_connection.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/25

	Description :   

	Changed Log	:
		Liujian  2019/11/25			- Creation

*****************************************************************************/
#ifndef __SLAVE_DEVICE1_CONNECTION_H__
#define __SLAVE_DEVICE1_CONNECTION_H__

#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "ke_task.h"        // Kernel Task
#include "rwip_config.h"             // SW configuration
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "arch.h"                 // Platform Definitions
#include <string.h>
#include "co_utils.h"
#include "ke_timer.h"             // Kernel timer
#include "gattc_task.h"
#include "prf.h"
#include "rdw_mcu.h"
#include "master_com_task.h"

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

enum app_device_connetion_state
{
    /// scan activity does not exists
    APP_DEVICE_CONNECTION_STATE_IDLE = 0,
    /// Creating connection activity
    APP_DEVICE_CONNECTION_STATE_CREATING,
	
		//
		APP_DEVICE_CONNECTION_STATE_CREATED,
    

};


/// connection environment definition
struct connection_device_env_tag
{
    ///connection window interval (in slots)
    uint16_t                 interval;
    /// Status of the connection
    uint8_t                  connection_state;
    /// connection activity identifier
    uint8_t                  act_id;
 
		/// Connection Index
    uint8_t  								conidx;
		/// Connection handle
    uint16_t 								conhdl;
		
//2020/2/14 add liujian

		uint8_t  receive_data[COBRA4_MTU];
	
		uint8_t  receive_index;    //要拷贝的数据的地址
	
		uint8_t  receive_begin;
	
		uint8_t  receive_end;
	
	  uint8_t  receive_length;
	
		ke_task_id_t  src_id;
	
		uint16_t  need_data_number;  //需要的 698 数据个数
		
		uint32_t  connection_pin_code;
		
		uint8_t   connection_pair_flag;
		
		uint8_t   task_ok;
} ;



void app_slave_device1_connection_init(void);

void app_slave_device1_connection_device_config(void);


void app_slave_device1_connection_create(void);

void app_slave_device1_connection_start(struct gap_bdaddr peer_addr);

//发现对方设备的 服务
void app_slave_device1_connection_find_service(ke_task_id_t dest_id);

void app_slave_device1_connection_add_profile(void);

void app_slave_device_reconnect(void);

void app_duanluqi_test_led_change(void);

//发送数据到  server 串口
void app_slave_device1_send_data_to_uart_server(unsigned char length, unsigned char *p_buffer);


void app_slave_device1_enable_uart_report(ke_task_id_t  dest_id);

unsigned int app_slave_device1_is_connected(void);

void app_slave_device1_disconnect(void);

void app_slave_device1_exchange_mtu(void);

void app_slave_device1_reconnect(void);

void app_slave_device1_bond_command();

void app_slave_device1_set_pincode(unsigned int code);

unsigned int app_slave_device1_get_pincode(void);

void app_slave_device1_set_pair_flag(unsigned char flag);

unsigned int app_slave_device1_get_pair_flag(void);

void app_slave_device1_delete_connection(void);

void app_slave_device1_always_connection(void);

void app_slave_connetion_device1_rx_data(struct uart_client_rx_data_ind  const *param);

void app_slave_device1_set_task_ready(void);

unsigned int app_slave_device1_task_is_ready(void);

#endif

#endif

