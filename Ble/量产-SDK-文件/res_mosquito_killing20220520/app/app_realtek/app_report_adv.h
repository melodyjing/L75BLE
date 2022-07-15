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
#ifndef __APP_REPORT_ADV_H__
#define __APP_REPORT_ADV_H__
#include <stdint.h>
#include "rwip_config.h"
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API

#include "def_self.h"
typedef struct {
		UINT8  length;
	  UINT8  mac_address[6];
	  UINT16  rssi;
	  UINT8  adv_data[31];
}ADV_STRUCT;

#define MAX_ADV_NUMBER   10

void app_adv_report_init(void);

void app_adv_report_process(struct gapm_ext_adv_report_ind* p_param);

void app_adv_report_stop(void);

void app_adv_report_begin(void);

void app_adv_report_to_mcu(void);

void app_adv_reset_database(void);
#endif

