/**
*
* author: yaobohua
* date: 2014/8/26
* function: ΢������BLE����ͨ�ÿ��
*	modified  by anqiren  2014/12/02  V1.0bat
*
**/
#include "comsource.h"
#include "crc32.h"
#include <string.h>

extern ble_wechat_t m_ble_wechat;
extern data_handler *m_mpbledemo2_handler;
          

int main(void) {
	resource_init();
	advertising_start();
	for(;;) 
	{
		wait_app_event();
		m_mpbledemo2_handler->m_data_main_process_func(&m_ble_wechat);
	}
}
