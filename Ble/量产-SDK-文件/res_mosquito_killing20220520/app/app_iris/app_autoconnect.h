/******************************************************************************
	File Name	:	app_autoconnect.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/29

	Description :   

	Changed Log	:
		Liujian  2020/2/29			- Creation

*****************************************************************************/
#ifndef __APP_AUTOCONNECT_H__
#define __APP_AUTOCONNECT_H__
#include "cobra_system.h"
#include "app_network.h"
#include "rwip_config.h"
#include "rdw_queue.h"
#include "rdw_console.h"
#include "rdw_command.h"
#include "ke_event.h"
#include "device1_scan.h"
#include "device1_scan_task.h"
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"
#include "app_host_uart.h"
#include "app_host2_uart.h"

#define CONNECT_CHANNEL1  1
#define CONNECT_CHANNEL2  2
#define CONNECT_CHANNEL3  3

#define CONNECT_OK		1
#define CONNECT_FAIL  0


#define AUTO_CONNECT_STOP_STATE			0
#define AUTO_CONNECT_START_STATE  	1
#define AUTO_CONNECT_TRY_STATE			2

typedef struct {
	
   struct gap_bdaddr  mac_adress;
	 unsigned short     character_code;
	
}__attribute__((packed)) ADV_RESULT_INFORMATION ;

void app_iris_autoconnect_process(unsigned short length, unsigned char * p_buffer);

void app_iris_process_adv_data(struct gapm_ext_adv_report_ind* p_param);

void app_iris_attconnect_report_try_state(unsigned int index, unsigned int state);

#if DUANLUQI_ENABLE
unsigned int app_iris_duanluqi_compare_modified_current_command(unsigned char *p_buffer, unsigned char length);

void app_iris_duanluqi_respond_modified_current_command(unsigned int connection_index);

#endif
unsigned int  app_iris_is_modified_value(uint8_t length,  uint8_t *p_buffer);
void app_iris_autoconnect_process_sample_data(unsigned short length, unsigned char *p_buffer);

//判断电表是否处于自动连接蓝牙模块状态
unsigned int app_iris_auto_connect_state(void);

void app_iris_autoconnect_get_adv_record(void);

unsigned int app_iris_autoconnect_get_report_flag(void);

unsigned int app_iris_get_scan_mode(void);

void app_iris_recode_adv_data(struct gapm_ext_adv_report_ind* p_param);
#endif
