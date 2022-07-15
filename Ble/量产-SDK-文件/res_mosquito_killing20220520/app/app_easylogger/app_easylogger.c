/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_easylogger.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/2

	Description :  ��¼
	Changed Log	:

		Liujian  2020/11/2		- Creation
*****************************************************************************/

#include "app_easylogger.h"
#include <elog.h>

#if (COBRA_LOGGER_ENABLE == 1)

void app_easylogger_init(void)
{
	  elog_init();
    /* set EasyLogger log format */
//    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
//    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
//    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
//    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
//    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
//    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    /* start EasyLogger */
    elog_start();
	
}

///**
// * EasyLogger demo
// */
//void test_elog(void) {
//    /* test log output for all level */
//    log_a("Hello EasyLogger!");
//    log_e("Hello EasyLogger!");
//    log_w("Hello EasyLogger!");
//    log_i("Hello EasyLogger!");
//    log_d("Hello EasyLogger!");
//    log_v("Hello EasyLogger!");
////    elog_raw("Hello EasyLogger!");
//}

#endif
