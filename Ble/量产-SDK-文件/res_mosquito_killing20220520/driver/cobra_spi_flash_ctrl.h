/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_spi_flash_ctrl.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   

					the driver is SPI flash control, and can uesd to run from program.
					the XIP address is 0x60000000
					it is different SPI driver.


	Changed Log	:

		Liujian  2019/3/8			- Creation


*****************************************************************************/

#ifndef __COBRA_SPI_FLASH_CTRL_H__
#define __COBRA_SPI_FLASH_CTRL_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "cobra_system.h"
#include <cobra_config.h>

#if COBRA_SPI_FLASH_CTRL_EN
/* Status register flags. */
#define COBRA_SPI_FLASH_TX_EMPTY          0x0001
#define COBRA_SPI_FLASH_TX_FULL           0x0002
#define COBRA_SPI_FLASH_TX_OVERFLOW       0x0004
#define COBRA_SPI_FLASH_TX_UNDERRUN       0x0008
#define COBRA_SPI_FLASH_RX_EMPTY          0x0010
#define COBRA_SPI_FLASH_RX_FULL           0x0020
#define COBRA_SPI_FLASH_RX_OVERFLOW       0x0040
#define COBRA_SPI_FLASH_CONFIG_READY      0x8000

/* Control register bit masks. */
#define COBRA_SPI_FLASH_ENABLE_MASK       0x0001
#define COBRA_SPI_FLASH_TX_IE_MASK        0x0040
#define COBRA_SPI_FLASH_RX_IE_MASK        0x0080

/* Control register values. */
#define COBRA_SPI_FLASH_DISABLE           (0x0 << 0)
#define COBRA_SPI_FLASH_ENABLE            (0x1 << 0)
#define COBRA_SPI_FLASH_TX_INT_DISABLE    (0x0 << 6)
#define COBRA_SPI_FLASH_TX_INT_ENABLE     (0x1 << 6)
#define COBRA_SPI_FLASH_RX_INT_DISABLE    (0x0 << 7)
#define COBRA_SPI_FLASH_RX_INT_ENABLE     (0x1 << 7)
#define COBRA_SPI_FLASH_MODE_SERIAL       (0x0 << 8)
#define COBRA_SPI_FLASH_MODE_DUAL         (0x1 << 8)
#define COBRA_SPI_FLASH_MODE_QUAD         (0x2 << 8)
#define COBRA_SPI_FLASH_MODE_OUTPUT       (0x0 << 10)
#define COBRA_SPI_FLASH_MODE_IO           (0x1 << 10)
#define COBRA_SPI_FLASH_MODE_NORMAL       (0x0 << 11)
#define COBRA_SPI_FLASH_MODE_CONTINUOUS   (0x1 << 11)
#define COBRA_SPI_FLASH_NO_RESET          (0x0 << 12)
#define COBRA_SPI_FLASH_RESET             (0x1 << 12)
#define COBRA_SPI_FLASH_HOLD_CS           (0x1 << 13)
#define COBRA_SPI_FLASH_CE        		    (0x1 << 14)
#define COBRA_SPI_FLASH_MEMORY_MODE       (0x0 << 15)
#define COBRA_SPI_FLASH_CONFIG_MODE       (0x1 << 15)

/* I/O pin masks. Pins can be used as GPIOs when CS_N deasserted and in config mode. */
#define COBRA_SPI_FLASH_WP_N_MASK         0x01
#define COBRA_SPI_FLASH_HOLD_N_MASK       0x02
#define COBRA_SPI_FLASH_SO_MASK           0x04
#define COBRA_SPI_FLASH_SI_MASK           0x08
#define COBRA_SPI_FLASH_SCK_MASK          0x10
#define COBRA_SPI_FLASH_CS_N_MASK         0x20

/** SPI Flash registers. */
typedef struct cobra_spi_flash_t {
	__attribute__ ((aligned (4))) unsigned int TX_DATA;                                    /**< Transmit data register.  */
	__attribute__ ((aligned (4))) unsigned int RX_DATA;                                    /**< Received data register.  */
	__attribute__ ((aligned (4))) unsigned int STATUS;                                     /**< Status register.  */
	__attribute__ ((aligned (4))) unsigned int CONTROL;                                    /**< Control register.  */
	__attribute__ ((aligned (4))) unsigned int CYCLES_PER_BIT;                             /**< Number of cycles per bit.  */
	__attribute__ ((aligned (4))) unsigned int PIN_OUT;                                    /**< Value to output on pin when GPIO.  */
	__attribute__ ((aligned (4))) unsigned int PIN_IN;                                     /**< Value to read on pin.  */
	__attribute__ ((aligned (4))) unsigned int PIN_DIRECTION;                              /**< Direction of pin when GPIO.  */
	__attribute__ ((aligned (4))) unsigned int TIMING;                                     /**< Timing control register.  */
	__attribute__ ((aligned (4))) unsigned int ADDRESS;                                    /**< Address register.  */
}COBRA_SPI_FLASH_T;



typedef enum {
		RATE_256KHZ,
		RATE_512KHZ,
		RATE_1MHZ,
		RATE_2MHZ,
		RATE_3MHZ,
		RATE_4MHZ,
		RATE_8MHZ,
		RATE_16MHZ,
	  RATE_24MHZ,
		RATE_32MHZ
}SPI_RATE;

typedef enum {
		SERIAL_MODE,
		DUAL_MODE,
		QUAD_MODE

}SPI_WORK_MODE;

typedef enum {
		SPI_OUTPUT_MODE,
		SPI_I_O_MODE
}SPI_IO_MODE;

typedef enum{
	 SPI_NORMAL_MODE,
	 SPI_CONTINOUS_MODE
}SPI_CM_MODE;


#define COBRA_SPI_FLASH_BASE				(0x40003000UL)


extern void cobra_spi_flash_init(void);

extern void cobra_spi_set_io_mode(SPI_IO_MODE mode);

extern void cobra_spi_set_work_mode(SPI_WORK_MODE mode);

extern void cobra_spi_flash_set_rate(SPI_RATE  rate);

extern void cobra_spi_flash_serial_mode(void);

extern void cobra_spi_set_continue_mode(SPI_CM_MODE mode);

extern unsigned char cobra_spi_flash_get_manufacturer_id (void);

extern void  cobra_spi_flash_25x_chip_erase (void);

extern void  cobra_spi_flash_25x_sector_erase (long address);

extern void  cobra_spi_flash_25x_write_page (long address, int length, const unsigned char *data);

extern void  cobra_spi_flash_read (long address, int length, unsigned char *data, int fast);

extern void  cobra_spi_flash_25x_wait_until_ready (void);

extern int cobra_spi_flash_memory_setup(void);

void cobra_spi_flash_set_rate_hz(unsigned int hz);

void app_cobra_spi_flash_init(void);

void cobra_spi_flash_config_set_cs(bool enable);

unsigned char cobra_spi_flash_config_write_and_read(unsigned char write_data);

void app_cobra_sfud_port_flash_init(void) ;

void cobra_spi_flash_25x_power_down(void);

#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)
//ÄÚ²¿·â×° SPI Flash
void app_cobra4c_spi_flash_init(void);
void app_cobra4c_spi_flash_read_id(void);

void app_cobra4c_spi_flash_xip_mode(void);
#endif


#endif
#endif

