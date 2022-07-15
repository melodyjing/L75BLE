/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_spi.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/11

	Description :   
				come from coney project
	Changed Log	:

		Liujian  2019/4/11		- Creation



*****************************************************************************/

#ifndef __COBRA_SPI_H__
#define __COBRA_SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_SPI_EN

/* Status register flags. */
#define RDW_SPI_TX_EMPTY                0x0001                                  /* Transmit FIFO empty. */
#define RDW_SPI_TX_FULL                 0x0002                                  /* Transmit FIFO full. */
#define RDW_SPI_TX_OVERFLOW             0x0004                                  /* Transmit FIFO overflow. */
#define RDW_SPI_TX_UNDERRUN             0x0008                                  /* Transmit FIFO underrun. */
#define RDW_SPI_RX_EMPTY                0x0010                                  /* Receive FIFO empty. */
#define RDW_SPI_RX_FULL                 0x0020                                  /* Receive FIFO full. */
#define RDW_SPI_RX_OVERFLOW             0x0040                                  /* Receive FIFO overflow. */
#define RDW_SPI_BUSY                    0x2000                                  /* SPI busy transmitting or receiving. @since rev 6. */
#define RDW_SPI_TX_ALMOST_EMPTY         0x4000                                  /* Transmit FIFO almost empty. */
#define RDW_SPI_RX_ALMOST_FULL          0x8000                                  /* Receive FIFO almost full. */

/* Control register bit masks. */
#define RDW_SPI_ENABLE_MASK             0x0001
#define RDW_SPI_MODE_MASK               0x0002
#define RDW_SPI_WORD_SIZE_MASK          0x0004
#define RDW_SPI_BIT_ORDERING_MASK       0x0008
#define RDW_SPI_PHASE_MASK              0x0010
#define RDW_SPI_POLARITY_MASK           0x0020
#define RDW_SPI_TX_IE_MASK              0x0040
#define RDW_SPI_RX_IE_MASK              0x0080
#define RDW_SPI_RESET_FIFOS_MASK        0x0100                                  /**< Reset fifo control register bitmask. */
#define RDW_SPI_BPW_MASK                0xfc00                                  /**< Bits per word control register bitmask. */

/* Control register values. */
#define RDW_SPI_DISABLE                 (0x0 << 0)                              /**< Disable SPI interface. */
#define RDW_SPI_ENABLE                  (0x1 << 0)                              /**< Enable SPI interface. */
#define RDW_SPI_MASTER                  (0x0 << 1)                              /**< SPI master mode. */
#define RDW_SPI_SLAVE                   (0x1 << 1)                              /**< SPI slave mode. */
#define RDW_SPI_8_BITS                  (0x0 << 2)                              /**< 8-bits per word. Only used when CONTROL.BPW = 0. */
#define RDW_SPI_16_BITS                 (0x1 << 2)                              /**< 16-bits per word. Only used when CONTROL.BPW = 0. */
#define RDW_SPI_MSB_FIRST               (0x0 << 3)                              /**< Data is transmitted most-significant bit first. */
#define RDW_SPI_LSB_FIRST               (0x1 << 3)                              /**< Data is transmitted least-significant bit first. */
#define RDW_SPI_LEADING_EDGE            (0x0 << 4)                              /**< Sample on leading edge. */
#define RDW_SPI_TRAILING_EDGE           (0x1 << 4)                              /**< Sample on trailing edge. */
#define RDW_SPI_IDLE_LOW                (0x0 << 5)                              /**< Clock idles high. */
#define RDW_SPI_IDLE_HIGH               (0x1 << 5)                              /**< Clock idles low. */
#define RDW_SPI_TX_INT_DISABLE          (0x0 << 6)                              /**< Disable transmit interrupt. */
#define RDW_SPI_TX_INT_ENABLE           (0x1 << 6)                              /**< Enable transmit interrupt. */
#define RDW_SPI_RX_INT_DISABLE          (0x0 << 7)                              /**< Disable receive interrupt. */
#define RDW_SPI_RX_INT_ENABLE           (0x1 << 7)                              /**< Enable receive interrupt. */
#define RDW_SPI_RESET_FIFOS             (0x1 << 8)                              /**< Reset SPI FIFOs. */

/* FIFO threshold flags. */
#define RDW_SPI_TXAE_INT_DISABLE        (0x0 << 15)                             /**< Disable transmit FIFO almost empty interrupt. */
#define RDW_SPI_TXAE_INT_ENABLE         (0x1 << 15)                             /**< Enable transmit FIFO almost empty interrupt. */
#define RDW_SPI_RXAF_INT_DISABLE        (0x0 << 15)                             /**< Disable receive FIFO almost full interrupt. */
#define RDW_SPI_RXAF_INT_ENABLE         (0x1 << 15)                             /**< Enable receive FIFO almost full interrupt. */

/* Auto-TRX CONTROL register flags. */
#define RDW_SPI_AUTO_TRX_DISABLE        0x0                                     /**< Disable auto-TRX. @since rev 5. */
#define RDW_SPI_AUTO_TX                 0x1                                     /**< Enable auto-transmit. @since rev 5. */
#define RDW_SPI_AUTO_RX                 0x2                                     /**< Enable auto-receive. @since rev 5. */


typedef struct cobra_spi_type {
  __attribute__ ((aligned (4))) unsigned int TX_DATA;                                    /**< Transmit data register. */
  __attribute__ ((aligned (4))) unsigned int RX_DATA;                                    /**< Received data register. */
  __attribute__ ((aligned (4))) unsigned int STATUS;                                     /**< Status register. */
  __attribute__ ((aligned (4))) unsigned int CONTROL;                                    /**< Control register. */
  __attribute__ ((aligned (4))) unsigned int CYCLES_PER_BIT;                             /**< Number of cycles per bit. Must be >= 2. */
  __attribute__ ((aligned (4))) unsigned int TXAE_THRESH;                                /**< Transmit FIFO almost empty threshold. @since rev 3. */
  __attribute__ ((aligned (4))) unsigned int RXAF_THRESH;                                /**< Receive FIFO almost full threshold. @since rev 3. */
  __attribute__ ((aligned (4))) unsigned int TX_COUNT;                                   /**< Count of used entries in TX FIFO. @since rev 4. */
  __attribute__ ((aligned (4))) unsigned int RX_COUNT;                                   /**< Count of used entries in RX FIFO. @since rev 4. */
  __attribute__ ((aligned (4))) unsigned int AUTO_TRX_CONTROL;                           /**< Auto TRX control. @since rev 5. */
  __attribute__ ((aligned (4))) unsigned int AUTO_TRX_COUNT;                             /**< Auto TRX count. @since rev 5. */
}COBRA_SPI_TypeDef;

typedef enum{
	SPI0,
	SPI1
}spi_index;

typedef enum {
	SPI_MASTER,
	SPI_SLAVE
}operate_mode;

typedef enum {
	MSB_FIRST,
	LSB_FIRST
}bit_ording;

typedef enum {
	SAMPLE_ON_LEADING_EDGE,
	SAMPLE_ON_TRAILING_EDGE
}clk_phase;

typedef enum {
	IDLE_LOW,
	IDLE_HIGH
}clk_polarity;

typedef enum{
	SPI_10KHZ,
	SPI_50KHZ,
	SPI_100KHZ,
	SPI_200KHZ,
	SPI_500KHZ,
	SPI_1MHZ,
	SPI_2MHZ,
	SPI_4MHZ,
	SPI_8MHZ,
	SPI_12MHZ,
	SPI_16MHZ,
	SPI_32MHZ,
	SPI_48MHZ,
	SPI_64MHZ
}spi_bps;

typedef enum{
	SPI_8bit,
	SPI_16bit,
	SPI_32bit
}spi_word_size;




/// Dummy byte
#define   SPI_DUMMY_BYTE        (0x00)

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */

/// SPI channel parameters, holding data used for asynchronous R/W data transactions
struct spi_txrxchannel
{
    uint32_t  size_wr;
    uint32_t  size_rd;
    uint8_t  *bufptr;
    void     (*callback)(void);
};

///Structure defining SPI environment parameters
struct spi_env_tag
{
    struct spi_txrxchannel tx;
    struct spi_txrxchannel rx;
};



#define COBRA_SPI0_BASE				(0x400A4000UL)
#define COBRA_SPI1_BASE				(0x400CB000UL)


void cobra_spi_init(void);

//wait
void cobra_spi_write_word (spi_index index, unsigned int data);

//send a byte no wait
void cobra_spi_send_byte(spi_index index, unsigned int data);
	
unsigned int cobra_spi_read_word (spi_index index);
void cobra_spi_read_words (spi_index index, unsigned int *data, unsigned int count);

void cobra_spi_control_set (spi_index index, operate_mode mode, clk_phase cpha,  clk_polarity cpol, bit_ording ording);

void cobra_spi_set_bps(spi_index index, spi_bps bps);

void cobra_spi_set_bits_per_word (spi_index index, spi_word_size bits_per_word);

/** Enable SPI.  */
void cobra_spi_enable (spi_index index);

/** Disable SPI.  */
void cobra_spi_disable (spi_index index);

void cobra_spi_enable_tx_interrupt (spi_index index);

void app_cobra_spi_init(void);

void cobra_spi_disbale_rx_interrupt (spi_index index);
void cobra_spi_disbale_tx_interrupt (spi_index index);

/**
 ****************************************************************************************
 * @brief Receives data from SPI RX FIFO.
 ****************************************************************************************
 */
void app_cobra_spi_receive_data(spi_index index);

/**
 ****************************************************************************************
 * @brief Transmit data to SPI TX FIFO.
 ****************************************************************************************
 */
void app_cobra_spi_transmit_data(spi_index index);


void app_cobra_spi1_init(unsigned int master_enable);
void app_cobra_spi0_init(unsigned int master_enable);

void app_cobra_spi0_enviroment_init(void);
void app_cobra_spi1_enviroment_init(void);

void app_cobra_spi_read(spi_index index, uint8_t *bufptr, uint32_t size, void (*rx_callback)(void));
void app_cobra_spi_write(spi_index index, uint8_t *bufptr, uint32_t size, void (*tx_callback)(void));

 unsigned int cobra_spi_read_write_word (spi_index index, unsigned int data);
 
 void cobra_spi_set_bps_value(spi_index index, unsigned int bps);
#endif   //COBRA_SPI_EN



#endif

