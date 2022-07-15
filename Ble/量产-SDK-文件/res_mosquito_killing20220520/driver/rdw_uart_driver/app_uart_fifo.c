/******************************************************************************
	File Name	:	app_uart_fifo.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/10/25

	Description :   
		cobra4 的 UART FIFO 处理方式
	Changed Log	:
		Liujian  2019/10/25			- Creation

*****************************************************************************/
#include "app_uart_fifo.h"
#include "app_fifo.h"
#include "cobra_uart.h"

#if COBRA4_NEW_UART_DRIVER
static app_fifo_t                  m_rx_fifo;                               /**< RX FIFO buffer for storing data received on the UART until the application fetches them using app_uart_get(). */
static app_fifo_t                  m_tx_fifo;                               /**< TX FIFO buffer for storing data to be transmitted on the UART when TXD is ready. Data is put to the buffer on using app_uart_put(). */

static uint8_t tx_buffer[1];
static uint8_t rx_buffer[1];

uint32_t app_uart_init(app_uart_buffers_t * p_buffers)
{
		app_fifo_init(&m_rx_fifo, p_buffers->rx_buf, p_buffers->rx_buf_size);
		app_fifo_init(&m_tx_fifo, p_buffers->tx_buf, p_buffers->tx_buf_size);
		return 0;
}


/**@brief Function for putting a byte on the UART.
 *
 * @details This call is non-blocking.
 *
 * @param[in] byte   Byte to be transmitted on the UART.
 *
 * @retval NRF_SUCCESS        If the byte was successfully put on the TX buffer for transmission.
 * @retval NRF_ERROR_NO_MEM   If no more space is available in the TX buffer.
 *                            NRF_ERROR_NO_MEM may occur if flow control is enabled and CTS signal
 *                            is high for a long period and the buffer fills up.
 * @retval NRF_ERROR_INTERNAL If UART driver reported error.
 */
 
uint32_t app_uart_put(uint8_t byte)
{
		 uint32_t err_code;
		 uint8_t  temp;
		 app_fifo_put(&m_tx_fifo, byte);
     if (!cobra_uart_tx_in_progress(M3_UART0))
		 {
					cobra_uart_config_uart_tx_interrupt(M3_UART0, 1);
		 } 
     return err_code;	
}

void app_uart_send_done_event(void)
{
		while(!cobra_uart_tx_in_progress(M3_UART0))
		{
				if (app_fifo_get(&m_tx_fifo, tx_buffer))
				{
						cobra_uart_send_tx_data(M3_UART0, tx_buffer[0]);
						cobra_uart_config_uart_tx_interrupt(M3_UART0, 1);
				}
				else
				{
						break;
				}
		}
		
}

void app_uart_receive_done_event(uint8_t byte)
{	
		app_fifo_put(&m_rx_fifo, byte);
}

uint32_t app_uart_get(uint8_t * p_byte)
{
    uint32_t err_code;

    err_code =  app_fifo_get(&m_rx_fifo, p_byte);

    return err_code;
}

#endif

