/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_umcu.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/28

	Description :   
	//Base addr  0x40083000
	//Reg
                      bit   r/w  default  addr   des
    ---------------------------------------------------------------
     27'b0
     uc_memclk_sel      6    RW    0             0: HCLK 1: ~HCLK(uCPU) // CONEY only
     va_chn_sel         5    RW    0
     uc_done            4    R
     ram_clk_sven       3    RW    0    
     uc_owner_p0u1      2    RW    0(cobra)1(coney) 0: AHB  1: uCPU
     uc_enable          1    RW    0    
     uc_mem_sel         0    RW    0             0: ROM  1: RAM    // CONEY only
                                          0x000
    ---------------------------------------------------------------
     22'b0
     aon_pad_cfg4     9:8    RW    0
     aon_pad_cfg3     7:6    RW    0
     aon_pad_cfg2     5:4    RW    0
     aon_pad_cfg1     3:2    RW    0
     aon_pad_cfg0     1:0    RW    0
                                          0x004
    ---------------------------------------------------------------
     16'b0
     uc_r16_0        15:0    RW    0   
                                          0x008
    ---------------------------------------------------------------
     16'b0
     uc_r16_1        15:0    RW    0   
                                          0x00C
    ---------------------------------------------------------------
     16'b0
     uc_r32_0        31:0    RW    0   
                                          0x010
    ---------------------------------------------------------------
     16'b0
     uc_r32_1        31:0    RW    0   
                                          0x014    //share with voice_awake
     .cfg_clk1fs_mode                    (uc_r32_0[7:6]                    ),
     .cfg_voice_awake_enable             (uc_r32_1[31 ]       ),
     .cfg_remove_mean_vld                (uc_r32_1[30]        ),
     .cfg_dc_offset                      (uc_r32_0[5:0]       ),
     .cfg_noise_level_vld                (uc_r32_1[29]        ),
     .cfg_noise_level                    (uc_r32_1[28:23]     ),
     .cfg_gain_steps_vld                 (uc_r32_1[22 ]       ),
     .cfg_gain_steps                     (uc_r32_1[21:18 ]    ),
     .cfg_threshold_vld                  (uc_r32_1[17 ]       ),
     .cfg_threshold                      (uc_r32_1[16:5 ]     ),
     .cfg_voice_awake_filter_points_vld  (uc_r32_1[4 ]        ),
     .cfg_voice_awake_filter_points      (uc_r32_1[3:0 ]      ),

    ---------------------------------------------------------------

     dbg_0           31:0    R      
                                          0x040
    ---------------------------------------------------------------
     dbg_1           31:0    R      
                                          0x044
    ---------------------------------------------------------------
     dbg_2           31:0    R             // cobra2
     dbg_2[31:4]     31:4    R             // coney
       sen[ 3:0]      3:0    R             // coney
                                          0x048
    ---------------------------------------------------------------
     30'b0
     intr_wevent        1    RW    0               wr 1 to clear, write wb_addr
     intr_done          0    RW    0               wr 1 to clear
                                          0x05c
    ---------------------------------------------------------------

//RAM
addr[11:0]= 0x800-0xFFF  (depth=512 )  RW only 15-bit access



	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/


#ifndef __COBRA_UMCU_H__
#define __COBRA_UMCU_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_UMCU_EN

typedef struct cobra_umcu_type
{
		__attribute__ ((aligned (4)))   uint32_t  umcu_control;      //0x00
	
		__attribute__ ((aligned (4)))   uint32_t  aon_pad_control;
	
		__attribute__ ((aligned (4)))   uint32_t  umcu_r16_bit_data0;    //16 bit valid
	
		__attribute__ ((aligned (4)))   uint32_t  umcu_r16_bit_data1;    //16 bit valid
	
		__attribute__ ((aligned (4)))   uint32_t  umcu_r32_bit_data0;    //32 bit valid
	
		__attribute__ ((aligned (4)))   uint32_t  umcu_r32_bit_data1;    //32 bit valid
	
   //0x14	
	//	__attribute__ ((aligned (4)))   uint32_t  voice_wake_up_control;
	
	//0x18
		__attribute__ ((aligned (4)))   uint32_t  reserver0[10];
	
	//0x40
		__attribute__ ((aligned (4)))   uint32_t  umcu_dbug0;
	  __attribute__ ((aligned (4)))   uint32_t  umcu_dbug1;
	  __attribute__ ((aligned (4)))   uint32_t  umcu_dbug2;
	  __attribute__ ((aligned (4)))   uint32_t  umcu_dbug3;
		
	//0x50
	  __attribute__ ((aligned (4)))   uint32_t  reserver1;
		__attribute__ ((aligned (4)))   uint32_t  reserver2;
		__attribute__ ((aligned (4)))   uint32_t  reserver3;
	//0x5C	
		__attribute__ ((aligned (4)))   uint32_t  umcu_interrupt;


} COBRA_UMCU_TypeDef; 



typedef enum {
		UMCU_OWNER,
		CORTEX_OWNER	
}OWNER_INDEX;


typedef enum  {
	AON_PAD0,
	AON_PAD1,
	AON_PAD2,
	AON_PAD3,
	AON_PAD4,
	
}AON_PINMUX_INDEX;

typedef enum {
	AON_PIN_MODE_0,
	AON_PIN_MODE_1,
	AON_PIN_MODE_2,
	AON_PIN_MODE_3,
	
}AON_PINMODE;



/*
AON PAD 0 function
pad_dio	
IO<o_rmt0_tx_key [0],uc_gpout [0],uc_gpoe [0]>	
uc_gpin
*/
#define AON_PAD0_PAD_DIO					AON_PIN_MODE_0   //pad_dio
#define AON_PAD0_RMT0_TX_KEY0			AON_PIN_MODE_1
#define AON_PAD0_UC_GPIN					AON_PIN_MODE_2

/*
AON PAD 1 function 
pad_dio	
IO<o_rmt0_tx_key [1],uc_gpout [1],uc_gpoe [1]>	
uc_gpcc
*/
#define AON_PAD1_PAD_DIO					AON_PIN_MODE_0
#define AON_PAD1_RMT0_TX_KEY1			AON_PIN_MODE_1
#define AON_PAD1_UC_GPCC					AON_PIN_MODE_2

/*
AON PAD 2 function
pad_dio	
IO<o_rmt0_tx_key [2],uc_gpout [2],uc_gpoe [2]>	
o_va_pdm_ld
*/
#define AON_PAD2_PAD_DIO							AON_PIN_MODE_0
#define AON_PAD2_RMT0_TX_KEY2					AON_PIN_MODE_1
#define AON_PAD2_O_VA_PDM_LD					AON_PIN_MODE_2

/*
AON PAD 3 function
pad_dio	
IO<o_rmt0_tx_key [3],uc_gpout [3],uc_gpoe [3]>	
o_va_pdm_rd	
*/
#define AON_PAD3_PAD_DIO							AON_PIN_MODE_0
#define AON_PAD3_RMT0_TX_KEY3					AON_PIN_MODE_1
#define AON_PAD3_O_VA_PDM_RD					AON_PIN_MODE_2

/*
AON PAD 4 function
pad_dio	
IO<o_rmt0_tx_key [4],uc_gpout [4],uc_gpoe [4]>	
i_va_pdm_clk	
rmt0_txd
*/
#define AON_PAD4_PAD_DIO							AON_PIN_MODE_0
#define AON_PAD4_RMT0_TX_KEY4					AON_PIN_MODE_1
#define AON_PAD4_O_VA_PDM_CLK					AON_PIN_MODE_2
#define AON_PAD4_RMT0_TXD							AON_PIN_MODE_3




#define  COBRA_UMCU_BASE							(0x40083000UL)
#define  COBRA_UMCU_PROGRAM_ADDRESS	  (COBRA_UMCU_BASE + 0x800)

void cobra_umcu_init(void);

void cobra_mcu_map_aon_pinmodeset(AON_PINMUX_INDEX  pin,  AON_PINMODE mode);

void app_cobra_voice_wakeup_init(void);

void cobra_m3_voice_wakeup_interrupt(void);

void app_cobra_key_scan_init(void);

void cobra_umcu_clear_intr_wevent(void);

void cobra_umcu_clear_intr_done(void);

void cobra_m3_voice_wakeup_interrupt(void);

void app_cobra_umcu_wakeup_m0_init(void);

void app_cobra_umcu_wakeup_m0_run(void);

void cobra_umcu_wakeup_m0_interrupt(void);

void cobra_umcu_copy_program(uint16_t * p_program, unsigned int size);

void app_cobra_umcu_run(void);

void app_cobra_umcu_clk_enable(unsigned int en);

void app_cobra4_umcu_enable_sram(void);

void cobra_umcu_enable_umcu(unsigned int en);
#endif   //COBRA_UMCU_EN


#endif

