/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_umcu.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/28

	Description :   


	Changed Log	:

		Liujian  2019/3/28			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_umcu.h"
#include "cobra_irda.h"
#include "cobra_aon_pmu.h"
#include "cobra4_aon_top.h"
#if COBRA_UMCU_EN

#if (APP_COBRA_UMCU_EN) && (APP_COBRA_VOICE_WAKEUP_EN)
#error "Key pad scan and voice wakeup can not work together"
#endif

#if (APP_COBRA_UMCU_EN) && (APP_COBRA_UMCU_WAKEUP_M0_EN)
#error "IRDA and UMCU wakeup M0 can nor work together"
#endif

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_UMCU_TypeDef * cobra_umcu		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_UMCU_TypeDef * cobra_umcu; 
#endif

void cobra_umcu_init(void)
{
		cobra_umcu = (COBRA_UMCU_TypeDef * )COBRA_UMCU_BASE;
}

unsigned int cobra_umcu_uc_done(void)
{
		if(cobra_umcu->umcu_control & (1 << 4))
				return 1;
		else	
				return 0;
}

void cobra_umcu_who_owner_the_cpu(OWNER_INDEX owner)
{
		if(owner == UMCU_OWNER)
		{
				cobra_umcu->umcu_control |= (1 << 2);
		}
		else if(owner == CORTEX_OWNER)
		{
				cobra_umcu->umcu_control &= ~(1 << 2);
		}
}

void cobra_umcu_enable_umcu(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_control |= (1 << 1);
		}
		else
		{
				cobra_umcu->umcu_control &= ~(1 << 1);
		}
}

void app_cobra_umcu_clk_enable(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_control |= (1 << 7);
		}
		else
		{
				cobra_umcu->umcu_control &= ~(1 << 7);
		}
	
}

void app_cobra4_umcu_enable_sram(void)
{
	
		cobra_umcu->umcu_control |= 0x01;
}

void app_cobra_umcu_run(void)
{
		cobra_umcu->umcu_control |= 0x47; //
}

void cobra_umcu_reset_umcu(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_control |= (1 << 0);
		}
		else
		{
				cobra_umcu->umcu_control &= ~(1 << 0);
		}
}

/*
 wr 1 to clear, write wb_addr
*/
void cobra_umcu_clear_intr_wevent(void)
{
		cobra_umcu->umcu_interrupt = 0x01;
}

/*
 wr 1 to clear
*/
void cobra_umcu_clear_intr_done(void)
{
		cobra_umcu->umcu_interrupt = 0x02;
}

/*
PAD 31-27 are AON pad 4-0
Addr=0x40083004 {22'0,aon_pad_cfg4[1:0],aon_pad_cfg3[1:0],aon_pad_cfg2[1:0],aon_pad_cfg1[1:0],aon_pad_cfg0[1:0]}
// aon_pad_cfg: 
// 0:pad_dio
// 1:rmt
// 2:va
// 3:gp_adc
*/

static void aon_pin0_function(AON_PINMODE  mode)
{
		cobra_umcu->aon_pad_control &= ~(0x03);
		cobra_umcu->aon_pad_control |= mode;	
}

static void aon_pin1_function(AON_PINMODE  mode)
{
		cobra_umcu->aon_pad_control &= ~(0x03 << 2);
		cobra_umcu->aon_pad_control |= (mode << 2);	
}

static void aon_pin2_function(AON_PINMODE  mode)
{
		cobra_umcu->aon_pad_control &= ~(0x03 << 4);
		cobra_umcu->aon_pad_control |= (mode << 4);	
}

static void aon_pin3_function(AON_PINMODE  mode)
{
		cobra_umcu->aon_pad_control &= ~(0x03 << 6);
		cobra_umcu->aon_pad_control |= (mode << 6);	
}

static void aon_pin4_function(AON_PINMODE  mode)
{
		cobra_umcu->aon_pad_control &= ~(0x03 << 8);
		cobra_umcu->aon_pad_control |= (mode << 8);	
}

void test__aon_20190625(void)
{
	 printf("an_on 0x%x \n", cobra_umcu->aon_pad_control);
	
}

void cobra_mcu_map_aon_pinmodeset(AON_PINMUX_INDEX  pin,  AON_PINMODE mode)
{
		switch(pin)
		{
				case AON_PAD0:
						aon_pin0_function(mode);
				break;
				case AON_PAD1:
						aon_pin1_function(mode);
				break;
				case AON_PAD2:
						aon_pin2_function(mode);
				break;
				case AON_PAD3:
						aon_pin3_function(mode);
				break;
				case AON_PAD4:
						aon_pin4_function(mode);
				break;
				default:
				break;	
		}	
}

unsigned short cobra_umcu_read_umcu_r16_bit_data0(void)
{
		return cobra_umcu->umcu_r16_bit_data0 & 0xffff;
}

unsigned short cobra_umcu_read_umcu_r16_bit_data1(void)
{
		return cobra_umcu->umcu_r16_bit_data1 & 0xffff;
}

unsigned short cobra_umcu_read_umcu_r32_bit_data0(void)
{
		return cobra_umcu->umcu_r32_bit_data0;
}

unsigned short cobra_umcu_read_umcu_r32_bit_data1(void)
{
		return cobra_umcu->umcu_r32_bit_data1;
}
//----------
void cobra_umcu_write_umcu_r16_bit_data0(unsigned short data)
{
		cobra_umcu->umcu_r16_bit_data0 = data;
}

void cobra_umcu_write_umcu_r16_bit_data1(unsigned short data)
{
		cobra_umcu->umcu_r16_bit_data1 = data;
}

void cobra_umcu_write_umcu_r32_bit_data0(unsigned int data)
{
		cobra_umcu->umcu_r32_bit_data0 = data;
}

void cobra_umcu_write_umcu_r32_bit_data1(unsigned int data)
{
		cobra_umcu->umcu_r32_bit_data1 = data;
}

//voice wakeup function

// which_pin_to_use, //0 : left,  1:right; decided by hardware.
//ch = 0 	left channel
//ch = 1 	right channel
void cobra_umcu_va_chn_sel(unsigned int ch)
{
		if(ch)
		{
				cobra_umcu->umcu_control |= (1 << 5);
		}
		else
		{
				cobra_umcu->umcu_control &= ~(1 << 5);
		}
}

//filter ........
void cobra_umcu_cfg_voice_awake_filter_points(unsigned int value)
{
			cobra_umcu->umcu_r32_bit_data1 &= ~0x0f;
			cobra_umcu->umcu_r32_bit_data1 |= value&0x0f;	
}

void cobra_umcu_cfg_voice_awake_filter_points_vld_en(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= (1 << 4) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~(1 << 4) ;
		}
}
//threshold
void cobra_umcu_cfg_threshold (unsigned int value)
{
			cobra_umcu->umcu_r32_bit_data1 &= ~(0xfff << 5);
			cobra_umcu->umcu_r32_bit_data1 |= (value&0xfff << 5);	
}

void cobra_umcu_cfg_threshold_vld_en(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= (1 << 17) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~(1 << 17) ;
		}
}

//gain steps
void cobra_umcu_cfg_gain_steps(unsigned int value)
{
			cobra_umcu->umcu_r32_bit_data1 &= ~(0x0f << 18);
			cobra_umcu->umcu_r32_bit_data1 |= (value&0x0f << 18);	
}

void cobra_umcu_cfg_gain_steps_vld_en(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= (1 << 22) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~(1 << 22) ;
		}
}

//noise level
void cobra_umcu_cfg_noise_level(unsigned int value)
{
			cobra_umcu->umcu_r32_bit_data1 &= ~(0x3f << 23);
			cobra_umcu->umcu_r32_bit_data1 |= (value&0x3f << 23);	
}

void cobra_umcu_cfg_noise_level_vld_en(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= (1 << 29) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~(1 << 29) ;
		}
}

//DC offset
void cobra_umcu_cfg_dc_offset(unsigned int value)
{
			cobra_umcu->umcu_r32_bit_data0 &= ~(0x3f);
			cobra_umcu->umcu_r32_bit_data0 |= (value&0x3f);	
}


//cfg_remove_mean_vld    
void cobra_umcu_cfg_remove_mean_vld_en(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= (1 << 30) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~(1 << 30) ;
		}
}

//cfg_voice_awake_enable    
void cobra_umcu_cfg_voice_awake_enable(unsigned int en)
{
		if(en)
		{
				cobra_umcu->umcu_r32_bit_data1 |= ((unsigned int)1 << 31) ;
		}
		else
		{
				cobra_umcu->umcu_r32_bit_data1 &= ~((unsigned int)1 << 31) ;
		}
}


void cobra_umcu_cfg_clk1fs_mode(unsigned int mode)
{
			cobra_umcu->umcu_r32_bit_data0 &= ~(0x3 << 6);
			cobra_umcu->umcu_r32_bit_data0 |= (mode&0x3 << 6);	
}

//-------
void cobra_umcu_copy_program(uint16_t * p_program, unsigned int size)
{
		uint16_t i;
		volatile uint32_t * p_temp;
		p_temp = (volatile uint32_t *)(COBRA_UMCU_PROGRAM_ADDRESS);
	  for(i = 0; i < size; i++)
		{
				*p_temp++ = p_program[i];
		}
}

unsigned short test_power_down[] = {
0x6c02,
0x4000,
0x004c,
0x8003,
0x008c,
0x1c4c,
0x8000,
0x008c,
0x224c,
0x8000,
0x008c,
0x240a,
0xac14,
0xc000,
0x81fc,
0x4411,
0x400f,
0x4025,
0x004c,
0x801a,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x801e,
0x40e8,
0x004c,
0xb000,
0x008c,
0x080c,
0x4420,
0x401e,
0x004c,
0x140c,
0x4424,
0x4022,
0x401a,
0x0051,
0x008a,
0x782c,
0x00ca,
0x7834,
0x00ca,
0x782c,
0x004c,
0x8060,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x80be,
0x40e8,
0x004c,
0x803c,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x80be,
0x40e8,
0x004c,
0xc002,
0xb040,
0x008c,
0x080c,
0x4443,
0x4041,
0x0047,
0x008c,
0xcc00,
0x010c,
0xd000,
0x85a0,
0x008c,
0x140c,
0x444d,
0x404b,
0x004c,
0xc000,
0x8008,
0x008c,
0x060c,
0x0043,
0x008a,
0x00ca,
0x7858,
0x060c,
0x4052,
0x004c,
0x8060,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x80e7,
0x40e8,
0x004c,
0xc002,
0xb040,
0x008c,
0x080c,
0x4467,
0x4065,
0x0047,
0x008c,
0xcc00,
0x010c,
0xd000,
0xb4e0,
0x008c,
0x140c,
0x4471,
0x406f,
0x004c,
0xc000,
0x8008,
0x008c,
0x060c,
0x0043,
0x008a,
0x00ca,
0x787c,
0x060c,
0x4076,
0x004c,
0x8084,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x810b,
0x40e8,
0x004c,
0xc002,
0xa000,
0x008c,
0x080c,
0x448b,
0x4089,
0x004e,
0x008a,
0x00ca,
0x00ca,
0x7894,
0x004c,
0xc480,
0x8301,
0x4097,
0x004c,
0xc480,
0x8300,
0x008c,
0x140c,
0x449b,
0x4099,
0x004c,
0x080c,
0x004c,
0x8200,
0x008c,
0x060c,
0x004c,
0x120c,
0x004c,
0xa8e0,
0x8000,
0x000c,
0x44a9,
0x40a7,
0x008c,
0x78b1,
0x0043,
0x008a,
0x00ca,
0x78b1,
0x060c,
0x40a3,
0x0046,
0x008c,
0x120c,
0x004c,
0x80bc,
0x008c,
0x1e0c,
0x004c,
0xc000,
0x8145,
0x40e8,
0xac0c,
0xc000,
0x8002,
0x44c1,
0x40bf,
0xa504,
0xc000,
0x8122,
0x44c6,
0x40c4,
0x004c,
0xc000,
0x800f,
0x008a,
0x00ca,
0x4ccd,
0x40ca,
0x004c,
0xb004,
0x008c,
0x080c,
0x44d3,
0x40d1,
0x004c,
0xc000,
0x8040,
0x008c,
0x140c,
0x44da,
0x40d8,
0x40db,
0xa504,
0xc000,
0x813f,
0x44e0,
0x40de,
0xac0c,
0xc000,
0x8000,
0x44e5,
0x40e3,
0x4012,
0x7adb,
0x40e6,
0x008c,
0x160c,
0x004c,
0xb000,
0x008c,
0x080c,
0x44f0,
0x40ee,
0x004c,
0x80c6,
0x008c,
0x140c,
0x004c,
0x004c,
0x004c,

};

#include "ll.h"
#include "cobra_m4_gpio.h"
#include "cobra4_aon_wk.h"

void cobra_umuc_test_power_down(void)
{
	   cobra4_aon_pd_top_protection_en(1);
		 cobra4_aon_pd_top_ucpu_pclk_en(1);
		 cobra4_aon_pd_top_umcu_srst_req_en(1);
		 cobra4_aon_pd_top_protection_en(0);
	   
	   app_cobra_umcu_clk_enable(1);
		 app_cobra4_umcu_enable_sram();
	
	   cobra_umcu_clear_intr_wevent();    //清中断
		 cobra_umcu_clear_intr_done();

		
		 cobra_umcu_copy_program(test_power_down, sizeof(test_power_down)/sizeof(uint16_t));
	
			cobra4_aon_pd_top_protection_en(1);
			cobra4_aon_pd_top_hf_clock_selection(HF_RC_OSC24M);
			cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_RC_OSC24M);
			cobra4_aon_pd_top_icytrx_reset_en(1);    //复位 icy_tr
	
			cobra_m4_power_off_enable(0);       //天宁的寄存器里面的   
			cobra4_aon_wk_protection_en(1);
		
			GLOBAL_INT_STOP();
 
	
		  app_cobra_umcu_run();
	
}


#if APP_COBRA_VOICE_WAKEUP_EN
void app_cobra_voice_wakeup_init(void)
{
		cobra_mcu_map_aon_pinmodeset(AON_PAD4, AON_PAD4_O_VA_PDM_CLK);
		cobra_mcu_map_aon_pinmodeset(AON_PAD3, AON_PAD3_O_VA_PDM_RD);
		cobra_mcu_map_aon_pinmodeset(AON_PAD2, AON_PAD2_O_VA_PDM_LD);
	
		cobra_umcu_cfg_clk1fs_mode(0x02);
	
		cobra_umcu_cfg_remove_mean_vld_en(1);
	
		cobra_umcu_cfg_gain_steps_vld_en(1);
		cobra_umcu_cfg_gain_steps(5);
	
		cobra_umcu_cfg_threshold_vld_en(1);
		cobra_umcu_cfg_threshold(120);
	
		cobra_umcu_cfg_voice_awake_filter_points_vld_en(1);
		cobra_umcu_cfg_voice_awake_filter_points(1);
	
	
		cobra_aon_pmu_protection_en(1);
		cobra_aon_pmu_aon2mcu_source3_selection(VOICE_WAKEUP_EVENT);
		cobra_aon_pmu_wakeup_source2_selection(VOICE_WAKEUP_EVENT);
		cobra_aon_pmu_protection_en(0);
	
//	cobra_aon_pmu_wakeup_source0_selection(IO2_EDGE_INT_EVENT);
//			cobra_aon_pmu_aon2mcu_source0_selection(IO2_EDGE_INT_EVENT);
		NVIC_SetPriority(AON_EVENT3_IRQn, 1);
		NVIC_ClearPendingIRQ(AON_EVENT3_IRQn);
		NVIC_EnableIRQ(AON_EVENT3_IRQn);
				
		cobra_umcu_cfg_voice_awake_enable(1);
			
}
#endif

void cobra_m3_umcu_interrupt(void)
{
	//读取 一个寄存器 0x10 中的值, 这个值 是按键的值, 是umcu 稍描得到的 放到了 0x10  中
	  printf("%d \n", cobra_umcu->umcu_r32_bit_data0);
		cobra_umcu_clear_intr_wevent();    //清中断
		cobra_umcu_clear_intr_done();
		app_cobra_umcu_run();
}


void cobra_m3_voice_wakeup_interrupt(void)
{
	 printf("cobra_m3_voice_wakeup_interrupt \n");
}


#if (APP_COBRA_UMCU_EN) && (COBRA_IRDA_EN) 
uint16_t  key_scan_umcu_program[] = {
0x6c02,
0x4000,
0x004c,
0x0a0c,
0xb010,
0xc000,
0x008c,
0x080c,
0x440a,
0x4008,
0x004c,
0x801f,
0xc000,
0x0086,
0x0406,
0x004c,
0x81ff,
0xc000,
0x008a,
0x00ca,
0x4c16,
0x4013,
0x0049,
0x0086,
0x0406,
0x004c,
0x8032,
0xc000,
0x008a,
0x00ca,
0x6262,
0x6468,
0x666e,
0x6875,
0x6a7c,
0x4c25,
0x401d,
0x0049,
0x0086,
0x0106,
0x0406,
0x004c,
0x8032,
0xc000,
0x008a,
0x00ca,
0x6482,
0x6689,
0x6890,
0x6a96,
0x4c34,
0x402d,
0x0049,
0x0086,
0x0106,
0x0106,
0x0406,
0x004c,
0x8032,
0xc000,
0x008a,
0x00ca,
0x669d,
0x68a4,
0x6aac,
0x4c43,
0x403d,
0x0049,
0x0086,
0x0106,
0x0106,
0x0106,
0x0406,
0x004c,
0x8032,
0xc000,
0x008a,
0x00ca,
0x68b4,
0x6abb,
0x4c52,
0x404d,
0x0049,
0x0086,
0x0106,
0x0106,
0x0106,
0x0106,
0x0406,
0x004c,
0x8032,
0xc000,
0x008a,
0x00ca,
0x6ac3,
0x4c61,
0x405d,
0x4000,
0x004a,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0046,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x004a,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0046,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x004b,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x004b,
0x008a,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x004b,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x008a,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x0086,
0x008a,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x0086,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x008b,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x008b,
0x008a,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x0048,
0x008b,
0x0086,
0x008c,
0x0c0c,
0x000c,
0x140c,
0x40cb,
0x44cd,
0x40cb,
0x004c,
0xb000,
0xc000,
0x008c,
0x080c,
0x44d4,
0x40d2,
0x004c,
0x800b,
0xc000,
0x008c,
0x140c,
0x44db,
0x40d9,
0x004a,
0x008a,
0x0a0c,
0x4000,

};
void app_cobra_key_scan_init(void)
{	
		cobra_umcu_clear_intr_wevent();    //清中断
		cobra_umcu_clear_intr_done();
	
		cobra_aon_pmu_protection_en(1);
		cobra_aon_pmu_io0_posedge_detect_enable(1);   //AIO0 上升缘触发
		cobra_aon_pmu_aon2mcu_source3_selection(UMCU_INT_EVENT);
		cobra_aon_pmu_protection_en(0);
	
		cobra_irda_tx_key_invert(1);
		
		cobra_umcu_copy_program(key_scan_umcu_program, sizeof(key_scan_umcu_program)/sizeof(uint16_t));
	
		cobra_mcu_map_aon_pinmodeset(AON_PAD0, AON_PAD0_RMT0_TX_KEY0);
		cobra_mcu_map_aon_pinmodeset(AON_PAD1, AON_PAD1_RMT0_TX_KEY1);
		cobra_mcu_map_aon_pinmodeset(AON_PAD2, AON_PAD2_RMT0_TX_KEY2);
		cobra_mcu_map_aon_pinmodeset(AON_PAD3, AON_PAD3_RMT0_TX_KEY3);
	  cobra_mcu_map_aon_pinmodeset(AON_PAD4, AON_PAD4_RMT0_TX_KEY4);
	
		app_cobra_umcu_run();
		
}

#endif

#if (APP_COBRA_UMCU_WAKEUP_M0_EN)

uint16_t  cobra_umcu_wakeup_m0_program[] = {
0x6c02,
0x4000,
0x004c,
0x0a0c,
0x004c,
0x8010,
0xc000,
0x008a,
0x00ca,
0x4c0b,
0x4008,
0x004a,
0x008a,
0x0a0c,
0x004c,
0xb010,
0xc000,
0x008c,
0x080c,
0x4415,
0x4013,
0x004c,
0x8001,
0xc000,
0x008c,
0x140c,
0x441c,
0x401a,
0x004c,
0x0a0c,
0x004c,
0xb010,
0xc000,
0x008c,
0x080c,
0x4425,
0x4023,
0x004c,
0x8001,
0xc000,
0x008c,
0x140c,
0x442c,
0x402a,
0x004c,
0xb000,
0xc000,
0x008c,
0x080c,
0x4433,
0x4031,
0x004c,
0x140c,
0x4437,
0x4035,
0x4037,


};


void app_cobra_umcu_wakeup_m0_init(void)
{	
		cobra_umcu_clear_intr_wevent();    //清中断
		cobra_umcu_clear_intr_done();
	
//		cobra_aon_pmu_protection_en(1);
//		cobra_aon_pmu_aon2mcu_source0_selection(UMCU_INT_EVENT);
//		cobra_aon_pmu_aon2mcu_source1_selection(UMCU_INT_EVENT);
	
//		cobra_aon_pmu_protection_en(0);
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_aon2mcu_source0_selection(AON_UMCU_INT);
		cobra4_aon_pd_top_aon2mcu_source1_selection(AON_UMCU_INT);
		cobra4_aon_pd_top_protection_en(0);
	
		cobra_umcu_copy_program(cobra_umcu_wakeup_m0_program, sizeof(cobra_umcu_wakeup_m0_program)/sizeof(uint16_t));
	

		NVIC_SetPriority(AON_EVENT0_IRQn, 1);
		NVIC_ClearPendingIRQ(AON_EVENT0_IRQn);
		NVIC_EnableIRQ(AON_EVENT0_IRQn);
	

	
}

void app_cobra_umcu_wakeup_m0_run(void)
{
//		cobra_aon_pmu_protection_en(1);
//		cobra_aon_pmu_ucpu_hclk_enable(1);
//	  cobra_aon_pmu_ucpu_presetn(0);    //do not reset 
//		cobra_aon_pmu_protection_en(0);
	
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_ucpu_pclk_en(1);
		cobra4_aon_pd_top_umcu_srst_req_en(1);
		cobra4_aon_pd_top_protection_en(0);
		app_cobra_umcu_run();
}


void cobra_umcu_wakeup_m0_interrupt(void)
{
	//	printf("cobra_umcu_wakeup_m0_interrupt \n");
//		cobra_ipc_test_send_flag();
	  cobra_umcu_enable_umcu(0);
//		cobra_aon_pmu_protection_en(1);
//		cobra_aon_pmu_ucpu_presetn(1);   //reset it
//		cobra_aon_pmu_ucpu_presetn(0);
//		cobra_aon_pmu_protection_en(0);
	
		cobra_umcu_clear_intr_wevent();    //清中断
		cobra_umcu_clear_intr_done();
		cobra_umcu_write_umcu_r16_bit_data0(0x00);
		cobra_umcu_write_umcu_r16_bit_data0(0x01);
		
}


#endif

#endif














