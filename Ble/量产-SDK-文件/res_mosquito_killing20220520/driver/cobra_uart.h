/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/2

	Description :   come from coney project



	Changed Log	:

		Liujian  2019/4/2		- Creation



*****************************************************************************/

#ifndef __COBRA_UART_H__
#define __COBRA_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_UART_EN

/* Status register bit masks.  */
#define RDW_UART_TX_EMPTY               0x0001                                  /**< Transmit FIFO empty. */
#define RDW_UART_TX_FULL                0x0002                                  /**< Transmit FIFO full. */
#define RDW_UART_TX_OVERFLOW            0x0004                                  /**< Transmit FIFO overflow. */
#define RDW_UART_RX_EMPTY               0x0008                                  /**< Receive FIFO empty. */
#define RDW_UART_RX_FULL                0x0010                                  /**< Receive FIFO full. */
#define RDW_UART_RX_OVERFLOW            0x0020                                  /**< Receive FIFO overflow. */
#define RDW_UART_RX_PARITY              0x0040                                  /**< Parity bit of received data. */
#define RDW_UART_RX_BREAK               0x0080                                  /**< Break received. */
#define RDW_UART_FRAMING_ERROR          0x0100                                  /**< Framing error detected. */            
#define RDW_UART_PARITY_ERROR           0x0200                                  /**< Parity error detected. */             
#define RDW_UART_RETRY_LIMIT_REACHED    0x0400                                  /**< Retry limit reached. ISO 7816 only. */
#define RDW_UART_RX_TIMEOUT             0x0800                                  /**< Receive timeout. ISO 7816 only. */
#define RDW_UART_TX_ALMOST_EMPTY        0x4000                                  /**< Transmit FIFO almost empty. */ 
#define RDW_UART_RX_ALMOST_FULL         0x8000                                  /**< Receive FIFO almost full. */

/* Control register bit masks.  */
#define RDW_UART_ENABLE_MASK            0x0001                                  /**< Enable flag bitmask. */
#define RDW_UART_STOP_BITS_MASK         0x0002                                  /**< Stop bits bitmask. */
#define RDW_UART_DATA_BITS_MASK         0x0004                                  /**< Data bits bitmask. */
#define RDW_UART_PARITY_BITS_MASK       0x0038                                  /**< Partiy bits bitmask. */
#define RDW_UART_FLOW_CONTROL_MASK      0x0040                                  /**< Flow control bitmask. */
#define RDW_UART_TX_BREAK_MASK          0x0080                                  /**< Transmit break bitmask. */ 
#define RDW_UART_TX_IE_MASK             0x0100                                  /**< Transmit interrupt enable bitmask. */
#define RDW_UART_RX_IE_MASK             0x0200                                  /**< Receive interrupt enable bitmask. */
#define RDW_UART_RX_BREAK_IE_MASK       0x0400                                  /**< Receive break interrupt bitmask. */
#define RDW_UART_RLR_IE_MASK            0x0800                                  /**< Retry limit reacher interrupt enable bitmask. ISO 7816 only. */
#define RDW_UART_ENDIAN_MASK            0x1000                                  /**< Endian bitmask. */
#define RDW_UART_RT_INT_MASK            0x4000                                  /**< Receive timeout interrupt enable bitmask. ISO 7816 only. */
#define RDW_UART_DUPLEX_MASK            0x8000                                  /**< Duplex bitmask. ISO 7816 only. */

/* Control register values.  */
#define RDW_UART_DISABLE                (0x0 << 0)                              /**< Disable UART. */
#define RDW_UART_ENABLE                 (0x1 << 0)                              /**< Enable UART. */
#define RDW_UART_1_STOP_BIT             (0x0 << 1)                              /**< 1 stop bit. */
#define RDW_UART_2_STOP_BITS            (0x1 << 1)                              /**< 2 stop bits. */
#define RDW_UART_8_DATA_BITS            (0x0 << 2)                              /**< 8 data bits. */
#define RDW_UART_7_DATA_BITS            (0x1 << 2)                              /**< 7 data bits. */
#define RDW_UART_PARITY_OFF             (0x0 << 3)                              /**< No partity bit. */
#define RDW_UART_PARITY_EVEN            (0x1 << 3)                              /**< Even parity. */
#define RDW_UART_PARITY_ODD             (0x2 << 3)                              /**< Odd parity. */
#define RDW_UART_PARITY_MARK            (0x3 << 3)                              /**< Transmit a mark as the parity bit. */
#define RDW_UART_PARITY_SPACE           (0x4 << 3)                              /**< Transmit a space as the parity bit. */
#define RDW_UART_PARITY_ISO_7816_DC     (0x5 << 3)                              /**< ISO 7816 parity direct convention. ISO 7816 only. */
#define RDW_UART_PARITY_ISO_7816_IC     (0x6 << 3)                              /**< ISO 7816 parity inverse convention. ISO 7816 only. @since rev 5. */
#define RDW_UART_FLOW_CONTROL_OFF       (0x0 << 6)                              /**< No flow control. */
#define RDW_UART_FLOW_CONTROL_RTS_CTS   (0x1 << 6)                              /**< RTS/CTS flow control. */
#define RDW_UART_TX_BREAK_DISABLE       (0x0 << 7)                              /**< Do not transit break. */
#define RDW_UART_TX_BREAK_ENABLE        (0x1 << 7)                              /**< Transmit break. */
#define RDW_UART_TX_INT_DISABLE         (0x0 << 8)                              /**< Disable transmit interrupt . */
#define RDW_UART_TX_INT_ENABLE          (0x1 << 8)                              /**< Enable transmit interrupt. */
#define RDW_UART_RX_INT_DISABLE         (0x0 << 9)                              /**< Disable receive interrupt. */ 
#define RDW_UART_RX_INT_ENABLE          (0x1 << 9)                              /**< Enable receive interrupt. */   
#define RDW_UART_RX_BREAK_INT_DISABLE   (0x0 << 10)                             /**< Disable receive break interrupt . */
#define RDW_UART_RX_BREAK_INT_ENABLE    (0x1 << 10)                             /**< Enable receive break interrupt. */  
#define RDW_UART_RLR_INT_DISABLE        (0x0 << 11)                             /**< Disable retry limit reached interrupt. */
#define RDW_UART_RLR_INT_ENABLE         (0x1 << 11)                             /**< Enable retry limit reached interrupt. ISO 7816 only. */
#define RDW_UART_LSB_FIRST              (0x0 << 12)                             /**< Transmit least-significant bit first. */
#define RDW_UART_MSB_FIRST              (0x1 << 12)                             /**< Transmit most-significant bit first. @since rev 3. */
#define RDW_UART_RESET_FIFOS            (0x1 << 13)                             /**< Reset FIFOs. */ 
#define RDW_UART_RT_INT_DISABLE         (0x0 << 14)                             /**< Receive timeout interrupt disable. */
#define RDW_UART_RT_INT_ENABLE          (0x1 << 14)                             /**< Receive timeout interrupt enable. ISO 7816 only. @since rev 7. */
#define RDW_UART_FULL_DUPLEX            (0x0 << 15)                             /**< Full duplex. */
#define RDW_UART_HALF_DUPLEX            (0x1 << 15)                             /**< Half duplex. ISO 7816 only. */

/* FIFO threshold flags. */
#define RDW_UART_TXAE_INT_DISABLE       (0x0 << 15)                             /**< Disable transmit FIFO almost empty interrupt. */
#define RDW_UART_TXAE_INT_ENABLE        (0x1 << 15)                             /**< Enable transmit FIFO almost empty interrupt. */
#define RDW_UART_RXAF_INT_DISABLE       (0x0 << 15)                             /**< Disable receive FIFO almost full interrupt. */ 
#define RDW_UART_RXAF_INT_ENABLE        (0x1 << 15)                             /**< Enable receive FIFO almost full interrupt. */  



typedef struct cobra_uart_type
{
	__attribute__ ((aligned (4))) volatile unsigned int UART_TX_DATA;                  /**< Transmit data.  */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_DATA;                  /**< Received data.  */
  __attribute__ ((aligned (4))) volatile unsigned int UART_STATUS;                   /**< Status register.  */
  __attribute__ ((aligned (4))) volatile unsigned int UART_CONTROL;                  /**< Control register.  */
  __attribute__ ((aligned (4))) volatile unsigned int UART_CYCLES_PER_BIT;           /**< Number of clock cycles per bit.  */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RETRIES;                  /**< Number of RETRIES and delay between retries. ISO 7816 only. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_TXAE_THRESH;              /**< Transmit FIFO almost empty threshold. @since rev 4. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RXAF_THRESH;              /**< Receive FIFO almost full threshold. @since rev 4. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_TX_ERROR_START;           /**< Start of transmit error window. ISO 7816 only. @since rev 5. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_TX_ERROR_LENGTH;          /**< Length of transmit error window. ISO 7816 only. @since rev 5. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_ERROR_START;           /**< Start of receive error window. ISO 7816 only. @since rev 5. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_ERROR_LENGTH;          /**< Length of receive error window. ISO 7816 only. @since rev 5. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_TX_COUNT;                 /**< Count of used entries in TX FIFO. @since rev 6. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_COUNT;                 /**< Count of used entries in RX FIFO. @since rev 6. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_TX_GUARD;                 /**< Guard time between transmission. @since rev 7. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_TIMEOUT;               /**< Receive timeout. ISO 7816 only. @since rev 7. */
  __attribute__ ((aligned (4))) volatile unsigned int UART_RX_TIMEOUT_COUNT;         /**< Receive timeout counter. ISO 7816 only. @since rev 7. */

} COBRA_UART_TypeDef;

/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */
/// UART TX RX Channel
struct uart_txrxchannel
{
    /// call back function pointer
    void (*callback) (void*, uint8_t);
    /// Dummy data pointer returned to callback when operation is over.
    void* dummy;
};

/// UART environment structure
struct cobra_uart_env_tag
{
    /// tx channel
    struct uart_txrxchannel tx;
    /// rx channel
    struct uart_txrxchannel rx;
    /// error detect
    uint8_t errordetect;
    /// external wakeup
    bool ext_wakeup;
	
	  uint32_t  receive_index;
	  uint32_t  total;
		uint8_t  *p_buffer;
		uint8_t   rx_busy;
	
		
		uint32_t   tx_index;
		uint32_t   tx_total;
		uint8_t  	*p_tx_buffer;
		uint8_t   tx_busy;
	#if LOW_POWER_WAKE_UP_BY_UART
		uint8_t  re_bz;
	  uint32_t re_index;
		uint32_t re_total;
	#endif	
		
	
};

typedef enum {
		PARITY_EVEN,
	  PARITY_ODD,
	  PARITY_NULL,	  
}PARITY_MODE;

typedef enum uart_id_temp{
	M3_UART0,
	M3_UART1,
	M4_NULL,
//	M0_UART10,
//	M0_UART11,
}uart_id;

typedef void (*p_uart_receive_callback)(unsigned char ch);

void cobra4_uart_set_receive_callback(p_uart_receive_callback func);

void cobra_uart_init(void);
void cobra_uart_init_rate(uart_id uart_index, uint32_t rate, PARITY_MODE parity);

unsigned char cobra_uart_read(uart_id uart_index);
void cobra_uart_write(uart_id uart_index, unsigned char ch);

void * cobra_uart_get_rx_address(uart_id uart_index);
void * cobra_uart_get_tx_address(uart_id uart_index);


void profile_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void profile_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void profile_uart_clear(void);

void cobra_uart_clear_tx_buffer(uart_id uart_index);

void app_cobra_urat_print_hex(uart_id uart_index, unsigned char *data, int length);



#if COBRA_TRIM_EN
void app_trim_cobra_uart_baud_rate(uart_id uart_index, uint32_t value);
#endif

void cobra_m3_uart1_flow_on(void);
bool cobra_m3_uart1_flow_off(void);
void cobra_m3_uart1_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void cobra_m3_uart1_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);

#if  H4TL_SUPPORT	
void cobra4_uart0_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void cobra4_uart0_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void cobra4_uart0_flow_on(void);
bool cobra4_uart0_flow_off(void);
#endif

void cobra4_uart_dummy_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void cobra4_uart_dummy_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void cobra4_uart_dummy_flow_on(void);
bool cobra4_uart_dummy_flow_off(void);

//2019/10/25 add
//--------------------------------------------------------------------
//是否串口在发送中
//true 发送中
//false 没有发送
bool cobra_uart_tx_in_progress(uart_id uart_index);

void cobra_uart_config_uart_tx_interrupt(uart_id uart_index, bool en);

void cobra_uart_send_tx_data(uart_id uart_index, uint8_t  data);
void cobra_uart_config_uart_rx_interrupt(uart_id uart_index, bool en);
unsigned short cobra_uart_get_interrupt_statue(uart_id uart_index);
void cobra_uart_clear_interrupt(uart_id uart_index, unsigned short data);
unsigned short cobra_uart_get_interrupt_control(uart_id uart_index);
unsigned char cobra_uart_get_rx_data(uart_id uart_index);
//#define M0_UART10_BASE         (0x40120000UL + 0x0000UL)
//#define M0_UART11_BASE         (0x40120000UL + 0x1000UL)

#if LOW_POWER_WAKE_UP_BY_UART

void uart_profile_print(void);
bool get_uart0_tx_doing_status(void);
void low_power_uart_start_receive(void);
bool get_uart0_rx_doing_status(void);

#endif

#define M3_UART0_BASE     		 (0x400A0000UL + 0x0000UL)
#define M3_UART1_BASE     		 (0x400A0000UL + 0x1000UL)

#endif   //COBRA_UART_EN



#endif

