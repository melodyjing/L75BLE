/******************************************************************************
	File Name	:	device3_connection.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/6

	Description :   
          上海 建立
	Changed Log	:
		Liujian  2019/12/6			- Creation

*****************************************************************************/
#ifndef __DEVICE3_CONNECTION_H__
#define __DEVICE3_CONNECTION_H__

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
#include "def_self.h"
#include "device1_client_uart_task.h"

enum app_device3_connetion_state
{
    /// scan activity does not exists
    APP_DEVICE3_CONNECTION_STATE_IDLE = 0,
    /// Creating connection activity
    APP_DEVICE3_CONNECTION_STATE_CREATING,
	
		//
		APP_DEVICE3_CONNECTION_STATE_CREATED,
    

};


/// connection environment definition
struct connection_device3_env_tag
{
    ///connection window interval (in slots)
    UINT16                 interval;
    /// Status of the connection
    UINT8                  connection_state;
    /// connection activity identifier
    UINT8                  act_id;
 
		/// Connection Index
    UINT8  								conidx;
		/// Connection handle
    UINT16 								conhdl;

//2020/2/14 add liujian

		UINT8  receive_data[COBRA4_MTU];
	
		UINT8  receive_index;    //要拷贝的数据的地址
	
		UINT8  receive_begin;
	
		UINT8  receive_end;
	
	  UINT8  receive_length;
	
	//	UINT8  connection_index;
	  ke_task_id_t  src_id;
			
		UINT16  need_data_number;  //需要的 698 数据个数
		
		UINT8   task_ok;
		
} ;



void app_device3_connection_init(void);

void app_device3_connection_device_config(void);


void app_device3_connection_create(void);

void app_device3_connection_start(struct gap_bdaddr peer_addr);

//发现对方设备的 服务
void app_device3_connection_find_service(ke_task_id_t dest_id);

void app_device3_connection_add_profile(void);
void app_device3_reconnect(void);

//发送数据到  server 串口
void app_device3_send_data_to_uart_server(UCHAR length, UCHAR * p_buffer);

void app_device3_enable_uart_report(ke_task_id_t  dest_id);

void app_device3_exchange_mtu(void);

UINT app_device3_is_connected(void);

void app_device3_disconnect(void);

void app_device3_delete_connection(void);

void app_device3_always_connection(void);

void app_connetion_device3_rx_data(struct uart_client_rx_data_ind  const *param);

void app_device3_set_task_ready(void);

UINT app_device3_task_is_ready(void);

void app_device3_bond_command(void);

#endif

