/******************************************************************************
	File Name	:	app_iris_console.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/19

	Description :   

	Changed Log	:
		Liujian  2019/12/19		- Creation

*****************************************************************************/
#ifndef __APP_IRIS_CONSOLE_H__
#define __APP_IRIS_CONSOLE_H__
#include "cobra_system.h"
#include "app_network.h"
#include "rwip_config.h"
#include "rdw_queue.h"
#include "rdw_console.h"
#include "rdw_command.h"
#include "ke_event.h"
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"
#include "app_host2_uart.h"
#include "app_host_uart.h"
#include "device1_scan.h"
#include "host_advertiser1.h"
#include "app_autoconnect.h"
#include "app_report_adv.h"

#if(DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1)
typedef struct uart_cmd_frame{
		unsigned char length_l;
		unsigned char length_h;
	
		unsigned char mac_address[6];
		unsigned char sta;
		unsigned char reserve;
		unsigned char control_cmd;  //?? ?
		unsigned char user_data[1024];
		unsigned short checksum;
		unsigned short user_length;
}UART_CMD_FRAME;


enum UART_COMMAND_STATE
{
		UART_CMD_IDLE,
		UART_CMD_LOOK_PREMBLE,
		UART_CMD_LOOK_LENGTH_L,
		UART_CMD_LOOK_LENGTH_H,
		UART_CMD_LOOK_ADDRESS,
		UART_CMD_LOOK_STA1,
		UART_CMD_LOOK_RESERVE,
		UART_CMD_LOOK_SECOND_BEGIN,
		UART_CMD_LOOK_CONTROL_CMD,
		UART_CMD_LOOK_USER_DATA,
		UART_CMD_LOOK_CHECKSUM,
		UART_CMD_LOOK_END,
	
};

void iris_dianbiao_slave_number_rsp(void);

void iris_dianbiao_master_number_rsp(void);

void iris_uart_task_callback(void);

void app_iris_up_collection_data(unsigned char connection_index, unsigned char length, unsigned char *p_buffer);


void iris_calibration_loop(void);

void app_iris_dianbiao_uart_output(uint8_t index, uint8_t length,  uint8_t *p_buffer);

void  iris_dianbiao_console_init(void);

unsigned char * iris_uart_get_address(void);

unsigned char * iris_dianbiao_get_taiti_address(void);

//报告蓝牙上设置的从机信息
void iris_dianbiao_slave_information_rsp(void);


unsigned char * app_get_meter_master_pincode(void);

#endif

#endif
