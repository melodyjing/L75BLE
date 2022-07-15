/******************************************************************************
	File Name	  :	  ota_shell.c
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/27

	Description :  
     通过命令的方式, 用 xmode 协议的方式烧写文件

	Changed Log	:
		Liujian  2020/08/27	   - Creation

*****************************************************************************/
#include "ota_shell.h"
#include "cobra_uart.h"
#include "ota_device.h"
#include "cobra4A_linked_softtimer.h"
#include "rdw_queue.h"
#include "app_bk_task.h"

#include <stdio.h>
#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1)&&(COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))

#define OTA_STOP_STATE			0
#define OTA_START_STATE			1

#define PACKET_SOH_STATE		1
#define PACKET_FIND_DATA		2

static unsigned short crc16_ccitt(const unsigned char *buf, int len);
static int ota_crc_check(int crc, const unsigned char *buf, int sz);
int calcrc(char *ptr, int count);

static COBRA_SOFTWARE_TIMER  * p_ota_soft_timer;

static COBRA_SOFTWARE_TIMER  * p_ota_send_soft_timer;

//static COBRA_SOFTWARE_TIMER  * p_receive_char_timeout;

static unsigned int  time_out_count;
static unsigned char ota_state;
static unsigned char packet_state;
static unsigned int  packet_length;
static unsigned char patcket_data[256];
QUEUE64_TYPE   ota_uart_buffer;

static void xmode_receive_begin(void)
{
		cobra_uart_write(M3_UART0, 'C');
}

static void ota_shell_receive_data(unsigned char ch)
{
	
	  QueueIn(ota_uart_buffer, ch);
	
	  cobra_app_set_ota_shell_task();
	
}

static void ota_soft_timer_callback(COBRA_SOFTWARE_TIMER * temp)
{
	   xmode_receive_begin();
	   time_out_count++;
	   if(time_out_count > 60)
		 {
				cobra_linked_softtimer_delete(p_ota_soft_timer);
			  printf("\n timer out \n");
			  cobra4_uart_set_receive_callback(app_rdw_shell_handler);
		 }
}

//说明在接收每一个字节的时候,等待的Timeout
static void ota_soft_timer_char_timeout_callback(COBRA_SOFTWARE_TIMER * temp)
{
	  packet_state = PACKET_FIND_DATA;
	  cobra_uart_write(M3_UART0, NAK);
//	  cobra_linked_softtimer_stop(p_receive_char_timeout);
	
}

static void ota_soft_timer_send_callback(COBRA_SOFTWARE_TIMER * temp)
{
	   if(app_ota_device_patch_data_send_state() == 0)
		 {
			     app_ota_device_patch_data(&patcket_data[3], 128);
					 cobra_linked_softtimer_stop(p_ota_send_soft_timer);
	         cobra_uart_write(M3_UART0, ACK);
		 }
	
}

void ota_shell_main(unsigned int argc, unsigned char * argv[])
{
	   //printf("argc = %d \n", argc);
	   
	   if(!app_ota_device_ready())
		 {
			  printf("device is not ready \n");
			  return;
		 }
		 
		 ota_state = OTA_STOP_STATE;
		 
		 memset((uint8_t *)&ota_uart_buffer, 0, sizeof(ota_uart_buffer));
		 
		 QueueInit(ota_uart_buffer);
		 
		 cobra4_uart_set_receive_callback(ota_shell_receive_data);
		 
		 time_out_count = 0;
		 
//		 p_receive_char_timeout = cobra_linked_softtimer_create(NULL, ota_soft_timer_char_timeout_callback, 3000);
		 
		 p_ota_send_soft_timer = cobra_linked_softtimer_create(NULL, ota_soft_timer_send_callback, 10);
		 
		 p_ota_soft_timer = cobra_linked_softtimer_create(NULL, ota_soft_timer_callback, 1000);
		 
	   cobra_linked_softtimer_start(p_ota_soft_timer);	
		 
	   app_ota_device_set_memory_information();
		 
		 app_ota_device_patch_len(128);
		 
	   return;
}

unsigned int app_ota_check_receive_data(void)
{
		if(patcket_data[1] == (unsigned char)(~patcket_data[2]))
		{
			 if(ota_crc_check(1, &patcket_data[3], 128))
				 return 1;
		}
		return 0;
}

void app_ota_shell_task_loop(void)
{
	 uint8_t cChar;		
	
	 while(1)
	 {
		  if (true == QueueOut(ota_uart_buffer, cChar)) 
			{
				   if(ota_state == OTA_STOP_STATE)
					 {
				       if(cChar == SOH)
					     {  
									cobra_linked_softtimer_delete(p_ota_soft_timer);
								  ota_state = OTA_START_STATE;
								  packet_state = PACKET_SOH_STATE;
								  packet_length = 0;
								  patcket_data[packet_length] = cChar;
								  packet_length++;
//								  cobra_linked_softtimer_start(p_receive_char_timeout);
					     }
					 }
					 else if(ota_state == OTA_START_STATE)
					 {
						   if(packet_state == PACKET_SOH_STATE)
							 { 
								    patcket_data[packet_length] = cChar;
								    packet_length++;
									  if(packet_length == (128 + 5))
										{
										   //要开始计算 CRC 
											  if(app_ota_check_receive_data())
												{
												     cobra_linked_softtimer_start(p_ota_send_soft_timer);
														 packet_length = 0;
													   packet_state = PACKET_FIND_DATA;
												}
												else
												{
													  packet_state = PACKET_FIND_DATA;
	                          cobra_uart_write(M3_UART0, NAK);
												}
										}
							 }
							 else if(packet_state == PACKET_FIND_DATA)
							 {
									  if(cChar == SOH)
					          {
                        packet_state = PACKET_SOH_STATE;
								        packet_length = 0;
								        patcket_data[packet_length] = cChar;
								        packet_length++;
                    }
                    else if(cChar == EOT)
										{
											  cobra_uart_write(M3_UART0, ACK);
											  cobra4_uart_set_receive_callback(app_rdw_shell_handler);
											  app_ota_device_ota_cmd(SUOTAR_IMG_END);
											  app_ota_device_ota_cmd(SUOTAR_REBOOT);
										}											
							 }	 
					 }

			}
			else
			{
				 break;
			} 
	 }

}


static int ota_crc_check(int crc, const unsigned char *buf, int sz)
{
    if (crc)
    {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
        if (crc == tcrc)
            return 1;
    }
    else
    {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i)
        {
            cks += buf[i];
        }
        if (cks == buf[sz])
            return 1;
    }
    return 0;
}


static const unsigned short crc16tab[256]= {
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
 0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
 0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
 0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
 0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
 0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
 0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
 0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
 0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
 0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
 0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
 0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
 0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
 0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
 0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
 0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
 0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
 0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
 0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
 0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
 0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
 0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
 0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
 0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
 0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
 0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
 0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
 0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
 0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
 0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
 0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

unsigned short crc16_ccitt(const unsigned char *buf, int len)
{
    register int counter;
    register unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}
#endif