/******************************************************************************
	File Name	:	  app_sec2.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/12/07

	Description :   
	          Ϊ��֧�ֵڶ������豸Ҳ�������ɵȼ��İ�ȫ����  SMP 

	Changed Log	:
		Liujian     2020/12/07			- Creation

*****************************************************************************/
#ifndef __APP_SEC2_H__
#define __APP_SEC2_H__
#include "rwip_config.h"

#if (DIANBIAO_SPM_ENABLE == 1)

#include <stdint.h>          // Standard Integer Definition

struct app_sec_env2_tag
{
    bool bonded;
};

void app_sec2_send_security_req(uint8_t conidx);

extern const struct app_subtask_handlers app_sec2_handlers;

bool app_sec2_get_bond_status(void);

#endif

#endif
