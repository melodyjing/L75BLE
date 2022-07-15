/******************************************************************************
	File Name	  :	  ota_shell_cmd.c
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/28

	Description :  
     通过命令, 来查询周边设备的状态

	Changed Log	:
		Liujian  2020/08/28	   - Creation

*****************************************************************************/
#include "ota_shell_cmd.h"
#include "app_rdw_shell.h"
#include "app_ota_scan.h"
#include "ota_device.h"
#include <stdio.h>

#if (APP_OTA_MATSER_ENABLE == 1)
void ota_shell_list_ota_device(unsigned int argc, unsigned char * argv[])
{
      unsigned int number = app_ota_get_ota_device_number();
	    unsigned int i;
	    struct  ota_scan_data * p;
	    if(number > 0)
			{
				  p = ota_device;
				  printf("|--------------------------------------------------|\n");
				  printf("| index  |   Bluetooth Address            | RSSI   |\n");
				  printf("|--------------------------------------------------|\n");
				  for(i = 0; i < number; i++)
				  {
						printf("| %04x   | 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x  | %4d   |\n",  i, \
						                               p->bd_address.addr.addr[0], p->bd_address.addr.addr[1], p->bd_address.addr.addr[2],\
						                               p->bd_address.addr.addr[3], p->bd_address.addr.addr[4], p->bd_address.addr.addr[5], \
                                           p[i].rssi);
             p++;						
				  }
          printf("|--------------------------------------------------|\n");
			}
	    
}

void ota_shell_connect_ota_device(unsigned int argc, char * argv[])
{
	   unsigned int i;
	   unsigned int number = app_ota_get_ota_device_number();
	   struct gap_bdaddr  bd_addr;
	   if(argc < 2)
			 return ;
		 i = atoi(argv[1]);
		 if(i < number)
		 {
			  bd_addr = ota_device[i].bd_address;
			  app_ota_set_connect_addr(bd_addr);
			  app_ota_scan_again();
		 }
		 else
		 {
			  printf("no ota device .... \n");
		 }
	
}


void ota_shell_disconnect_ota_device(unsigned int argc, char * argv[])
{
	   struct gap_bdaddr addr;
	   memset(&addr, 0 , sizeof(addr));
	   app_ota_set_connect_addr(addr);
	   app_ota_device_disconnect();
	   app_ota_scan_again();
}
#endif
