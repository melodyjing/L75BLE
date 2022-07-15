/******************************************************************************
	File Name	:	  module_report_adv.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/8/18

	Description :   
	   处理自动接收广播包

	Changed Log	:
	
		Liujian  2020/8/18		- Creation

*****************************************************************************/
#include "module_report_adv.h"
#include <string.h>
#include "cobra_uart.h"
#include "master_scan.h"
#if (APP_METER_AS_SLAVE_ENABLE == 0)
#if (COBRA_LOGGER_ENABLE == 1)
#include <elog.h>
#endif

#define MAX_ADV_NUMBER   10

#if (APP_METER_AT_COMMAND == 1)

static uint8_t  adv_receive_flag;   // 0 停止接收蓝牙广播, 1 开始接收蓝牙广播
static uint8_t  adv_save_index;
static ADV_STRUCT adv_database[MAX_ADV_NUMBER];

static unsigned int adv_number;
void app_adv_report_init(void)
{
	  adv_receive_flag = 0;
	  adv_number = 0;
	  adv_save_index = 0;
		memset(adv_database, 0, 10*sizeof(ADV_STRUCT));
}

/*
 开始接收广播信号， 把接收到的数据放到数据库中
*/
void app_adv_report_begin(void)
{
		memset(adv_database, 0, 10*sizeof(ADV_STRUCT));
	  adv_receive_flag = 1;
	  adv_number = 0;
	  adv_save_index = 0;
}

/*
 停止处理接收到的广播信号
*/
void app_adv_report_stop(void)
{
	  adv_receive_flag = 0;
}

void app_adv_reset_database(void)
{
	  adv_number = 0;
	  adv_save_index = 0;
	  memset(adv_database, 0, 10*sizeof(ADV_STRUCT));
}

/*
   接收到的扫描信号， 处理那些信号
*/
void app_adv_report_process(struct gapm_ext_adv_report_ind* p_param)
{
	  uint8_t  i;
	  uint8_t  find_flag = 0;
	  uint8_t  length;
	  uint8_t  filter_ok = 0;
	
//	  if(adv_receive_flag == 0) return;    //不需要处理广播信号
	
//	  if(p_param->trans_addr.addr.addr[5] == 0xC4) 
//		{			
//	      printf("RSSI = %d  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", p_param->rssi , \
//	                p_param->trans_addr.addr.addr[0], p_param->trans_addr.addr.addr[1], \
//	                p_param->trans_addr.addr.addr[2], p_param->trans_addr.addr.addr[3], \
//	                p_param->trans_addr.addr.addr[4], p_param->trans_addr.addr.addr[5] );
//	  }	
		
	  if(p_param->rssi > -75)
		{	
				find_flag = 0;
			  
				for(i = 0; i < adv_number; i++)
				{
						if(!memcmp((void *)&adv_database[i].mac_address, (void *)&(p_param->trans_addr.addr), 6))
						{
								find_flag = 1;   //列表中已经存在符合条件的断路器的地址了
								break;
						}
				}
				if(!find_flag)
				{
						if(adv_number < MAX_ADV_NUMBER)
						{
								memcpy((void *)&adv_database[adv_number].mac_address, (void *)&(p_param->trans_addr.addr), 6);
								length = (p_param->length > 31) ? 31:p_param->length;									
								memcpy((void *)&adv_database[adv_number].adv_data, p_param->data, length);
								adv_database[adv_number].length = length;
							  adv_database[adv_number].rssi = p_param->rssi;
								adv_number++;
						}
						else
						{
							  memcpy((void *)&adv_database[adv_save_index].mac_address, (void *)&(p_param->trans_addr.addr), 6);
								length = (p_param->length > 31) ? 31:p_param->length;									
								memcpy((void *)&adv_database[adv_save_index].adv_data, p_param->data, length);
								adv_database[adv_save_index].length = length;
							  adv_database[adv_save_index].rssi = p_param->rssi;
							  adv_save_index++;
							  if(adv_save_index >= MAX_ADV_NUMBER)
									adv_save_index = 0;
						}
				}
				else
				{
						    memcpy((void *)&adv_database[i].mac_address, (void *)&(p_param->trans_addr), sizeof(struct gap_bdaddr));
								length = (p_param->length > 31) ? 31:p_param->length;									
								memcpy((void *)&adv_database[i].adv_data, p_param->data, length);
								adv_database[i].length = length;
				}
		}
}

/*
在广播的数据中寻找 名称
*/
void app_adv_find_name(uint8_t *p_data, uint8_t length)
{
       uint8_t i;
	
	     uint8_t *p_cursor = p_data;
	
	     uint8_t *p_end_cursor = p_data + length;
	
	     while (p_cursor < p_end_cursor)
       {
         // Extract AD type
          uint8_t ad_type = *(p_cursor + 1);
				 
			    if(ad_type == GAP_AD_TYPE_COMPLETE_NAME)
					{
#if (COBRA_LOGGER_ENABLE == 1)
						  for(i = 0; i <  (*p_cursor - 1); i++)
              elog_raw("%c", *(p_cursor + 2 + i));
#endif						
						  break;
					}
					
				  p_cursor += (*p_cursor + 1);
		  }
	
}

/*
   读取广播内容命令
*/
void app_adv_report_to_module(void)
{
	    unsigned short length = 4;
		  unsigned short checksum = 0;
	    unsigned char  i, j;
	
	
#if (COBRA_LOGGER_ENABLE == 1)
	   elog_raw("+SCAN{\n");	
     for(i = 0; i < adv_number; i++)
     {
				  elog_raw("%02x%02x%02x%02x%02x%02x,", adv_database[i].mac_address[0], \
			                                      adv_database[i].mac_address[1], \
			                                      adv_database[i].mac_address[2], \
			                                      adv_database[i].mac_address[3], \
			                                      adv_database[i].mac_address[4], \
			                                      adv_database[i].mac_address[5]); 
			 
			    app_adv_find_name(adv_database[i].adv_data, adv_database[i].length);
			    elog_raw("\n");
     }
     elog_raw("+SCAN}\n");		 
#endif

			
		
	
}
#endif

#endif



