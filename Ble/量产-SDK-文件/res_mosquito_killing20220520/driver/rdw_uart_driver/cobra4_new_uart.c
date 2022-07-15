/******************************************************************************
	File Name	:	cobra4_new_uart.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/10/25

	Description :   
		cobra4 的 UART FIFO 处理方式
	Changed Log	:
		Liujian  2019/10/25			- Creation

*****************************************************************************/

#include "cobra4_new_uart.h"
#include "cobra_uart.h"
#include "cobra_system.h"
#include "rdw_mcu.h"

#if COBRA4_NEW_UART_DRIVER

static void cobra_new_uart_task_callback(void);

static struct 	cobra_uart_env_tag 	cobra4_new_uart0_env;

static uart_app_callback cobra4_uart_app_callback_func = NULL;
static uint8_t ch;

void cobra4_new_uart_register_callback(uart_app_callback callback)
{
	cobra4_uart_app_callback_func = callback;	
}

void cobra4_new_uart_unregister_callback(void)
{
	 cobra4_uart_app_callback_func = NULL;	
}

static uint8_t     rx_buf[UART_RX_BUF_SIZE];                                                    
static uint8_t     tx_buf[UART_TX_BUF_SIZE];          
/*
初始化串口, 让串口接收和发送都产生中端

*/
void cobra4_new_uart_init(void)
{
			app_uart_buffers_t buffers;                                                                
																																																																									
			buffers.rx_buf      = rx_buf;                                                              
			buffers.rx_buf_size = sizeof (rx_buf);                                                     
			buffers.tx_buf      = tx_buf;                                                              
			buffers.tx_buf_size = sizeof (tx_buf);                                                     
			app_uart_init(&buffers);       

			ke_event_callback_set(KE_EVENT_NEW_UART, &cobra_new_uart_task_callback);

			cobra_uart_config_uart_rx_interrupt(M3_UART0, 1);
	
}

static void (*send_callback) (void*, uint8_t) = NULL;
static void * send_dummy;
void cobra_m4_uart0_interrupt(void)
{
				volatile unsigned short uart_status;
				volatile unsigned short uart_control;

				uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
				cobra_uart_clear_interrupt(M3_UART0, uart_status);
	
				uart_control = cobra_uart_get_interrupt_control(M3_UART0);
	
				
	      //串口接收中断
				if(uart_control & 0x200)
				{
					  if(!(uart_status & 0x08)) 
						{
								  ch = cobra_uart_get_rx_data(M3_UART0);
							
								  app_uart_receive_done_event(ch);
									ke_event_set(KE_EVENT_NEW_UART);
//								app_fifo_put(&m_tx_fifo, byte);
//								if(cobra4_uart_app_callback_func != NULL)
//								{
//										cobra4_uart_app_callback_func(ch);	
//								}
						}
					
				}
				
				//串口发送中断
				if(uart_control & 0x100)
				{ 
						 if(!(uart_status&0x02))
						 {
							     app_uart_send_done_event();
						 }
						 
						 uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
						 
						 if(uart_status & 0x01)   //为空， 关掉中断
						 {
									void (*callback) (void*, uint8_t) = NULL;
									void* data =NULL;
							 
									cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
									cobra4_new_uart0_env.tx_busy = 0;
									callback = cobra4_new_uart0_env.tx.callback;
									data     = cobra4_new_uart0_env.tx.dummy;

									if(callback != NULL)
									{
											// Clear callback pointer
											cobra4_new_uart0_env.tx.callback = NULL;
											cobra4_new_uart0_env.tx.dummy    = NULL;
											// Call handler
											cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
											callback(data, RWIP_EIF_STATUS_OK);	
									}
												
						 }
						 
				}
	
}

static void cobra_new_uart_task_callback(void)
{
			uint8_t byte;
			ke_event_clear(KE_EVENT_NEW_UART);
	
			if(cobra4_new_uart0_env.rx_busy) 
			{
					while(app_uart_get(&byte))
					{
							cobra4_new_uart0_env.p_buffer[cobra4_new_uart0_env.receive_index] = byte; //temp;
							cobra4_new_uart0_env.receive_index++;
							if(cobra4_new_uart0_env.receive_index == cobra4_new_uart0_env.total)
							{
									void (*callback) (void*, uint8_t) = NULL;
									void* data =NULL;
			
									cobra4_new_uart0_env.rx_busy = 0;
									callback = cobra4_new_uart0_env.rx.callback;
									data     = cobra4_new_uart0_env.rx.dummy;

									if(callback != NULL)
									{
											// Clear callback pointer
											cobra4_new_uart0_env.rx.callback = NULL;
											cobra4_new_uart0_env.rx.dummy    = NULL;

											// Call handler
											//cobra_uart_config_uart_rx_interrupt(M3_UART0, 0);
											callback(data, RWIP_EIF_STATUS_OK);
										  break;
									}
								
							}
				 }
		}
}


/////////////////////////////////////////////////////////////////////////////////////

void cobra4_uart0_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
	
    if(bufptr == NULL) return;
    if(size == 0)  return;
    if(callback == NULL) return;
	
    cobra4_new_uart0_env.rx.callback = callback;
    cobra4_new_uart0_env.rx.dummy    = dummy;


		if(cobra4_new_uart0_env.rx_busy) return;
	
		cobra4_new_uart0_env.p_buffer = bufptr;
	  cobra4_new_uart0_env.total = size;
		cobra4_new_uart0_env.receive_index = 0;
		cobra4_new_uart0_env.rx_busy = 1;
	  cobra_uart_config_uart_rx_interrupt(M3_UART0, 1);

} 


void cobra4_new_uart0_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
		uint32_t i; 
	  // Sanity check
    if(bufptr == NULL) return;
    if(size == 0)  return;
    if(callback == NULL) return;
	
		if(cobra4_new_uart0_env.tx_busy) return;
	
	
    cobra4_new_uart0_env.tx.callback = NULL;
    cobra4_new_uart0_env.tx.dummy    = dummy;
	
	
		for(i = 0; i < size; i++)
		{
				app_uart_put(bufptr[i]);
		}
		
		cobra4_new_uart0_env.tx.callback = callback;
		cobra4_new_uart0_env.tx_busy = 1;

	
}

void cobra4_new_uart0_flow_on(void)
{
    // Configure modem (HW flow control enable)
    //uart_force_rts_setf(0);
		GLOBAL_INT_DISABLE();
	  cobra4_new_uart0_env.rx_busy = 0;
		cobra4_new_uart0_env.tx_busy = 0;
		cobra4_new_uart0_env.rx.callback = NULL;
    cobra4_new_uart0_env.tx.callback = NULL;
	
		GLOBAL_INT_RESTORE();
	
		
}

bool cobra4_new_uart0_flow_off(void)
{
    bool flow_off = true;
	
		GLOBAL_INT_DISABLE();
	  if(cobra4_new_uart0_env.rx_busy || cobra4_new_uart0_env.tx_busy)
			 flow_off = false; 
		GLOBAL_INT_RESTORE();
    return flow_off;
}



#endif

