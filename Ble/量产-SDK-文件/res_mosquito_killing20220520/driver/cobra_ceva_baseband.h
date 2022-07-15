/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_ceva_baseband.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/29

	Description :   
			CEAV 蓝牙基带协议
	
	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/

#ifndef __COBRA_CEVA_BASEBAND_H__
#define __COBRA_CEVA_BASEBAND_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>
#if COBRA_CEVA_BASEBADN_EN

typedef struct cobra_bluetooth_type
{
	//0x00
	//Standard registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rw_ble_cntl;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_version;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rw_ble_conf;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_int_cntl;
	//0x10
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_int_stat;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_int_raw_stat;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_int_ack;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_slot_clk;
	//0x20
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_fine_time_cnt;
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_et_current_rx_descrptr;
	
		__attribute__ ((aligned (4)))   uint32_t  reserver0;
		__attribute__ ((aligned (4)))   uint32_t  reserver1;
		
	//Deep sleep registers	
	//0x30
		 __attribute__ ((aligned (4)))   uint32_t  bluetooth_deep_sleep_cntl;
		 
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_deep_sleep_wakeup;
		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_deep_sleep_stat;
		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_enbp_reset;
	//0x40
	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_fine_cnt_corr;
		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_clk_cnt_corr;
		
		__attribute__ ((aligned (4)))   uint32_t  reserver2;
		__attribute__ ((aligned (4)))   uint32_t  reserver3;
		
	//0x50
//Validation registers	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_diag_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_diag_stat;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_debug_add_max;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_debug_add_min;
	//0x60	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_error_type_stat;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_sw_profiling;
		__attribute__ ((aligned (4)))   uint32_t  reserver4;
		__attribute__ ((aligned (4)))   uint32_t  reserver5;
	//0x70	
	//Radio registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_cntl3;
	//0x80
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_pwr_up_down0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_pwr_up_down1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_pwr_up_down2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_pwr_up_down3;
	
	//0x90
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_tx_rx_tim0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_tx_rx_tim1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_tx_rx_tim2;		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_radio_tx_rx_tim3;
	//0xA0

		__attribute__ ((aligned (4)))   uint32_t  bluetooth_spi_ptr_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_spi_ptr_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_spi_ptr_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_spi_ptr_cntl3;

//	0xB0
//Encryption registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_key_31_0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_key_63_32;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_key_95_64;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_key_127_96;
//0xC4		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_aes_ptr;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_tx_mic_value;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rx_mic_value;

//0xD0
//Regulatory Body and RF Testing Register
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rf_test_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rf_test_tx_stat;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_rf_test_rx_stat;
		__attribute__ ((aligned (4)))   uint32_t  reserver6;

//0xE0
//Timing Generator Register
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_time_gen_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_gross_time_target;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_fine_time_target;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_clkn_target;
//0xF0		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_half_micro_second_target;
		__attribute__ ((aligned (4)))   uint32_t  reserver7;
		__attribute__ ((aligned (4)))   uint32_t  reserver8;
		__attribute__ ((aligned (4)))   uint32_t  reserver9;

//0x100
//Event Scheduler Control registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_le_scheduler_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_start_event_clkn_count;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_start_event_fine_count;
		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_end_event_clkn_count;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_end_event_fine_count;
//0x114		
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_skip_event_clkn_count;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_skip_event_fine_count;
		
		__attribute__ ((aligned (4)))   uint32_t  reserver10;

		
//0x120
//Advertising timer and Scanning timer registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_advertising_time;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_scanning_time;
		__attribute__ ((aligned (4)))   uint32_t  reserver11;
		__attribute__ ((aligned (4)))   uint32_t  reserver12;
		
//0x130
//Device filtering registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_white_list_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_white_list_current_ptr;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_period_advertiser_list_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_period_advertiser_list_current_ptr;
	//0x140	
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_adi_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_adi_current_ptr;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_search_time_out;
		__attribute__ ((aligned (4)))   uint32_t  reserver13;
	
//0x150
//WLAN Coexistence Register
	  __attribute__ ((aligned (4)))   uint32_t  bluetooth_coexistence_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_coexistence_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_coexistence_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ble_priority0;
		//0x160
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ble_priority1;
		__attribute__ ((aligned (4)))   uint32_t  reserver14;
		__attribute__ ((aligned (4)))   uint32_t  reserver15;
		__attribute__ ((aligned (4)))   uint32_t  reserver16;
		
//0x170	
//Resolving Address List Registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ral_cntl;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ral_current_ptr;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ral_local_rnd;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_ral_peer_rnd;

//0x180
//Isochronous Channel 0 Registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_mute_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_tx_ptr0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_rx_ptr0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_tr_cntl0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_lower0;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_upper0;
		__attribute__ ((aligned (4)))   uint32_t  reserver17;

//0x1A0
//Isochronous Channel 1 Registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_mute_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_tx_ptr1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_rx_ptr1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_tr_cntl1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_lower1;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_upper1;
		__attribute__ ((aligned (4)))   uint32_t  reserver18;

//0x1C0
//Isochronous Channel 2 Registers
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_mute_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_tx_ptr2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_current_rx_ptr2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_tr_cntl2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_lower2;
		__attribute__ ((aligned (4)))   uint32_t  bluetooth_iso_event_cnt_offset_upper2;
		__attribute__ ((aligned (4)))   uint32_t  reserver19;
		
}COBRA_BLUETOOTH_TypeDef;


#define COBRA_CEVA_BASEBAND_BASE		(0x40100000UL)

#define COBRA_RF_SW_SPI_OFFSET		   0x400
#define COBRA_RF_BASE_ADDR 					(COBRA_CEVA_BASEBAND_BASE + 0x4000)	  //0x40104000
#define COBRA_RF_SPI_ADRESS         (COBRA_RF_BASE_ADDR + COBRA_RF_SW_SPI_OFFSET)

#define ICYV2_SPIWR                   0x80
#define ICYV2_SPIRD                   0x00

void cobra_ceva_baseband_init(void);
void cobra_ceva_baseband_spiptr_set(uint16_t spiptr);

uint32_t cobra_ceva_baseband_csem_register_read(uint16_t addr);
void cobra_ceva_baseband_csem_register_write (uint16_t addr, uint32_t value);
void cobra_ceva_baseband_csem_register_write_word (uint16_t addr, uint32_t value);
void cobra_ceva_baseband_csem_register_write_half_word (uint16_t addr, uint32_t value);
void cobra_ceva_baseband_xrfsel_setf(uint8_t xrfsel);

void cobra_ceva_baseband_jef_select_setf(uint8_t jefselect);
void cobra_ceva_baseband_sync_pulse_mode_setf(uint8_t syncpulsemode);
void cobra_ceva_baseband_sync_pulse_src_setf(uint8_t syncpulsesrc);
void cobra_ceva_baseband_csem_register_write_n_byte (uint16_t addr, uint8_t *p_buffer, uint8_t length);
//open csem mode 48M
void cobra_ceva_baseband_icytrx_xtal_48m_enable(unsigned int en);
//init csem 
//open the power
void cobra_ceva_baseband_csem_init(void);
#endif   //COBRA_CEVA_BASEBADN_EN
#endif

