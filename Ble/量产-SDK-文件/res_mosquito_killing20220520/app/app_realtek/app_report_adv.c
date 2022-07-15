/************************************************************************
  Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
  Project Name:      
  File name:   
Description: 	   处理自动接收广播包, 然后上报到管理芯

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/
#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "def_self.h"
#include "app_report_adv.h"
#include <string.h>
#include "cobra_uart.h"
#include "device1_scan.h"

#define MAX_ADV_NUMBER   10

LOCAL UINT8  adv_receive_flag;   // 0 停止接收蓝牙广播, 1 开始接收蓝牙广播
LOCAL UINT8  adv_save_index;
LOCAL ADV_STRUCT adv_database[MAX_ADV_NUMBER];
LOCAL UINT adv_number;

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
	  UINT8  i;
	  UINT8  find_flag = 0;
	  UINT8  length;
	  UINT8  filter_ok = 0;
	  ADV_FILTER_TYPE  adv_filter;
	
	  if(adv_receive_flag == 0) return;    //不需要处理广播信号
	
//	  if(p_param->trans_addr.addr.addr[5] == 0xC4) 
//		{			
//	      printf("RSSI = %d  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", p_param->rssi , \
//	                p_param->trans_addr.addr.addr[0], p_param->trans_addr.addr.addr[1], \
//	                p_param->trans_addr.addr.addr[2], p_param->trans_addr.addr.addr[3], \
//	                p_param->trans_addr.addr.addr[4], p_param->trans_addr.addr.addr[5] );
//	  }	
//		
	  adv_filter = app_adv_get_filter_kind();
	  filter_ok = 0;
	  switch(adv_filter)
	  {
		  case FILTER_NULL:
			  filter_ok = 1;
			  break;
		  case FILTER_BY_MAC:
		  {
			 for(i = 0; i < app_adv_get_filter_number(); i++)
             {
                  if(!memcmp(p_param->trans_addr.addr.addr, app_adv_get_filter_mac_address(i), 6))
				  {
					   filter_ok = 1;
					   break;
				  }
             }				 
		  }
		  break;
		  case FILTER_BY_KIND:
			  for(i = 0; i < app_adv_get_filter_number(); i++)
              {
                  if(p_param->trans_addr.addr.addr[5] == app_adv_get_filter_mac_kind(i))
				  {
					   filter_ok = 1;
					   break;
				  }
              }						 
			  break;
		  default:
			  filter_ok = 1;
			  break;
	  }
	  
	  if(filter_ok == 0) return;

		
	    if(((p_param->rssi > -75)&&(adv_filter == FILTER_NULL)) || (adv_filter == FILTER_BY_MAC) || (adv_filter == FILTER_BY_KIND))
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
   读取广播内容命令
*/
void app_adv_report_to_mcu(void)
{
	    USHORT length = 4;
		  USHORT checksum = 0;
	    UCHAR  i, j;
	
	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
	        for(i = 0; i < adv_number; i++)
	        {
		      length += (9 + adv_database[i].length);
	        }
			cobra_uart_write(M3_UART0, length&0xff);
			cobra_uart_write(M3_UART0, (length>>8)&0xff);
			checksum += length&0xff;
			checksum += ((length>>8)&0xff);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
			checksum += 0xFF;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x82);
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x04);
				
			checksum += 0x68;
			checksum += 0x82;
			
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x04;
	//广播数据	
            for(i = 0; i < adv_number; i++)
            {
                  cobra_uart_write(M3_UART0, adv_database[i].length + 8 ); 
				  checksum += (adv_database[i].length + 8);
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[0]); 
				  checksum += adv_database[i].mac_address[0];
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[1]); 
				  checksum += adv_database[i].mac_address[1];
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[2]);
                  checksum += adv_database[i].mac_address[2];	
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[3]); 
				  checksum += adv_database[i].mac_address[3];
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[4]); 
				  checksum += adv_database[i].mac_address[4];
				
				  cobra_uart_write(M3_UART0, adv_database[i].mac_address[5]); 
				  checksum += adv_database[i].mac_address[5];
				
				  cobra_uart_write(M3_UART0, (adv_database[i].rssi&0xff)); 
				  checksum += (adv_database[i].rssi&0xff);
					
				  cobra_uart_write(M3_UART0, ((adv_database[i].rssi >> 8)&0xff)); 
				  checksum += ((adv_database[i].rssi >> 8)&0xff);
					
				  for(j = 0; j < adv_database[i].length; j++)
				  {
						  cobra_uart_write(M3_UART0, adv_database[i].adv_data[j]); 
						  checksum += adv_database[i].adv_data[j];
				  }				
            }	
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
	
}

void Get_adv_number_and_database(UINT *number,ADV_STRUCT *adv_db)
{
	UINT i,j;
	*number = adv_number;
//	for(i = 0; i < *number; i++)
//	{
//	(adv_db +i)->length = adv_database[i].length; 

//  (adv_db +i)->mac_address[0] = adv_database[i].mac_address[0];
//  (adv_db +i)->mac_address[1] = adv_database[i].mac_address[1];
//	(adv_db +i)->mac_address[2] = adv_database[i].mac_address[2];
//	(adv_db +i)->mac_address[3] = adv_database[i].mac_address[3];
//	(adv_db +i)->mac_address[4] = adv_database[i].mac_address[4];
//	(adv_db +i)->mac_address[5] = adv_database[i].mac_address[5];

//	(adv_db +i)->rssi = adv_database[i].rssi;
//	cobra_uart_write(M3_UART0, (adv_database[i].rssi&0xff)); 

//	for(j = 0; j < adv_database[i].length; j++)
//	{
//	(adv_db +i)->adv_data[j] = adv_database[i].adv_data[j]; 
//	}				
//	}	
	 
}
UINT16 Get_Filter_Adv_Info_length(void)
{
	UINT16 i,total;
	for(i = 0; i < adv_number; i++)
	   total += adv_database[i].length;
  return 	total;
}
void Print_Filter_Adv_Info(USHORT *checksum)
{
	UINT16 i,j; 
	for(i = 0; i < adv_number; i++)
		{	
			cobra_uart_write(M3_UART0, adv_database[i].length + 8 ); 
			*checksum += (adv_database[i].length + 8);
				
			cobra_uart_write(M3_UART0, adv_database[i].mac_address[0]); 
			*checksum += adv_database[i].mac_address[0];

			cobra_uart_write(M3_UART0, adv_database[i].mac_address[1]); 
			*checksum += adv_database[i].mac_address[1];

			cobra_uart_write(M3_UART0, adv_database[i].mac_address[2]);
			*checksum += adv_database[i].mac_address[2];	

			cobra_uart_write(M3_UART0, adv_database[i].mac_address[3]); 
			*checksum += adv_database[i].mac_address[3];

			cobra_uart_write(M3_UART0, adv_database[i].mac_address[4]); 
			*checksum += adv_database[i].mac_address[4];

			cobra_uart_write(M3_UART0, adv_database[i].mac_address[5]); 
			*checksum += adv_database[i].mac_address[5];

			cobra_uart_write(M3_UART0, (adv_database[i].rssi&0xff)); 
			*checksum += (adv_database[i].rssi&0xff);

			cobra_uart_write(M3_UART0, ((adv_database[i].rssi >> 8)&0xff)); 
			*checksum += ((adv_database[i].rssi >> 8)&0xff);
		 
			for(j = 0; j < adv_database[i].length; j++)
			{
			cobra_uart_write(M3_UART0, adv_database[i].adv_data[j]); 
			*checksum += adv_database[i].adv_data[j];
			}	
		}	
}

#endif