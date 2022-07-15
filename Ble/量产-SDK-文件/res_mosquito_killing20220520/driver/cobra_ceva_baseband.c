/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_ceva_baseband.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/29

	Description :   


	Changed Log	:

		Liujian  2019/3/29			- Creation



*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cobra_config.h>
#include "cobra_aon_dcdc.h"
#include "cobra_aon_pmu.h"
#include "cobra_pcrm.h"
#include "cobra_ceva_baseband.h"

#if COBRA_CEVA_BASEBADN_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_BLUETOOTH_TypeDef * cobra_ceva_baseband		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_BLUETOOTH_TypeDef * cobra_ceva_baseband;
#endif



void cobra_ceva_baseband_init(void)
{
		cobra_ceva_baseband = (COBRA_BLUETOOTH_TypeDef *)COBRA_CEVA_BASEBAND_BASE;
	
//		cobra_ceva_baseband_spiptr_set(COBRA_RF_SW_SPI_OFFSET);
	
}

/*
SW driven SPI structure pointer.
Value set by the RW-BLE Software

*/
void cobra_ceva_baseband_spiptr_set(uint16_t spiptr)
{
		cobra_ceva_baseband->bluetooth_radio_cntl0 &= ~((unsigned int)0xffff << 16); 
		cobra_ceva_baseband->bluetooth_radio_cntl0 |= spiptr << 16;	
}

/*
Radio Selection
To be defined by the user
*/

void cobra_ceva_baseband_xrfsel_setf(uint8_t xrfsel)
{
		cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(0x3f << 4);
		cobra_ceva_baseband->bluetooth_radio_cntl1 |= ((xrfsel&0x3f) << 4);
}

/*
Selects Jitter Elimination FIFO
0 Not selected
1 Selected
To be defined by the user
*/
void cobra_ceva_baseband_jef_select_setf(uint8_t jefselect)
{
		if(jefselect)
		{
				cobra_ceva_baseband->bluetooth_radio_cntl1 |= 1 << 12;
		}
		else
		{
			  cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(1 << 12);
		}
}

/*
Defines whether Access Address Synchronization detection is a pulse or a level
0: Synchronization detection is a pulse
1: Synchronization detection is a level
To be defined by the user
*/
void cobra_ceva_baseband_sync_pulse_mode_setf(uint8_t syncpulsemode)
{
		if(syncpulsemode)
		{
			 cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(1 << 15);
		}
		else
		{
			 cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(1 << 15);
		}
}

/*
Defines whether Access Address Synchronization detection is generated internally or comes from the Radio
0: Internal detection
1: External detection
To be defined by the user

*/
void cobra_ceva_baseband_sync_pulse_src_setf(uint8_t syncpulsesrc)
{
   if(syncpulsesrc)
		{
			 cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(1 << 14);
		}
		else
		{
			 cobra_ceva_baseband->bluetooth_radio_cntl1 &= ~(1 << 14);
		}
}


/*
SW driven SPI Access triggered when written with a 1

Always read at 0
*/
void cobra_ceva_baseband_spigo_enable(uint8_t en)
{	
		if(en)
		{
				cobra_ceva_baseband->bluetooth_radio_cntl0 |= 0x01;
		}
		else
		{
			  cobra_ceva_baseband->bluetooth_radio_cntl0 &= ~0x01;
		}
}

/*
SW driven SPI Access completion
0 SW driven SPI Access pending or on-going
1: SW driven SPI Access completed

*/
unsigned int cobra_ceva_baseband_spigo_complete(void)
{
		if(cobra_ceva_baseband->bluetooth_radio_cntl0 & (1 << 1))
				return 1;
		else
				return 0;
	
}


void cobra_ceva_baseband_spi_go(void)
{
		volatile unsigned int test_i;
    //launch SPI transfer
    cobra_ceva_baseband_spigo_enable(1);

    //wait for transfer to be completed
    while (!cobra_ceva_baseband_spigo_complete());
	//	for(test_i = 0; test_i < 100; test_i++); 
}


static uint8_t cobra_ceva_rf_buffer[16];
uint32_t cobra_ceva_baseband_csem_register_read(uint16_t addr)
{
    // Next Pointr to 0x0
    cobra_ceva_rf_buffer[0] = (uint8_t)(0);  // Next Pointer set to 0x0000 to stop the SPI Chained access
    cobra_ceva_rf_buffer[1] = (uint8_t)(0);

    //copy control and number of u32 to send
    cobra_ceva_rf_buffer[2] = (uint8_t)(ICYV2_SPIRD + 1);

    //copy address
    cobra_ceva_rf_buffer[3] = (uint8_t)(addr & 0x00FF);
    cobra_ceva_rf_buffer[4] = (uint8_t)((addr & 0xFF00) >> 8 );

    memcpy((void *)COBRA_RF_SPI_ADRESS, cobra_ceva_rf_buffer, 5);

    //do the transfer
    cobra_ceva_baseband_spi_go();

    return (uint32_t)(*((uint8_t *)(COBRA_RF_SPI_ADRESS + 5)));
}

void cobra_ceva_baseband_csem_register_write (uint16_t addr, uint32_t value)
{
    cobra_ceva_rf_buffer[0] = (uint8_t)(0);  // Next Pointer set to 0x0000 to stop the SPI Chained access
    cobra_ceva_rf_buffer[1] = (uint8_t)(0);

    //inversion for EM reading by U8 on ATL SPI side
    //copy control and number of u32 to send
    cobra_ceva_rf_buffer[2] = (uint8_t)(ICYV2_SPIWR + 1);

    //copy address
    cobra_ceva_rf_buffer[3] = (uint8_t)(addr & 0x00FF);
    cobra_ceva_rf_buffer[4] = (uint8_t)((addr & 0xFF00) >> 8 );

    // Byte to be written
    cobra_ceva_rf_buffer[5]= (uint8_t)value;

    memcpy((void *)COBRA_RF_SPI_ADRESS, cobra_ceva_rf_buffer, 6);

    //do the transfer
    cobra_ceva_baseband_spi_go();
}

/*
Ð´Ò»¸öword 4¸öBYTE
*/
void cobra_ceva_baseband_csem_register_write_word (uint16_t addr, uint32_t value)
{
    cobra_ceva_rf_buffer[0] = (uint8_t)(0);  // Next Pointer set to 0x0000 to stop the SPI Chained access
    cobra_ceva_rf_buffer[1] = (uint8_t)(0);

    //inversion for EM reading by U8 on ATL SPI side
    //copy control and number of u32 to send
    cobra_ceva_rf_buffer[2] = (uint8_t)(ICYV2_SPIWR + 4);

    //copy address
    cobra_ceva_rf_buffer[3] = (uint8_t)(addr & 0x00FF);
    cobra_ceva_rf_buffer[4] = (uint8_t)((addr & 0xFF00) >> 8 );

    // Byte to be written
    cobra_ceva_rf_buffer[5]= (uint8_t)value;
	  cobra_ceva_rf_buffer[6]= (uint8_t)(value>>8);
	  cobra_ceva_rf_buffer[7]= (uint8_t)(value>>16);
	  cobra_ceva_rf_buffer[8]= (uint8_t)(value>>24);

    memcpy((void *)COBRA_RF_SPI_ADRESS, cobra_ceva_rf_buffer, 9);

    //do the transfer
    cobra_ceva_baseband_spi_go();
}

void cobra_ceva_baseband_csem_register_write_half_word (uint16_t addr, uint32_t value)
{
    cobra_ceva_rf_buffer[0] = (uint8_t)(0);  // Next Pointer set to 0x0000 to stop the SPI Chained access
    cobra_ceva_rf_buffer[1] = (uint8_t)(0);

    //inversion for EM reading by U8 on ATL SPI side
    //copy control and number of u32 to send
    cobra_ceva_rf_buffer[2] = (uint8_t)(ICYV2_SPIWR + 2);

    //copy address
    cobra_ceva_rf_buffer[3] = (uint8_t)(addr & 0x00FF);
    cobra_ceva_rf_buffer[4] = (uint8_t)((addr & 0xFF00) >> 8 );

    // Byte to be written
    cobra_ceva_rf_buffer[5]= (uint8_t)value;
	  cobra_ceva_rf_buffer[6]= (uint8_t)(value>>8);

    memcpy((void *)COBRA_RF_SPI_ADRESS, cobra_ceva_rf_buffer, 7);

    //do the transfer
    cobra_ceva_baseband_spi_go();
}

void cobra_ceva_baseband_csem_register_write_n_byte (uint16_t addr, uint8_t *p_buffer, uint8_t length)
{
	  
#if 0	
    cobra_ceva_rf_buffer[0] = (uint8_t)(0);  // Next Pointer set to 0x0000 to stop the SPI Chained access
    cobra_ceva_rf_buffer[1] = (uint8_t)(0);

    //inversion for EM reading by U8 on ATL SPI side
    //copy control and number of u32 to send
    cobra_ceva_rf_buffer[2] = (uint8_t)(ICYV2_SPIWR + length);

    //copy address
    cobra_ceva_rf_buffer[3] = (uint8_t)(addr & 0x00FF);
    cobra_ceva_rf_buffer[4] = (uint8_t)((addr & 0xFF00) >> 8 );

	 memcpy(&cobra_ceva_rf_buffer[5], p_buffer, length);
    // Byte to be written
//    cobra_ceva_rf_buffer[5]= (uint8_t)value;
//	  cobra_ceva_rf_buffer[6]= (uint8_t)(value>>8);

    memcpy((void *)COBRA_RF_SPI_ADRESS, cobra_ceva_rf_buffer, 5+length);
#else
	
	  uint8_t *p_rf_buffer = (uint8_t *)(COBRA_RF_SPI_ADRESS);
		
		*p_rf_buffer++ = 0x00;
		*p_rf_buffer++ = 0x00;
	
	  *p_rf_buffer++ = (uint8_t)(ICYV2_SPIWR + length);
		
		*p_rf_buffer++ = (uint8_t)(addr & 0x00FF);
		*p_rf_buffer++ = (uint8_t)((addr & 0xFF00) >> 8 );
		
		memcpy(p_rf_buffer, p_buffer, length);
				
#endif	
    //do the transfer
    cobra_ceva_baseband_spi_go();
}


/*

Bits 			Name 														Description 																									|	Reset
-------------------------------------------------------------------------------------------------------------
31:28 		xo_thr_high 						High threshold for xtal trimming 																						|	0xc
--------------------------------------------------------------------------------------------------------------
27:24 		xo_thr_low 							Low threshold for xtal trimming 																						|	0x3
--------------------------------------------------------------------------------------------------------------
23:22 		xo_a_s_curr_sel_high 		Value of after_startup_curr_sel 
																	when level is higher than xo_thr_high 																| 0x2
----------------------------------------------------------------------------------------------------------------
21:20 	  xo_a_s_curr_sel_low     Value of after_startup_curr_sel 
																	when level is lower than xo_thr_low   																| 0x0
----------------------------------------------------------------------------------------------------------------
19 			  low_clk_ready_th_en     If set to 1, the clk_ready threshold is set to a lower value 					|	0x0
----------------------------------------------------------------------------------------------------------------
18 			  xtal_ctrl_bypass 				Bypass the Xtal control algorithm 																		|	0x0
-----------------------------------------------------------------------------------------------------------------
17 			  dig_clk_in_sel 		  		If set to 1 selects the clk_in_dig signal for the digital block,
																	otherwise the internal xtal 																						  0x0
------------------------------------------------------------------------------------------------------------------
16 				xo_en_b_reg 			  		Xtal oscillator enable (active low) 																		   0x1
----------------------------------------------------------------------------------------------------------------
15:14 		xtal_ckdiv Xtal 				trimming speed: 
																	00 => 43us, 
																	01 => 85us, 
																	10 => 171us, 
																	11 => 341us 																															0x0
------------------------------------------------------------------------------------------------------------------

13				 clk_out_en_b 					When high, 
																	disable the output clock to go to main IP (clk_out output stay low). 			0x0
------------------------------------------------------------------------------------------------------------------
12 				 reg_value_sel
																	When low, all main ctrl signals are used instead of corresponding 
																	ctrl signal or some control bits of xtal_reg. 
																	They are: xo_en_b, ext_clk_mode and lp_mode. 															0x0
																	When high, corresponding ctrl signal and some control bits of 
																	xtal_reg are used instead of main ctrl signals. They are:
																	xo_en_b_reg, ext_clk_mode (bit 0) and lp_mode (bit 1).
-------------------------------------------------------------------------------------------------------------------															

11:10 		afterstartup_curr_sel		Selection of the current before amplitude stabilization but 
																	after starting-up in active transistors of the core oscillator: 
																	'00': typ. 0.15 mA, 
																	'01': typ. 0.24 mA, 
																	'10': typ. 0.40 mA, 
																	'11': typ. 0.61 mA                                                        0x1
--------------------------------------------------------------------------------------------------------------------
9:8 			startup_curr_sel        Selection of the starting-up current in active transistors of 
																	the core oscillator: 
																	'00': typ. 0.41mA, 
																	'01': typ. 0.59 mA, 
																	'10': typ. 0.88 mA, 
																	'11': typ. 1.24 mA																												0x1
--------------------------------------------------------------------------------------------------------------------
7 				inv_clk_dig 						Invert clock on clk_dig output 																						0x0
--------------------------------------------------------------------------------------------------------------------
6 				inv_clk_pll 						Invert clock on clk_pll output 																						0x0
-------------------------------------------------------------------------------------------------------------------
5 			 force_clk_ready					Debug: allow to force output clocks on 
																	clk_pll, clk_dig and clk_out (if these outputs are enabled) 
																	and bypass the xtal internal clock detector that 
																	gates these clock outputs.																								0x0
-------------------------------------------------------------------------------------------------------------------
4 				clk_dig_en_b 						When high, disable the output clock to go to digital
																	(clk_dig output stay low). 																								0x0
--------------------------------------------------------------------------------------------------------------------
3 				buff_en_b								When low (and if xtal_en_b(_reg) is low), 
																	the xtal buffer is enabled otherwise it is disabled.
																	Could be used to decrease the power consumption of the xtal 
																	while maintaining oscillation in the xtal oscillator											0x0
---------------------------------------------------------------------------------------------------------------------
2 				hp_mode									When high, bias current in the clock buffer is increased 
																	compared to normal operation 
																	(high	bandwidth mode in 132 MHz clock input buffer).											0x0
-----------------------------------------------------------------------------------------------------------------------																	
1 				lp_mode									When high, bias current in the clock buffer is reduced 
																	compared to normal operation (low	power mode). 
																	Usable only if bit 12 is high (see below) 
																	otherwise it is bypassed by lp_mode	pin input on main interface						0x0
-----------------------------------------------------------------------------------------------------------------------
0 				ext_clk_mode						When high, allow to uses xtal_p (and eventually xtal_n) 
																	has external clock input(s). 
																	The XTAL	oscillator core is disabled. 
																	Usable only if bit 12 is high (see below) otherwise 
																	it is bypassed by ext_clk_mode pin input on main interface								0x0
------------------------------------------------------------------------------------------------------------------------

*/
void cobra_ceva_baseband_icytrx_xtal_48m_enable(unsigned int en)
{

		if(en)
		{	
					cobra_ceva_baseband_csem_register_write(0xB4, 0x80);
			    cobra_ceva_baseband_csem_register_write(0xB5, 0x15);
			    cobra_ceva_baseband_csem_register_write(0xB6, 0x84);
			    cobra_ceva_baseband_csem_register_write(0xB7, 0xC3);
		}
		else
		{
					cobra_ceva_baseband_csem_register_write(0xB4, 0x00);
			    cobra_ceva_baseband_csem_register_write(0xB5, 0x05);
			    cobra_ceva_baseband_csem_register_write(0xB6, 0x81);
			    cobra_ceva_baseband_csem_register_write(0xB7, 0xC3);		
		}
	
}

/*
//init csem 
//open the power
*/
void cobra_ceva_baseband_csem_init(void)
{
	  volatile int i;
		//open the icy power
		cobra_ceva_baseband_spiptr_set(COBRA_RF_SW_SPI_OFFSET);
		cobra_aon_dcdc_ptat_enb(1);
		cobra_aon_dcdc_bgp_en(1);
		cobra_aon_dcdc_ldo_vddd_en(1);
		
		for(i = 0; i < 10; i++); 
		
//		cobra_pcrm_icytrx_isolate_en(0);     //enable the icytrx signal
#if COBRA_MCU_IS_VER1
		cobra_pcrm_ldoadc_isolate_en(0);
		cobra_pcrm_ldopll_isolate_en(0);
#endif
		
			cobra_ceva_baseband_xrfsel_setf(0x04);
			cobra_ceva_baseband_jef_select_setf(1);
			cobra_ceva_baseband_sync_pulse_mode_setf(1);
			cobra_ceva_baseband_sync_pulse_src_setf(1);
	
}

#endif   //COBRA_CEVA_BASEBADN_EN














