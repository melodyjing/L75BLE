/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_uart.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/2

	Description :   come from coney project


	Changed Log	:

		Liujian  2019/4/2			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_system.h"
#include "rdw_mcu.h"
#include "rdw_uart_tx.h"
#include "cobra4_common.h"
#if MCU_RUN_SYSTEM == MCU_M3
#include "cobra_cortex_m3_nvic.h"
#elif MCU_RUN_SYSTEM == MCU_M0
#include "cobra_cortex_m0_nvic.h"
#elif MCU_RUN_SYSTEM == MCU_M4
#endif
#include "rwip.h"
#include "cobra_uart.h"

#if (COBRA_POWER_DOWN_ENABLE)
#include "cobra4_power_down.h"
extern volatile bool ble_deep_sleep;
#endif

#if COBRA_UART_EN

#if LOW_POWER_WAKE_UP_BY_UART
void low_power_uart_receive_handle(void);
#endif


#if(( BLE_APP_UART == 1) && (UART_MODEL_TIMEOUT_ENABLE == 0))	
#if (COBRA_POWER_DOWN_ENABLE)
static struct cobra_uart_env_tag uart_env __attribute__((section("retention_area"),zero_init));
#else
static struct cobra_uart_env_tag uart_env;
#endif
#endif


#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_UART_TypeDef *cobra_m3_uart0 __attribute__((section("retention_area"),zero_init)); //UART0
static volatile COBRA_UART_TypeDef *cobra_m3_uart1 __attribute__((section("retention_area"),zero_init)); //UART1
static struct cobra_uart_env_tag cobra4_uart0_env __attribute__((section("retention_area"),zero_init));
static p_uart_receive_callback uart_receive_callback __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_UART_TypeDef * cobra_m3_uart0;               //UART0
static volatile COBRA_UART_TypeDef * cobra_m3_uart1; 							 //UART1	
//static volatile COBRA_UART_TypeDef * cobra_m0_uart10; 						 //UART10
//static volatile COBRA_UART_TypeDef * cobra_m0_uart11; 						 //UART11
static struct 	cobra_uart_env_tag 	cobra4_uart0_env;
static p_uart_receive_callback uart_receive_callback;
#endif

#if (COBRA_POWER_DOWN_ENABLE)	
void cobra_uart_power_down_init(void);
#endif

void cobra_uart_init(void)
{
		cobra_m3_uart0 = 	(COBRA_UART_TypeDef * )M3_UART0_BASE;
		cobra_m3_uart1 = 	(COBRA_UART_TypeDef * )M3_UART1_BASE;
	  uart_receive_callback = NULL;
	
#if (COBRA_POWER_DOWN_ENABLE)		
    cobra_uart_power_down_init();	
#endif
} 


/** Set the baud rate the UART will transmit and receive at.  
 * @param uart - A pointer to the UART to set the baud rate for.
 * @param uart_clock_frequency - The frequency in Hz of the UART's clock.
 * @param baud_rate - The baud rate to set the uart to use.  
 */
void rdw_uart_set_baud_rate (volatile COBRA_UART_TypeDef *uart, long uart_clock_frequency, long baud_rate)
{
		/* Set the number of clock cycles per bit.  */
		uart->UART_CYCLES_PER_BIT = uart_clock_frequency / baud_rate;
}

#if COBRA_TRIM_EN
void rdw_uart_trim_set_baud_rate (volatile COBRA_UART_TypeDef *uart, long baud_rate)
{
		/* Set the number of clock cycles per bit.  */
		uart->UART_CYCLES_PER_BIT = baud_rate;
}
#endif


/** Enable a UART.  
 * @param uart - A pointer to the UART to enable.
*/  
void rdw_uart_enable (volatile COBRA_UART_TypeDef *uart)
{
		/* Set the enable flag.  */
		uart->UART_CONTROL |= RDW_UART_ENABLE;
}



/** Set UART tx interrupt.  
 * @param uart - A pointer to the UART to enable.
 */  

void rdw_uart_set_txinterrup(volatile COBRA_UART_TypeDef *uart, bool en)
{
		 if(en)
		 {

			uart->UART_CONTROL |= (RDW_UART_TX_IE_MASK);
		 }
		 else
		 {
			 uart->UART_CONTROL &= ~(RDW_UART_TX_IE_MASK);

		 }
}



/** Set UART rx interrupt.  
 * @param uart - A pointer to the UART to enable.
 */  

void rdw_uart_set_rxinterrup(volatile COBRA_UART_TypeDef *uart, bool en)
{
	 if(en)
	 {

		uart->UART_CONTROL |= (RDW_UART_RX_IE_MASK);
	 }
	 else
	 {
		 uart->UART_CONTROL &= ~(RDW_UART_RX_IE_MASK);
	 }
}

unsigned short rdw_uart_get_interrupt_status(volatile COBRA_UART_TypeDef *uart)
{
			return (uart->UART_STATUS & 0xffff);

}


unsigned short rdw_uart_get_interrupt_control(volatile COBRA_UART_TypeDef *uart)
{
			return (uart->UART_CONTROL & 0xffff);
}



void rdw_uart_set_rx_fifo(volatile COBRA_UART_TypeDef *uart, unsigned short length)
{
			uart->UART_RXAF_THRESH = 0x0000|length;

}

void rdw_uart_set_tx_fifo(volatile COBRA_UART_TypeDef *uart, unsigned short length)
{
			uart->UART_TXAE_THRESH = 0x0000|length;

}



/** Write a byte of data to the UART's transmit FIFO. 
 * This function blocks until the data is able to be written to the FIFO.  
 * @param uart - A pointer to the UART to write the data too.
 * @param data - The byte of data to write to the UART. 
 */ 

void rdw_uart_write_byte (volatile COBRA_UART_TypeDef *uart, unsigned char data)
{
  /* Wait until we have space in the transmit FIFO.  */
  while (uart->UART_STATUS & RDW_UART_TX_FULL)
    ;
  /* Write data into transmit FIFO.  */
  uart->UART_TX_DATA = data;
}



/** Read a byte of data from the UART's receive FIFO. 
 * This function blocks until data is available.  
 * @param uart - A pointer to the UART to read the data from.
 * @return The byte of data read from the UART's receive FIFO. 
 */
unsigned char rdw_uart_read_byte (volatile COBRA_UART_TypeDef *uart)
{
  /* Wait until some data is received.  */
//	unsigned char temp;
		while (uart->UART_STATUS & RDW_UART_RX_EMPTY);

    return (unsigned char)uart->UART_RX_DATA;
}


/** Wait until the UART's transmit FIFO is empty.
 * This function blocks until the TX FIFO is empty.
 * @param uart - A pointer to the UART.
 */
void rdw_uart_wait_until_tx_empty (volatile COBRA_UART_TypeDef *uart)
{
  /* Wait until the transmit FIFO is empty.  */
  while (!(uart->UART_STATUS & RDW_UART_TX_EMPTY))
    ;
}

void rdw_uart_cleat_interrupt(volatile COBRA_UART_TypeDef *uart, unsigned short data)
{
	   uart->UART_STATUS = data;
}


void rdw_uart_send_tx_data(volatile COBRA_UART_TypeDef *uart, unsigned char data)
{
	uart->UART_TX_DATA = data;
}

bool rdw_uart_is_tx_full(volatile COBRA_UART_TypeDef *uart)
{
		return  (uart->UART_STATUS & RDW_UART_TX_FULL);
}

//--------------------------------------------------------------------
void cobra_uart_write(uart_id uart_index, unsigned char ch)
{
		if(uart_index == M3_UART0)
		{
#if (APP_UART_TASK_ENABLE)
			  rdw_uart_tx_output_ch(ch);
#else			
				rdw_uart_write_byte(cobra_m3_uart0, ch);
#endif
		}
		else if(uart_index == M3_UART1)
		{
				rdw_uart_write_byte(cobra_m3_uart1, ch);
		}
//		else if(uart_index == M0_UART10)
//		{
//				rdw_uart_write_byte(cobra_m0_uart10, ch);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				rdw_uart_write_byte(cobra_m0_uart11, ch);
//		}
		
}

unsigned char cobra_uart_read(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
			return rdw_uart_read_byte(cobra_m3_uart0);
		}
		else if(uart_index == M3_UART1)
		{
			return rdw_uart_read_byte(cobra_m3_uart1);
		}
//		else if(uart_index == M0_UART10)
//		{
//			return rdw_uart_read_byte(cobra_m0_uart10);
//		}
//		else if(uart_index == M0_UART11)
//		{
//			return rdw_uart_read_byte(cobra_m0_uart11);
//		}
		return 0;
}

unsigned char cobra_uart_get_rx_data(uart_id uart_index)
{

		if(uart_index == M3_UART0)
		{
				return (cobra_m3_uart0->UART_RX_DATA & 0xff);
		}
		else if(uart_index == M3_UART1)
		{
				return (cobra_m3_uart1->UART_RX_DATA & 0xff);
		}
//		else if(uart_index == M0_UART10)
//		{
//				return (cobra_m0_uart10->UART_RX_DATA & 0xff);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				return (cobra_m0_uart11->UART_RX_DATA & 0xff);
//		}
		return 0;
}


unsigned short cobra_uart_get_interrupt_statue(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				return rdw_uart_get_interrupt_status(cobra_m3_uart0);
		}
		else if(uart_index == M3_UART1)
		{
				return rdw_uart_get_interrupt_status(cobra_m3_uart1);
		}
//		else if(uart_index == M0_UART10)
//		{
//				return rdw_uart_get_interrupt_status(cobra_m0_uart10);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				return rdw_uart_get_interrupt_status(cobra_m0_uart11);
//		}
		return 0;

}

void cobra_uart_clear_interrupt(uart_id uart_index, unsigned short data)
{
		if(uart_index == M3_UART0)
		{
				rdw_uart_cleat_interrupt(cobra_m3_uart0, data);
		}
		else if(uart_index == M3_UART1)
		{
				rdw_uart_cleat_interrupt(cobra_m3_uart1, data);
		}
//		else if(uart_index == M0_UART10)
//		{
//				rdw_uart_cleat_interrupt(cobra_m0_uart10, data);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				rdw_uart_cleat_interrupt(cobra_m0_uart11, data);
//		}
}

unsigned short cobra_uart_get_interrupt_control(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				return rdw_uart_get_interrupt_control(cobra_m3_uart0);
		}
		else if(uart_index == M3_UART1)
		{
				return rdw_uart_get_interrupt_control(cobra_m3_uart1);
		}
//		else if(uart_index == M0_UART10)
//		{
//				return rdw_uart_get_interrupt_control(cobra_m0_uart10);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				return rdw_uart_get_interrupt_control(cobra_m0_uart11);
//		}
		return 0;
}

void cobra_uart_clear_tx_buffer(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				rdw_uart_wait_until_tx_empty(cobra_m3_uart0);
		}
		else if(uart_index == M3_UART1)
		{
				rdw_uart_wait_until_tx_empty(cobra_m3_uart1);
		}
//		else if(uart_index == M0_UART10)
//		{
//				rdw_uart_wait_until_tx_empty(cobra_m0_uart10);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				rdw_uart_wait_until_tx_empty(cobra_m0_uart11);
//		}
}

void cobra_uart_send_tx_data(uart_id uart_index, uint8_t  data)
{
			if(uart_index == M3_UART0)
			{
					rdw_uart_send_tx_data(cobra_m3_uart0, data);
			}
			else if(uart_index == M3_UART1)
			{
					rdw_uart_send_tx_data(cobra_m3_uart1, data);
			}
//			else if(uart_index == M0_UART10)
//			{
//					rdw_uart_send_tx_data(cobra_m0_uart10, data);
//			}
//			else if(uart_index == M0_UART11)
//			{
//					rdw_uart_send_tx_data(cobra_m0_uart11, data);
//			}
}

void cobra_uart_config_uart_tx_interrupt(uart_id uart_index, bool en)
{
		if(uart_index == M3_UART0)
		{
				rdw_uart_set_txinterrup(cobra_m3_uart0, en);
		}
		else if(uart_index == M3_UART1)
		{
				rdw_uart_set_txinterrup(cobra_m3_uart1, en);
		}
//		else if(uart_index == M0_UART10)
//		{
//				rdw_uart_set_txinterrup(cobra_m0_uart10, en);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				rdw_uart_set_txinterrup(cobra_m0_uart11, en);
//		}

}

void cobra_uart_config_uart_rx_interrupt(uart_id uart_index, bool en)
{

		if(uart_index == M3_UART0)
		{
				rdw_uart_set_rxinterrup(cobra_m3_uart0, en);
		}
		else if(uart_index == M3_UART1)
		{
				rdw_uart_set_rxinterrup(cobra_m3_uart1, en);
		}
//		else if(uart_index == M0_UART10)
//		{
//				rdw_uart_set_rxinterrup(cobra_m0_uart10, en);
//		}
//		else if(uart_index == M0_UART11)
//		{
//				rdw_uart_set_rxinterrup(cobra_m0_uart11, en);
//		}	
}

void * cobra_uart_get_tx_address(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				return (void *)(&(cobra_m3_uart0->UART_TX_DATA));
		}
		else if(uart_index == M3_UART1)
		{
				return (void *)(&(cobra_m3_uart1->UART_TX_DATA));
		}
//		else if(uart_index == M0_UART10)
//		{
//				return (void *)(&(cobra_m0_uart10->UART_TX_DATA));
//		}
//		else if(uart_index == M0_UART11)
//		{
//				return (void *)(&(cobra_m0_uart11->UART_TX_DATA));
//		}	
		
		return NULL;
	
}

void * cobra_uart_get_rx_address(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				return (void *)(&(cobra_m3_uart0->UART_RX_DATA));
		}
		else if(uart_index == M3_UART1)
		{
				return (void *)(&(cobra_m3_uart1->UART_RX_DATA));
		}
//		else if(uart_index == M0_UART10)
//		{
//				return (void *)(&(cobra_m0_uart10->UART_RX_DATA));
//		}
//		else if(uart_index == M0_UART11)
//		{
//				return (void *)(&(cobra_m0_uart11->UART_RX_DATA));
//		}	
		return NULL;
}

//2019/10/25 add
//--------------------------------------------------------------------
//是否串口在发送中
//true 发送中
//false 没有发送
bool cobra_uart_tx_in_progress(uart_id uart_index)
{
		if(uart_index == M3_UART0)
		{
				return (rdw_uart_is_tx_full(cobra_m3_uart0));
		}
		else if(uart_index == M3_UART1)
		{
				return (rdw_uart_is_tx_full(cobra_m3_uart1));
		}
}

void cobra4_uart_set_receive_callback(p_uart_receive_callback func)
{
	  uart_receive_callback = func;
}

#if(( BLE_APP_UART == 1) && (UART_MODEL_TIMEOUT_ENABLE == 0))	

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// uart environment structure
static struct cobra_uart_env_tag uart_env;

#endif

#if COBRA_TRIM_EN
void app_trim_cobra_uart_baud_rate(uart_id uart_index, uint32_t value)
{
		if(uart_index == M3_UART0)
		{
				rdw_uart_trim_set_baud_rate(cobra_m3_uart0, value);
		}	
}
#endif

void cobra_uart_init_rate(uart_id uart_index, uint32_t rate, PARITY_MODE parity)
{
		unsigned int sys_temp;
	
	//	sys_temp = cobra_system_get_cpu_frequency();
	
		if(uart_index == M3_UART0)
		{
				sys_temp = cobra_system_get_m3_apb1_clk();
			
				rdw_uart_set_baud_rate(cobra_m3_uart0, sys_temp, rate);  
				/* Enable the UART TX and RX interrupt. */
				cobra_m3_uart0->UART_CONTROL |= 0x2000;   //reset fifo
			//1bit 停止位， Even 校验
      if(parity == PARITY_EVEN)
			{
				cobra_m3_uart0->UART_CONTROL 	|= RDW_UART_PARITY_EVEN;
			}
			else if(parity == PARITY_ODD)
			{
				cobra_m3_uart0->UART_CONTROL 	|= RDW_UART_PARITY_ODD;
			}
			else if(parity == PARITY_NULL)
			{
					cobra_m3_uart0->UART_CONTROL 	|= RDW_UART_PARITY_OFF;
			}
			
				cobra_m3_uart0->UART_CONTROL 	|= RDW_UART_1_STOP_BIT;
  
				rdw_uart_set_rx_fifo(cobra_m3_uart0, 2);
				rdw_uart_set_tx_fifo(cobra_m3_uart0, 2);
			
				rdw_uart_wait_until_tx_empty(cobra_m3_uart0);
				rdw_uart_enable(cobra_m3_uart0);
#if(( BLE_APP_UART == 1) && (UART_MODEL_TIMEOUT_ENABLE == 0))
         #if (COBRA_POWER_DOWN_ENABLE)
				    if(ble_deep_sleep == false)
		        {
				       uart_env.rx.callback = NULL;
				       uart_env.tx.callback = NULL;
				    }
		     #else
				     uart_env.rx.callback = NULL;
				     uart_env.tx.callback = NULL;				
		     #endif
#endif
						
				cobra_uart_config_uart_rx_interrupt(uart_index, 1);

				NVIC_SetPriority(COBRA4_UART0_IRQn, 1);
				NVIC_EnableIRQ(COBRA4_UART0_IRQn);

		}
		else if(uart_index == M3_UART1)
		{
				sys_temp = cobra_system_get_m3_apb1_clk();
			
				rdw_uart_set_baud_rate(cobra_m3_uart1, sys_temp, rate);  
				/* Enable the UART TX and RX interrupt. */
				cobra_m3_uart1->UART_CONTROL |= 0x2000;   //reset fifo
			
			  if(parity == PARITY_EVEN)
			  {
				   cobra_m3_uart1->UART_CONTROL 	|= RDW_UART_PARITY_EVEN;
			  }
			  else if(parity == PARITY_ODD)
			  {
				   cobra_m3_uart1->UART_CONTROL 	|= RDW_UART_PARITY_ODD;
			  }
			  else if(parity == PARITY_NULL)
			  {
					 cobra_m3_uart1->UART_CONTROL 	|= RDW_UART_PARITY_OFF;
			  }
				
			  cobra_m3_uart1->UART_CONTROL 	|= RDW_UART_1_STOP_BIT;
				
				rdw_uart_set_rx_fifo(cobra_m3_uart1, 0);
				rdw_uart_set_tx_fifo(cobra_m3_uart1, 0);
			
				rdw_uart_wait_until_tx_empty(cobra_m3_uart1);
				rdw_uart_enable(cobra_m3_uart1);

				cobra_uart_config_uart_rx_interrupt(uart_index, 1);

				NVIC_SetPriority(COBRA4_UART1_IRQn, 1);
				NVIC_EnableIRQ(COBRA4_UART1_IRQn);
		
		}		
}


#if(( BLE_APP_UART == 1) && (UART_MODEL_TIMEOUT_ENABLE == 0))

void cobra_m4_uart0_interrupt_process(void)
{
				volatile unsigned short uart_status;
				volatile unsigned short uart_control;
				uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
				cobra_uart_clear_interrupt(M3_UART0, uart_status);
			
				uart_control = cobra_uart_get_interrupt_control(M3_UART0);

	
				//TX interrupt
		
				//	if(uart_status&0x01)
	      if(uart_control & 0x100)
				{
							if(uart_status&0x01)
							{
							void (*callback) (void*, uint8_t) = NULL;
							void* data =NULL;
							if(uart_env.tx_busy) 
							{
									if(uart_env.tx_total  == uart_env.tx_index)
									{
												uart_env.tx_busy = 0;
												callback = uart_env.tx.callback;
												data     = uart_env.tx.dummy;

												if(callback != NULL)
												{
														// Clear callback pointer
														uart_env.tx.callback = NULL;
														uart_env.tx.dummy    = NULL;
														// Call handler
													  cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
														callback(data, RWIP_EIF_STATUS_OK);
													
														
												}
												else
												{
														cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
												}
									}
									else
									{
											//hci_uart_SendChar(RDW_UART0, *(uart_env.p_tx_buffer+uart_env.tx_index));
											cobra_uart_send_tx_data(M3_UART0, *(uart_env.p_tx_buffer+uart_env.tx_index));
											uart_env.tx_index++;
									}
							}
						}
				
	
			
				}
		
		    //RX full
				//   if(!(uart_status & 0x08))
				 if(uart_control & 0x200)
				 {
							if(!(uart_status & 0x08)) 
							{
										#if LOW_POWER_WAKE_UP_BY_UART
										if(uart_env.re_bz == 0)
										{
										   low_power_uart_receive_handle();
										   uart_env.re_bz=1;	
										}
										
										cobra_low_power_timer0_start();
										
										#endif
										
															
										if(uart_env.rx_busy) 
										{
												uart_env.p_buffer[uart_env.receive_index] = cobra_uart_get_rx_data(M3_UART0); //temp;
												uart_env.receive_index++;
												if(uart_env.receive_index == uart_env.total)
												{
														void (*callback) (void*, uint8_t) = NULL;
														void* data =NULL;
								
														uart_env.rx_busy = 0;
														callback = uart_env.rx.callback;
														data     = uart_env.rx.dummy;

														if(callback != NULL)
														{
																// Clear callback pointer
																uart_env.rx.callback = NULL;
																uart_env.rx.dummy    = NULL;

																// Call handler
																cobra_uart_config_uart_rx_interrupt(M3_UART0, 0);
																callback(data, RWIP_EIF_STATUS_OK);
														}
													
												}
									}
									else
									{
												cobra_uart_get_rx_data(M3_UART0);	
									}									
						}
						
				}
			
}


void profile_uart_clear(void)
{
    if(app_common_get_work_mode() == MODE_NORMAL)
    {	
		    cobra_uart_config_uart_rx_interrupt(M3_UART0, 0);
		    uart_env.rx.callback = NULL;
		    uart_env.rx_busy = 0;
    }
}

void profile_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
	  if(app_common_get_work_mode() == MODE_NORMAL)
    {	
				if(bufptr == NULL) return;
				if(size == 0)  return;
		//    if(callback == NULL) return;
			
				uart_env.rx.callback = callback;
				uart_env.rx.dummy    = dummy;


				if(uart_env.rx_busy) return;
			
				uart_env.p_buffer = bufptr;
				uart_env.total = size;
				uart_env.receive_index = 0;
				uart_env.rx_busy = 1;
				cobra_uart_config_uart_rx_interrupt(M3_UART0, 1);
    }
} 

void profile_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
   if(app_common_get_work_mode() == MODE_NORMAL)
   {	
			// Sanity check
				if(bufptr == NULL) return;
				if(size == 0)  return;
		//    if(callback == NULL) return;
			
				uart_env.tx.callback = callback;
				uart_env.tx.dummy    = dummy;
			
				if(uart_env.tx_busy) return;
				
				uart_env.p_tx_buffer = bufptr;
				uart_env.tx_total = size;
				uart_env.tx_index = 0;
				uart_env.tx_busy = 1;
			
			//	cobra_uart_send_tx_data(M3_UART0, '1');
			//  uart_env.tx_index++;
				cobra_uart_config_uart_tx_interrupt(M3_UART0, 1);
	 }
	 
}

#endif


#if CFG_APP_CONSOLE_CMD
#include "rdw_console.h"
void cobra_m3_uart0_interrupt(void)
{
		volatile unsigned short uart_status;
	  volatile unsigned short uart_control;
	  uint8_t   ch;
		uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
		cobra_uart_clear_interrupt(M3_UART0, uart_status);
	
	  uart_control = cobra_uart_get_interrupt_control(M3_UART0);
	
		if(uart_control & 0x200)
		{
					if(!(uart_status & 0x08)) 
					{
  							ch = cobra_uart_get_rx_data(M3_UART0);	
						    
						    if(uart_receive_callback != NULL)
			          {
					         uart_receive_callback(ch);
			          }

//#if  CFG_APP_CONSOLE_CMD
//								rdw_console_msg(ch);
//#endif	
								
					}
		}
	
}


#endif


void app_cobra_urat_print_hex(uart_id uart_index, unsigned char *data, int length)
{
		int i;
  
		for (i = 0; i < length; i++) 
				cobra_uart_write (uart_index, data[i]);
	
}

#endif


//**********************************************************************************************************************/
//**********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/


void cobra_m4_uart0_interrupt(void)
{
  if(app_common_get_work_mode() == MODE_HCITL)
	{
				volatile unsigned short uart_status;
				volatile unsigned short uart_control;
				uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
				cobra_uart_clear_interrupt(M3_UART0, uart_status);
				uart_control = cobra_uart_get_interrupt_control(M3_UART0);

				//TX interrupt
		
				//	if(uart_status&0x01)
	      if(uart_control & 0x100)
				{
							if(uart_status&0x01)
							{
							void (*callback) (void*, uint8_t) = NULL;
							void* data =NULL;
							if(cobra4_uart0_env.tx_busy) 
							{
									if(cobra4_uart0_env.tx_total  == cobra4_uart0_env.tx_index)
									{
												cobra4_uart0_env.tx_busy = 0;
												callback = cobra4_uart0_env.tx.callback;
												data     = cobra4_uart0_env.tx.dummy;

												if(callback != NULL)
												{
														// Clear callback pointer
														cobra4_uart0_env.tx.callback = NULL;
														cobra4_uart0_env.tx.dummy    = NULL;
														// Call handler
													  cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
														callback(data, RWIP_EIF_STATUS_OK);
													
														
												}
												else
												{
														cobra_uart_config_uart_tx_interrupt(M3_UART0, 0);
												}
									}
									else
									{
											//hci_uart_SendChar(RDW_UART0, *(uart_env.p_tx_buffer+uart_env.tx_index));
											cobra_uart_send_tx_data(M3_UART0, *(cobra4_uart0_env.p_tx_buffer+cobra4_uart0_env.tx_index));
											cobra4_uart0_env.tx_index++;
									}
							}
						}
				
	
			
				}
		
		    //RX full
				//   if(!(uart_status & 0x08))
				 if(uart_control & 0x200)
				 {
							if(!(uart_status & 0x08)) 
							{
										if(cobra4_uart0_env.rx_busy) 
										{
												cobra4_uart0_env.p_buffer[cobra4_uart0_env.receive_index] = cobra_uart_get_rx_data(M3_UART0); //temp;
												cobra4_uart0_env.receive_index++;
												if(cobra4_uart0_env.receive_index == cobra4_uart0_env.total)
												{
														void (*callback) (void*, uint8_t) = NULL;
														void* data =NULL;
								
														cobra4_uart0_env.rx_busy = 0;
														callback = cobra4_uart0_env.rx.callback;
														data     = cobra4_uart0_env.rx.dummy;

														if(callback != NULL)
														{
																// Clear callback pointer
																cobra4_uart0_env.rx.callback = NULL;
																cobra4_uart0_env.rx.dummy    = NULL;

																// Call handler
																cobra_uart_config_uart_rx_interrupt(M3_UART0, 0);
																callback(data, RWIP_EIF_STATUS_OK);
														}
													
												}
									}
									else
									{
												cobra_uart_get_rx_data(M3_UART0);	
									}
						}
						
				}
		}
	else
	{
#if(( BLE_APP_UART == 1) && (UART_MODEL_TIMEOUT_ENABLE == 0))	
        cobra_m4_uart0_interrupt_process();
#else		
				volatile unsigned short uart_status;
				volatile unsigned short uart_control;
				uint8_t   ch;
				uart_status = cobra_uart_get_interrupt_statue(M3_UART0);
				cobra_uart_clear_interrupt(M3_UART0, uart_status);
			
				uart_control = cobra_uart_get_interrupt_control(M3_UART0);
			
				if(uart_control & 0x200)
				{
							if(!(uart_status & 0x08)) 
							{
										ch = cobra_uart_get_rx_data(M3_UART0);	
								    
								    if(uart_receive_callback != NULL)
			              {
					             uart_receive_callback(ch);
			              }
								
//#if  CFG_APP_CONSOLE_CMD
//										rdw_console_msg(ch);
//#endif
										
							}
				}
#endif
				
	 }
}



#if  H4TL_SUPPORT	
void cobra4_uart0_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
	
    if(bufptr == NULL) return;
    if(size == 0)  return;
    if(callback == NULL) return;
	
    cobra4_uart0_env.rx.callback = callback;
    cobra4_uart0_env.rx.dummy    = dummy;


		if(cobra4_uart0_env.rx_busy) return;
	
		cobra4_uart0_env.p_buffer = bufptr;
	  cobra4_uart0_env.total = size;
		cobra4_uart0_env.receive_index = 0;
		cobra4_uart0_env.rx_busy = 1;
	  cobra_uart_config_uart_rx_interrupt(M3_UART0, 1);

} 

void cobra4_uart0_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
			// Sanity check
    if(bufptr == NULL) return;
    if(size == 0)  return;
    if(callback == NULL) return;
	
    cobra4_uart0_env.tx.callback = callback;
    cobra4_uart0_env.tx.dummy    = dummy;
	
		if(cobra4_uart0_env.tx_busy) return;
		
		cobra4_uart0_env.p_tx_buffer = bufptr;
	  cobra4_uart0_env.tx_total = size;
		cobra4_uart0_env.tx_index = 0;
		cobra4_uart0_env.tx_busy = 1;
	//  uart_env.tx_index++;
		cobra_uart_config_uart_tx_interrupt(M3_UART0, 1);
	
	
}

void cobra4_uart0_flow_on(void)
{
    // Configure modem (HW flow control enable)
    //uart_force_rts_setf(0);
		GLOBAL_INT_DISABLE();
	  cobra4_uart0_env.rx_busy = 0;
		cobra4_uart0_env.tx_busy = 0;
		cobra4_uart0_env.rx.callback = NULL;
    cobra4_uart0_env.tx.callback = NULL;
	
		GLOBAL_INT_RESTORE();
	
		
}

bool cobra4_uart0_flow_off(void)
{
    bool flow_off = true;
	
		GLOBAL_INT_DISABLE();
	  if(cobra4_uart0_env.rx_busy || cobra4_uart0_env.tx_busy)
			 flow_off = false; 
		GLOBAL_INT_RESTORE();
    return flow_off;
}

#endif

void cobra4_uart_dummy_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
	
}
void cobra4_uart_dummy_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
	
}

void cobra4_uart_dummy_flow_on(void)
{
	
}
bool cobra4_uart_dummy_flow_off(void)
{
	
}


void cobra_m4_uart1_interrupt(void)
{
		volatile unsigned short uart_status;
	  volatile unsigned short uart_control;
//	  uint8_t   ch;
		uart_status = cobra_uart_get_interrupt_statue(M3_UART1);
		cobra_uart_clear_interrupt(M3_UART1, uart_status);
	
	  uart_control = cobra_uart_get_interrupt_control(M3_UART1);
	
		if(uart_control & 0x200)
		{
					if(!(uart_status & 0x08)) 
					{
//  							ch = cobra_uart_get_rx_data(M3_UART0);	
////							rdw_uart_send_tx_data(ch);
//#if  CFG_APP_CONSOLE_CMD
//								rdw_console_msg(ch);
//#endif						
					}
		}
	
}

#if (COBRA_POWER_DOWN_ENABLE)	
void cobra_uart_power_down_init(void)
{
#if (BLE_APP_UART == 1)	
	  memset((void*)&uart_env,0,sizeof(uart_env));
#endif	

  	memset((void*)&cobra4_uart0_env,0,sizeof(cobra4_uart0_env));

#if defined(COBRA_UART1_EN)
	  memset((void*)&cobra4_uart1_env,0,sizeof(cobra4_uart1_env));
#endif
}
#endif


#if LOW_POWER_WAKE_UP_BY_UART

void uart_profile_print(void)
{
	 rdw_uart_wait_until_tx_empty(cobra_m3_uart0);
}

bool get_uart0_tx_doing_status(void)   //uart0 有数据需要发送 true,  没有数据需要发送 false
{
	
	if(!(cobra_uart_get_interrupt_statue(M3_UART0)& RDW_UART_TX_EMPTY))
	{
		  return true;
	}
	else 
	{
		  return false;
	}
	
}

bool get_uart0_rx_doing_status(void)   //uart0 有数据需要接受 true,  没有数据需要接受 false
{
	  if(!(cobra_uart_get_interrupt_statue(M3_UART0)& RDW_UART_RX_EMPTY))
	  {
		    return true;
	  }
	  else 
	  {
		   return false;
	  }
}


void low_power_uart_start_receive(void)
{
	  uart_env.re_bz = 0;
	  uart_env.re_index=uart_env.receive_index;
}

void low_power_uart_receive_handle(void)
{
	  rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
}

uint8_t get_uart0_bz(void)
{
	  return uart_env.re_bz;
}
#endif











