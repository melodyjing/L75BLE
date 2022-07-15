/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_i2c.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/28

	Description :   


	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/

#ifndef __COBRA_I2C_H__
#define __COBRA_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_I2C_EN


typedef struct cobra_i2c_type
{
	__attribute__ ((aligned (4))) unsigned int I2C_TX_DATA;                                    /**< Transmit data.  */
  __attribute__ ((aligned (4))) unsigned int I2C_RX_DATA;                                    /**< Received data.  */
  __attribute__ ((aligned (4))) unsigned int I2C_STATUS;                                     /**< Status register.  */
  __attribute__ ((aligned (4))) unsigned int I2C_CONTROL;                                    /**< Control register.  */
  __attribute__ ((aligned (4))) unsigned int I2C_CYCLES_PER_BIT;                             /**< Number of clock cycles per bit.  */
  __attribute__ ((aligned (4))) unsigned int I2C_ADDRESS;                                    /**< Address when a slave device.  */
  __attribute__ ((aligned (4))) unsigned int I2C_TX_HOLD_CYCLES;                             /**< Transmit SDA hold time.  */
  __attribute__ ((aligned (4))) unsigned int I2C_RX_HOLD_CYCLES;                             /**< Receive SDA hold time.  */
  __attribute__ ((aligned (4))) unsigned int I2C_FILTER_CYCLES;                              /**< Input filter cycle count.  */
  __attribute__ ((aligned (4))) unsigned int I2C_TXAE_THRESH;                                /**< Transmit FIFO almost empty threshold. @since rev 7. */
  __attribute__ ((aligned (4))) unsigned int I2C_RXAF_THRESH;                                /**< Receive FIFO almost full threshold. @since rev 7. */
  __attribute__ ((aligned (4))) unsigned int I2C_TX_COUNT;                                   /**< Count of used entries in TX FIFO. @since rev 9. */
  __attribute__ ((aligned (4))) unsigned int I2C_RX_COUNT;                                   /**< Count of used entries in RX FIFO. @since rev 9. */

} COBRA_I2C_TypeDef; 


/* Status register bit masks.  */
#define RDW_I2C_TX_EMPTY                0x0001                                  /**< Transmit buffer empty status register bitmask */
#define RDW_I2C_TX_FULL                 0x0002                                  /**< Transmit buffer full status register bitmask */
#define RDW_I2C_TX_OVERFLOW             0x0004                                  /**< Transmit buffer overflow status register bitmask */
#define RDW_I2C_RX_EMPTY                0x0008                                  /**< Receive buffer empty status register bitmask */              
#define RDW_I2C_RX_FULL                 0x0010                                  /**< Receive buffer full status register bitmask */    
#define RDW_I2C_RX_OVERFLOW             0x0020                                  /**< Receive buffer overflow status register bitmask */
#define RDW_I2C_RX_UNDERFLOW            0x0040                                  /**< Receive buffer underflow status register bitmask */
#define RDW_I2C_ARB_LOST                0x0080                                  /**< Arbitration lost status register bitmask */ 
#define RDW_I2C_NACK                    0x0100                                  /**< NACK status register bitmask */
#define RDW_I2C_START                   0x0200                                  /**< START condition status register bitmask */
#define RDW_I2C_STOP                    0x0400                                  /**< STOP condition status register bitmask */
#define RDW_I2C_TX_UNDERFLOW            0x0800                                  /**< Transmit buffer underflow status register bitmask */
#define RDW_I2C_IF_BUSY                 0x1000                                  /**< I2C interface busy status register bitmask */
#define RDW_I2C_BUS_BUSY                0x2000                                  /**< I2C bus busy status register bitmask */
#define RDW_I2C_TX_ALMOST_EMPTY         0x4000                                  /**< Transmit FIFO almost empty. */ 
#define RDW_I2C_RX_ALMOST_FULL          0x8000                                  /**< Receive FIFO almost full. */

/* Control register bit masks.  */
#define RDW_I2C_ENABLE_MASK             0x0001                                  /**< Enable control register bitmask */
#define RDW_I2C_RESET_FIFOS_MASK        0x0002                                  /**< Reset fifo control register bitmask */
#define RDW_I2C_MASTER_SLAVE_MASK       0x0004                                  /**< Master/slave control register bitmask */
#define RDW_I2C_NACK_MASK               0x0008                                  /**< NACK control register bitmask */
#define RDW_I2C_TX_IE_MASK              0x0010                                  /**< Transmit interrupt enable control register bitmask */
#define RDW_I2C_RX_IE_MASK              0x0020                                  /**< Receive interrupt enable control register bitmask */
#define RDW_I2C_ARB_LOST_IE_MASK        0x0040                                  /**< Aribtration lost interrupt enable control register bitmask */
#define RDW_I2C_NACK_IE_MASK            0x0080                                  /**< NACK interrupt enable control register bitmask */
#define RDW_I2C_START_IE_MASK           0x0100                                  /**< START condition interrupt enable control register bitmask */
#define RDW_I2C_STOP_IE_MASK            0x0200                                  /**< STOP condition interrupt enable control register bitmask */
#define RDW_I2C_CLOCK_STRETCH_MASK      0x0400                                  /**< Clock stretching control register bitmask */
#define RDW_I2C_RESET_FSM_MASK          0x0800                                  /**< Reset FSM control register bitmask */

/* Control register values.  */
#define RDW_I2C_DISABLE                 (0x0 << 0)                              /**< I2C disable */
#define RDW_I2C_ENABLE                  (0x1 << 0)                              /**< I2C enable */
#define RDW_I2C_RESET_FIFOS             (0x1 << 1)                              /**< Reset I2C FIFOs */
#define RDW_I2C_MASTER                  (0x0 << 2)                              /**< Master mode */
#define RDW_I2C_SLAVE                   (0x1 << 2)                              /**< Slave mode */ 
#define RDW_I2C_TX_ACK                  (0x0 << 3)                              /**< Transmit ACK */
#define RDW_I2C_TX_NACK                 (0x1 << 3)                              /**< Transmit NACK */
#define RDW_I2C_TX_INT_DISABLE          (0x0 << 4)                              /**< Transmit interrupt disable */
#define RDW_I2C_TX_INT_ENABLE           (0x1 << 4)                              /**< Transmit interrupt enable */
#define RDW_I2C_RX_INT_DISABLE          (0x0 << 5)                              /**< Receive interrupt disable */  
#define RDW_I2C_RX_INT_ENABLE           (0x1 << 5)                              /**< Receive interrupt enable */   
#define RDW_I2C_ARB_LOST_INT_DISABLE    (0x0 << 6)                              /**< Arbitration lost interrupt disable */
#define RDW_I2C_ARB_LOST_INT_ENABLE     (0x1 << 6)                              /**< Arbitration lost interrupt enable */ 
#define RDW_I2C_NACK_INT_DISABLE        (0x0 << 7)                              /**< NACK interrupt disable */
#define RDW_I2C_NACK_INT_ENABLE         (0x1 << 7)                              /**< NACK interrupt enable */ 
#define RDW_I2C_START_INT_DISABLE       (0x0 << 8)                              /**< START condition interrupt disable */
#define RDW_I2C_START_INT_ENABLE        (0x1 << 8)                              /**< START condition interrupt enable */ 
#define RDW_I2C_STOP_INT_DISABLE        (0x0 << 9)                              /**< STOP condition interrupt disable */
#define RDW_I2C_STOP_INT_ENABLE         (0x1 << 9)                              /**< STOP condition interrupt enable */ 
#define RDW_I2C_CLOCK_STRETCH_DISABLE   (0x0 << 10)                             /**< Disable clock stretching */
#define RDW_I2C_CLOCK_STRETCH_ENABLE    (0x1 << 10)                             /**< Enable clock stretching */
#define RDW_I2C_RESET_FSM               (0x1 << 11)                             /**< Reset FSM */

/* Control byte values.  */
#define RDW_I2C_TX_START                0x01                                    /**< Generate START condition */
#define RDW_I2C_TX_STOP                 0x02                                    /**< Generate STOP condition */
#define RDW_I2C_ACK_LAST                0x04                                    /**< ACK last byte */
#define RDW_I2C_NO_ADDRESS              0x08                                    /**< No address byte */
#define RDW_I2C_TX_STOP_ON_NACK         0x10                                    /**< Stop on NACK */

/* Address byte values.  */
#define RDW_I2C_WRITE                   0x00                                    /**< Write transaction */
#define RDW_I2C_READ                    0x01                                    /**< Read transaction */

/* FIFO threshold flags. */
#define RDW_I2C_TXAE_INT_DISABLE        (0x0 << 15)                             /**< Disable transmit FIFO almost empty interrupt. */
#define RDW_I2C_TXAE_INT_ENABLE         (0x1 << 15)                             /**< Enable transmit FIFO almost empty interrupt. */
#define RDW_I2C_RXAF_INT_DISABLE        (0x0 << 15)                             /**< Disable receive FIFO almost full interrupt. */ 
#define RDW_I2C_RXAF_INT_ENABLE         (0x1 << 15)                             /**< Enable receive FIFO almost full interrupt. */  



#define COBRA_I2C0_BASE				(0x400A6000UL)
#define COBRA_I2C1_BASE				(0x400A7000UL)


typedef enum {
	 M3_I2C0,
	 M3_I2C1
}rdw_i2c;


typedef enum {
	SLAVE_WRITE,
	SLAVE_READ,
	SLAVE_NULL

}i2c_slave_mode;

#ifdef COBRA_I2C_NEW
extern volatile COBRA_I2C_TypeDef * cobra_i2c0;
extern volatile COBRA_I2C_TypeDef * cobra_i2c1;
#endif

void cobra_i2c_init(void);
void app_m3_i2c_init(void);
int cobra_i2c_write_byte(rdw_i2c index, unsigned char data);
int cobra_i2c_read_byte (rdw_i2c index);

uint16_t cobra_i2c_get_status(rdw_i2c index);

void  cobra_i2c_set_bit_rate(rdw_i2c index, long i2c_clock_frequency, long bit_rate);

void cobra_i2c_maste_enable(rdw_i2c index);
#ifdef COBRA_I2C_NEW
int rdw_i2c_write_byte (volatile COBRA_I2C_TypeDef *i2c, unsigned char data);
int rdw_i2c_read_byte (volatile COBRA_I2C_TypeDef *i2c);
void rdw_i2c_set_bit_rate (volatile COBRA_I2C_TypeDef *i2c, long i2c_clock_frequency, long bit_rate);
void rdw_i2c_master_enable (volatile COBRA_I2C_TypeDef *i2c);
#endif

#endif   //COBRA_I2C_EN



#endif

