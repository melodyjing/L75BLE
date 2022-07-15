/******************************************************************************
	File Name	:	app_uart_fifo.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/10/25

	Description :   

	Changed Log	:
		Liujian  2019/10/25			- Creation

*****************************************************************************/

#ifndef __APP_UART_FIFO_H__
#define __APP_UART_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>
#include <stdlib.h>
#include <cobra_config.h>

#if COBRA4_NEW_UART_DRIVER	
	
typedef struct
{
    uint8_t * rx_buf;      /**< Pointer to the RX buffer. */
    uint32_t  rx_buf_size; /**< Size of the RX buffer. */
    uint8_t * tx_buf;      /**< Pointer to the TX buffer. */
    uint32_t  tx_buf_size; /**< Size of the TX buffer. */
} app_uart_buffers_t;

uint32_t app_uart_init(app_uart_buffers_t *     p_buffers);	
/**@brief Function for getting a byte from the UART.
 *
 * @details This function will get the next byte from the RX buffer. If the RX buffer is empty
 *          an error code will be returned and the app_uart module will generate an event upon
 *          reception of the first byte which is added to the RX buffer.
 *
 * @param[out] p_byte    Pointer to an address where next byte received on the UART will be copied.
 *
 * @retval NRF_SUCCESS          If a byte has been received and pushed to the pointer provided.
 * @retval NRF_ERROR_NOT_FOUND  If no byte is available in the RX buffer of the app_uart module.
 */
uint32_t app_uart_get(uint8_t * p_byte);

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
uint32_t app_uart_put(uint8_t byte);

/**@brief Function for flushing the RX and TX buffers (Only valid if FIFO is used).
 *        This function does nothing if FIFO is not used.
 *
 * @retval  NRF_SUCCESS  Flushing completed (Current implementation will always succeed).
 */
uint32_t app_uart_flush(void);

/**@brief Function for closing the UART module.
 *
 * @retval  NRF_SUCCESS             If successfully closed.
 * @retval  NRF_ERROR_INVALID_PARAM If an invalid user id is provided or the user id differs from
 *                                  the current active user.
 */
uint32_t app_uart_close(void);


void app_uart_send_done_event(void);

void app_uart_receive_done_event(uint8_t byte);

#endif

#ifdef __cplusplus
}
#endif

#endif
