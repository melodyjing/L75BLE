#ifndef _WM_8978_H
#define _WM_8978_H

#include <cobra_config.h>
#include <stdint.h>
#include <stdbool.h>
#if COBRA_I2C_EN
#include "cobra_i2c.h"
#endif
/* Status register bits.  */
#define RDW_I2S_TX_EMPTY                0x0001
#define RDW_I2S_TX_FULL                 0x0002
#define RDW_I2S_TX_OVERFLOW             0x0004
#define RDW_I2S_TX_UNDERRUN             0x0008
#define RDW_I2S_TX_ALMOST_EMPTY         0x0010
#define RDW_I2S_RX_EMPTY                0x0020
#define RDW_I2S_RX_FULL                 0x0040
#define RDW_I2S_RX_OVERFLOW             0x0080
#define RDW_I2S_RX_UNDERRUN             0x0100
#define RDW_I2S_RX_ALMOST_FULL          0x0200

/* Control register bit masks.  */
#define RDW_I2S_TX_ENABLE_MASK          0x0001
#define RDW_I2S_RX_ENABLE_MASK          0x0002

/* Control register values.  */
#define RDW_I2S_TX_DISABLE              (0x0 << 0)
#define RDW_I2S_TX_ENABLE               (0x1 << 0)
#define RDW_I2S_RX_DISABLE              (0x0 << 1)
#define RDW_I2S_RX_ENABLE               (0x1 << 1)
#define RDW_I2S_TX_FIFO_FLUSH           (0x1 << 2)
#define RDW_I2S_RX_FIFO_FLUSH           (0x1 << 3)
#define RDW_I2S_TX_INT_DISABLE          (0x0 << 4)
#define RDW_I2S_TX_INT_ENABLE           (0x1 << 4)
#define RDW_I2S_RX_INT_DISABLE          (0x0 << 5)
#define RDW_I2S_RX_INT_ENABLE           (0x1 << 5)
#define RDW_I2S_TX_ER_INT_DISABLE       (0x0 << 6)
#define RDW_I2S_TX_ER_INT_ENABLE        (0x1 << 6)
#define RDW_I2S_RX_ER_INT_DISABLE       (0x0 << 7)
#define RDW_I2S_RX_ER_INT_ENABLE        (0x1 << 7)

/* Bits per word register values */
#define RDW_I2S_TX_8_BIT                (8 << 0)
#define RDW_I2S_TX_16_BIT               (16 << 0)
#define RDW_I2S_TX_20_BIT               (20 << 0)
#define RDW_I2S_TX_24_BIT               (24 << 0)
#define RDW_I2S_TX_32_BIT               (32 << 0)
#define RDW_I2S_RX_8_BIT                (8 << 8)
#define RDW_I2S_RX_16_BIT               (16 << 8)
#define RDW_I2S_RX_20_BIT               (20 << 8)
#define RDW_I2S_RX_24_BIT               (24 << 8)
#define RDW_I2S_RX_32_BIT               (32 << 8)

/* FIFO threshold flags. */
#define RDW_I2S_TXAE_INT_DISABLE        (0x0 << 15)                             /**< Disable transmit FIFO almost empty interrupt. */
#define RDW_I2S_TXAE_INT_ENABLE         (0x1 << 15)                             /**< Enable transmit FIFO almost empty interrupt. */
#define RDW_I2S_RXAF_INT_DISABLE        (0x0 << 15)                             /**< Disable receive FIFO almost full interrupt. */
#define RDW_I2S_RXAF_INT_ENABLE         (0x1 << 15)                             /**< Enable receive FIFO almost full interrupt. */

/** eSi-I2S registers. */
typedef struct rdw_i2s {
    __attribute__ ((aligned (4))) unsigned int TX_DATA;                                    /**< Transmit data.  */
    __attribute__ ((aligned (4))) unsigned int RX_DATA;                                    /**< Received data.  */
    __attribute__ ((aligned (4))) unsigned int STATUS;                                     /**< Status register.  */
    __attribute__ ((aligned (4))) unsigned int CONTROL;                                    /**< Control register.  */
    __attribute__ ((aligned (4))) unsigned int BITS_PER_WORD;                              /**< Bits per word register.  */
    __attribute__ ((aligned (4))) unsigned int TXAE_THRESH;                                /**< Transmit FIFO almost empty threshold. @since rev 6. */
    __attribute__ ((aligned (4))) unsigned int RXAF_THRESH;                                /**< Receive FIFO almost full threshold. @since rev 6. */
} RDW_I2S_T;

static inline void rdw_i2s_control(volatile RDW_I2S_T *i2s, uint16_t ctl)
{
    i2s->CONTROL = ctl;
}

static inline uint16_t rdw_i2s_status(volatile RDW_I2S_T *i2s)
{
    return i2s->STATUS;
}

static inline void rdw_i2s_bits_per_word(volatile RDW_I2S_T *i2s,uint32_t bits_per_word)
{
    i2s->BITS_PER_WORD = bits_per_word;
}

static inline uint32_t rdw_i2s_read_data(volatile RDW_I2S_T *i2s)
{
    return i2s->RX_DATA;
}

static inline void rdw_i2s_write_data(volatile RDW_I2S_T *i2s,uint32_t data)
{
    i2s->TX_DATA = data;
}




#define	RDW_I2S_LEFT				(0x0 << 8)
#define	RDW_I2S_RIGHT					(0x1 << 8)

#define	RDW_I2S_MASTER					(0x0 << 9)
#define	RDW_I2S_SLAVE					(0x1 << 9)

#define	RDW_I2S_MODE_PHILIPS			(0x00 << 10)
#define	RDW_I2S_MODE_MSB				(0x01 << 10)
#define	RDW_I2S_MODE_PCM				(0x10 << 10)
//#define	RDW_I2S_MODE_PHILIPS2			(0x11 << 10)

#define	RDW_I2S_SD_TX_MASK				(0x1 << 12)

/*VM8978 MODE*/
#define	VM8978_MSB_MODE						0UL
#define VM8978_LSB_MODE						1UL
#define	VM8978_I2S_MODE						2UL
#define VM8978_PCM_MODE						3UL

/*VM8978 LEN*/
#define	VM8978_LEN_16							0UL
#define VM8978_LEN_20							1UL
#define	VM8978_LEN_24							2UL
#define VM8978_LEN_32							3UL

#define RDW_I2S0_BASE				(0x400CE000UL + 0x0000UL)
#define RDW_I2S1_BASE				(0x400CF000UL + 0x0000UL)
#define RDW_I2S0            ((volatile RDW_I2S_T   *) RDW_I2S0_BASE )
#define RDW_I2S1            ((volatile RDW_I2S_T   *) RDW_I2S1_BASE )
	
#if COBRA_I2C_EN
extern uint8_t WM8978_Init_Send(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster);
extern uint8_t WM8978_Init_Send_PLL(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster);
extern uint8_t WM8978_Init_Receive(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster);
extern uint8_t WM8978_Init_Receive_Send(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster);
void WM8978_HPvol_Set(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t voll,uint8_t volr);
void WM8978_I2S_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t fmt,uint8_t len);
void WM8978_ADDA_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t dacen,uint8_t adcen);
void WM8978_Input_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t micen,uint8_t lineinen,uint8_t auxen);
extern void i2s_received_test(uint32_t *databuffer,uint32_t datanum);
#endif
extern void i2s_send_test(uint32_t *databuffer,uint32_t datanum);
#endif
