/******************************************************************************
	File Name	:	  master_scan_task.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/08

	Description : 
      蓝牙模组, 可以通过 AT COMMAND 来控制


	Changed Log	:
	
		Liujian  2021/03/08	 - Creation

*****************************************************************************/
#ifndef __MASTER_SCAN_TASK_H__
#define __MASTER_SCAN_TASK_H__
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))


extern const struct ke_task_desc TASK_DESC_MASTER_SCAN;


#endif

#endif
