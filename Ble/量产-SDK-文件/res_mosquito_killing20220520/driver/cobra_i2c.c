/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_i2c.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/4

	Description :   
			modified from coney project

	Changed Log	:

		Liujian  2019/4/4 		- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_i2c.h"


#if COBRA_I2C_EN

#if (COBRA_POWER_DOWN_ENABLE)

#ifdef COBRA_I2C_NEW
volatile COBRA_I2C_TypeDef * cobra_i2c0	       __attribute__((section("retention_area"),zero_init));
volatile COBRA_I2C_TypeDef * cobra_i2c1	       __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_I2C_TypeDef * cobra_i2c0	__attribute__((section("retention_area"),zero_init));
static volatile COBRA_I2C_TypeDef * cobra_i2c1	__attribute__((section("retention_area"),zero_init));
#endif
static unsigned char i2c0_slave_address		      __attribute__((section("retention_area"),zero_init));
static unsigned char i2c1_slave_address		      __attribute__((section("retention_area"),zero_init));

#else    //////////////////////////////////////////
#ifdef COBRA_I2C_NEW
volatile COBRA_I2C_TypeDef * cobra_i2c0;
volatile COBRA_I2C_TypeDef * cobra_i2c1;
#else
static volatile COBRA_I2C_TypeDef * cobra_i2c0;
static volatile COBRA_I2C_TypeDef * cobra_i2c1;
#endif

static unsigned char i2c0_slave_address;
static unsigned char i2c1_slave_address;
#endif

void cobra_i2c_init(void)
{
		cobra_i2c0 = (COBRA_I2C_TypeDef *)COBRA_I2C0_BASE;
	  cobra_i2c1 = (COBRA_I2C_TypeDef *)COBRA_I2C1_BASE;
}


uint16_t rdw_i2c_status (volatile COBRA_I2C_TypeDef *i2c)
{
		/* Set the enable flag.  */
		return i2c->I2C_STATUS;
}


/** Enable I2C.  
 * This must be called before the I2C will be able to transmit or receive.
 * @param i2c - Pointer to the I2C registers.
 */
void rdw_i2c_enable (volatile COBRA_I2C_TypeDef *i2c)
{
		/* Set the enable flag.  */
		i2c->I2C_CONTROL |= RDW_I2C_ENABLE;
}

/** Disable I2C.
 * @param i2c - Pointer to the I2C registers.
 */
void rdw_i2c_disable (volatile COBRA_I2C_TypeDef *i2c)
{
		/* Clear the enable flag.  */
		i2c->I2C_CONTROL &= ~RDW_I2C_ENABLE;
}



/** Reset the transmit and receive FIFOs. 
 * This should be called after a NACK or arbitration is lost.  
 * @param i2c - Pointer to the I2C registers.
 */

void rdw_i2c_reset_fifos (volatile COBRA_I2C_TypeDef *i2c)
{
		/* Set the reset FIFOs flag. This flag clears itself automatically. */
		i2c->I2C_CONTROL |= RDW_I2C_RESET_FIFOS;
}



/** Set the bit rate the I2C will transmit at.
 * @param i2c - Pointer to the I2C registers.
 * @param i2c_clock_frequency - Frequency of the I2C interface clock (This is not the frequency of SCL).
 * @param bit_rate - Desired bit rate. 
 */
#ifdef COBRA_I2C_NEW
void rdw_i2c_set_bit_rate (volatile COBRA_I2C_TypeDef *i2c, long i2c_clock_frequency, long bit_rate)
#else
static void rdw_i2c_set_bit_rate (volatile COBRA_I2C_TypeDef *i2c, long i2c_clock_frequency, long bit_rate)
#endif
{
		/* Set the number of clock cycles per bit.  */
		i2c->I2C_CYCLES_PER_BIT = i2c_clock_frequency / 3 / bit_rate - 1;
}


/** Write a byte of data to I2C transmit FIFO. 
 * This function blocks until the data is able to be written to the FIFO. 
 * @param i2c - Pointer to the I2C registers.
 * @param data - Byte to write to I2C transmit FIFO.
 * @return 0 if the data was written to the FIFO or -1 if a NACK or Arbitration Lost occured while trying.    
 */ 
#ifdef COBRA_I2C_NEW
int rdw_i2c_write_byte (volatile COBRA_I2C_TypeDef *i2c, unsigned char data)
#else
static int rdw_i2c_write_byte (volatile COBRA_I2C_TypeDef *i2c, unsigned char data)
#endif
{
  /* Wait until we have space in the transmit FIFO.  */
  while (i2c->I2C_STATUS & RDW_I2C_TX_FULL)
    {
      if (i2c->I2C_STATUS & (RDW_I2C_NACK | RDW_I2C_ARB_LOST))
        {
          rdw_i2c_reset_fifos (i2c);
          return -1;
        }
    }
  /* Write data into transmit FIFO.  */
  i2c->I2C_TX_DATA = data;
  return 0;
}



/** Read a byte of data from the I2C receive FIFO. 
 * This function blocks until data is available.
 * @param i2c - Pointer to the I2C registers. 
 * @return The byte of data read from the receive FIFO, or -1 if a NACK or Arbitration Lost occured 
 * while waiting for the data.    
 */
#ifdef COBRA_I2C_NEW
int rdw_i2c_read_byte (volatile COBRA_I2C_TypeDef *i2c)
#else
static int rdw_i2c_read_byte (volatile COBRA_I2C_TypeDef *i2c)
#endif
{
  /* Wait until some data is available.  */
  while (i2c->I2C_STATUS & RDW_I2C_RX_EMPTY)
    {
      if (i2c->I2C_STATUS & (RDW_I2C_NACK | RDW_I2C_ARB_LOST))
        {
          rdw_i2c_reset_fifos (i2c);
          return -1;
        }
    }
  /* Read and return data.  */
  return (int)i2c->I2C_RX_DATA;
}
#ifdef COBRA_I2C_NEW
void rdw_i2c_master_enable (volatile COBRA_I2C_TypeDef *i2c)
#else
static void rdw_i2c_master_enable (volatile COBRA_I2C_TypeDef *i2c)
#endif
{
		i2c->I2C_CONTROL = RDW_I2C_ENABLE|RDW_I2C_RESET_FIFOS|RDW_I2C_MASTER;
}

static void rdw_i2c_slave_enable (volatile COBRA_I2C_TypeDef *i2c)
{

	i2c->I2C_CONTROL = RDW_I2C_ENABLE|RDW_I2C_RESET_FIFOS|RDW_I2C_SLAVE;

}

void rdw_i2c_reset_fsm (volatile COBRA_I2C_TypeDef *i2c)
{
		/* Set the reset FIFOs flag. This flag clears itself automatically. */
		i2c->I2C_CONTROL |= RDW_I2C_RESET_FSM;
}


//-----------------------------------------------------------------------------


void  cobra_i2c_set_bit_rate(rdw_i2c index, long i2c_clock_frequency, long bit_rate)
{
		if(index == M3_I2C0)
		{
				rdw_i2c_set_bit_rate(cobra_i2c0, i2c_clock_frequency, bit_rate);	
		}
		else if(index == M3_I2C1)
		{
				rdw_i2c_set_bit_rate(cobra_i2c1, i2c_clock_frequency, bit_rate);	
		}
}


int cobra_i2c_write_byte(rdw_i2c index, unsigned char data)
{
		if(index == M3_I2C0)
		{
				return rdw_i2c_write_byte(cobra_i2c0, data);
		}
		else if(index == M3_I2C1)
		{
				return rdw_i2c_write_byte(cobra_i2c1, data);
		}
		
		return -1;
}


int cobra_i2c_read_byte (rdw_i2c index)
{
		if(index == M3_I2C0)
		{
				return rdw_i2c_read_byte(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				return rdw_i2c_read_byte(cobra_i2c1);
		}
		return -1;
}


void cobra_i2c_maste_enable(rdw_i2c index)
{
		if(index == M3_I2C0)
		{
				rdw_i2c_master_enable(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				rdw_i2c_master_enable(cobra_i2c1);
		}
}

void cobra_i2c_slave_enable(rdw_i2c index)
{
		if(index == M3_I2C0)
		{
				rdw_i2c_slave_enable(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				rdw_i2c_slave_enable(cobra_i2c1);
		}
}




void cobra_i2c_reset_fifo(rdw_i2c index)
{	
		if(index == M3_I2C0)
		{
				rdw_i2c_reset_fifos(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				rdw_i2c_reset_fifos(cobra_i2c1);
		}
	
}

void cobra_i2c_reset_fsm(rdw_i2c index)
{	
		if(index == M3_I2C0)
		{
				rdw_i2c_reset_fsm(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				rdw_i2c_reset_fsm(cobra_i2c1);
		}
	
}

uint16_t cobra_i2c_get_status(rdw_i2c index)
{
		
		if(index == M3_I2C0)
		{
				return rdw_i2c_status(cobra_i2c0);
		}
		else if(index == M3_I2C1)
		{
				return rdw_i2c_status(cobra_i2c1);
		}
		
		return 0;
	
}


//En 1, Enable  I2C
//En 0. Disable I2C

void cobra_i2c_enable_i2c(rdw_i2c index, int En)
{
		if(index == M3_I2C0)
		{
				if(En)
				{
					rdw_i2c_enable(cobra_i2c0);	
				}
				else
				{
					rdw_i2c_disable(cobra_i2c0);
				}
		}
		else if(index == M3_I2C1)
		{
				if(En)
				{
					rdw_i2c_enable(cobra_i2c1);
				}
				else
				{
					rdw_i2c_disable(cobra_i2c1);
				}
		}
}



void cobra_i2c_set_slave_address(rdw_i2c index, unsigned char address)
{
			if(index == M3_I2C0)
			{
					i2c0_slave_address = address&0x7f;
			}
			else if(index == M3_I2C1)
			{
					i2c1_slave_address = address&0x7f;
			}
}


i2c_slave_mode cobra_i2c_slave_work_mode(rdw_i2c index)
{
		unsigned char data;
		unsigned char address = 0;
		
		data = cobra_i2c_read_byte(index);
	
		if(index == M3_I2C0)
		{
				address = i2c0_slave_address;
		}
		else if(index == M3_I2C1)
		{
				address = i2c1_slave_address;
		}


		if(((data >> 1)&0x7f) == address)
		{
				if(data & 0x01)
				{
					return SLAVE_READ;    //need give data to master
				}
				else
				{
					return SLAVE_WRITE;   //need receive data
				}
		}
		return SLAVE_NULL;
}

//--------------------------------------------------------------

void app_m3_i2c_init(void)
{
	//need set I2C0 pinmux .......
		cobra_i2c_reset_fsm(M3_I2C0);		
}

unsigned char app_m3_i2c_slave_receive_byte(rdw_i2c index)
{
	    i2c_slave_mode mode;
	    unsigned char data;
			while(1)
			{
				mode = cobra_i2c_slave_work_mode(index);
				if(mode == SLAVE_NULL)
				{
					continue;
				}

				if(mode == SLAVE_WRITE )
				{
					data = cobra_i2c_read_byte(index);
					return data;
				}
			}
}

void  app_m3_i2c_slave_write_byte(rdw_i2c index, unsigned char byte)
{
			i2c_slave_mode mode;
			while(1)
			{
				mode = cobra_i2c_slave_work_mode(index);
				if(mode == SLAVE_NULL)
				{
					continue;
				}

				if(mode == SLAVE_READ )
				{
					cobra_i2c_write_byte(index, byte);
					break;
				}
			}
}

/*
just a example 
unsigned int app_download_i2c_program(void)
{

		unsigned char  key_word;
		unsigned char  cmd;
		unsigned short  length;
		unsigned char  * p_length;
		unsigned char  check_sum;
		unsigned char  temp_read;
		unsigned int  uart_index = 0;
		unsigned int  address_flag = 0;
		unsigned int  address;
		unsigned int  run_address;


		unsigned char * p_buffer; // = (unsigned char * )(address);


		unsigned int i;


		p_length = (unsigned char *)&length;



		RDW_I2C_slave_enable(I2C0);
		RDW_I2C_set_slave_address(I2C0, 0x3C);

		RDW_I2C_Reset_fifo();


		while(1)
		{

			key_word = i2c_slave_receive_byte();

			if(key_word != 0xAA)
			{
				continue;
			}


			cmd = i2c_slave_receive_byte();


			*p_length = i2c_slave_receive_byte();
			*(p_length+1) = i2c_slave_receive_byte();

			if(length > MAX_LENGTH)
			{
				continue;
			}


			check_sum = 0;
			for(i = 0; i < length; i++)
			{
				download_buffer[i] = i2c_slave_receive_byte();
				check_sum += download_buffer[i];
			}

			temp_read = i2c_slave_receive_byte();



			if(temp_read != check_sum)
			{
				rdw_uart_write(UART0, 0x88);

				i2c_slave_write_byte(0x55);
				i2c_slave_write_byte(0x02);

				continue;
			}

			rdw_uart_write(UART0, 0x99);
			i2c_slave_write_byte(0x55);
			i2c_slave_write_byte(0x01);

			uart_index = 0;


			if((cmd == 0x01) &&(address_flag))  //Data write
			{
				if(length)
				{
					//memmove(p_buffer, &download_buffer[uart_index], length);
				}
				p_buffer += length;
			}

			if(cmd == 0x02)  //COMMAND
			{
				if(download_buffer[0] == 0x01)
				{
					address_flag = 1;
					address = download_buffer[4];
					address <<= 8;
					address += download_buffer[3];
					address <<= 8;
					address += download_buffer[2];
					address <<= 8;
					address += download_buffer[1];

					p_buffer = (unsigned char * )(address);

				}
				else if(download_buffer[0] == 0x02)
				{
					run_address = download_buffer[4];
					run_address <<= 8;
					run_address += download_buffer[3];
					run_address <<= 8;
					run_address += download_buffer[2];
					run_address <<= 8;
					run_address += download_buffer[1];
					break;
				}
			}


			for(i = 0; i < MAX_LENGTH; i++)
			{
				download_buffer[i] = 0x00;
			}

		}
		return run_address;



}
*/

#endif














