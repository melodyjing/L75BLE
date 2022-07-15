/******************************************************************************
	File Name	:	  app_report_adv.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/8/18

	Description :   
	   处理自动接收广播包, 然后上报到管理芯

	Changed Log	:
	
		Liujian  2020/8/18		- Creation

*****************************************************************************/
#ifndef __MODULE_REPORT_ADV_H__
#define __MODULE_REPORT_ADV_H__
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include <stdint.h>
#include "rwip_config.h"
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API

typedef struct {
		uint8_t  length;
	  uint8_t  mac_address[6];
	  int16_t  rssi;
	  uint8_t  adv_data[31];
}ADV_STRUCT;


void app_adv_report_init(void);

void app_adv_report_process(struct gapm_ext_adv_report_ind* p_param);

void app_adv_report_stop(void);

void app_adv_report_begin(void);

void app_adv_report_to_module(void);

void app_adv_reset_database(void);

#endif

#endif

