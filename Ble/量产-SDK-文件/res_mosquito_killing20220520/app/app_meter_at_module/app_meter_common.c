/******************************************************************************
	File Name	:	app_meter_common.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	    2021/03/15

	Description : 


	Changed Log	:
		Liujian  2021/03/15		- Creation

*****************************************************************************/
#include <string.h>

static char meter_adv_name[5];

void app_meter_set_adv_name( const char * adv_name)
{
	  strncpy(meter_adv_name, adv_name, 4);	
}

char * app_meter_get_adv_name(void)
{
	  return meter_adv_name;
}
