/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_pcrm.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   accord from cobra2 register document
					APB BUS3
					PCRM module
					Base address is 0x400E0000
					offset address is 0x2000

	Changed Log	:

		Liujian  2019/3/8			- Creation


*****************************************************************************/

#ifndef __COBRA_PCRM_H__
#define __COBRA_PCRM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_PCRM_EN

typedef struct cobra_pcrm_t
{
	__attribute__((aligned(4))) volatile unsigned int pwr_state_and_rst_ctrl;

	__attribute__((aligned(4))) volatile unsigned int shutdown_req;

	__attribute__((aligned(4))) volatile unsigned int standby_req;
	__attribute__((aligned(4))) volatile unsigned int sys_soft_reset_req;

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver0; // primary mcu id=0

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver1; //0x14
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver2;
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver3; // primary mcu id=1;
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver4; //0x20
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver5;

	//VIM domain
	__attribute__((aligned(4))) volatile unsigned int vim_power_ctrl; //0x28

	//p1 p2 domain
	__attribute__((aligned(4))) volatile unsigned int p1_p2_pwr_req1; //0x2c

	__attribute__((aligned(4))) volatile unsigned int p1_pwr_req2; //0x30
	__attribute__((aligned(4))) volatile unsigned int p2_pwr_req2;

	//STCALIB
	__attribute__((aligned(4))) volatile unsigned int stclk_calib; //0x38

	//sram ret
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver6;

	//m3 debug m0 dlck
	__attribute__((aligned(4))) volatile unsigned int m3_dbgen_m0_dclk_en; //0x40

	//master cpu control
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver7; //0x44

	__attribute__((aligned(4))) volatile unsigned int m3_srst_req;

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver8; //0x4C

	//0x50
	__attribute__((aligned(4))) volatile unsigned int m3_remap;			//vim tcm remap control
																		//---------------------------------------------
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver9; //0x54

	__attribute__((aligned(4))) volatile unsigned int pudly_p1_p2_window_config; //0x58

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver11;
	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver12;

	//	0x64
	__attribute__((aligned(4))) volatile unsigned int enclica_cact_mask;

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver13; // 0x68

	//0x6C
	__attribute__((aligned(4))) volatile unsigned int hf_clk_div_pmc_clk_cfg;

	__attribute__((aligned(4))) volatile unsigned int cobra4_reserver14; //0x70

	__attribute__((aligned(4))) volatile unsigned int sbus_apb1_cfg_apb2_cfg;

	//0x78
	__attribute__((aligned(4))) volatile unsigned int sbus_apb3_cfg_sbus_rfm0_cfg;

	__attribute__((aligned(4))) volatile unsigned int module_clock1_active_ctrl_en;

	__attribute__((aligned(4))) volatile unsigned int module_clock1_sleep_ctrl_en;

	__attribute__((aligned(4))) volatile unsigned int module_clock1_deepsleep_ctrl_en; //0x80

	//0x88
	__attribute__((aligned(4))) volatile unsigned int module_clock2_active_ctrl_en;

	__attribute__((aligned(4))) volatile unsigned int module_clock2_sleep_ctrl_en;

	__attribute__((aligned(4))) volatile unsigned int module_clock2_deepsleep_ctrl_en;

	//0x94
	__attribute__((aligned(4))) volatile unsigned int module_srst_req1;
	__attribute__((aligned(4))) volatile unsigned int module_srst_req2;

	__attribute__((aligned(4))) volatile unsigned int apll_cfg1;	   //0x9C
	__attribute__((aligned(4))) volatile unsigned int PLL_PARAM_CTRL0; //0xA0
	__attribute__((aligned(4))) volatile unsigned int PLL_PARAM_CTRL1; //0xA4
	__attribute__((aligned(4))) volatile unsigned int PLL_PARAM_CTRL2; //0xA8
	__attribute__((aligned(4))) volatile unsigned int PLL_FRACT_NUM;   //0xAC

	__attribute__((aligned(4))) volatile unsigned int reserver9;

	//0xB4
	__attribute__((aligned(4))) volatile unsigned int clk_tx_mdiv_x1_y1;
	__attribute__((aligned(4))) volatile unsigned int clk_rx_mdiv_x1_y1;
	__attribute__((aligned(4))) volatile unsigned int clk_tx_sdiv1;

	//0xC0
	__attribute__((aligned(4))) volatile unsigned int tx_rx_mode1;

	__attribute__((aligned(4))) volatile unsigned int reserver10;
	__attribute__((aligned(4))) volatile unsigned int reserver11;

	//0xCC
	__attribute__((aligned(4))) volatile unsigned int clk_tx_mdiv_x0_y0;
	__attribute__((aligned(4))) volatile unsigned int clk_rx_mdiv_x0_y0;
	__attribute__((aligned(4))) volatile unsigned int clk_tx_sdiv0;

	//0xD8
	__attribute__((aligned(4))) volatile unsigned int tx_rx_mode0;

	__attribute__((aligned(4))) volatile unsigned int reserver12;
	__attribute__((aligned(4))) volatile unsigned int reserver13;

	//0xE4
	__attribute__((aligned(4))) volatile unsigned int sram_clk_en;

	__attribute__((aligned(4))) volatile unsigned int spwm_clk_div_dualtimers_clk_div;

	__attribute__((aligned(4))) volatile unsigned int irda_high_low_cnt_div;

	//0xF0

	__attribute__((aligned(4))) volatile unsigned int i2s0_i2s1_clk_en;

	__attribute__((aligned(4))) volatile unsigned int reserver15;

	__attribute__((aligned(4))) volatile unsigned int m3_lockup_reset;

	__attribute__((aligned(4))) volatile unsigned int reserverse_4b;

	//0x100
	__attribute__((aligned(4))) volatile unsigned int ana_cfg0; //0x100

	__attribute__((aligned(4))) volatile unsigned int ana_cfg1; //0x104

	__attribute__((aligned(4))) volatile unsigned int ana_cfg2; //0x108

	__attribute__((aligned(4))) volatile unsigned int ana_cfg3; //0x10c

	//0x110
	__attribute__((aligned(4))) volatile unsigned int ana_cfg4; //0x110
	__attribute__((aligned(4))) volatile unsigned int ana_cfg5; //0x114

	__attribute__((aligned(4))) unsigned int reserver19[8];

	__attribute__((aligned(4))) volatile unsigned int apll_status; //0x138

	__attribute__((aligned(4))) volatile unsigned int reserver20;
	//0x140
	__attribute__((aligned(4))) volatile unsigned int chip_id0; //41343030

	__attribute__((aligned(4))) volatile unsigned int chip_id1; //434F4252

} COBRA_PCRM_T;

typedef enum
{
	M3_IS_PRIMARY_MCU,
	M0_IS_PRIMARY_MCU
} PRIMARY_MCU_ID;

typedef enum
{
	CPU_HARDWARE_MODE,
	CPU_SOFTWARE_MODE
} CPU_CONTROL_MODE;

//typedef enum {
//		VIM_POWERD_WHEN_M3_POWERD,
//		VIM_POWERD_WHEN_BUS_POWERD
//}VIM_POWERD_MODE;

typedef enum
{
	VIM_CLK_FREE_RUM,
	VIM_CLK_GATED_BY_M4_DEEP_SLEEP

} VIM_CLOCK_MODE;

typedef enum
{
	BUS_CLOCK_FREE_RUN,
	BUS_CLK_GATED_BY_M4_DEEP_SLEEP

} BUS_CLOCK_MODE;

typedef enum
{
	M4_FCLK_FREE_RUN,
	M4_FCLK_GATED_BY_M4_DEEP_SLEEP

} M4_FLCK_CLOCK_MODE;

//typedef enum {
//		VIM_HARDWARE_CTRL_MODE,
//		VIM_SOFTWARE_CTRL_MODE
//}VIM_CONTROL_MODE;

typedef enum
{
	MODE_MCLK_COME_FROM_PCLK,
	MODE_MCLK_COM_FROM_APLLL,
	MODE_MCLK_COM_FROM_RX_MCLK,
	MODE_MCLK_COME_FROM_EXTERNAL_MCLK,
	MODE_SHARE_RX_SCLK_RX_WS,
	MODE_SLAVE
} I2S_TX_WORK_MODE;

typedef enum
{
	SCLK_COME_FROM_TX_SCLK,
	SCLK_COME_FROM_RX_SCLK
} I2S_SCLK_SOURCE;

typedef enum
{
	TX_MCLK_COME_FROM_RX_MCLK,
	TX_MCLK_COME_FROM_TX_MCLK
} I2S_TX_MCLK_SOURCE;

typedef enum
{
	TX_MCLK_OUTPUT_48MHZ,
	TX_MCLK_OUTPUT_APLL
} I2S_TX_MCLK_OUTPUT_SOURCE;

typedef enum
{
	PHASE_0,
	PHASE_180
} I2S_PHASE;


typedef enum
{
	MASTER_MODE_PCLK_NO_MCLK_OUTPUT,
	MASTER_MODE_PCLK_WITH_MCLK_OUTPUT,
	MASTER_MODE_SHARING_RX_MCLK,
	TYPICAL_SLAVE_MODE,
	TYPICAL_SLAVE_MODE_WITH_MCLK,
	SHARE_RXSCK_RX_WS,
	MASTER_MODE_FORM_APLL,
	MASTER_MODE_EXTERNAL_MCLK
} I2S_TRANSMITTER_MODE;

typedef enum
{
	RECEIVER_MODE_PCLK_NO_MCLK_OUTPUT,
	RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT,
	RECEIVER_MODE_SHARING_RX_MCLK,
	RECEIVER_TYPICAL_SLAVE_MODE,
	RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK,
	RECEIVER_SHARE_RXSCK_RX_WS,
	RECEIVER_MASTER_MODE_FORM_APLL,
	RECEIVER_MASTER_MODE_EXTERNAL_MCLK
} I2S_RECEIVER_MODE;

#define COBRA_PCRM_BASE				(0x400e2000UL)


void cobra_pcrm_init(void);


unsigned int cobra_pcrm_rfm0_pwr_ready(void);

//Active mode  clock gate
void cobra_pcrm_m3_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_rfm0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_bluetooth_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_system_bus_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_sys_bus_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_spiflash_ctrl_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_udma_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_sha_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_aon_inf_bridge_hclk_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_cache_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_eflash_ctrl_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_pinmux_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_bt_cpu_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart10_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart11_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_timer10_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_apll_dclk_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart1_active_mode_clock_enable(unsigned int en);

void cobra_pcrm_uart0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_mpwm_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_spwm_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_spi0_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_can0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2c0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2c1_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_qdec_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_aes_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_trng_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_timer0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_timer1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_dualtimer_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_spi1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_pdm0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_pdm1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2s0_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2s1_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_ecc_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_apll_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_trng_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_irda_active_mode_clock_enable(unsigned int en);
void cobra_pcrm_irda_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_irda_deep_sleep_mode_clock_enable(unsigned int en);

//void cobra_pcrm_gpio0_active_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio1_active_mode_clock_enable(unsigned int en);

void cobra_pcrm_p2_power_ctrl(unsigned int En);
void cobra_pcrm_p1_power_ctrl(unsigned int En);

//void cobra_pcrm_vim_power_up_hardware_ctrl_mode(VIM_POWERD_MODE mode);
void cobra_pcrm_vim_power_up_software_ctrl_en(unsigned int en);
//void cobra_pcrm_vim_control_mode(VIM_CONTROL_MODE mode);

void  cobra_pcrm_m0_power_soft_request_en(unsigned int en);
void  cobra_pcrm_m3_reset_m0(unsigned int en);

void cobra_pcrm_m0_warm_request_en(unsigned int en);
void cobra_pcrm_m3_warm_request_en(unsigned int en);
void cobra_pcrm_whole_chip_warm_request_en(unsigned int en);



//void cobra_pcrm_icytrx_isolate_en(unsigned int en);
#if COBRA_MCU_IS_VER1
void	cobra_pcrm_ldoadc_isolate_en(unsigned int en);
void cobra_pcrm_ldopll_isolate_en(unsigned int en);
#endif
//reset function
void cobra_pcrm_udma_soft_reset(void);

void cobra_pcrm_shift_pattern_length(unsigned int length);


void cobra_pcrm_system_clk_divider(unsigned int divider);

//void cobra_pcrm_rfm0_apb_cfg_shift_pattern(unsigned int pattern);   //M0 APB0 
//void cobra_pcrm_apb0_clk_shift_pattern(unsigned int pattern);
void cobra_pcrm_apb1_clk_shift_pattern(unsigned int pattern);
void cobra_pcrm_apb2_clk_shift_pattern(unsigned int pattern);
void cobra_pcrm_apb3_clk_shift_pattern(unsigned int pattern);

void cobra_pcrm_m0_system_clk_shift_pattern(unsigned int pattern);   //M0 AHB frequency
void cobra_pcrm_m3_system_clk_shift_pattern(unsigned int pattern);		//M3 AHB frequency


//sleep mode
void cobra_pcrm_timer0_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_timer1_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_dualtimer_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_rfm0_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_bluetooth_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_bt_cpu_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart10_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart11_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart0_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart1_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2s0_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio0_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio1_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_pdm0_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_udma_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_trng_sleep_mode_clock_enable(unsigned int en);
//deep sleep mode
//void cobra_pcrm_rfm0_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_bluetooth_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_system_bus_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_sys_bus_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_spiflash_ctrl_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_udma_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_sha_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_aon_inf_bridge_hclk_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_cache_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_eflash_ctrl_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_pinmux_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_bt_cpu_deep_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart10_deep_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_uart11_deep_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_timer10_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_apll_dclk_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart0_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_uart1_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_m3_deep_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio0_deep_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_gpio1_deep_sleep_mode_clock_enable(unsigned int en);

void cobra_pcrm_pmu_clk_divider(unsigned int divider);

void cobra_pcrm_spi_flash_control_clk_mask_en(unsigned int en);
/*
"chip shutdown mode request
0:  disable
1:  enable"

*/
void cobra_pcrm_shutdown_mode_en(unsigned int en);
/*
"chip standby mode request
0:  disable
1:  enable"

*/
void cobra_pcrm_standby_mode_en(unsigned int en);
	

//void  cobra_pcrm_m3_power_enable_ctrl(unsigned int en);
//void  cobra_pcrm_rfm0_power_enable_ctrl(unsigned int en);

void cobra_pcrm_apll_dclk_sleep_mode_clock_enable(unsigned int en);
//void cobra_pcrm_timer10_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_pinmux_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_cache_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_m3_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_sys_bus_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_system_bus_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_spiflash_ctrl_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_vim_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_eflash_ctrl_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_i2c0_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_mpwm_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_spwm_sleep_mode_clock_enable(unsigned int en);


//unsigned int cobra_pcrm_whether_m3_system_soft_reset(void);
//unsigned int cobra_pcrm_whether_m3_system_lockup_reset(void);
//unsigned int cobra_pcrm_whether_m3_system_watchdog_reset(void);
//void cobra_pcrm_icytrx_soft_reset(void);
//void cobra_pcrm_vim_sram1_retention_en(unsigned int en);
//void cobra_pcrm_vim_sram2_retention_en(unsigned int en);
//void cobra_pcrm_vim_sram3_retention_en(unsigned int en);
//void cobra_pcrm_vim_sram4_retention_en(unsigned int en);

void cobra_pcrm_exchange_memory_retention_en(unsigned int en);  //exchange memory retention

//PLL
//d2a_pll_pfd_en
void cobra_pcrm_d2a_pll_pfd_en(unsigned int en);
void cobra_pcrm_d2a_pll_cp_en(unsigned int en);
void cobra_pcrm_d2a_pll_cp_iupcom(unsigned int en);
void cobra_pcrm_d2a_pll_cp_sw(unsigned int en);
void cobra_pcrm_d2a_pll_en(unsigned int en);
void cobra_pcrm_d2a_pll_lct_en(unsigned int en);
void cobra_pcrm_d2a_pll_lpf_sms(unsigned int en);
void cobra_pcrm_d2a_pll_pfd_en(unsigned int en);
void cobra_pcrm_d2a_pll_pfd_ms(unsigned int en);
void cobra_pcrm_d2a_pll_retimer_en(unsigned int en);
void cobra_pcrm_d2a_pll_test_en(unsigned int en);
void cobra_pcrm_d2a_pll_vco_en(unsigned int en);
void cobra_pcrm_d2a_pll_lpf_c1(unsigned int value);
void cobra_pcrm_d2a_pll_vco_vbias(unsigned int value);
void cobra_pcrm_d2a_pll_pfd_delay(unsigned int value);
void cobra_pcrm_d2a_pll_lpf_c2(unsigned int value);
void cobra_pcrm_d2a_pll_cp_idac(unsigned int value);
//ģ�ⲿ��
//ana_cfg0

//d2a_vdac_en
void cobra_pcrm_d2a_vdac_en(unsigned int en);
void cobra_pcrm_d2a_cpa_en(unsigned int en);
void cobra_pcrm_d2a_cpb_en(unsigned int en);
void cobra_pcrm_d2a_vdd3_adc_en(unsigned int en);
void cobra_pcrm_d2a_ldodig_pd0_high_curr_en(unsigned int en);
void cobra_pcrm_d2a_ldodig_pd0_vreg_trim(unsigned int value);
void cobra_pcrm_d2a_ldodig_pd1_high_curr_en(unsigned int en);
void cobra_pcrm_d2a_ldodig_pd1_vreg_trim(unsigned int value);
void cobra_pcrm_d2a_efbrwot_en(unsigned int en);
void cobra_pcrm_d2a_bgln_t_trim(unsigned int value);
void cobra_pcrm_d2a_bgln_vref_trim(unsigned int value);
void cobra_pcrm_d2a_ldoanaforpll_high_curr_en(unsigned int en);
void cobra_pcrm_d2a_ldoanaforpll_trim(unsigned int value);

unsigned int cobra_pcrm_pll_cnt_save(void);
unsigned int cobra_pcrm_pll_trim_end(void);
unsigned int cobra_pcrm_pll_trim_suc(void);
unsigned int cobra_pcrm_trim_out_o(void);
unsigned int cobra_pcrm_try_cnt(void);

//
void cobra_pcrm_set_pll_cnt_judge(unsigned int judge);
void cobra_pcrm_apll_control_status_clr(unsigned int en);
void cobra_pcrm_set_try_num(unsigned char number);
void cobra_pcrm_trim_start(unsigned int start);
void cobra_pcrm_ctrl_cnt_cfg(unsigned int cfg);
void cobra_pcrm_wait_trim_cycle(unsigned int cycle);
void cobra_pcrm_trim_gain(unsigned int gain);
void cobra_pcrm_trim_value(unsigned int value);
void cobra_pcrm_trim_cfg_update(unsigned int update);
void cobra_pcrm_trim_en_ctrl(unsigned int en);
void cobra_pcrm_trim_en_pll_cnt(unsigned int en);
void cobra_pcrm_step_value(unsigned int step_value);
void cobra_pcrm_lock_detect_en(unsigned int en);
void cobra_pcrm_lock_cfg(unsigned int lock_cfg);
void cobra_pcrm_pll_freq_off(unsigned int pll_freq_off);
void cobra_pcrm_fract_num(unsigned int fract_number);
void cobra_pcrm_p_num(unsigned int p_num);
void cobra_pcrm_work_mode(unsigned int work_mode);
void cobra_pcrm_update_cfg(unsigned int en);

void cobra_pcrm_apll_sleep_mode_clock_enable(unsigned int en);

void cobra_pcrm_write_apll_cfg1(unsigned int value);
void cobra_pcrm_write_apll_cfg2(unsigned int value);
void cobra_pcrm_write_apll_cfg3(unsigned int value);
void cobra_pcrm_write_apll_cfg4(unsigned int value);


void cobra4_pll_debug_information(void);

//i2s informatin
void cobra_pcrm_i2s0_transmitter_mode(I2S_TRANSMITTER_MODE work_mode);
void cobra_pcrm_i2s0_tx_work_mode(I2S_TX_WORK_MODE source);
void cobra_pcrm_i2s0_tx_mode_mclk_div(unsigned int divX, unsigned int divY);
void cobra_pcrm_i2s0_rx_mode_mclk_div(unsigned int divX, unsigned int divY);
void cobra_pcrm_i2s0_tx_mode_sclk_div(unsigned int div);
void cobra_pcrm_i2s0_rx_mode_sclk_div(unsigned int div);

void cobra_pcrm_i2s0_receiver_mode(I2S_RECEIVER_MODE work_mode);
void cobra_pcrm_i2s0_clk_en(unsigned int en);

void cobra_pcrm_i2s1_transmitter_mode(I2S_TRANSMITTER_MODE work_mode);
void cobra_pcrm_i2s1_receiver_mode(I2S_RECEIVER_MODE work_mode);
void cobra_pcrm_i2s1_tx_mode_sclk_div(unsigned int div);
void cobra_pcrm_i2s1_tx_mode_mclk_div(unsigned int divX, unsigned int divY);
void cobra_pcrm_i2s1_rx_mode_sclk_div(unsigned int div);
/*

rx_mclk = i2s_clk*(divX/divY)

*/
void cobra_pcrm_i2s1_rx_mode_mclk_div(unsigned int divX, unsigned int divY);
void cobra_pcrm_i2s1_clk_en(unsigned int en);

void cobra_pcrm_i2s0_tx_sclk_phase(I2S_PHASE phase);
void cobra_pcrm_i2s0_rx_sclk_phase(I2S_PHASE phase);

void cobra_pcrm_i2s0_soft_reset(void);
void cobra_pcrm_i2s1_soft_reset(void);

void cobra_pcrm_pdm0_soft_reset(void);
void cobra_pcrm_pdm1_soft_reset(void);


void cobra_pcrm_spi_flash_soft_reset(void);

void cobra_pcrm_uart0_soft_reset(void);

void cobra_pcrm_mpwm_deep_sleep_mode_clock_enable(unsigned int en);
void cobra_pcrm_spwm_deep_sleep_mode_clock_enable(unsigned int en);
#endif   //COBRA_PCRM_EN



#endif

