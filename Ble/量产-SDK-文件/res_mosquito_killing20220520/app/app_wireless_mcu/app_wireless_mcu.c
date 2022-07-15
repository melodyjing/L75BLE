/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_wireless_mcu.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	    2019/1/1

	Description :   
	  Test the  rf module to transter data 				

	Changed Log	:

		Liujian  2019/7/1			- Creation


*****************************************************************************/
#include "app_wireless_mcu.h"
#include "cobra_ceva_baseband.h"
#include "rdw_mcu.h"
#include "rwip_config.h"  
#include "ke_event.h"
#include "cobra_soft_timer.h"
#include <string.h>
#if(APP_RF_NORMAL_PROTOCOL_ENABLE == 1)

#define 	CSEM_RF_INIT_TBL_1MBPS_SIZE        0xFC

/* csem mode  static settings - v0x20- 1Mbps table */ 
static  const uint8_t CSEM_RF_TBL_1MBPS[CSEM_RF_INIT_TBL_1MBPS_SIZE] =
{
    0x22, 0x00, 0x00, 0x80,   0x00, 0x22, 0x0b, 0x14,    0x10, 0x20, 0x00, 0x00,   0x00, 0x03, 0x00, 0xff, // 0x0*
    0xff, 0x0f, 0x00, 0x00,   0x80, 0x23, 0x82, 0x00,    0x30, 0xcb, 0x15, 0x82,   0x00, 0x01, 0x01, 0x00, // 0x1*
    0x72, 0x1c, 0x87, 0x82,   0xff, 0x00, 0x55, 0x00,    0x00, 0x00, 0x00, 0x00,   0x29, 0x41, 0x76, 0x71, // 0x2*
    0x00, 0x23, 0x08, 0xdf,   0x2d, 0x03, 0x80, 0x00,    0x55, 0x55, 0x55, 0x00,   0x21, 0x00, 0x00, 0x00, // 0x3*
    0x00, 0x29, 0x10, 0xc0,   0x00, 0x00, 0x00, 0x00,    0x00, 0x00, 0x01, 0x02,   0x07, 0x10, 0x20, 0x37, // 0x4*
    0x50, 0x66, 0x76, 0x7e,   0x21, 0x74, 0xca, 0xfe,    0x80, 0x50, 0x40, 0x00,   0x40, 0x02, 0x11, 0x28, // 0x5*
    0x3e, 0x0b, 0x55, 0x15,   0x11, 0x00, 0x19, 0x0c,    0x0c, 0x73, 0x0b, 0x16,   0x00, 0xfe, 0xb0, 0x30, // 0x6*
    0x59, 0x71, 0x20, 0x05,   0x00, 0x00, 0x04, 0xa0,    0x00, 0x49, 0x28, 0x42,   0x16, 0xf2, 0x90, 0x97, // 0x7*
    0xfc, 0xe4, 0x47, 0x7f,   0x0e, 0x21, 0x35, 0xc1,    0x4c, 0x94, 0xad, 0x26,   0x01, 0x22, 0x51, 0x01, // 0x8*
    0x00, 0x00, 0x00, 0x73,   0x66, 0x38, 0x77, 0x74,    0x8a, 0x77, 0x66, 0x9f,   0x95, 0x06, 0x8a, 0x67, // 0x9*
    0x17, 0x00, 0x00, 0xa5,   0x00, 0x00, 0x08, 0x46,    0x46, 0x1f, 0xd8, 0x02,   0x64, 0x00, 0x11, 0x24, // 0xa*
    0x1b, 0x80, 0x00, 0x00,   0x80, 0x15, 0x84, 0xc3,    0x05, 0x80, 0xf0, 0x03,   0xd0, 0xf1, 0xc3, 0x00, // 0xb*
    0x00, 0x00, 0xc0, 0x00,   0x00, 0x89, 0x33, 0x00,    0x00, 0x3c, 0x2f, 0xae,   0x80, 0x04, 0x01, 0x38, // 0xc*
    0x00, 0x00, 0x01, 0x03,   0xa5, 0x14, 0x00, 0x00,    0x00, 0x25, 0x0e, 0x30 
};

#define 	COBRA_BOARDCAST_ADDRESS 0x8888
#define   COBRA_DEVICE_ADDRESS		0x0050


//配置参数
#define   TRY_SEND_TIMES   				10              //设置最多发射多少次认为失败了
#define   TRY_SEND_ACK_TIMES			10

static unsigned char cobra_csem_irq_status;

static void cobra_csem_task_callback(void);

static unsigned int global_receive_id;

static p_csem_rf_receive app_csem_rf_receive_function;

static p_csem_rf_send_callback app_csem_rf_send_callback;

static APP_CSEM_RF_MODE  csem_rf_mode;

static unsigned int 		global_send_pid;
static unsigned int     global_try_send_time;
static unsigned int 		global_send_ack_time; 

static cobra_csem_payload 		global_send_payload;

static cobra_csem_payload     globa_ack_payload;

static unsigned char 		global_receive_length;
static unsigned char 		global_receive_buffer[MAX_PAYLOAD_SIZE];

static uint32_t rf_ack_time_index;

static uint32_t rf_send_again_timer_index;

static uint32_t rf_send_busy;

void app_csem_send_ack_function(unsigned int param);

void app_csem_send_again_function(unsigned int param);
/*
初始化和建立一个任务来处理中断发送过来的消息

*/
int app_wireless_mcu_init(void)
{
			unsigned char i;

      ke_event_callback_set(KE_EVENT_CSEM_TASK, &cobra_csem_task_callback);

			app_csem_idle_mode();

			cobra_ceva_baseband_csem_register_write(0x017,0x00);
	  
			for(i = 0; i < CSEM_RF_INIT_TBL_1MBPS_SIZE; i++)
			{
					cobra_ceva_baseband_csem_register_write(i, CSEM_RF_TBL_1MBPS[i]);
			}		

			global_receive_id = 0xffffffff;
			
			global_send_pid = 0;
			
			rf_send_busy = 0;
			
			app_csem_rf_receive_function = NULL;
			
			app_csem_set_device_address(COBRA_DEVICE_ADDRESS);
			
			app_csem_set_broadcast_address(COBRA_BOARDCAST_ADDRESS);
			
			csem_rf_mode = CSEM_RF_IDLE_MODE;
			
			rf_ack_time_index = cobra_soft_timer_create(app_csem_send_ack_function);	
			
			rf_send_again_timer_index = cobra_soft_timer_create(app_csem_send_again_function);	
			
			return 0;	
}



/*
使能各种发送接受中断

*/
void app_csem_irq_enable(CSEM_IRQ_TYPE irq)
{
			unsigned char value;
			unsigned char temp;
			switch(irq)
			{
				case CSEM_IRQ_TX_FLAG:
					value = 0x01;
					break;
				case CSEM_IRQ_RXSTOP_FLAG:
					value = 0x02;
					break;
				case CSEM_IRQ_RECEIVED_FLAG:
					value = 0x04;
					break;
				case CSEM_IRQ_SYNC_FLAG:
					value = 0x08;
					break;
				case CSEM_IRQ_TXFIFO_FLAG:
					value = 0x10;
					break;
				case CSEM_IRQ_RXFIFO_FLAG:
					value = 0x20;
					break;
				default:
					value = 0x00;
					break;
				
			}
			temp = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_CONF);
			temp |= value&0x3f;
		  cobra_ceva_baseband_csem_register_write(CS565_REG_IRQ_CONF, temp);
}

int app_csem_send_data(unsigned char * p_send_buffer, unsigned char number)
{
		unsigned char i;
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, number);
	
		for(i = 0; i < number; i++)
		{
				cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, p_send_buffer[i]);
		}
		
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
		
		return 0;
	
}

int  app_receive_csem_data(unsigned char * p_buffer, unsigned int max_size)
{
		unsigned char length;
		unsigned char i;
	  unsigned char decode_state;
	
		decode_state = cobra_ceva_baseband_csem_register_read(CS565_REG_DESER_STATUS);
		if(decode_state & 0x02)
		{
				return 0;
		}
		
		length = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);

		if(length != max_size)
		{
				return  0;
		}

	  for(i = 0; i < length; i++)
		{
				p_buffer[i] = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
			
		}
		
		return 1;
		
}

unsigned char app_csem_read_rx_fifo(void)
{
		unsigned char temp;
		temp = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
		return temp;
}

unsigned char app_csem_read_deser_status(void)
{
		volatile unsigned char temp;
		temp = cobra_ceva_baseband_csem_register_read(CS565_REG_DESER_STATUS);
		return temp;
	
}

unsigned char app_csem_read_fsm_status(void)
{
		volatile unsigned char temp;
		temp = cobra_ceva_baseband_csem_register_read(CS565_REG_FSM_STATUS);
		return temp;
	
}


void app_csem_set_broadcast_address(unsigned short address)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_ADDRESS_BROADCAST, address&0xff);
		cobra_ceva_baseband_csem_register_write(CS565_REG_ADDRESS_BROADCAST+1, address>>8&0xff);
	
}

void app_csem_set_device_address(unsigned short address)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_ADDRESS, address&0xff);
		cobra_ceva_baseband_csem_register_write(CS565_REG_ADDRESS+1, address>>8&0xff);
	
}


void app_csem_flush_rx_fifo(void)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_RXFIFO_STATUS, 0x01);
}


void app_csem_flush_tx_fifo(void)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO_STATUS, 0x01);
}


void app_csem_set_sync_word(unsigned int sync)
{
			cobra_ceva_baseband_csem_register_write(CS565_REG_PATTERN, sync&0xff);
			cobra_ceva_baseband_csem_register_write(CS565_REG_PATTERN+1, (sync>>1)&0xff);
			cobra_ceva_baseband_csem_register_write(CS565_REG_PATTERN+2, (sync>>2)&0xff);
			cobra_ceva_baseband_csem_register_write(CS565_REG_PATTERN+3, (sync>>3)&0xff);	
}

unsigned char app_csem_read_irq_status(void)
{
		volatile unsigned char temp;
		temp = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_STATUS);
		return temp;
}

void app_csem_set_receive_mode(void)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_PACKET_HANDLING, 0xf3); //en_packet, en_pattern, en_preamble
		cobra_ceva_baseband_csem_register_write(CS565_REG_CHANNEL, 0x09);  
	  app_csem_set_sync_word(0x12341234);
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x03);
}

void app_csem_idle_mode(void)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x08);
	//	app_csem_flush_rx_fifo();
	//	app_csem_flush_tx_fifo();
}

void app_csem_send_ack(void)
{
	//	cobra_csem_payload 		ack_payload;
	
		globa_ack_payload.payload_pid = global_receive_id;
	
//		globa_ack_payload.payload_no_ack = 1;
	
//		globa_ack_payload.payload_no_ack = 1;
//		globa_ack_payload.payload_length = 1;
//		
//		ack_payload.data[0] = global_receive_id;
	
		app_csem_idle_mode();
		//cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, global_receive_id);
		//cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
		app_csem_send_data((unsigned char *)&globa_ack_payload, sizeof(globa_ack_payload));
		app_csem_irq_enable(CSEM_IRQ_TX_FLAG);
}

void app_csem_send_ack_function(unsigned int param)
{
			global_send_ack_time++;
			if(global_send_ack_time < TRY_SEND_ACK_TIMES)
			{
					app_csem_send_ack();
			}				
}

void app_csem_send_again_function(unsigned int param)
{
					global_try_send_time++;
				
					if(global_try_send_time >= TRY_SEND_TIMES) 
					{
				
							if(app_csem_rf_send_callback != NULL)   //发射失败
							{
									rf_send_busy = 0;
									app_csem_rf_send_callback(0, 0, NULL);
							}
				 }
				 else
				 {

						app_csem_tx_go();
				}
}



static void cobra_csem_task_callback(void)
{
			int ret;
	
			cobra_csem_payload p_paybload;
	
	    //printf("cobra_csem_task_callback \n");
			ke_event_clear(KE_EVENT_CSEM_TASK);
	
	
	    if(cobra_csem_irq_status & 0x04)    //接收中断
			{
				if(csem_rf_mode == CSEM_RF_RX_MODE)
				{
					ret = app_receive_csem_data((unsigned char *)&p_paybload, sizeof(p_paybload));
					if(ret == 0)
					{
							app_csem_receive_go();
							return;  //receive error
					}
					
					if(p_paybload.payload_no_ack)
					{
						  app_csem_receive_go();         //这里也可以认为是收到了新的数据
					}
					else
					{
							if(p_paybload.payload_pid != global_receive_id)
							{
								 
									global_receive_id = p_paybload.payload_pid;    //同时这里可以认为收到了新的数据
								  
									if(app_csem_rf_receive_function != NULL)
									{
											app_csem_rf_receive_function(p_paybload.data, p_paybload.payload_length);
									}
									
									app_csem_idle_mode();
									global_send_ack_time = 0;
									cobra_soft_timer_start(rf_ack_time_index, 1, 0);
									
									
							}
							else
							{			
									//printf("2\n");
									app_csem_idle_mode();
									global_send_ack_time = 0;
									cobra_soft_timer_start(rf_ack_time_index, 1, 0);
									//app_csem_send_ack();    //发送ACK  收到相同的数据, 说明上次发送的ACK 对方没有收到
									
							}
					}
				}
				
				else if(csem_rf_mode == CSEM_RF_TX_MODE)   //receive ACK
				{ 
					  //说明发送过去之后,收到了对方的ACK, 要判断ACK 是否正确,还有是否是重复的ACK
						unsigned char state = 0;
						cobra_soft_timer_stop(rf_send_again_timer_index);
						app_receive_csem_data((unsigned char *)&p_paybload, sizeof(p_paybload));
					
						if(p_paybload.payload_pid == global_send_pid)
						{
								state = 1;
								app_csem_inc_send_pid();
								if(app_csem_rf_send_callback != NULL)   //发送成功
								{
										rf_send_busy = 0;
										app_csem_rf_send_callback(state, p_paybload.payload_length, p_paybload.data);
								}
						
						}
						else
						{
								app_csem_tx_go();
						}
																
				}
				
			}
			else if(cobra_csem_irq_status & 0x01)  //发送中断
			{
					 if(csem_rf_mode == CSEM_RF_RX_MODE)
					 {
										app_csem_receive_go();
										cobra_soft_timer_start(rf_ack_time_index, 10, 0);
						 
										
//									 send_ack_number++;
//									 if(send_ack_number < 20)
//									 {
//											app_csem_receive_go();
//											cobra_soft_timer_start(rf_ack_time_index, 10, 0);
//									 }
//									 else
//									 {
//											app_csem_receive_go();
//									 }
				  }
					else if(csem_rf_mode == CSEM_RF_TX_MODE)
					{
						  //发送完数据,然后要确认对方是否收到, 等待ACK
							if(global_send_payload.payload_no_ack == 0)  //wait ACK
							{
								  app_csem_receive_go();
									cobra_soft_timer_start(rf_send_again_timer_index, 10, 0);
							}
					}
			}
}

///-------------------interrupt function----------------------

void cobra_csem_irq_rx_stop_interrupt(void)
{
		 cobra_csem_irq_status = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_STATUS);
		 ke_event_set(KE_EVENT_CSEM_TASK);
	
	  // printf("cobra_csem_irq_rx_stop_interrupt \n");
	
}

void cobra_csem_irq_received_interupt(void)
{
		 
		 cobra_csem_irq_status = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_STATUS);	
		 ke_event_set(KE_EVENT_CSEM_TASK);
}

void cobra_csem_irq_sync_interrupt(void)
{
		
	  cobra_csem_irq_status = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_STATUS);
	  ke_event_set(KE_EVENT_CSEM_TASK);

	
}

void cobra_csem_irq_tx_interrupt(void)
{
	//	printf("cobra_csem_irq_tx_interrupt \n");
		cobra_csem_irq_status = cobra_ceva_baseband_csem_register_read(CS565_REG_IRQ_STATUS);
	  ke_event_set(KE_EVENT_CSEM_TASK);
	
}


//-------------------APPlication function----------------------------------------------------------


/*
开始接受数据	
*/
void app_csem_receive_go(void)
{
		app_csem_idle_mode();
		app_csem_irq_enable(CSEM_IRQ_RECEIVED_FLAG);
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x03);
}



void app_csem_protocol_init(unsigned char channel,  unsigned int syc_word)
{
		cobra_ceva_baseband_csem_register_write(CS565_REG_PACKET_HANDLING, 0xf3); //en_packet, en_pattern, en_preamble
		cobra_ceva_baseband_csem_register_write(CS565_REG_CHANNEL, channel&0x3f);  
	  app_csem_set_sync_word(syc_word);
	

		NVIC_ClearPendingIRQ(CSEM_IRQ_RXSTOP);
		NVIC_ClearPendingIRQ(CSEM_IRQ_RECEIVED);
		NVIC_ClearPendingIRQ(CSEM_IRQ_SYNC);
		NVIC_ClearPendingIRQ(CSEM_IRQ_TX);
	
		NVIC_EnableIRQ(CSEM_IRQ_RXSTOP);
		NVIC_EnableIRQ(CSEM_IRQ_RECEIVED);
		//NVIC_EnableIRQ(CSEM_IRQ_SYNC);
		NVIC_EnableIRQ(CSEM_IRQ_TX);
	
}


void app_csem_receive_setting(unsigned char ch, unsigned int sync_word, unsigned char ack_length, unsigned char * p_buffer, p_csem_rf_receive function)
{
			unsigned int i;
			app_csem_protocol_init(ch, sync_word);
			app_csem_rf_receive_function = function;
			app_csem_irq_enable(CSEM_IRQ_RECEIVED_FLAG);
			app_csem_receive_go();
			app_csem_set_mode(CSEM_RF_RX_MODE);
			globa_ack_payload.payload_no_ack = 1;
			if(p_buffer == NULL)
			{
					globa_ack_payload.payload_length = 0;
					return;
			}
			if(ack_length >= MAX_PAYLOAD_SIZE)
				ack_length = MAX_PAYLOAD_SIZE;
			
			for(i = 0; i < ack_length; i++)
			{
					globa_ack_payload.data[i] = p_buffer[i];
			}
			globa_ack_payload.payload_length = ack_length;

}

void app_csem_set_mode(APP_CSEM_RF_MODE rf_mode)
{
		csem_rf_mode = rf_mode;
}

int app_csem_send_setting_have_ack(unsigned char *p_buffer, unsigned char length, \
			unsigned char ch, unsigned int sync_word, p_csem_rf_send_callback function)
{
			if(rf_send_busy) return 0;
			app_csem_idle_mode();
			app_csem_protocol_init(ch, sync_word);
			global_send_payload.payload_length = length;
			global_send_payload.payload_no_ack = 0;
			if(length >= MAX_PAYLOAD_SIZE) return 0;
			memcpy(global_send_payload.data, p_buffer, length);
	
			app_csem_rf_send_callback = function;
	
			app_csem_tx_go();
	
	    global_try_send_time = 0;
	
			rf_send_busy = 1;
	
			return 1;
}

int app_csem_send_setting_no_ack(unsigned char *p_buffer, unsigned char length, \
			unsigned char ch, unsigned int sync_word, p_csem_rf_send_callback function)
{
			if(rf_send_busy) return 0;
	
			app_csem_idle_mode();
			app_csem_protocol_init(ch, sync_word);
			global_send_payload.payload_length = length;
			global_send_payload.payload_no_ack = 1;
			if(length >= MAX_PAYLOAD_SIZE) return 0;
			memcpy(global_send_payload.data, p_buffer, length);
	
			app_csem_rf_send_callback = function;
	
			
			app_csem_tx_go();
	
			global_try_send_time = 0;
	
			rf_send_busy = 1;
	
			return 1;
}

void app_csem_tx_go(void)
{
		unsigned char i;
	
		app_csem_idle_mode();
	
		app_csem_set_mode(CSEM_RF_TX_MODE);
	
		app_csem_irq_enable(CSEM_IRQ_TX_FLAG);
	
	
		global_send_payload.payload_pid = global_send_pid;

	  app_csem_send_data((unsigned char *)&global_send_payload, sizeof(global_send_payload));
	
		
		
}

void app_csem_inc_send_pid(void)
{
		global_send_pid++;
		if(global_send_pid >= 128) global_send_pid = 0x00;
}


//------------------------------------------------------------------------------------


//void app_wireless_protocol_init(void)
//{
//	  ke_init();
//	  
//	  // Initialize memory heap used by kernel.
//    // Memory allocated for environment variables
//    ke_mem_init(KE_MEM_ENV,           (uint8_t*)rwip_heap_env,     RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_ENV_SIZE));
//    #if (BLE_HOST_PRESENT)
//    // Memory allocated for Attribute database
//    ke_mem_init(KE_MEM_ATT_DB,        (uint8_t*)rwip_heap_db,      RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_DB_SIZE));
//    #endif // (BLE_HOST_PRESENT)
//    // Memory allocated for kernel messages
//    ke_mem_init(KE_MEM_KE_MSG,        (uint8_t*)rwip_heap_msg,     RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_MSG_SIZE));
//    // Non Retention memory block
//    ke_mem_init(KE_MEM_NON_RETENTION, (uint8_t*)rwip_heap_non_ret, RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_NON_RET_SIZE));

//	
//}



#endif



