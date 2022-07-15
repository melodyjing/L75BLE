/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_spi.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/11

	Description :   
           modified from coney project

	Changed Log	:

		Liujian  2019/4/11			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "cobra_spi.h"


#if COBRA_SPI_EN

///SPI environment
#if (COBRA_POWER_DOWN_ENABLE)
static struct spi_env_tag spi0_env		__attribute__((section("retention_area"),zero_init));
static struct spi_env_tag spi1_env		__attribute__((section("retention_area"),zero_init));

static volatile COBRA_SPI_TypeDef * cobra_spi0	__attribute__((section("retention_area"),zero_init));
static volatile COBRA_SPI_TypeDef * cobra_spi1	__attribute__((section("retention_area"),zero_init));
#else
static struct spi_env_tag spi0_env;
static struct spi_env_tag spi1_env;

static volatile COBRA_SPI_TypeDef * cobra_spi0;
static volatile COBRA_SPI_TypeDef * cobra_spi1;
#endif

void cobra_spi_init(void)
{
		cobra_spi0 = (COBRA_SPI_TypeDef *)COBRA_SPI0_BASE;
	  cobra_spi1 = (COBRA_SPI_TypeDef *)COBRA_SPI1_BASE;
}


/** 
Write a word of data to the SPI. 
This function will block until data can be written. 
The length of a word is determined by control.BPW register.
*/
void cobra_spi_write_word (spi_index index, unsigned int data)
{
  /* Wait until there is space in the transmit FIFO.  */
  if(index == SPI0)
  {
			while (cobra_spi0->STATUS & RDW_SPI_TX_FULL) {
			}
			/* Write data into the transmit FIFO.  */
			cobra_spi0->TX_DATA = data;
  }
  else if(index == SPI1)
  {
			while (cobra_spi1->STATUS & RDW_SPI_TX_FULL) 
			{
			}
			/* Write data into the transmit FIFO.  */
			cobra_spi1->TX_DATA = data;
  }
}

//send a byte no wait
void cobra_spi_send_byte(spi_index index, unsigned int data)
{
	  if(index == SPI0)
	  {
				cobra_spi0->TX_DATA = data;
	  }
	  else if(index == SPI1)
	  {
				cobra_spi1->TX_DATA = data;
	  }
}

/**
Read a word of data from the SPI. 
This function will block until data can be read. 
The length of a word is determined by control.BPW register.
*/
unsigned int cobra_spi_read_word (spi_index index)
{
	 if(index == SPI0)
	 {
		  /* Wait until data has been received.  */
		  while (cobra_spi0->STATUS & RDW_SPI_RX_EMPTY) {
		  }
		  /* Return the received data.  */
		  return cobra_spi0->RX_DATA;
	 }
	 else if(index == SPI1)
	 {
			 /* Wait until data has been received.  */
		  while (cobra_spi1->STATUS & RDW_SPI_RX_EMPTY) {
		  }
		  /* Return the received data.  */
		  return cobra_spi1->RX_DATA;
	 }
	 return 0xFFFFFFFF;
}

void cobra_spi_read_words (spi_index index, unsigned int *data, unsigned int count)
{
//	if((index != SPI0) &&(index != SPI1))  return ;
	if(index == SPI0)
	{
			cobra_spi0->AUTO_TRX_CONTROL = RDW_SPI_AUTO_TX;
			cobra_spi0->AUTO_TRX_COUNT = count;
			while (count > 0)
			{
				*data++ = cobra_spi_read_word (index);
				count--;
			}
	}
	else if(index == SPI1)
	{
			cobra_spi1->AUTO_TRX_CONTROL = RDW_SPI_AUTO_TX;
			cobra_spi1->AUTO_TRX_COUNT = count;
			while (count > 0)
			{
				*data++ = cobra_spi_read_word (index);
				count--;
			}
	}
}


/** 
Write and read a word of data from the SPI.
This function will block until data can be written. 
The length of a word is determined by control.BPW register.
*/
 unsigned int cobra_spi_read_write_word (spi_index index, unsigned int data)
{
	  cobra_spi_write_word (index, data);
	  return cobra_spi_read_word(index);
}

/**
Write and read a byte of data from the SPI. 
This function will block until data can be written.  
*/
 unsigned char rdw_spi_read_write_byte (spi_index index, unsigned char data)
{
	  cobra_spi_write_word (index, data);
	  return cobra_spi_read_word(index)&0xff;
}

void cobra_spi_control_set (spi_index index, operate_mode mode, clk_phase cpha,  clk_polarity cpol, bit_ording ording)
{
	unsigned short control;
	if((index != SPI0) &&(index != SPI1))  return ;
	if(index == SPI0)
		control = cobra_spi0->CONTROL;
	else 
		control = cobra_spi1->CONTROL;


	control &= ~(RDW_SPI_MODE_MASK | RDW_SPI_PHASE_MASK|RDW_SPI_POLARITY_MASK | RDW_SPI_BIT_ORDERING_MASK);
	switch(mode)
	{
		case  SPI_MASTER:
			control &= ~(RDW_SPI_MODE_MASK );
			break;
		case  SPI_SLAVE:
			control |= (RDW_SPI_MODE_MASK );
			break;
		default:
			break;
	}

	switch(cpha)
	{
		case SAMPLE_ON_LEADING_EDGE:
			control &= ~(RDW_SPI_PHASE_MASK);
			break;
		case SAMPLE_ON_TRAILING_EDGE:
			control |= (RDW_SPI_PHASE_MASK);
			break;
		default:
			break;
	}

	switch(cpol)
	{
		case IDLE_LOW:
			control &= ~(RDW_SPI_POLARITY_MASK);
			break;
		case IDLE_HIGH:
			control |= (RDW_SPI_POLARITY_MASK);
			break;
		default:
			break;
	}

	switch (ording)
	{
		case MSB_FIRST:
			control &= ~(RDW_SPI_BIT_ORDERING_MASK);
			break;
		case LSB_FIRST:
			control |= (RDW_SPI_BIT_ORDERING_MASK);
			break;
		default:
			break;
	}


	control |= 1<<8;   //reset fifo


	if(index == SPI0)
		cobra_spi0->CONTROL = control;
	else
		cobra_spi1->CONTROL = control;


}

void cobra_spi_set_bps_value(spi_index index, unsigned int bps)
{
		unsigned int apb_sys_clk;
	
		unsigned int cycles_per_bits;
		
		if((index != SPI0) &&(index != SPI1))  return ;
	
		if(index == SPI0)
		{
				apb_sys_clk = cobra_system_get_m3_apb1_clk();
		}
		else
		{
				apb_sys_clk = cobra_system_get_m3_apb2_clk();
		}
		
		if(bps >= apb_sys_clk)
		{
				bps = apb_sys_clk;
		}
		
		cycles_per_bits = apb_sys_clk/bps;
		
		if(index == SPI0)
				cobra_spi0->CYCLES_PER_BIT = cycles_per_bits;
		else
				cobra_spi1->CYCLES_PER_BIT = cycles_per_bits;

}	

void cobra_spi_set_bps(spi_index index, spi_bps bps)
{
		unsigned short cycles_per_bits;
	
		unsigned int apb_sys_clk;
		
		if((index != SPI0) &&(index != SPI1))  return ;
	
		if(index == SPI0)
		{
				apb_sys_clk = cobra_system_get_m3_apb1_clk();
		}
		else
		{
				apb_sys_clk = cobra_system_get_m3_apb2_clk();
		}
		switch(bps)
		{
			case SPI_10KHZ:
				cycles_per_bits = apb_sys_clk/10000;
				break;
			case SPI_50KHZ:
				cycles_per_bits = apb_sys_clk/50000;
				break;
			case SPI_100KHZ:
				cycles_per_bits = apb_sys_clk/100000;
				break;
			case SPI_200KHZ:
				cycles_per_bits = apb_sys_clk/200000;
				break;
			case SPI_1MHZ:
				cycles_per_bits = apb_sys_clk/1000000;
				break;
			case SPI_2MHZ:
				cycles_per_bits = apb_sys_clk/2000000;
				break;
			case SPI_4MHZ:
				cycles_per_bits = apb_sys_clk/4000000;
				break;
			case SPI_8MHZ:
				cycles_per_bits = apb_sys_clk/8000000;
				break;
			case SPI_12MHZ:
				cycles_per_bits = apb_sys_clk/12000000;
				break;
			case SPI_16MHZ:
				cycles_per_bits = apb_sys_clk/16000000;
				break;
			case SPI_32MHZ:
				cycles_per_bits = apb_sys_clk/32000000;
				break;
			case SPI_48MHZ:
				cycles_per_bits = apb_sys_clk/48000000;
				break;
			case SPI_64MHZ:
				cycles_per_bits = apb_sys_clk/64000000;
				break;
			default:
				break;
		}

		if(index == SPI0)
				cobra_spi0->CYCLES_PER_BIT = cycles_per_bits;
		else
				cobra_spi1->CYCLES_PER_BIT = cycles_per_bits;
}

/** Set SPI bits per word.  */
void cobra_spi_set_bits_per_word (spi_index index, spi_word_size bits_per_word)
{
	  unsigned short control;

	  if((index != SPI0) &&(index != SPI1))  return ;
	  /* Mask out existing size. */
	  if(index == SPI0)
		  control = cobra_spi0->CONTROL;
	  else
		  control = cobra_spi1->CONTROL;


	  control &= ~(RDW_SPI_BPW_MASK | RDW_SPI_WORD_SIZE_MASK);

	  switch(bits_per_word)
	  {
	  	  case SPI_8bit:
	  		control |= RDW_SPI_8_BITS;
	  		  break;
	  	  case SPI_16bit:
	  		control |= RDW_SPI_16_BITS;
	  		  break;
	  	  case SPI_32bit:
	  		control |= (32 << 10);
	  		  break;
	  	  default:
	  		  break;
	  }

	  if(index == SPI0)
		  	cobra_spi0->CONTROL = control;
	  else
		    cobra_spi1->CONTROL = control;
}

/** Enable SPI.  */
void cobra_spi_enable (spi_index index)
{
  /* Set the enable flag.  */
	if(index == SPI0)
			cobra_spi0->CONTROL |= RDW_SPI_ENABLE;
	else if(index == SPI1)
			cobra_spi1->CONTROL |= RDW_SPI_ENABLE;
}

/** Disable SPI.  */
void cobra_spi_disable (spi_index index)
{
  /* Clear the enable flag.  */
	if(index == SPI0)
			cobra_spi0->CONTROL &= ~RDW_SPI_ENABLE;
	else
			cobra_spi1->CONTROL &= ~RDW_SPI_ENABLE;
}



void cobra_spi_enable_tx_interrupt (spi_index index)
{
		if(index == SPI0)
				cobra_spi0->CONTROL |= RDW_SPI_TX_INT_ENABLE;
		else if(index == SPI1)
				cobra_spi1->CONTROL |= RDW_SPI_TX_INT_ENABLE;
}

void cobra_spi_disbale_tx_interrupt (spi_index index)
{

		if(index == SPI0)
				cobra_spi0->CONTROL &= ~RDW_SPI_TX_INT_ENABLE;
		else if(index == SPI1)
				cobra_spi1->CONTROL &= ~RDW_SPI_TX_INT_ENABLE;
}


void  cobra_spi_enable_rx_interrupt (spi_index index)
{
		if(index == SPI0)
			cobra_spi0->CONTROL |= RDW_SPI_RX_INT_ENABLE;
		else if(index == SPI1)
			cobra_spi1->CONTROL |= RDW_SPI_RX_INT_ENABLE;
}

void cobra_spi_disbale_rx_interrupt (spi_index index)
{
		if(index == SPI0)
				cobra_spi0->CONTROL &= ~RDW_SPI_RX_INT_ENABLE;
		else if(index == SPI1)
				cobra_spi1->CONTROL &= ~RDW_SPI_RX_INT_ENABLE;
}

void cobra_spi_reset_fifo (spi_index index)
{
		if(index == SPI0)
				cobra_spi0->CONTROL |= RDW_SPI_RESET_FIFOS;
		else if(index == SPI1)
				cobra_spi1->CONTROL |= RDW_SPI_RESET_FIFOS;
}

bool cobra_spi_get_tx_fifo_full_state (spi_index index)
{
	 if(index == SPI0)
	 {
			return cobra_spi0->STATUS & RDW_SPI_TX_FULL;
	 }
	 else if(index == SPI1)
	 {
		 return cobra_spi1->STATUS & RDW_SPI_TX_FULL;
	 }
	 
	 return true;

}

bool cobra_spi_get_tx_fifo_empty_state (spi_index index)
{
	 
	 if(index == SPI0)
	 {
			return cobra_spi0->STATUS & RDW_SPI_TX_EMPTY;
	 }
	 else if(index == SPI1)
	 {
			return cobra_spi1->STATUS & RDW_SPI_TX_EMPTY;
	 }
	 
	 return true;

}

bool cobra_spi_get_rx_fifo_full_state (spi_index index)
{
	 if(index == SPI0)
	 {
			return cobra_spi0->STATUS & RDW_SPI_RX_FULL;
	 }
	 else if(index == SPI1)
	 {
		 return cobra_spi1->STATUS & RDW_SPI_RX_FULL;
	 }
	 
	 return true;

}

bool cobra_spi_get_rx_fifo_empty_state (spi_index index)
{

	 if(index == SPI0)
	 {
			return cobra_spi0->STATUS & RDW_SPI_RX_EMPTY;
	 }
	 else if(index == SPI1)
	 {
			return cobra_spi1->STATUS & RDW_SPI_RX_EMPTY;
	 }
	 
	 return true;

}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///application

//void app_cobra_spi_init(void)
//{
//		cobra_pcrm_spi0_active_mode_clock_enable(1);
//		cobra_pcrm_spi1_active_mode_clock_enable(1);
//	
//		cobra_spi_control_set(SPI0, SPI_SLAVE, SAMPLE_ON_LEADING_EDGE, IDLE_LOW, MSB_FIRST);
//		cobra_spi_control_set(SPI1, SPI_MASTER, SAMPLE_ON_TRAILING_EDGE, IDLE_HIGH, MSB_FIRST);

//		cobra_spi_set_bps(SPI0, SPI_100KHZ);
//		cobra_spi_set_bps(SPI1, SPI_100KHZ);

//		cobra_spi_set_bits_per_word(SPI0, SPI_8bit);
//		cobra_spi_set_bits_per_word(SPI1, SPI_8bit);

//		cobra_spi_disbale_tx_interrupt(SPI0);
//		cobra_spi_disbale_tx_interrupt(SPI1);

//		cobra_spi_disbale_rx_interrupt(SPI0);
//		cobra_spi_disbale_rx_interrupt(SPI1);
//	
//		NVIC_SetPriority(SPI0_IRQn, 1);
//		NVIC_EnableIRQ(SPI0_IRQn);
//	

//		cobra_spi_enable(SPI0);
//}

void app_cobra_spi0_init(unsigned int master_enable)
{
		cobra_pcrm_spi0_active_mode_clock_enable(1);

		if(master_enable)
		{
				cobra_spi_control_set(SPI0, SPI_MASTER, SAMPLE_ON_TRAILING_EDGE, IDLE_HIGH, MSB_FIRST);
		}
		else
		{
				cobra_spi_control_set(SPI0, SPI_SLAVE, SAMPLE_ON_LEADING_EDGE, IDLE_LOW, MSB_FIRST);
		}

		cobra_spi_set_bps(SPI0, SPI_100KHZ);

		cobra_spi_set_bits_per_word(SPI0, SPI_8bit);

		cobra_spi_disbale_tx_interrupt(SPI0);

		cobra_spi_disbale_rx_interrupt(SPI0);

		cobra_spi_enable(SPI0);
	
		NVIC_ClearPendingIRQ(SPI0_IRQn);
	
		NVIC_SetPriority(SPI0_IRQn, 1);
		NVIC_EnableIRQ(SPI0_IRQn);
		
}

void app_cobra_spi1_init(unsigned int master_enable)
{
		cobra_pcrm_spi1_active_mode_clock_enable(1);

		if(master_enable)
		{
				cobra_spi_control_set(SPI1, SPI_MASTER, SAMPLE_ON_TRAILING_EDGE, IDLE_HIGH, MSB_FIRST);
		}
		else
		{
				cobra_spi_control_set(SPI1, SPI_SLAVE, SAMPLE_ON_LEADING_EDGE, IDLE_LOW, MSB_FIRST);
		}

		cobra_spi_set_bps(SPI1, SPI_100KHZ);

		cobra_spi_set_bits_per_word(SPI1, SPI_8bit);

		cobra_spi_disbale_tx_interrupt(SPI1);

		cobra_spi_disbale_rx_interrupt(SPI1);

		cobra_spi_enable(SPI1);
	
		NVIC_ClearPendingIRQ(SPI1_IRQn);
	
		NVIC_SetPriority(SPI1_IRQn, 1);
		NVIC_EnableIRQ(SPI1_IRQn);
		
}


void app_cobra_spi_receive_data(spi_index index)
{
		void  (*callback)(void)  = NULL;
		if(index == SPI0)
		{
			  /* Wait until data has been received.  */
			  while (!(cobra_spi0->STATUS & RDW_SPI_RX_EMPTY)) {
					  // Read the received data in the FIFO
					  *spi0_env.rx.bufptr = cobra_spi0->RX_DATA;
					  // Decrement size
					  spi0_env.rx.size_rd--;
					  // Increment buffer
					  spi0_env.rx.bufptr++;
			  }
			  // Check if all expected data have been received
			  if (spi0_env.rx.size_rd > 0)
			  {
				  // Send dummy bytes to receive data
				  while ((!cobra_spi_get_rx_fifo_full_state(SPI0)) && (!cobra_spi_get_tx_fifo_full_state(SPI0)) && spi0_env.rx.size_wr > 0)
				  {
					    // Fill dummy byte
					    cobra_spi_send_byte(SPI0, SPI_DUMMY_BYTE);

							// Decrement size
							spi0_env.rx.size_wr--;
				  }

				  // Enable RX FIFO interrupt to detect incoming bytes
				  cobra_spi_enable_tx_interrupt(SPI0);
			  }
			  else
			  {
				  callback = spi0_env.rx.callback;

				  // Reset RX parameters
				  spi0_env.rx.bufptr = NULL;
				  spi0_env.rx.callback = NULL;

				  // Call end of reception callback
				  if(callback != NULL)
				  {
							callback();
				  }
			  }

		}
		else if(index == SPI1)
		{
				/* Wait until data has been received.  */
				while (!(cobra_spi1->STATUS & RDW_SPI_RX_EMPTY)) {
						// Read the received data in the FIFO
						*spi1_env.rx.bufptr = cobra_spi1->RX_DATA;
						// Decrement size
						spi1_env.rx.size_rd--;
						// Increment buffer
						spi1_env.rx.bufptr++;
				}
				// Check if all expected data have been received
				if (spi1_env.rx.size_rd > 0)
				{
					// Send dummy bytes to receive data
					while ((!cobra_spi_get_rx_fifo_full_state(SPI1)) && (!cobra_spi_get_tx_fifo_full_state(SPI1)) && spi1_env.rx.size_wr > 0)
					{
							// Fill dummy byte
							cobra_spi_send_byte(SPI1, SPI_DUMMY_BYTE);

							// Decrement size
							spi1_env.rx.size_wr--;
					}

					// Enable RX FIFO interrupt to detect incoming bytes
					cobra_spi_enable_tx_interrupt(SPI1);
				}
				else
				{
					callback = spi1_env.rx.callback;

					// Reset RX parameters
					spi1_env.rx.bufptr = NULL;
					spi1_env.rx.callback = NULL;

					// Call end of reception callback
					if(callback != NULL)
					{
							callback();
					}
				}
		}
}



void app_cobra_spi_transmit_data(spi_index index)
{
		void  (*callback)(void)  = NULL;
		volatile uint8_t dummy;
		if(index == SPI1)
		{
			// Flush RX FIFO
			while (!cobra_spi_get_rx_fifo_empty_state(SPI1))
			{
				// Flush the received data in the FIFO
				dummy = cobra_spi1->RX_DATA;

				// Decrement size
				spi1_env.tx.size_rd--;
			}

			// Check if all expected data have been sent
			if (spi1_env.tx.size_rd > 0)
			{
				// Fill TX data into FIFO
				while ((!cobra_spi_get_rx_fifo_full_state(SPI1)) && (!cobra_spi_get_tx_fifo_full_state(SPI1)) && spi1_env.tx.size_wr > 0)
				{
						// Write the TX data into the FIFO
						cobra_spi_send_byte(SPI1, *spi1_env.tx.bufptr);

						// Decrement size
						spi1_env.tx.size_wr--;

						// Increment buffer
						spi1_env.tx.bufptr++;

				}

				// Enable RX FIFO interrupt to detect incoming bytes
				cobra_spi_enable_tx_interrupt(SPI1);

			}
			else
			{
				callback = spi1_env.tx.callback;

				// Reset TX parameters
				spi1_env.tx.bufptr = NULL;
				spi1_env.tx.callback = NULL;

				// Call end of transmission callback
				if(callback != NULL)
				{
					callback();
				}
			}
		}
		else if(index == SPI0)
		{
				// Flush RX FIFO
				while (!cobra_spi_get_rx_fifo_empty_state(SPI0))
				{
						// Flush the received data in the FIFO
						dummy = cobra_spi0->RX_DATA;

						// Decrement size
						spi0_env.tx.size_rd--;
				}

				// Check if all expected data have been sent
				if (spi0_env.tx.size_rd > 0)
				{
						// Fill TX data into FIFO
						while ((!cobra_spi_get_rx_fifo_full_state(SPI0)) && (!cobra_spi_get_tx_fifo_full_state(SPI0)) && spi0_env.tx.size_wr > 0)
						{
							// Write the TX data into the FIFO
							cobra_spi_send_byte(SPI0, *spi0_env.tx.bufptr);

							// Decrement size
							spi0_env.tx.size_wr--;

							// Increment buffer
							spi0_env.tx.bufptr++;

						}

						// Enable RX FIFO interrupt to detect incoming bytes
						cobra_spi_enable_tx_interrupt(SPI0);

				}
				else
				{
						callback = spi0_env.tx.callback;

						// Reset TX parameters
						spi0_env.tx.bufptr = NULL;
						spi0_env.tx.callback = NULL;

						// Call end of transmission callback
						if(callback != NULL)
						{
							 callback();
						}
				}
		}

}


void app_cobra_spi0_enviroment_init(void)
{
	 	 //Configure SPI environment
	    spi0_env.rx.bufptr = NULL;
	    spi0_env.rx.size_rd = 0;
	    spi0_env.rx.size_wr = 0;
	    spi0_env.rx.callback = NULL;
	    spi0_env.tx.bufptr = NULL;
	    spi0_env.tx.size_rd = 0;
	    spi0_env.tx.size_wr = 0;
	    spi0_env.tx.callback = NULL;

	    cobra_spi_reset_fifo(SPI0);

}

void app_cobra_spi1_enviroment_init(void)
{
	 	  spi1_env.rx.bufptr = NULL;
	    spi1_env.rx.size_rd = 0;
	    spi1_env.rx.size_wr = 0;
	    spi1_env.rx.callback = NULL;
	    spi1_env.tx.bufptr = NULL;
	    spi1_env.tx.size_rd = 0;
	    spi1_env.tx.size_wr = 0;
	    spi1_env.tx.callback = NULL;

	    cobra_spi_reset_fifo(SPI1);
}


//void app_cobra_spi_enviroment_init(void)
//{
//	 	 //Configure SPI environment
//	    spi0_env.rx.bufptr = NULL;
//	    spi0_env.rx.size_rd = 0;
//	    spi0_env.rx.size_wr = 0;
//	    spi0_env.rx.callback = NULL;
//	    spi0_env.tx.bufptr = NULL;
//	    spi0_env.tx.size_rd = 0;
//	    spi0_env.tx.size_wr = 0;
//	    spi0_env.tx.callback = NULL;

//	    spi1_env.rx.bufptr = NULL;
//	    spi1_env.rx.size_rd = 0;
//	    spi1_env.rx.size_wr = 0;
//	    spi1_env.rx.callback = NULL;
//	    spi1_env.tx.bufptr = NULL;
//	    spi1_env.tx.size_rd = 0;
//	    spi1_env.tx.size_wr = 0;
//	    spi1_env.tx.callback = NULL;

//	    cobra_spi_reset_fifo(SPI0);
//	    cobra_spi_reset_fifo(SPI1);

//}

void app_cobra_spi_read(spi_index index, uint8_t *bufptr, uint32_t size, void (*rx_callback)(void))
{
	if((index != SPI0) &&(index != SPI1))  return ;

    //Store environment parameters
	if(index == SPI0)
	{
			spi0_env.rx.bufptr = bufptr;
			spi0_env.rx.size_wr = size;
			spi0_env.rx.size_rd = size;
			spi0_env.rx.callback = rx_callback;
	}
	else
	{
			spi1_env.rx.bufptr = bufptr;
			spi1_env.rx.size_wr = size;
			spi1_env.rx.size_rd = size;
			spi1_env.rx.callback = rx_callback;
	}

    // Start data reception
    app_cobra_spi_receive_data(index);
}

void app_cobra_spi_write(spi_index index, uint8_t *bufptr, uint32_t size, void (*tx_callback)(void))
{
		if((index != SPI0) &&(index != SPI1))  return ;
			//Store environment parameters
		if(index == SPI0)
		{
			spi0_env.tx.bufptr = bufptr;
			spi0_env.tx.size_wr = size;
			spi0_env.tx.size_rd = size;
			spi0_env.tx.callback = tx_callback;
		}
		else
		{
			spi1_env.tx.bufptr = bufptr;
			spi1_env.tx.size_wr = size;
			spi1_env.tx.size_rd = size;
			spi1_env.tx.callback = tx_callback;
		}
		// Start data transmission
		app_cobra_spi_transmit_data(index);
}

void cobra_m3_spi0_interrupt(void)
{

		if(spi0_env.rx.bufptr != NULL)
		{
				app_cobra_spi_receive_data(SPI0);
		}
		else if(spi0_env.tx.bufptr != NULL)
		{
				app_cobra_spi_transmit_data(SPI0);
		}

}

void cobra_m3_spi1_interrupt(void)
{
		// RX or TX
		if(spi1_env.rx.bufptr != NULL)
		{
				app_cobra_spi_receive_data(SPI1);
		}
		else if(spi1_env.tx.bufptr != NULL)
		{
				app_cobra_spi_transmit_data(SPI1);
		}

}










#endif














