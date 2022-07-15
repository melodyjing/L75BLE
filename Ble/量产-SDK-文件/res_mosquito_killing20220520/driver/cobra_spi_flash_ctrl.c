/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_spi_flash_ctrl.c
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

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_spi_flash_ctrl.h"
#include "cobra_pinmux.h"
#include "cobra_pcrm.h"
#include "cobra4_aon_top.h"
#if COBRA_AON_PMU_EN
#include "cobra_aon_pmu.h"
#endif


#if COBRA_SPI_FLASH_CTRL_EN
/* SPI FLASH commands. Verified for ATMEL/ST/Winbond/Spansion 25-series parts.  */
#define SPI_FLASH_CMD_25X_WRITE_ENABLE 		0x06
#define SPI_FLASH_CMD_25X_WRITE_DISABLE		0x04
#define SPI_FLASH_CMD_25X_READ_STATUS 		0x05
#define SPI_FLASH_CMD_25X_WRITE_STATUS		0x01
#define SPI_FLASH_CMD_25X_PAGE_PROGRAM		0x02
#define SPI_FLASH_CMD_25X_SECTOR_ERASE		0x20
#define SPI_FLASH_CMD_CHIP_ERASE			0xc7
#define SPI_FLASH_CMD_POWER_DOWN			0xb9
#define SPI_FLASH_CMD_READ_DATA				0x03
#define SPI_FLASH_CMD_JEDEC_ID				0x9f
#define SPI_FLASH_CMD_GET_ID					0x90

/* Valid for Spansion, Winbond. Not for Atmel/Macronix/Micron. */
#define SPI_FLASH_CMD_25X_READ_STATUS_2     0x35

/* Valid for Winbond, Atmel. Not for Spansion/Macronix/Micron. */
#define SPI_FLASH_CMD_25X_WRITE_STATUS_2    0x31

/* SPI FLASH manufacturer identifiers.  */
#define SPI_FLASH_MANUFACTURER_ID_ATMEL     0x1f
#define SPI_FLASH_MANUFACTURER_ID_ST        0x20
#define SPI_FLASH_MANUFACTURER_ID_WINBOND   0xef
#define SPI_FLASH_MANUFACTURER_ID_SPANSION  0x01
#define SPI_FLASH_MANUFACTURER_ID_MACRONIX  0xc2
#define SPI_FLASH_MANUFACTURER_ID_MICRON    0x20

/* SPI FLASH status register bits.  */
#define SPI_FLASH_STATUS_25X_BUSY           0x01
#define SPI_FLASH_STATUS_25X_WEL            0x02


/* SPI FLASH page size.  */
#define SPI_FLASH_PAGE_SIZE					256

static volatile COBRA_SPI_FLASH_T *cobra_spi_flash;

/** Write and read a byte of data from the SPI FLASH. This function will block until data can be written.  */
static unsigned char cobra_spi_flash_read_write_byte (volatile COBRA_SPI_FLASH_T *spi_flash, unsigned char data)
{
	  /* Wait until there is space in the transmit buffer.  */
	  while (spi_flash->STATUS & COBRA_SPI_FLASH_TX_FULL) ;

	  /* Wait until ready. */
	  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
		;
	  /* Write data into the transmit buffer.  */
	  spi_flash->TX_DATA = data;
	  /* Wait until data has been received.  */
	  while (spi_flash->STATUS & COBRA_SPI_FLASH_RX_EMPTY) {
	  }
	  /* Return the received data.  */
	  return (unsigned char)spi_flash->RX_DATA;
}

/** Assert chip-select for the SPI FLASH.  */
static void cobra_spi_flash_assert_cs (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Clear CS_N bit to assert, as active-low. */
  spi_flash->PIN_OUT &= ~COBRA_SPI_FLASH_CS_N_MASK;
	
//#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)	
//	app_m3_gpio_write_pin(GPIO_PIN_2, GPIO_LOW);
//#endif	
	
}

/** Deassert chip-select for the SPI FLASH. */
static void cobra_spi_flash_deassert_cs (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Set CS_N bit to deassert, as active-low. */
  spi_flash->PIN_OUT |= COBRA_SPI_FLASH_CS_N_MASK;

//#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)		
//	app_m3_gpio_write_pin(GPIO_PIN_2, GPIO_HIGH);
//#endif
	
}


/**WP  = 0 */
static void cobra_spi_flash_assert_wp (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Clear CS_N bit to assert, as active-low. */
  spi_flash->PIN_OUT &= ~COBRA_SPI_FLASH_WP_N_MASK;
}

/** WP  = 1. */
static void cobra_spi_flash_deassert_wp (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Set CS_N bit to deassert, as active-low. */
  spi_flash->PIN_OUT |= COBRA_SPI_FLASH_WP_N_MASK;
}


/**HOLD  = 0 */
static void cobra_spi_flash_assert_hold (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Clear CS_N bit to assert, as active-low. */
  spi_flash->PIN_OUT &= ~COBRA_SPI_FLASH_HOLD_N_MASK;
}

/** HOLD  = 1. */
static  void cobra_spi_flash_deassert_hold(volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
  /* Set CS_N bit to deassert, as active-low. */
  spi_flash->PIN_OUT |= COBRA_SPI_FLASH_HOLD_N_MASK;
}




/** Enable SPI FLASH.  */
static  void cobra_spi_flash_enable (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Set the enable flag.  */
  spi_flash->CONTROL |= COBRA_SPI_FLASH_ENABLE | COBRA_SPI_FLASH_RESET;

}

/** Disable SPI FLASH.  */
static  void cobra_spi_flash_disable (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Clear the enable flag.  */
  spi_flash->CONTROL &= ~COBRA_SPI_FLASH_ENABLE;
}

/** Reset any FIFOs/buffers. */
static void cobra_spi_reset_fifos (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  volatile unsigned int unused = 0;

  /* Read receive buffer until it is empty. */
  while (spi_flash->STATUS & COBRA_SPI_FLASH_RX_FULL)
    unused = spi_flash->RX_DATA;
}

/** Set SPI FLASH to config mode.  */
static void cobra_spi_flash_set_config_mode (volatile COBRA_SPI_FLASH_T *spi_flash)
{
  /* Set the config flag.  */
	spi_flash->CONTROL |= COBRA_SPI_FLASH_CONFIG_MODE;//|RDW_SPI_FLASH_CE;
	while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
	spi_flash->CONTROL &= ~(COBRA_SPI_FLASH_MODE_QUAD|COBRA_SPI_FLASH_MODE_IO|COBRA_SPI_FLASH_MODE_CONTINUOUS);
  /* Wait until ready. */
  while (!(spi_flash->STATUS & COBRA_SPI_FLASH_CONFIG_READY))
    ;
}

/*Set clock div*/
static void cobra_spi_flash_set_cycle_per_bit (volatile COBRA_SPI_FLASH_T *spi_flash,uint32_t per)
{
  /* Set the config flag.  */
  spi_flash->CYCLES_PER_BIT = per;
}

/** Set SPI FLASH pin direction output.  */
static void cobra_spi_flash_set_pin_output (volatile COBRA_SPI_FLASH_T *spi_flash,uint32_t dir)
{
  /* Set the config flag.  */
  spi_flash->PIN_DIRECTION |= dir;
  /* Wait until ready. */
}

/** Set SPI FLASH pin direction output 1.  */
static void cobra_spi_flash_set_pin_output_1 (volatile COBRA_SPI_FLASH_T *spi_flash,uint32_t pin)
{
  /* Set the config flag.  */
  spi_flash->PIN_OUT |= pin;
  /* Wait until ready. */
}


/** Set SPI FLASH pin direction output 0.  */
static  void cobra_spi_flash_set_pin_output_0 (volatile COBRA_SPI_FLASH_T *spi_flash,uint32_t pin)
{
  /* Set the config flag.  */
  spi_flash->PIN_OUT &= ~pin;
  /* Wait until ready. */
}


/** Set SPI FLASH pin direction input.  */
static void cobra_spi_flash_set_pin_input (volatile COBRA_SPI_FLASH_T *spi_flash,uint32_t dir)
{
  /* Set the config flag.  */
  spi_flash->PIN_DIRECTION &= ~dir;
  /* Wait until ready. */
}

/** Set SPI FLASH to memory mode.  */
static  void cobra_spi_flash_set_memory_mode (volatile COBRA_SPI_FLASH_T *spi_flash)
{
	  spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_CONTINUOUS;
//	spi_flash->CONTROL &= ~RDW_SPI_FLASH_CE;
    spi_flash->CONTROL &= ~COBRA_SPI_FLASH_CONFIG_MODE;
}

static  bool cobra_spi_flash_get_continue_mode(volatile COBRA_SPI_FLASH_T *spi_flash)
{
	return (spi_flash->CONTROL & COBRA_SPI_FLASH_MODE_CONTINUOUS)>>11;
}

static  void cobra_spi_flash_set_spi_mode(volatile COBRA_SPI_FLASH_T *spi_flash, unsigned int mode)
{
		spi_flash->CONTROL &= ~(0x3 << 8);
		if(mode == 0)
		{
			spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_SERIAL;
		}
		else if(mode == 1)
		{
			spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_DUAL;
		}
		else if(mode == 2)
		{
			spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_QUAD;
		}
}

static  void cobra_spi_flash_set_io_mode(volatile COBRA_SPI_FLASH_T *spi_flash, unsigned int mode)
{
	if(mode) //output mode addreee transmitted serial
	{
		spi_flash->CONTROL &= ~COBRA_SPI_FLASH_MODE_IO;
	}
	else
	{
		spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_IO;
	}
}

static void cobra_spi_flash_set_cm_mode(volatile COBRA_SPI_FLASH_T *spi_flash, unsigned int mode)
{
	if(mode)
	{
		spi_flash->CONTROL &= ~COBRA_SPI_FLASH_MODE_CONTINUOUS;
	}
	else
	{
		spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_CONTINUOUS;
	}
}

#if 0
/* Set SPI FLASH to memory mode.  */
static void cobra_spi_flash_set_memory_mode (volatile COBRA_SPI_FLASH_T *spi_flash)
{
	 spi_flash->CONTROL &= ~COBRA_SPI_FLASH_CONFIG_MODE;
}
#endif


void cobra_spi_flash_set_rate_hz(unsigned int hz)
{
		unsigned int cycles_per_bit ;
		unsigned int apb2_sys_clk ;

//	  if(cobra4_aon_pd_top_hf_clock_source() == HF_XTAL48M)
//		{
//				apb2_sys_clk = 48000000;
//		}
//		else
//		{
//				apb2_sys_clk = 24000000;
//		}
		apb2_sys_clk = cobra_system_get_cpu_frequency();
		cycles_per_bit = (apb2_sys_clk/hz)/2 - 1;
		cobra_spi_flash_set_cycle_per_bit(cobra_spi_flash, cycles_per_bit);
}
/*
 * set sck out hz
 */
void cobra_spi_flash_set_rate(SPI_RATE  rate)
{
		unsigned int cycles_per_bit ;
		float temp_hz;
		float apb2_sys_clk ;
		unsigned int temp_rate;
	
	  if(cobra4_aon_pd_top_hf_clock_source() == HF_XTAL48M)
		{
				apb2_sys_clk = 48000000;
		}
		else
		{
				apb2_sys_clk = 24000000;
		}
		switch(rate)
		{
			case RATE_256KHZ:
			{
				temp_hz = 256000;
				break;
			}
			case RATE_512KHZ:
			{
				temp_hz = 512000;
				break;
			}
			case RATE_1MHZ:
			{
				temp_hz = 1000000;
				break;
			}
			case RATE_2MHZ:
			{
				temp_hz = 2000000;
				break;
			}
			case RATE_3MHZ:
			{
				temp_hz = 3000000;
				break;
			}
			case RATE_4MHZ:
			{
				temp_hz = 4000000;
				break;
			}
			case RATE_8MHZ:
			{
				temp_hz = 8000000;
				break;
			}
			case RATE_16MHZ:
			{
				temp_hz = 16000000;
				break;
			}
			case RATE_24MHZ:
			{
				temp_hz = 24000000;
				break;
			}
			case RATE_32MHZ:
			{
				temp_hz = 32000000;
				break;
			}
			default:
				temp_hz = apb2_sys_clk/2;
				break;
		}
		cycles_per_bit = (apb2_sys_clk/temp_hz)/2 - 1;
		temp_rate = (unsigned int)cycles_per_bit;
		cobra_spi_flash_set_cycle_per_bit(cobra_spi_flash, temp_rate);
	}

void cobra_spi_set_work_mode(SPI_WORK_MODE mode)
{
			switch(mode)
			{
				case SERIAL_MODE:
				{
					cobra_spi_flash_set_spi_mode(cobra_spi_flash, 0);
					break;
				}
				case  DUAL_MODE:
				{
					cobra_spi_flash_set_spi_mode(cobra_spi_flash, 1);
					break;
				}
				case QUAD_MODE:
				{
					cobra_spi_flash_set_spi_mode(cobra_spi_flash, 2);
					break;
				}
				default:
					break;
			}
}

void cobra_spi_set_io_mode(SPI_IO_MODE mode)
{
		if(mode == SPI_OUTPUT_MODE)
		{
			 cobra_spi_flash_set_io_mode(cobra_spi_flash, 1);
		}
		else if(mode == SPI_I_O_MODE)
		{
			 cobra_spi_flash_set_io_mode(cobra_spi_flash, 0);
		}
}

void cobra_spi_set_continue_mode(SPI_CM_MODE mode)
{
		if(mode == SPI_NORMAL_MODE)
		{
				cobra_spi_flash_set_cm_mode(cobra_spi_flash, 1);
		}
		else if(mode == SPI_CONTINOUS_MODE)
		{
				cobra_spi_flash_set_cm_mode(cobra_spi_flash, 0);
		}
}

void cobra_spi_flash_dual_mode(void)
{
	    cobra_spi_set_work_mode(DUAL_MODE);
//			cobra_spi_set_io_mode(SPI_I_O_MODE);
//			cobra_spi_set_continue_mode(SPI_CONTINOUS_MODE);
	
}


void cobra_spi_flash_quad_mode(void)
{
			cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_WP_N_MASK);
			cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_HOLD_N_MASK);
			cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_SO_MASK);
			cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SCK_MASK);
			cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_CS_N_MASK);

			cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_SI_MASK);

			cobra_spi_set_work_mode(QUAD_MODE);
			cobra_spi_set_io_mode(SPI_I_O_MODE);
			cobra_spi_set_continue_mode(SPI_CONTINOUS_MODE);
}


void cobra_spi_flash_serial_mode(void)
{
		
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_WP_N_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_HOLD_N_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SO_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SCK_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_CS_N_MASK);

		cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_SI_MASK);

		cobra_spi_set_work_mode(SERIAL_MODE);
		cobra_spi_set_io_mode(SPI_OUTPUT_MODE);
		cobra_spi_set_continue_mode(SPI_NORMAL_MODE);

}

/* Get the manufacturer ID of a SPI FLASH.  */
unsigned char cobra_spi_flash_get_manufacturer_id (void)
{
		unsigned char id;

		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_JEDEC_ID);
		id = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0xff);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
		return id;
}

unsigned char coney_spi_flash_get_id(void)
{
		unsigned char id;

		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_GET_ID);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		id = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
		return id;
}

/* Write the specified value to the status register in the specified SPI FLASH.  */
static void cobra_spi_flash_25x_write_status (unsigned char status)
{
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_WRITE_STATUS);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Read the status register from the specified SPI FLASH.  */
static unsigned char cobra_spi_flash_25x_read_status (void)
{
		unsigned char status;

		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS);
		status = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
		return status;
}

/* Wait until the SPI FLASH is ready to accept a new command.  */
void cobra_spi_flash_25x_wait_until_ready (void)
{
	while(cobra_spi_flash_25x_read_status () & SPI_FLASH_STATUS_25X_BUSY)
		;
}

/* Send the write enable command to the SPI FLASH.  */
static void cobra_spi_flash_25x_write_enable ()
{
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_WRITE_ENABLE);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Write CMD_POWER_DOWN.  */
void cobra_spi_flash_25x_power_down()
{
	cobra_spi_flash_assert_cs (cobra_spi_flash);
	cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_POWER_DOWN);
	cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Erase a SPI FLASH.  */
void  cobra_spi_flash_25x_chip_erase (void)
{
		/* Clear the write protect flags in the status register.  */
		cobra_spi_flash_25x_wait_until_ready ();
		cobra_spi_flash_25x_write_enable ();
		cobra_spi_flash_25x_write_status (0x00);

		/* Erase the entire SPI FLASH device.  */
		cobra_spi_flash_25x_wait_until_ready ( );
		cobra_spi_flash_25x_write_enable ();
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_CHIP_ERASE);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* erase sector.  */
void cobra_spi_flash_25x_sector_erase (long address)
{
		cobra_spi_flash_25x_wait_until_ready ();
		cobra_spi_flash_25x_write_enable ();
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_SECTOR_ERASE);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 16) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 8) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, address & 0xff);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Write a page of data in the SPI FLASH.  */
void cobra_spi_flash_25x_write_page (long address, int length, const unsigned char *data)
{
		int i;

		cobra_spi_flash_25x_wait_until_ready ();
		cobra_spi_flash_25x_write_enable ();
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_PAGE_PROGRAM);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 16) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 8) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, address & 0xff);
		for (i = 0; i < length; i++) {
				cobra_spi_flash_read_write_byte (cobra_spi_flash, data[i]);
		}
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Read data from the specified SPI FLASH.  */
void cobra_spi_flash_read (long address, int length, unsigned char *data, int fast)
{
		int i;
	
    cobra_spi_flash_25x_wait_until_ready ();
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_READ_DATA);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 16) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, (address >> 8) & 0xff);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, address & 0xff);
		if (fast) {
				cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		}
		for (i = 0; i < length; i++) {
				data[i] = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		}
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}

/* Wait approximately 10ms. */
void cobra_spi_flash_25x_wait_10ms ()
{
		volatile unsigned int i;
		for(i = 0; i < 1600000; i++);
}

/* Enable quad mode by writing bit QE bit (bit 1) in status register 2. Only use for Spansion. */
static void cobra_spi_flash_enable_quad_s2s (void)
{
		unsigned char status1;
		unsigned char status2;
		unsigned char status3;

		/* Enable writes to the status register. */
		cobra_spi_flash_25x_write_enable ();

		/* Read status registers. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS);
		status1 = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS_2);
		status2 = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x33);
		status3 = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Set QE bit. */
		status2 |= 0x02;

		/* Update status registers. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x01);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status1);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status2);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status3);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Wait for status write to complete */
		cobra_spi_flash_25x_wait_until_ready ();
	}


/* Enable quad mode by writing bit QE bit (bit 1) in status register 2. Only use for Winbond. */
static void cobra_spi_flash_enable_quad_s2w (void)
{
		unsigned char status1, status2;

		/* Enable writes to the status register. */
		cobra_spi_flash_25x_write_enable();

		/* Read status 2 register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS_2);
		status2 = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Read status 1 register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS);
		status1 = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);


		/* Set QE bit. */
		status2 |= 0x02;

		/* Update status 2 register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_WRITE_STATUS);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status1);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status2);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Wait for status write to complete */
		cobra_spi_flash_25x_wait_until_ready ();
}


/* Enable quad mode by writing bit QE bit (bit 6) in status register 1. Only use for Macronix. */
static void cobra_spi_flash_enable_quad_s1 (void)
{
		unsigned char status;

		/* Enable writes to the status register. */
		cobra_spi_flash_25x_write_enable ( );

		/* Read status register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_READ_STATUS);
		status = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Set QE bit. */
		status |= 0x40;

		/* Update status register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, SPI_FLASH_CMD_25X_WRITE_STATUS);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, status);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Wait for status write to complete. This takes 40ms on Macronix. */
		cobra_spi_flash_25x_wait_until_ready ();
}


/* Enable quad mode by writing bit QE bit (bit 7) in config register. Only use for Atmel AT25DQ. */
static void cobra_spi_flash_enable_quad_c (void)
{
		unsigned char config;

		/* Enable writes to the status register. */
		cobra_spi_flash_25x_write_enable ();

		/* Read status register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x3f);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		config = cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x00);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Set QE bit. */
		config |= 0x80;

		/* Update status register. */
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0x3e);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, config);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);

		/* Wait for status write to complete. Takes 20ms. */
		cobra_spi_flash_25x_wait_until_ready ();
}


/* Enable quad output mode for specified flash device. */
static void cobra_spi_flash_25x_enable_quad_output (unsigned char id)
{
		/* Enable in the SPI Flash. */
		switch (id)
			{
			case SPI_FLASH_MANUFACTURER_ID_SPANSION:
				cobra_spi_flash_enable_quad_s2s ();
				break;
			case SPI_FLASH_MANUFACTURER_ID_WINBOND:
				cobra_spi_flash_enable_quad_s2w ();
				break;
			case SPI_FLASH_MANUFACTURER_ID_MACRONIX:
				cobra_spi_flash_enable_quad_s1 ();
				break;
			case SPI_FLASH_MANUFACTURER_ID_MICRON:
				// Nothing needed
				break;
			case SPI_FLASH_MANUFACTURER_ID_ATMEL:
				cobra_spi_flash_enable_quad_c ();
				break;
			default:
				// Unsupported
				return;
			}

		/* Enable in the eSi-SPI Flash interface control registers. */
		 cobra_spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_QUAD | COBRA_SPI_FLASH_HOLD_CS;

}



/* Enable quad I/O mode for specified flash device. */
void cobra_spi_flash_25x_enable_quad_io (unsigned char id)
{
		/* First, enable quad output mode. */
		 cobra_spi_flash_25x_enable_quad_output (id);

		/* Set appropriate number of dummy cycles for the given flash device. */
		switch (id)
			{
			case SPI_FLASH_MANUFACTURER_ID_MACRONIX:
			case SPI_FLASH_MANUFACTURER_ID_SPANSION:
			case SPI_FLASH_MANUFACTURER_ID_WINBOND:
				cobra_spi_flash->TIMING &= ~0x1f00;
				cobra_spi_flash->TIMING |= 6 << 8;
				break;
			case SPI_FLASH_MANUFACTURER_ID_MICRON:
				cobra_spi_flash->TIMING &= ~0x1f00;
				cobra_spi_flash->TIMING |= 10 << 8;
				break;
			}

			/* Enable I/O mode in eSi-SPI Flash interface. */
		switch (id)
			{
			case SPI_FLASH_MANUFACTURER_ID_SPANSION:
			case SPI_FLASH_MANUFACTURER_ID_WINBOND:
			case SPI_FLASH_MANUFACTURER_ID_MACRONIX:
			case SPI_FLASH_MANUFACTURER_ID_MICRON:
				cobra_spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_IO;
				break;
			case SPI_FLASH_MANUFACTURER_ID_ATMEL:
				/* Not supported in some parts - Perhaps some more recent ones? */
				break;
			}
}




void cobra_spi_flash_set_flash_out_qspi(void)
{
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0xFF);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
}


int cobra_spi_flash_memory_setup(void)
{

			/* Return to memory mode. */
			cobra_spi_flash_set_memory_mode (cobra_spi_flash);

			return 1;


}


void cobra_spi_flash_config_set_cs(bool enable)
{
	  if(enable)
		{
			  cobra_spi_flash_assert_cs (cobra_spi_flash);
		}
		else
		{
			  cobra_spi_flash_deassert_cs(cobra_spi_flash);
		}
}

unsigned char cobra_spi_flash_config_write_and_read(unsigned char write_data)
{
	   return cobra_spi_flash_read_write_byte(cobra_spi_flash, write_data);
}




void cobra_spi_flash_init(void)
{
		unsigned char chip_id;
		cobra_spi_flash = (COBRA_SPI_FLASH_T *)COBRA_SPI_FLASH_BASE;
		cobra_spi_flash_set_rate(RATE_1MHZ);
		cobra_spi_flash_enable(cobra_spi_flash);
		cobra_spi_flash_set_config_mode(cobra_spi_flash);



		cobra_spi_flash_deassert_wp(cobra_spi_flash);
		cobra_spi_flash_deassert_hold(cobra_spi_flash);


		cobra_spi_flash_serial_mode();

		cobra_spi_flash_deassert_cs(cobra_spi_flash);
		cobra_spi_reset_fifos(cobra_spi_flash);
		chip_id = cobra_spi_flash_get_manufacturer_id();

#if	DEBUG_PRINTF_EN
		printf("chip_id = %x \n", chip_id);
#endif

}

void app_cobra_spi_flash_init(void)
{
		unsigned char chip_id;
	
		cobra_spi_flash = (COBRA_SPI_FLASH_T *)COBRA_SPI_FLASH_BASE;
		cobra_pcrm_spiflash_ctrl_active_mode_clock_enable(1);
		cobra_pcrm_spi_flash_control_clk_mask_en(1);
	
	//	cobra_map_pinmodeset(PIN19_INDEX, PIN19_SPIF_CS);
	//	cobra_map_pinmodeset(PIN20_INDEX, PIN20_SPIF_SCK);
		cobra_map_pinmodeset(PIN21_INDEX, PIN21_SPIF_SO);
		cobra_map_pinmodeset(PIN22_INDEX, PIN22_SPIF_SI);
		cobra_map_pinmodeset(PIN23_INDEX, PIN23_SPIF_WP);
		cobra_map_pinmodeset(PIN24_INDEX, PIN24_SPIF_HOLD);
	
		cobra_spi_flash_set_rate(RATE_1MHZ);
	
		cobra_spi_flash_enable(cobra_spi_flash);
		cobra_spi_flash_set_config_mode(cobra_spi_flash);


		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_WP_N_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_HOLD_N_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SO_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SCK_MASK);
		cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_CS_N_MASK);

		cobra_spi_flash_set_pin_input(cobra_spi_flash, COBRA_SPI_FLASH_SI_MASK);
		
		cobra_spi_flash_set_pin_output_1(cobra_spi_flash, COBRA_SPI_FLASH_WP_N_MASK);
		cobra_spi_flash_set_pin_output_1(cobra_spi_flash, COBRA_SPI_FLASH_HOLD_N_MASK);


		cobra_spi_flash_deassert_cs(cobra_spi_flash);
		cobra_spi_reset_fifos(cobra_spi_flash);
		
		cobra_spi_flash_assert_cs (cobra_spi_flash);
		cobra_spi_flash_read_write_byte (cobra_spi_flash, 0xFF);
		cobra_spi_flash_deassert_cs (cobra_spi_flash);
	
	
		chip_id = cobra_spi_flash_get_manufacturer_id();
		
		cobra_spi_flash_25x_enable_quad_io(chip_id);
		
		cobra_spi_flash_set_memory_mode (cobra_spi_flash);
		
}


#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)

void app_cobra_sfud_port_flash_init(void)
{
		volatile unsigned char chip_id;
	
		cobra_spi_flash = (COBRA_SPI_FLASH_T *)COBRA_SPI_FLASH_BASE;
		cobra_pcrm_spiflash_ctrl_active_mode_clock_enable(1);
		cobra_pcrm_spi_flash_control_clk_mask_en(1);
#if(APP_MESH_LIGHT_ENABLE == 0)	
	  do {
	         cobra_pcrm_spi_flash_soft_reset();
	
           app_m3_gpio_set_direction(GPIO_PIN_2, GPIO_OUTPUT); 
	
#if (DRV_MESH_CWHSL_LIGHT_FUNTION == 1)
               cobra_map_pinmodeset(PIN0_INDEX, PIN0_SPIF_CS);
#else
		       cobra_map_pinmodeset(PIN3_INDEX, PIN3_SPIF_CS);
#endif
		       cobra_map_pinmodeset(PIN1_INDEX, PIN1_SPIF_SO);
		       cobra_map_pinmodeset(PIN16_INDEX, PIN16_SPIF_SI_IN);
		       cobra_map_pinmodeset(PIN17_INDEX, PIN17_SPIF_SCK);
	
		       cobra_spi_flash_set_rate(RATE_32MHZ);
				
	
		       cobra_spi_flash_enable(cobra_spi_flash);
		       cobra_spi_flash_set_config_mode(cobra_spi_flash);

		       cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SO_MASK);
		       cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_SCK_MASK);
		       cobra_spi_flash_set_pin_output(cobra_spi_flash, COBRA_SPI_FLASH_CS_N_MASK);
		       cobra_spi_flash_set_pin_input(cobra_spi_flash,  COBRA_SPI_FLASH_SI_MASK);

		       cobra_spi_flash_deassert_cs(cobra_spi_flash);
		       cobra_spi_reset_fifos(cobra_spi_flash);
		
		       cobra_spi_flash_assert_cs (cobra_spi_flash);
		       cobra_spi_flash_read_write_byte (cobra_spi_flash, 0xff);
		       cobra_spi_flash_deassert_cs (cobra_spi_flash);
		
		       chip_id = cobra_spi_flash_get_manufacturer_id();
#if (DRV_MESH_CWHSL_LIGHT_FUNTION == 1)
               if(chip_id == 0x0B) break;
#else
		       if(chip_id == 0xEF) break;
#endif	 
		 }while(1);
#endif
		 
		
}


void app_cobra4c_spi_flash_read_id(void)
{
	  unsigned char chip_id;
	  
	  chip_id = cobra_spi_flash_get_manufacturer_id();
		
		printf("SPI Flash ID = 0x%02x \n", chip_id);
			
}

void app_cobra4c_spi_flash_xip_mode(void)
{
		unsigned char chip_id;
	  
	  chip_id = cobra_spi_flash_get_manufacturer_id();
		
//		cobra_spi_flash_25x_enable_quad_io(chip_id);
	
//	  cobra_spi_flash->TIMING &= ~0x1f00;
//	 
//	  cobra_spi_flash->TIMING |= 4 << 8;
	
	  cobra_spi_flash->CONTROL |= COBRA_SPI_FLASH_MODE_DUAL | COBRA_SPI_FLASH_HOLD_CS;
	
//		cobra_spi_flash_dual_mode();
	
//	  cobra_spi_flash_assert_cs (cobra_spi_flash);
	
		cobra_spi_flash_set_memory_mode (cobra_spi_flash);
	
	  
}

#endif

#endif













