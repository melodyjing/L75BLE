/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_standby_program.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/23

	Description :   
	
	Changed Log	:

		Liujian  2019/5/23		- Creation



*****************************************************************************/


#ifndef __COBRA_STANDBY_PROGRAM_H__
#define __COBRA_STANDBY_PROGRAM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if APP_COBRA_STANDBY_EN 


void copy_standby_program_to_vim_ram(void);
void cobra_enter_vim_run(void);
#endif
#endif


