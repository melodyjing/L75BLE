/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_pcrm.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   
					cobra power manger control
					according from document cobra2 register
					
					Power, Reset and Clock Management


	Changed Log	:

		Liujian  2019/3/8			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_pcrm.h"


#if COBRA_PCRM_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_PCRM_T *cobra_pcrm __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_PCRM_T * cobra_pcrm;
#endif

void cobra_pcrm_init(void)
{
		cobra_pcrm = (COBRA_PCRM_T *)COBRA_PCRM_BASE;
	   
}

//mcu_pwrup_ready
unsigned int cobra_pcrm_mcu_pwrup_ready(void)
{
		return cobra_pcrm->pwr_state_and_rst_ctrl & 0x01;
}

//m3_pwrup_ready
unsigned int cobra_pcrm_m3_pwrup_ready(void)
{
		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 2) & 0x01;
}

////rfm0_pwrup_ready
//unsigned int cobra_pcrm_rfm0_pwr_ready(void)
//{
//		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 3) & 0x01;
//}

//sram vim 16KB power is ready
//vim_pwrup_ready
//unsigned int cobra_pcrm_vim_pwr_ready(void)
//{
//		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 4) & 0x01;
//}

////bus power up ready
//unsigned int cobra_pcrm_bus_pwr_ready(void)
//{
//		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 5) & 0x01;
//}

//p1 power up ready
unsigned int cobra_pcrm_p1_pwr_ready(void)
{
		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 6) & 0x01;
}

//p2 power up ready
unsigned int cobra_pcrm_p2_pwr_ready(void)
{
		return (cobra_pcrm->pwr_state_and_rst_ctrl >> 7) & 0x01;
}
/*
primary mcu id:
0: m3 is primary mcu
1: m0 is primary mcu"
*/
PRIMARY_MCU_ID cobra_pcrm_who_is_primary_mcu_id(void)
{
		if((cobra_pcrm->pwr_state_and_rst_ctrl >> 15) & 0x01)
		{
				return M0_IS_PRIMARY_MCU;
		}
		else
		{
				return M3_IS_PRIMARY_MCU;
		}
}

/*
"m3 warm reset request enable
0:  disable m3 warm reset request
1:  enable  m3 warm reset request"

warm reset include:SYSRESETREQ,m3_LOCKUP,watchdog reset

*/
void cobra_pcrm_m3_warm_request_en(unsigned int en)
{
		if(en)
		{
			  cobra_pcrm->pwr_state_and_rst_ctrl |= (1 << 16);
		}
		else
		{
				cobra_pcrm->pwr_state_and_rst_ctrl &= ~(1 << 16);
		}
}

///*
//"m0 warm reset request enable
//0:  disable m0 warm reset request
//1:  enable  m0 warm reset request"
//warm reset include:SYSRESETREQ,m3_LOCKUP,watchdog reset

//*/
//void cobra_pcrm_m0_warm_request_en(unsigned int en)
//{
//		if(en)
//		{
//			  cobra_pcrm->pwr_state_and_rst_ctrl |= (1 << 17);
//		}
//		else
//		{
//				cobra_pcrm->pwr_state_and_rst_ctrl &= ~(1 << 17);
//		}
//}

/*

"whole chip warm request control
0: disable
1: enable"

*/
void cobra_pcrm_whole_chip_warm_request_en(unsigned int en)
{
		if(en)
		{
			  cobra_pcrm->pwr_state_and_rst_ctrl |= (1 << 18);
		}
		else
		{
				cobra_pcrm->pwr_state_and_rst_ctrl &= ~(1 << 18);
		}
}

/*
"chip shutdown mode request
0:  disable
1:  enable"

*/
void cobra_pcrm_shutdown_mode_en(unsigned int en)
{
		volatile unsigned int temp;
	
		temp = cobra_pcrm->shutdown_req;
		if(en)
		{
			  temp &= ~0x03;
				temp |= 0x01;
		}
		else
		{
				temp &= ~0x03;
		}
		cobra_pcrm->shutdown_req = temp;
		
}

/*
"chip standby mode request
0:  disable
1:  enable"

*/
void cobra_pcrm_standby_mode_en(unsigned int en)
{
		volatile unsigned int temp;
		
		temp = cobra_pcrm->standby_req;
	
		if(en)
		{
			  temp &= ~0x03;
				temp |= 0x01;
		}
		else
		{
				temp &= ~0x03;
		}
		
		cobra_pcrm->standby_req = temp;
		
}

/*

soft reset request for whole chip
active high

*/
void cobra_pcrm_soft_reset_whole_chip(unsigned int en)
{
		if(en)
		{
			  cobra_pcrm->sys_soft_reset_req |= (1 << 0);
		}
		else
		{
				cobra_pcrm->sys_soft_reset_req &= ~(1 << 0);
		}
}

////m3 power soft request, for primary mcu id=1
//void  cobra_pcrm_m3_power_soft_request_en(unsigned int en)
//{	
//		PRIMARY_MCU_ID mcu_id;
//		
//	  mcu_id  = cobra_pcrm_who_is_primary_mcu_id();
//	
//		if(mcu_id == M3_IS_PRIMARY_MCU)
//		{
//				if(en)
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req0 |= 0x01;
//				}
//				else
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req0 &= ~0x01;
//				}
//		}
//		else
//		{
//				if(en)
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req1 |= 0x01;
//				}
//				else
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req1 &= ~0x01;
//				}
//		}
//	
//}

//m0 power soft request, for primary mcu id=1
//void  cobra_pcrm_m0_power_soft_request_en(unsigned int en)
//{	
//		PRIMARY_MCU_ID mcu_id;
//		
//	  mcu_id  = cobra_pcrm_who_is_primary_mcu_id();
//	
//		if(mcu_id == M3_IS_PRIMARY_MCU)
//		{
//				if(en)
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req0 |= 0x02;
//				}
//				else
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req0 &= ~0x02;
//				}
//		}
//		else
//		{
//				if(en)
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req1 |= 0x02;
//				}
//				else
//				{
//						cobra_pcrm->m3_or_m0_power_soft_reset_req1 &= ~0x02;
//				}
//		}
//	
//}

/*
¿ÉÒÔÀí½âÎªM3 µÄµçÔ´¿ª¹Ø
en.
1. M3 Power up
0. M3 Power Down

*/
//void  cobra_pcrm_m3_power_enable_ctrl(unsigned int en)
//{
//		PRIMARY_MCU_ID mcu_id;
//		
//	  mcu_id  = cobra_pcrm_who_is_primary_mcu_id();
//	
//		if(en)
//		{
//				cobra_pcrm_m3_power_soft_request_en(1);
//			
//				if(mcu_id == M3_IS_PRIMARY_MCU)
//				{
//						cobra_pcrm->m3_pwr_req0_h2 |= 0x01;
//				}
//				else
//				{
//						cobra_pcrm->m3_pwr_req1_h2 |= 0x01;
//				}
//		}
//		else
//		{
//				cobra_pcrm_m3_power_soft_request_en(0);
//			
//				if(mcu_id == M3_IS_PRIMARY_MCU)
//				{
//					cobra_pcrm->m3_pwr_req0_h2 &= ~0x01;
//				}
//				else
//				{
//					 cobra_pcrm->m3_pwr_req1_h2 &= ~0x01;
//				}
//		}
//}
/*
¿ÉÒÔÀí½âÎªM0 µÄµçÔ´¿ª¹Ø
en:
1. Power Up
0. Power Down


*/
//void  cobra_pcrm_rfm0_power_enable_ctrl(unsigned int en)
//{
//	
//		PRIMARY_MCU_ID mcu_id;		
//	  mcu_id  = cobra_pcrm_who_is_primary_mcu_id();
//	
//		if(en)
//		{
//				cobra_pcrm_m0_power_soft_request_en(1);
//				if(mcu_id == M3_IS_PRIMARY_MCU)
//				{
//						cobra_pcrm->rfm0_pwr_req0_h2 |= 0x01;
//				}
//				else
//				{
//						cobra_pcrm->rfm0_pwr_req1_h2 |= 0x01;
//				}
//		}
//		else
//		{
//				cobra_pcrm_m0_power_soft_request_en(0);
//			
//				if(mcu_id == M3_IS_PRIMARY_MCU)
//				{
//						cobra_pcrm->rfm0_pwr_req0_h2 &= ~0x01;
//				}
//				else
//				{
//						cobra_pcrm->rfm0_pwr_req1_h2 &= ~0x01;
//				}
//		}
//}

/*
"0: vim clock is free run
1: vim clock is gated by m4 deep sleep"


*/
void cobra_pcrm_vim_clock_mode(VIM_CLOCK_MODE mode)
{
		if(mode == VIM_CLK_FREE_RUM)
		{
				cobra_pcrm->vim_power_ctrl &= ~0x01;
		}
		else if(mode == VIM_CLK_GATED_BY_M4_DEEP_SLEEP)
		{
				cobra_pcrm->vim_power_ctrl |= 0x01;
		}
}

/*
"0: bus clock is free run
1: bus clock is gated by m4 deep sleep"

*/
void cobra_pcrm_bus_clock_mode(BUS_CLOCK_MODE mode)
{
		if(mode == BUS_CLOCK_FREE_RUN)
		{
				cobra_pcrm->vim_power_ctrl &= ~0x02;
		}
		else if(mode == BUS_CLK_GATED_BY_M4_DEEP_SLEEP)
		{
				cobra_pcrm->vim_power_ctrl |= 0x02;
		}
}

/*

"0: M4_FCLK clock is free run
1: M4_FCLK clock is gated by m4 deep sleep"

*/
void cobra_pcrm_m4_flck_clock_mode(M4_FLCK_CLOCK_MODE mode)
{
		if(mode == M4_FCLK_FREE_RUN)
		{
				cobra_pcrm->vim_power_ctrl &= ~0x04;
		}
		else if(mode == M4_FCLK_GATED_BY_M4_DEEP_SLEEP)
		{
				cobra_pcrm->vim_power_ctrl |= 0x04;
		}
}


//void cobra_pcrm_vim_power_on_enable(unsigned int en)
//{
//		if(cobra_pcrm->vim_power_ctrl & 0x04)  //softcontrol mode
//		{
//				cobra_pcrm_vim_power_up_software_ctrl_en(en);
//		}
//		else                            //hardware mode
//		{
//				cobra_pcrm_vim_power_up_hardware_ctrl_mode(en);
//		}
//	
//}



/*
P1 µçÔ´ÓòµÄ¿ØÖÆ
En
1. Power up
0. Power Down
*/
void cobra_pcrm_p1_power_ctrl(unsigned int En)
{
		if(En)
		{
				cobra_pcrm->p1_p2_pwr_req1 |= 0x01;
				cobra_pcrm->p1_pwr_req2 = 0x01;	
		}
		else
		{
				cobra_pcrm->p1_p2_pwr_req1 &= ~0x01;
				cobra_pcrm->p1_pwr_req2 = 0x00;	
		}
}

/*
P2 µçÔ´ÓòµÄ¿ØÖÆ
En
1. Power up
0. Power Down
*/
void cobra_pcrm_p2_power_ctrl(unsigned int En)
{
		if(En)
		{
				cobra_pcrm->p1_p2_pwr_req1 |= 0x02;
				cobra_pcrm->p2_pwr_req2 = 0x01;	
		}
		else
		{
				cobra_pcrm->p1_p2_pwr_req1 &= ~0x02;
				cobra_pcrm->p2_pwr_req2 = 0x00;	
		}
}



void cobra_pcrm_stclk_parameter_set(unsigned int parameter)
{
		cobra_pcrm->stclk_calib = parameter&0x3ffffff;	
}

///*
//SRAM0 retention enable
//en
//1. SRAM0 retention
//0. SRAM0 at always on domain
//*/

//void cobra_pcrm_sram0_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x01;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x01;
//		}
//}

///*
//SRAM1 retention enable
//en
//1. SRAM1 retention
//0. SRAM1 at always on domain
//*/

//void cobra_pcrm_sram1_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x02;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x02;
//		}
//}

///*
//SRAM2 retention enable
//en
//1. SRAM2 retention
//0. SRAM2 at always on domain
//*/

//void cobra_pcrm_sram2_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x04;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x04;
//		}
//}

///*
//SRAM3 retention enable
//en
//1. SRAM3 retention
//0. SRAM3 at always on domain
//*/

//void cobra_pcrm_sram3_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x08;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x08;
//		}
//}


////versatile Instruction Memory System 
///*
//VIM SRAM retention enable
//en
//1. VIM SRAM retention
//0. VIM SRAM at always on domain
//*/

//void cobra_pcrm_vim_sram1_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x10;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x10;
//		}
//}

//void cobra_pcrm_vim_sram2_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x20;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x20;
//		}
//}

//void cobra_pcrm_vim_sram3_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x40;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x40;
//		}
//}


//void cobra_pcrm_vim_sram4_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x80;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x80;
//		}
//}



///*
//bluetooth exchange memory retention enable
//en
//1. Exchange memory retention
//0. Exchange memory at always on domain
//*/

//void cobra_pcrm_exchange_memory_retention_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->sram_ret_cfg |= 0x100;
//		}
//		else
//		{
//				cobra_pcrm->sram_ret_cfg &= ~0x100;
//		}
//}


/*
M3 Debug enable

*/
void cobra_pcrm_m3_debug_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en |= 0x01;
		}
		else
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en &= ~0x01;
		}	
}

/*
M3 FIXMASTERTYPE
???????????????????????
*/
void cobra_pcrm_m3_fix_master_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en |= 0x02;
		}
		else
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en &= ~0x02;
		}	
}


/*
M0 DCLK en

*/
void cobra_pcrm_m0_dclk_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en |= 0x04;
		}
		else
		{
				cobra_pcrm->m3_dbgen_m0_dclk_en &= ~0x04;
		}	
}

/*
"soft mode for master cpu selection:
0: m3
1: m0"
*/

//void cobra_pcrm_master_cpu_soft_sel(PRIMARY_MCU_ID id)
//{
//		if(id == M3_IS_PRIMARY_MCU)
//		{
//				cobra_pcrm->master_cpu_ctrl &= ~0x01;
//		}
//		else if(id == M0_IS_PRIMARY_MCU)
//		{
//				cobra_pcrm->master_cpu_ctrl |= 0x01;
//		}	
//}

/*

"0: hard_mode, pri_mcu_id is used 
1: soft_mode,"

*/
//void cobra_pcrm_master_cpu_mode( CPU_CONTROL_MODE mode)
//{
//		if(mode == CPU_HARDWARE_MODE)
//		{
//				cobra_pcrm->master_cpu_ctrl &= ~0x02;
//		}
//		else if(mode == CPU_SOFTWARE_MODE)
//		{
//				cobra_pcrm->master_cpu_ctrl |= 0x02;
//		}
//}


/*
"m3 soft reset request, when pri_mcu_id is 1
1: enable reset request
0: disable reset request"
*/

void cobra_pcrm_reset_m3(unsigned int en)
{
//		PRIMARY_MCU_ID mcu_id;		
	
		if(en)
		{
				cobra_pcrm->m3_srst_req |= 0x01;
		}
		else
		{
				cobra_pcrm->m3_srst_req &= ~0x01;
		}

}

/*
"rfm0 soft reset request, when pri_mcu_id is 0
1: enable reset request
0: disable reset request"

0 ÊÇ²»¸´Î» M0
1 ÊÇ¸´Î»×¡ M0

*/
//void cobra_pcrm_m3_reset_m0(unsigned int en)
//{
//		PRIMARY_MCU_ID mcu_id;		
//	  mcu_id  = cobra_pcrm_who_is_primary_mcu_id();
//		
//		if(mcu_id == M3_IS_PRIMARY_MCU)
//		{
//				if(en)
//				{
//						cobra_pcrm->rfm0_srst_req |= 0x01;
//				}
//				else
//				{
//						cobra_pcrm->rfm0_srst_req &= ~0x01;
//				}
//		}	
//}

/*

vim tcm remap control
if en  == 1, VIMS_RAM(16KB) at address 0x0000
if en  == 0, VIMS_RAM(16KB) at address 0x00200000
*/
void cobra_pcrm_m3_remap(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->m3_remap |= 0x01;
		}
		else
		{
				cobra_pcrm->m3_remap &= ~0x01;		
		}
}

/*
P1 delay window config


*/
void cobra_pcmr_pudly_p1_p2_window_config_p1_value(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->pudly_p1_p2_window_config;
	temp &= ~0xff;
	temp |= value & 0xff;
	cobra_pcrm->pudly_p1_p2_window_config = temp;
}

/*
P2 delay window config


*/
void cobra_pcmr_pudly_p1_p2_window_config_p2_value(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->pudly_p1_p2_window_config;
	temp &= ~(0xff << 8);
	temp |= ((value & 0xff) << 8);
	cobra_pcrm->pudly_p1_p2_window_config = temp;
	
}


/*
SPI1 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"

mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_spi1_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<0);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<0);
		}
}

/*
AES clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_aes_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<1);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<1);
		}
}


/*
I2C1 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_i2c1_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<2);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<2);
		}
}

/*
I2C0 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_i2c0_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<3);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<3);
		}
}

/*
CAN0 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_can0_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<4);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<4);
		}
}


/*
SPI0 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_spi0_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<5);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<5);
		}
}

/*
UART1 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_uart1_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<6);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<6);
		}
}


/*
UART0 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_uart0_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<7);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<7);
		}
}

/*
SHA m clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_sha_m_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<8);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<8);
		}
}

/*
SHA work clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_sha_work_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<9);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<9);
		}
}


/*
SPI Flash control clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
*/
void cobra_pcrm_spi_flash_control_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<10);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<10);
		}
}


/*
UART11 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ

APB BUS10
M0
*/
void cobra_pcrm_uart11_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<11);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<11);
		}
}

/*
UART10 clock control
"enclica ip clock enable mask 
0: un-mask
1: mask, the clock can not be control by module active signal"
mask == 1,´ú±í¾ÍÒ»Ö±ÓÐclock ÔÚÍâÉèÉÏÃæ
APB BUS10
M0

*/
void cobra_pcrm_uart10_clk_mask_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->enclica_cact_mask |= (1<<12);
		}
		else
		{
				cobra_pcrm->enclica_cact_mask &= ~(1<<12);
		}
}

/*

rfm0 hclken shift pattern to rfm0
include:
			UART10
			UART11
			Watchdog10
			Timer10
			m0 FPB
pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
//void cobra_pcrm_rfm0_apb_cfg_shift_pattern(unsigned int pattern)
//{
//		cobra_pcrm->rfm0_apb_cfg = pattern&0xffff;
//}


/*
system clock divider, source is 24M or 48M

0: div by 1, 1: div by 2,… 255:div by 256

*/
void cobra_pcrm_system_clk_divider(unsigned int divider)
{
		unsigned int temp;
		temp = cobra_pcrm->hf_clk_div_pmc_clk_cfg;
		temp &= ~0xff;
		temp |= divider&0xff;	
		//cobra_pcrm->hf_clk_div_pmc_clk_cfg &= ~0xff;
		cobra_pcrm->hf_clk_div_pmc_clk_cfg = temp;	
}

/*
hclk_en shift length
0: shift length is 1, 15: shift length is 16

*/
void cobra_pcrm_shift_pattern_length(unsigned int length)
{
		unsigned int temp;
		temp = cobra_pcrm->hf_clk_div_pmc_clk_cfg;
		temp &= ~(0xf << 8);
		temp |= ((length&0xf) << 8);	
	//	cobra_pcrm->hf_clk_div_pmc_clk_cfg &= ~(0xf << 8);
		cobra_pcrm->hf_clk_div_pmc_clk_cfg = temp;	
}


void cobra_pcrm_pmu_clk_divider(unsigned int divider)
{
		unsigned int temp;
		temp = cobra_pcrm->hf_clk_div_pmc_clk_cfg;
		temp &= ~(0xff << 16);
		temp |= ((divider&0xff) << 16);	
	
		//cobra_pcrm->hf_clk_div_pmc_clk_cfg &= ~(0xff << 16);
		cobra_pcrm->hf_clk_div_pmc_clk_cfg = temp;	
}


/*

bus hclken shift pattern to m3
AHB BUS Matrix pattern
Default no divider 48Mhz

pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
//void cobra_pcrm_m3_system_clk_shift_pattern(unsigned int pattern)
//{
//		unsigned int temp;
//	  temp =  cobra_pcrm->m3_sbus_cfg_sbus_apb0_cfg;
//	  temp &= ~0xffff;
//	  temp |= pattern&0xffff;
//		cobra_pcrm->m3_sbus_cfg_sbus_apb0_cfg = temp;	
//}

/*

apb0 hclken shift pattern to bus
include RTC
				IRDA
				PMU/ADC/DCDC
				iRAM_CTRL
				GPADC CMP
				voice awake
				Timer1

				always on domain 


pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
//void cobra_pcrm_apb0_clk_shift_pattern(unsigned int pattern)
//{
//		unsigned int temp;
//		temp = cobra_pcrm->m3_sbus_cfg_sbus_apb0_cfg;
//	
//		temp &= ~(((unsigned int)0xffff) << 16);
//		temp |= ((pattern&0xffff)<<16);
//		cobra_pcrm->m3_sbus_cfg_sbus_apb0_cfg = temp;
//}

/*

apb1 hclken shift pattern to bus
include UART0
				UART1
				MPWM
				SPWM
				SPI0
				
				I2C0
				I2C1
				
				QDEC
				
				IRDA
pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
void cobra_pcrm_apb1_clk_shift_pattern(unsigned int pattern)
{
		unsigned int temp;
		temp = cobra_pcrm->sbus_apb1_cfg_apb2_cfg;
		temp &= ~(0xffff);
		temp |= (pattern&0xffff);
		cobra_pcrm->sbus_apb1_cfg_apb2_cfg = temp;
}

/*
apb2 hclken shift pattern to bus
include 
			AES
			
			ECC
			
			TRNG
			uDMAC
			CacheCtrl
			Timer0
			Timer1
			Dualtimer
			
			SPI1
			SRC 0
			I2S0
			I2S1

pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3
*/
void cobra_pcrm_apb2_clk_shift_pattern(unsigned int pattern)
{
		unsigned int temp;
		temp = cobra_pcrm->sbus_apb1_cfg_apb2_cfg;
		temp &= ~(((unsigned int)0xffff)<<16);
		temp |= ((pattern&0xffff) << 16);
	  cobra_pcrm->sbus_apb1_cfg_apb2_cfg = temp;
}

/*
apb3 hclken shift pattern to bus
include 
			Pinmux/DMA sel
			
			PCRM

pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
void cobra_pcrm_apb3_clk_shift_pattern(unsigned int pattern)
{
		unsigned int temp;
		
		temp = cobra_pcrm->sbus_apb3_cfg_sbus_rfm0_cfg;
		temp &= ~(0xffff);
		temp |= (pattern&0xffff);
		cobra_pcrm->sbus_apb3_cfg_sbus_rfm0_cfg = temp;
}

/*

bus hclken shift pattern to m0
M0 AHB  BUS matrix clk
default 48M , no divider

pattern 01  , clk ¶þ·ÖÆµ  Èç¹ûshiftpattern  ÊÇ 2
pattern 001 , clk Èý·ÖÆµ, Èç¹ûshiftpattern  ÊÇ 3

*/
void cobra_pcrm_m0_system_clk_shift_pattern(unsigned int pattern)
{
		unsigned int temp;
		
		temp = cobra_pcrm->sbus_apb3_cfg_sbus_rfm0_cfg;
		
		temp &= ~((unsigned int)0xffff << 16);
		temp |= ((pattern&0xffff) << 16);
	
		cobra_pcrm->sbus_apb3_cfg_sbus_rfm0_cfg = temp;
}


//--------------Active mode ---------
/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m3 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_m3_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<0);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m0 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
//void cobra_pcrm_rfm0_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<1);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<1);
//		}
//}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bluetooth Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_bluetooth_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_vim_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_system_bus_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<4);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus ºÍVIM BUS Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_sys_bus_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<7);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SPI Flash control Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spiflash_ctrl_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<8);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<8);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UDMA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_udma_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SHA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_sha_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aon_inf_bridge_hclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aon_inf_bridge_hclk_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<11);
		}
}


/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim_cache_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_cache_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<14);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<14);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ eflash_ctrl_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_eflash_ctrl_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<15);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<15);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pinmux_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pinmux_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<16);
		}
}

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bt_cpu_hclken Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_bt_cpu_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<18);
		}
}  

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart10_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm_uart10_clk_mask_en(1);
//				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<24);
//		}
//		else
//		{
//				cobra_pcrm_uart10_clk_mask_en(0);
//				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<24);
//		}
//}  

/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART11 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart11_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm_uart10_clk_mask_en(1);
//				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<25);
//		}
//		else
//		{
//				cobra_pcrm_uart11_clk_mask_en(0);
//				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<25);
//		}
//}  


/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ Timer10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_timer10_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_active_ctrl_en |= (1<<26);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_active_ctrl_en &= ~(1<<26);
//		}
//} 


/*
ÔÚActive ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ apll_dclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_dclk_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_active_ctrl_en |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_pcrm->module_clock1_active_ctrl_en &= ~((unsigned int)1<<31);
		}
} 

//*********************sleep mode********************************
//-----------------------
/*
ÔÚ  Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m3 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_m3_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<0);
		}
}

/*
ÔÚ  sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m0 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
//void cobra_pcrm_rfm0_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<1);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<1);
//		}
//}

/*
ÔÚ  sleep mode¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bluetooth Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_bluetooth_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_vim_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_system_bus_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<4);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus ºÍVIM BUS Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_sys_bus_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<7);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SPI Flash control Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spiflash_ctrl_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<8);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<8);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UDMA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_udma_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SHA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_sha_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aon_inf_bridge_hclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aon_inf_bridge_hclk_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<11);
		}
}


/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim_cache_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_cache_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<14);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<14);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ eflash_ctrl_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_eflash_ctrl_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<15);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<15);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pinmux_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pinmux_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<16);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bt_cpu_hclken Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_bt_cpu_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<18);
		}
}  

/*
ÔÚ sleep mode  ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart10_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<24);
//		}
//		else
//		{
//				cobra_pcrm_uart10_clk_mask_en(0);
//				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<24);
//		}
//}  

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART11 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart11_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<25);
//		}
//		else
//		{
//				cobra_pcrm_uart11_clk_mask_en(0);
//				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<25);
//		}
//}  


/*
ÔÚ sleep mode  ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ Timer10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_timer10_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en |= (1<<26);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~(1<<26);
//		}
//} 


/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ apll_dclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_dclk_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~((unsigned int)1<<31);
		}
} 

//*********************deep sleep mode********************************
//-----------------------
/*
ÔÚ  Deep Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m3 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_m3_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<0);
		}
}

/*
ÔÚ  sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ m0 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
//void cobra_pcrm_rfm0_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<1);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<1);
//		}
//}

/*
ÔÚ  sleep mode¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bluetooth Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_bluetooth_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ
*/
void cobra_pcrm_vim_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_system_bus_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<4);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ system bus ºÍVIM BUS Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_sys_bus_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<7);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SPI Flash control Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spiflash_ctrl_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<8);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<8);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UDMA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_udma_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ SHA Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_sha_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aon_inf_bridge_hclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aon_inf_bridge_hclk_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<11);
		}
}


/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ vim_cache_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_vim_cache_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<14);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<14);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ eflash_ctrl_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_eflash_ctrl_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<15);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<15);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pinmux_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pinmux_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<16);
		}
}

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ bt_cpu_hclken Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_bt_cpu_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<18);
		}
}  

/*
ÔÚ sleep mode  ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart10_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<24);
//		}
//		else
//		{
//				cobra_pcrm_uart10_clk_mask_en(0);
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<24);
//		}
//}  

/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ UART11 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_uart11_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<25);
//		}
//		else
//		{
//				cobra_pcrm_uart11_clk_mask_en(0);
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<25);
//		}
//}  


/*
ÔÚ sleep mode  ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ Timer10 Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_timer10_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en |= (1<<26);
//		}
//		else
//		{
//				cobra_pcrm->module_clock1_deepsleep_ctrl_en &= ~(1<<26);
//		}
//} 


/*
ÔÚ sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ apll_dclk_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_dclk_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_pcrm->module_clock1_sleep_ctrl_en &= ~((unsigned int)1<<31);
		}
} 
//-----------------------------moduel 2 active mode-----------------------------------------------------
//moduel 2 active mode
/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<0);
				cobra_pcrm_uart0_clk_mask_en(0);
		}
} 

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<1);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<1);
				cobra_pcrm_uart1_clk_mask_en(0);
		}
} 

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ mpwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_mpwm_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spwm_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<4);
				cobra_pcrm_spi0_clk_mask_en(0);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ can0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_can0_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<5);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<5);
//				cobra_pcrm_can0_clk_mask_en(0);
//		}
//}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<6);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<6);
				cobra_pcrm_i2c0_clk_mask_en(0);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<7);
				cobra_pcrm_i2c1_clk_mask_en(0);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_gpio1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<8);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<8);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ qdec_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_qdec_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ irda_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ  
*/
//2019/8/5 for cobra4
void cobra_pcrm_irda_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dvad_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ  
*/
//2019/8/5 for cobra4
void cobra_pcrm_dvad_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<11);
		}
}


/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aes0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aes_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<16);
				cobra_pcrm_aes_clk_mask_en(0);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ trng_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_trng_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<17);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<17);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<18);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<19);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<19);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dualtimer_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_dualtimer_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<20);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<20);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_gpio0_active_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<21);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<21);
//		}
//}


/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<22);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<22);
				cobra_pcrm_spi1_clk_mask_en(0);
		}
}


/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<24);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<24);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<25);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<25);
		}
}


/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s0_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<26);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<26);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s1_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<27);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<27);
		}
}


/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ ecc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_ecc_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<28);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<28);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpadc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra4
void cobra_pcrm_gpadc_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<29);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<29);
		}
}

/*
ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ src_apll_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra4
void cobra_pcrm_src_apll_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= (1<<30);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~(1<<30);
		}
}


/* ÔÚ Active mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ APLL_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_active_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_active_ctrl_en |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_pcrm->module_clock2_active_ctrl_en &= ~((unsigned int)1<<31);
		}
}

//**************************************Sleep mode **************************************
//moduel 2 sleep mode
/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<0);
				cobra_pcrm_uart0_clk_mask_en(0);
		}
} 

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<1);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<1);
				cobra_pcrm_uart1_clk_mask_en(0);
		}
} 

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ mpwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_mpwm_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spwm_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<4);
				cobra_pcrm_spi0_clk_mask_en(0);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ can0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_can0_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<5);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<5);
//				cobra_pcrm_can0_clk_mask_en(0);
//		}
//}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<6);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<6);
				cobra_pcrm_i2c0_clk_mask_en(0);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<7);
				cobra_pcrm_i2c1_clk_mask_en(0);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_gpio1_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<8);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<8);
//		}
//}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ qdec_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_qdec_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ irda_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/

//2019/8/5 for cobra4
void cobra_pcrm_irda_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dvad_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra4
void cobra_pcrm_dvad_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<11);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aes0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aes_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<16);
				cobra_pcrm_aes_clk_mask_en(0);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ trng_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_trng_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<17);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<17);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<18);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<19);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<19);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dualtimer_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_dualtimer_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<20);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<20);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_gpio0_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<21);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<21);
//		}
//}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<22);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<22);
				cobra_pcrm_spi1_clk_mask_en(0);
		}
}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<24);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<24);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<25);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<25);
		}
}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s0_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<26);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<26);
		}
}

/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s1_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<27);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<27);
		}
}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ ecc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_ecc_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<28);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<28);
		}
}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpadc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra4
void cobra_pcrm_gpadc_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<29);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<20);
		}
}


/*
ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ src_apll_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra4
void cobra_pcrm_src_apll_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= (1<<30);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~(1<<30);
		}
}

/* ÔÚ Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ APLL_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_pcrm->module_clock2_sleep_ctrl_en &= ~((unsigned int)1<<31);
		}
}


//**************************************Deep Sleep mode **************************************
//moduel 2 deep sleep mode
/*
ÔÚ deep sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<0);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<0);
				cobra_pcrm_uart0_clk_mask_en(0);
		}
} 

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ uart1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_uart1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<1);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<1);
				cobra_pcrm_uart1_clk_mask_en(0);
		}
} 

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ mpwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_mpwm_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<2);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<2);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spwm_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spwm_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<3);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<3);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<4);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<4);
				cobra_pcrm_spi0_clk_mask_en(0);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ can0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_can0_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<5);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<5);
//				cobra_pcrm_can0_clk_mask_en(0);
//		}
//}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<6);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<6);
				cobra_pcrm_i2c0_clk_mask_en(0);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2c1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2c1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<7);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<7);
				cobra_pcrm_i2c1_clk_mask_en(0);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//void cobra_pcrm_gpio1_deep_sleep_mode_clock_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<8);
//		}
//		else
//		{
//				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<8);
//		}
//}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ qdec_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_qdec_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<9);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<9);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ irda_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_irda_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<10);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<10);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dvad_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_dvad_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<11);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<11);
		}
}


/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ aes0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_aes_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<16);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<16);
				cobra_pcrm_aes_clk_mask_en(0);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ trng_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_trng_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<17);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<17);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<18);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<18);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ timer1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_timer1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<19);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<19);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ dualtimer_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_dualtimer_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<20);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<20);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpio0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_gpio0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<21);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<21);
		}
}


/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ spi1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_spi1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<22);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<22);
				cobra_pcrm_spi1_clk_mask_en(0);
		}
}


/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<24);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<24);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ pdm1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_pdm1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<25);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<25);
		}
}


/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s0_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s0_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<26);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<26);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ i2s1_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_i2s1_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<27);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<27);
		}
}


/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ ecc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_ecc_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<28);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<28);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ gpadc_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/

//2019/8/5 for cobra 4
void cobra_pcrm_gpadc_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<29);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<29);
		}
}

/*
ÔÚ Deep Sleep  mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ src_apll_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
//2019/8/5 for cobra 4
void cobra_pcrm_src_apll_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (1<<30);
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~(1<<30);
		}
}

/* ÔÚ Deep Sleep mode ¹¤×÷Ä£Ê½ÏÂ
¿ØÖÆ APLL_en Ê±ÖÓµÄ¹Ø¶Ï¹¦ÄÜ     
???????????????????????????
*/
void cobra_pcrm_apll_deep_sleep_mode_clock_enable(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en |= (((unsigned int)1<<31));
		}
		else
		{
				cobra_pcrm->module_clock2_deepsleep_ctrl_en &= ~((unsigned int)1<<31);
		}
}

//---------------------------IP reset module --------------------------------------------
///*
//m3 soft reset
//m0 use it
//*/
void cobra_pcrm_m3_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 0);
		cobra_pcrm->module_srst_req1 &= ~(1 << 0);
}

///*
//m3 used it
//m0 soft reset

//*/
//void cobra_pcrm_m0_soft_reset(void)
//{
//		cobra_pcrm->module_srst_req1 |= (1 << 1);
//		cobra_pcrm->module_srst_req1 &= ~(1 << 1);	
//}

/*
blue tooth reset

*/
void cobra_pcrm_bt_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 2);
		cobra_pcrm->module_srst_req1 &= ~(1 << 2);	
}

/*
vim soft reset

*/
void cobra_pcrm_vim_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 3);
		cobra_pcrm->module_srst_req1 &= ~(1 << 3);	
}

/*
system bus  soft reset
include bus-matrix ,sram, sbus_m0_bridge

*/
void cobra_pcrm_system_bus_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 4);
		cobra_pcrm->module_srst_req1 &= ~(1 << 4);	
}

/*
ÍâÉè  soft reset
*/
void cobra_pcrm_perpherial_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 5);
		cobra_pcrm->module_srst_req1 &= ~(1 << 5);	
}

/*
mas  soft reset
*/

void cobra_pcrm_mas_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 6);
		cobra_pcrm->module_srst_req1 &= ~(1 << 6);	
}

/*
spi flash  soft reset
*/

void cobra_pcrm_spi_flash_soft_reset(void)
{
	  volatile unsigned int i;
		cobra_pcrm->module_srst_req1 |= (1 << 8);
	  for(i = 0; i < 1000; i++);
		cobra_pcrm->module_srst_req1 &= ~(1 << 8);	
}

/*
udma  soft reset
*/

void cobra_pcrm_udma_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 9);
		cobra_pcrm->module_srst_req1 &= ~(1 << 9);	
}


/*
sha  soft reset
*/

void cobra_pcrm_sha_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 10);
		cobra_pcrm->module_srst_req1 &= ~(1 << 10);	
}

/*
chache  soft reset
*/

void cobra_pcrm_cache_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 14);
		cobra_pcrm->module_srst_req1 &= ~(1 << 14);	
}

/*
eflash  soft reset
*/

void cobra_pcrm_eflash_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 15);
		cobra_pcrm->module_srst_req1 &= ~(1 << 15);	
}

/*
pinmux  soft reset
*/

void cobra_pcrm_pinmux_soft_reset(void)
{
		cobra_pcrm->module_srst_req1 |= (1 << 16);
		cobra_pcrm->module_srst_req1 &= ~(1 << 16);	
}

/*
icytrx  soft reset
*/

//void cobra_pcrm_icytrx_soft_reset(void)
//{
//		cobra_pcrm->module_srst_req1 |= (1 << 23);
//		cobra_pcrm->module_srst_req1 &= ~(1 << 23);	
//}


/*
uart0  soft reset
*/

void cobra_pcrm_uart0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 0);
		cobra_pcrm->module_srst_req2 &= ~(1 << 0);	
}

/*
uart1  soft reset
*/

void cobra_pcrm_uart1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 1);
		cobra_pcrm->module_srst_req2 &= ~(1 << 1);	
}

/*
mpwm  soft reset
*/

void cobra_pcrm_mpwm_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 2);
		cobra_pcrm->module_srst_req2 &= ~(1 << 2);	
}

/*
spwm  soft reset
*/

void cobra_pcrm_spwm_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 3);
		cobra_pcrm->module_srst_req2 &= ~(1 << 3);	
}

/*
spi0  soft reset
*/

void cobra_pcrm_spi0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 4);
		cobra_pcrm->module_srst_req2 &= ~(1 << 4);	
}


/*
i2c0  soft reset
*/

void cobra_pcrm_i2c0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 6);
		cobra_pcrm->module_srst_req2 &= ~(1 << 6);	
}

/*
i2c1  soft reset
*/

void cobra_pcrm_i2c1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 7);
		cobra_pcrm->module_srst_req2 &= ~(1 << 7);	
}

/*
qdec  soft reset
*/

void cobra_pcrm_qdec_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 9);
		cobra_pcrm->module_srst_req2 &= ~(1 << 9);	
}

/*
irda  soft reset
*/

void cobra_pcrm_irda_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 10);
		cobra_pcrm->module_srst_req2 &= ~(1 << 10);	
}

/*
dvad  soft reset
*/

void cobra_pcrm_dvad_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 11);
		cobra_pcrm->module_srst_req2 &= ~(1 << 11);	
}

/*
aes0  soft reset
*/

void cobra_pcrm_aes0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 16);
		cobra_pcrm->module_srst_req2 &= ~(1 << 16);	
}

/*
trng  soft reset
*/

void cobra_pcrm_trng_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 17);
		cobra_pcrm->module_srst_req2 &= ~(1 << 17);	
}

/*
timer0  soft reset
*/

void cobra_pcrm_timer0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 18);
		cobra_pcrm->module_srst_req2 &= ~(1 << 18);	
}

/*
timer1  soft reset
*/

void cobra_pcrm_timer1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 19);
		cobra_pcrm->module_srst_req2 &= ~(1 << 19);	
}

/*
dualtimers  soft reset
*/

void cobra_pcrm_dualtimers_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 20);
		cobra_pcrm->module_srst_req2 &= ~(1 << 20);	
}

/*
spi1  soft reset
*/

void cobra_pcrm_spi1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 22);
		cobra_pcrm->module_srst_req2 &= ~(1 << 22);	
}

/*
pdm0  soft reset
*/

void cobra_pcrm_pdm0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 24);
		cobra_pcrm->module_srst_req2 &= ~(1 << 24);	
}

/*
pdm1  soft reset
*/

void cobra_pcrm_pdm1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 25);
		cobra_pcrm->module_srst_req2 &= ~(1 << 25);	
}

/*
i2s0  soft reset
*/

void cobra_pcrm_i2s0_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 26);
		cobra_pcrm->module_srst_req2 &= ~(1 << 26);	
}

/*
i2s1  soft reset
*/

void cobra_pcrm_i2s1_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 27);
		cobra_pcrm->module_srst_req2 &= ~(1 << 27);	
}

/*
ecc  soft reset
*/

void cobra_pcrm_ecc_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 28);
		cobra_pcrm->module_srst_req2 &= ~(1 << 28);	
}

/*
gpadc  soft reset
*/

void cobra_pcrm_gpadc_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 29);
		cobra_pcrm->module_srst_req2 &= ~(1 << 29);	
}

/*
audio  soft reset
*/

void cobra_pcrm_audio_soft_reset(void)
{
		cobra_pcrm->module_srst_req2 |= (1 << 30);
		cobra_pcrm->module_srst_req2 &= ~(1 << 30);	
}

//for cobra4 2019/8/5
//PLL control

//-------------------------------------------------------------------
void cobra_pcrm_set_pll_cnt_judge(unsigned int judge)
{
	cobra_pcrm->PLL_PARAM_CTRL0 &= ~0x3ff;
	cobra_pcrm->PLL_PARAM_CTRL0 |= (judge&0x3ff);
}

void cobra_pcrm_apll_control_status_clr(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->PLL_PARAM_CTRL0 |= 1 << 10;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL0 &= ~(1 << 10);
	}
}

void cobra_pcrm_set_try_num(unsigned char number)
{
	cobra_pcrm->PLL_PARAM_CTRL0 &= ~(0xf << 11);
	cobra_pcrm->PLL_PARAM_CTRL0 |= ((number&0x0f) << 11);
}

void cobra_pcrm_trim_start(unsigned int start)
{
	if(start)
	{
		cobra_pcrm->PLL_PARAM_CTRL0 |= 1<<15;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL0 &= ~(1<<15);
	}
}

void cobra_pcrm_ctrl_cnt_cfg(unsigned int cfg)
{
	cobra_pcrm->PLL_PARAM_CTRL0 &= ~(0x3f << 16);
	cobra_pcrm->PLL_PARAM_CTRL0 |= ((cfg&0x3f) << 16);
}

void cobra_pcrm_wait_trim_cycle(unsigned int cycle)
{
	cobra_pcrm->PLL_PARAM_CTRL0 &= ~(0xf << 22);
	cobra_pcrm->PLL_PARAM_CTRL0 |= ((cycle&0xf) << 22);
}

void cobra_pcrm_trim_gain(unsigned int gain)
{
	cobra_pcrm->PLL_PARAM_CTRL0 &= ~(0x3f << 26);
	cobra_pcrm->PLL_PARAM_CTRL0 |= ((gain&0x3f) << 26);
}

//////////////////////////////
void cobra_pcrm_trim_value(unsigned int value)
{
	cobra_pcrm->PLL_PARAM_CTRL1 &= ~0xff;
	cobra_pcrm->PLL_PARAM_CTRL1 |= (value & 0xff);
}

void cobra_pcrm_trim_cfg_update(unsigned int update)
{
	if(update)
	{
		cobra_pcrm->PLL_PARAM_CTRL1 |= 1<<8;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL1 &= ~(1<<8);
	}
}

void cobra_pcrm_trim_en_ctrl(unsigned int en)
{
	if(en)
	{
		cobra_pcrm->PLL_PARAM_CTRL1 |= 1<<9;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL1 &= ~(1<<9);
	}
}

void cobra_pcrm_trim_en_pll_cnt(unsigned int en)
{
	if(en)
	{
		cobra_pcrm->PLL_PARAM_CTRL1 |= 1<<10;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL1 &= ~(1<<10);
	}
}

void cobra_pcrm_step_value(unsigned int step_value)
{
	cobra_pcrm->PLL_PARAM_CTRL1 &= ~(0x1f<<11);
	cobra_pcrm->PLL_PARAM_CTRL1 |= ((step_value&0x1f) << 11);
}

void cobra_pcrm_lock_detect_en(unsigned int en)
{
	if(en)
	{
		cobra_pcrm->PLL_PARAM_CTRL1 |= 1<<16;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL1 &= ~(1<<16);
	}
}

void cobra_pcrm_lock_cfg(unsigned int lock_cfg)
{
	cobra_pcrm->PLL_PARAM_CTRL1 &= ~(0xff<<17);
	cobra_pcrm->PLL_PARAM_CTRL1 |= ((lock_cfg&0xff) << 17);
}

//¸ù¾Ýconey ¸Ä±ä

void cobra_pcrm_pll_freq_off(unsigned int pll_freq_off)
{
	cobra_pcrm->PLL_PARAM_CTRL2 &= ~0x1ff;
	cobra_pcrm->PLL_PARAM_CTRL2 |= pll_freq_off & 0x1ff;
}

//void cobra_pcrm_p_num(unsigned int p_num)
//{
//	cobra_pcrm->PLL_PARAM_CTRL2 &= ~(0xffff<<16);
//	cobra_pcrm->PLL_PARAM_CTRL2 |= ((p_num & 0xffff) << 16);
//}

void cobra_pcrm_fract_num(unsigned int fract_number)
{
	cobra_pcrm->PLL_FRACT_NUM = fract_number & 0x3ffffff;
}

void cobra_pcrm_p_num(unsigned int p_num)
{
	cobra_pcrm->PLL_PARAM_CTRL2 &= ~(0x7f << 16);
	cobra_pcrm->PLL_PARAM_CTRL2 |= ((p_num & 0x7f) << 16);
}

void cobra_pcrm_work_mode(unsigned int work_mode)
{
	cobra_pcrm->PLL_PARAM_CTRL2 &= ~(0x3 << 23);
	cobra_pcrm->PLL_PARAM_CTRL2 |= ((work_mode & 0x3) << 23);
}

void cobra_pcrm_update_cfg(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->PLL_PARAM_CTRL2 |= 1 << 25;
	}
	else
	{
		cobra_pcrm->PLL_PARAM_CTRL2 &= ~(1 << 25);
	}
}

#if COBRA_I2S1_EN
//--------------------------------I2S clk config-------------------------------
//I2S clk config
/*

tx_mclk = i2s_clk*(divX/divY)

*/
void cobra_pcrm_i2s1_tx_mode_mclk_div(unsigned int divX, unsigned int divY)
{
	cobra_pcrm->clk_tx_mdiv_x1_y1 &= ~0xff;
	cobra_pcrm->clk_tx_mdiv_x1_y1 |= divX & 0xff;

	cobra_pcrm->clk_tx_mdiv_x1_y1 &= ~(0xff << 16);
	cobra_pcrm->clk_tx_mdiv_x1_y1 |= ((divY & 0xff) << 16);
}

/*

rx_mclk = i2s_clk*(divX/divY)

*/
void cobra_pcrm_i2s1_rx_mode_mclk_div(unsigned int divX, unsigned int divY)
{
	cobra_pcrm->clk_rx_mdiv_x1_y1 &= ~0xff;
	cobra_pcrm->clk_rx_mdiv_x1_y1 |= divX & 0xff;

	cobra_pcrm->clk_rx_mdiv_x1_y1 &= ~(0xff << 16);
	cobra_pcrm->clk_rx_mdiv_x1_y1 |= ((divY & 0xff) << 16);
}

/*

tx_sclk = tx_mclk/div

0: div by 1, 1: div by 2


*/
void cobra_pcrm_i2s1_tx_mode_sclk_div(unsigned int div)
{
	cobra_pcrm->clk_tx_sdiv1 &= ~0x3f;
	cobra_pcrm->clk_tx_sdiv1 |= (div & 0x3f);
}

/*

rx_sclk = rx_mclk/div

0: div by 1, 1: div by 2


*/
void cobra_pcrm_i2s1_rx_mode_sclk_div(unsigned int div)
{
	cobra_pcrm->clk_tx_sdiv1 &= ~(0x3f << 16);
	cobra_pcrm->clk_tx_sdiv1 |= ((div & 0x3f) << 16);
}

//en = 1
/*
pad tx_mclk output enable

0:  pad tx_mclk input, 1: pad tx_mclk output 

*/
void cobra_pcrm_i2s1_pad_tx_mclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 3);
	}
	else
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 3);
	}
}

//en = 1
/*
pad tx_sclk output enable

0:  pad tx_sclk input, 1: pad tx_sclk output

*/
void cobra_pcrm_i2s1_pad_tx_sclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 5);
	}
	else
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 5);
	}
}

/*
"00: tx 48NHz fractional divider
//01: tx apll divider;in the case, when tx_mode[3] set to 1, tx_mode[4] should be set to 1;
10: tx_mclk_i, in the case, pad_tx_mclk set to input  , tx_mode[3] should be set to 0(tx_sck set to input)
11: tx_sck,in the case, pad_tx_sck set to input   , tx_mode[5] should be set to 0(tx_sck set to input)"

*/
void cobra_pcrm_i2s1_tx_work_mode(I2S_TX_WORK_MODE source)
{
	if (source == MODE_MCLK_COME_FROM_PCLK)
  {
     cobra_pcrm->tx_rx_mode1 &= ~0x07;
     cobra_pcrm->tx_rx_mode1 &= ~0x40;
     cobra_pcrm->tx_rx_mode1 &= ~0x10;
  }
  else if (source == MODE_MCLK_COM_FROM_APLLL)
  {
      cobra_pcrm->tx_rx_mode1 &= ~0x03;
      cobra_pcrm->tx_rx_mode1 &= ~0x40;
      cobra_pcrm->tx_rx_mode1 &= ~0x10;
      cobra_pcrm->tx_rx_mode1 |= 0x11;
  }

	else if (source == MODE_MCLK_COM_FROM_RX_MCLK)
	{
		cobra_pcrm->tx_rx_mode1 &= ~0x03;
		cobra_pcrm->tx_rx_mode1 &= ~0x40;
		cobra_pcrm->tx_rx_mode1 &= ~0x10;
		cobra_pcrm->tx_rx_mode1 |= 0x41;
	}
	else if (source == MODE_MCLK_COME_FROM_EXTERNAL_MCLK)
	{
		cobra_pcrm->tx_rx_mode1 &= ~0x03;
		cobra_pcrm->tx_rx_mode1 &= ~0x40;
		cobra_pcrm->tx_rx_mode1 &= ~0x10;
		cobra_pcrm->tx_rx_mode1 |= 0x02;
	}
	else if (source == MODE_SHARE_RX_SCLK_RX_WS)
	{
		cobra_pcrm->tx_rx_mode1 &= ~0x03;
		cobra_pcrm->tx_rx_mode1 &= ~0x40;
		cobra_pcrm->tx_rx_mode1 &= ~0x10;
		cobra_pcrm->tx_rx_mode1 |= 0x05;
	}
	else if (source == MODE_SLAVE)
	{
		cobra_pcrm->tx_rx_mode1 &= ~0x03;
		cobra_pcrm->tx_rx_mode1 &= ~0x40;
		cobra_pcrm->tx_rx_mode1 &= ~0x10;
		cobra_pcrm->tx_rx_mode1 |= 0x03;
	}
}

/*
tx_sclk phase control

0:  phase is 0, 1: phase is 180


*/
void cobra_pcrm_i2s1_tx_sclk_phase(I2S_PHASE phase)
{
	if (phase == PHASE_0)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 7);
	}
	else if (phase == PHASE_180)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 7);
	}
}

void cobra_pcrm_i2s1_transmitter_mode(I2S_TRANSMITTER_MODE work_mode)
{
	if (work_mode == MASTER_MODE_PCLK_NO_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_PCLK_WITH_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_SHARING_RX_MCLK)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_MCLK_COM_FROM_RX_MCLK);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == TYPICAL_SLAVE_MODE)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == TYPICAL_SLAVE_MODE_WITH_MCLK)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == SHARE_RXSCK_RX_WS)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_SHARE_RX_SCLK_RX_WS);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_FORM_APLL)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_MCLK_COM_FROM_APLLL);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_EXTERNAL_MCLK)
	{
		cobra_pcrm_i2s1_tx_work_mode(MODE_MCLK_COME_FROM_EXTERNAL_MCLK);
		cobra_pcrm_i2s1_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s1_tx_sclk_phase(PHASE_0);
	}
}

//************************************i2s1 RX mode config ***********************************
//en = 1
/*
pad tx_mclk output enable

0:  pad tx_mclk input, 1: pad tx_mclk output 

*/
void cobra_pcrm_i2s1_pad_rx_mclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 19);
	}
	else
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 19);
	}
}

//en = 1
/*
pad tx_sclk output enable

0:  pad tx_sclk input, 1: pad tx_sclk output

*/
void cobra_pcrm_i2s1_pad_rx_sclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 21);
	}
	else
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 21);
	}
}

/*
"00: tx 48NHz fractional divider
//01: tx apll divider;in the case, when tx_mode[3] set to 1, tx_mode[4] should be set to 1;
10: tx_mclk_i, in the case, pad_tx_mclk set to input  , tx_mode[3] should be set to 0(tx_sck set to input)
11: tx_sck,in the case, pad_tx_sck set to input   , tx_mode[5] should be set to 0(tx_sck set to input)"

*/
void cobra_pcrm_i2s1_rx_work_mode(I2S_TX_WORK_MODE source)
{
	if (source == MODE_MCLK_COME_FROM_PCLK)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x07 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
	}
	else if (source == MODE_MCLK_COM_FROM_APLLL)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode1 |= (0x11 << 16);
	}
	else if (source == MODE_MCLK_COM_FROM_RX_MCLK)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode1 |= (0x41 << 16);
	}
	else if (source == MODE_MCLK_COME_FROM_EXTERNAL_MCLK)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode1 |= (0x02 << 16);
	}
	else if (source == MODE_SHARE_RX_SCLK_RX_WS)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode1 |= (0x05 << 16);
	}
	else if (source == MODE_SLAVE)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode1 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode1 |= (0x03 << 16);
	}
}

/*
tx_sclk phase control

0:  phase is 0, 1: phase is 180


*/
void cobra_pcrm_i2s1_rx_sclk_phase(I2S_PHASE phase)
{
	if (phase == PHASE_0)
	{
		cobra_pcrm->tx_rx_mode1 &= ~(1 << 23);
	}
	else if (phase == PHASE_180)
	{
		cobra_pcrm->tx_rx_mode1 |= (1 << 23);
	}
}

void cobra_pcrm_i2s1_receiver_mode(I2S_RECEIVER_MODE work_mode)
{
	if (work_mode == RECEIVER_MODE_PCLK_NO_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MODE_SHARING_RX_MCLK)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_MCLK_COM_FROM_RX_MCLK);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_TYPICAL_SLAVE_MODE)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_SHARE_RXSCK_RX_WS)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_SHARE_RX_SCLK_RX_WS);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MASTER_MODE_FORM_APLL)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_MCLK_COM_FROM_APLLL);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MASTER_MODE_EXTERNAL_MCLK)
	{
		cobra_pcrm_i2s1_rx_work_mode(MODE_MCLK_COME_FROM_EXTERNAL_MCLK);
		cobra_pcrm_i2s1_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s1_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s1_rx_sclk_phase(PHASE_0);
	}
}


#endif


#if COBRA_I2S0_EN
//--------------------------------I2S clk config-------------------------------
//I2S clk config
/*

tx_mclk = i2s_clk*(divX/divY)

*/
void cobra_pcrm_i2s0_tx_mode_mclk_div(unsigned int divX, unsigned int divY)
{
	cobra_pcrm->clk_tx_mdiv_x0_y0 &= ~0xff;
	cobra_pcrm->clk_tx_mdiv_x0_y0 |= divX & 0xff;

	cobra_pcrm->clk_tx_mdiv_x0_y0 &= ~(0xff << 16);
	cobra_pcrm->clk_tx_mdiv_x0_y0 |= ((divY & 0xff) << 16);
}

/*

rx_mclk = i2s_clk*(divX/divY)

*/
void cobra_pcrm_i2s0_rx_mode_mclk_div(unsigned int divX, unsigned int divY)
{
	cobra_pcrm->clk_rx_mdiv_x0_y0 &= ~0xff;
	cobra_pcrm->clk_rx_mdiv_x0_y0 |= divX & 0xff;

	cobra_pcrm->clk_rx_mdiv_x0_y0 &= ~(0xff << 16);
	cobra_pcrm->clk_rx_mdiv_x0_y0 |= ((divY & 0xff) << 16);
}

/*

tx_sclk = tx_mclk/div

0: div by 1, 1: div by 2


*/
void cobra_pcrm_i2s0_tx_mode_sclk_div(unsigned int div)
{
	cobra_pcrm->clk_tx_sdiv0 &= ~0x3f;
	cobra_pcrm->clk_tx_sdiv0 |= (div & 0x3f);
}

/*

rx_sclk = rx_mclk/div

0: div by 1, 1: div by 2


*/
void cobra_pcrm_i2s0_rx_mode_sclk_div(unsigned int div)
{
	cobra_pcrm->clk_tx_sdiv0 &= ~(0x3f << 16);
	cobra_pcrm->clk_tx_sdiv0 |= ((div & 0x3f) << 16);
}

//en = 1
/*
pad tx_mclk output enable

0:  pad tx_mclk input, 1: pad tx_mclk output 

*/
void cobra_pcrm_i2s0_pad_tx_mclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 3);
	}
	else
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 3);
	}
}

//en = 1
/*
pad tx_sclk output enable

0:  pad tx_sclk input, 1: pad tx_sclk output

*/
void cobra_pcrm_i2s0_pad_tx_sclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 5);
	}
	else
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 5);
	}
}

/*
"00: tx 48NHz fractional divider
//01: tx apll divider;in the case, when tx_mode[3] set to 1, tx_mode[4] should be set to 1;
10: tx_mclk_i, in the case, pad_tx_mclk set to input  , tx_mode[3] should be set to 0(tx_sck set to input)
11: tx_sck,in the case, pad_tx_sck set to input   , tx_mode[5] should be set to 0(tx_sck set to input)"

*/
void cobra_pcrm_i2s0_tx_work_mode(I2S_TX_WORK_MODE source)
{
	if (source == MODE_MCLK_COME_FROM_PCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x07;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
	}
	else if (source == MODE_MCLK_COM_FROM_APLLL)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x03;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
		cobra_pcrm->tx_rx_mode0 |= 0x11;
	}
	else if (source == MODE_MCLK_COM_FROM_RX_MCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x03;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
		cobra_pcrm->tx_rx_mode0 |= 0x41;
	}
	else if (source == MODE_MCLK_COME_FROM_EXTERNAL_MCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x03;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
		cobra_pcrm->tx_rx_mode0 |= 0x02;
	}
	else if (source == MODE_SHARE_RX_SCLK_RX_WS)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x03;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
		cobra_pcrm->tx_rx_mode0 |= 0x05;
	}
	else if (source == MODE_SLAVE)
	{
		cobra_pcrm->tx_rx_mode0 &= ~0x03;
		cobra_pcrm->tx_rx_mode0 &= ~0x40;
		cobra_pcrm->tx_rx_mode0 &= ~0x10;
		cobra_pcrm->tx_rx_mode0 |= 0x03;
	}
}

/*
tx_sclk phase control

0:  phase is 0, 1: phase is 180


*/
void cobra_pcrm_i2s0_tx_sclk_phase(I2S_PHASE phase)
{
	if (phase == PHASE_0)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 7);
	}
	else if (phase == PHASE_180)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 7);
	}
}

void cobra_pcrm_i2s0_transmitter_mode(I2S_TRANSMITTER_MODE work_mode)
{
	if (work_mode == MASTER_MODE_PCLK_NO_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_PCLK_WITH_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_SHARING_RX_MCLK)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COM_FROM_RX_MCLK);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == TYPICAL_SLAVE_MODE)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == TYPICAL_SLAVE_MODE_WITH_MCLK)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == SHARE_RXSCK_RX_WS)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_SHARE_RX_SCLK_RX_WS);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_FORM_APLL)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COM_FROM_APLLL);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(1);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
	else if (work_mode == MASTER_MODE_EXTERNAL_MCLK)
	{
		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COME_FROM_EXTERNAL_MCLK);
		cobra_pcrm_i2s0_pad_tx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_tx_mclk_output_enable(0);
		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_0);
	}
}

//************************************i2s1 RX mode config ***********************************
//en = 1
/*
pad tx_mclk output enable

0:  pad tx_mclk input, 1: pad tx_mclk output 

*/
void cobra_pcrm_i2s0_pad_rx_mclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 19);
	}
	else
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 19);
	}
}

//en = 1
/*
pad tx_sclk output enable

0:  pad tx_sclk input, 1: pad tx_sclk output

*/
void cobra_pcrm_i2s0_pad_rx_sclk_output_enable(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 21);
	}
	else
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 21);
	}
}

/*
"00: tx 48NHz fractional divider
//01: tx apll divider;in the case, when tx_mode[3] set to 1, tx_mode[4] should be set to 1;
10: tx_mclk_i, in the case, pad_tx_mclk set to input  , tx_mode[3] should be set to 0(tx_sck set to input)
11: tx_sck,in the case, pad_tx_sck set to input   , tx_mode[5] should be set to 0(tx_sck set to input)"

*/
void cobra_pcrm_i2s0_rx_work_mode(I2S_TX_WORK_MODE source)
{
	if (source == MODE_MCLK_COME_FROM_PCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x07 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
	}
	else if (source == MODE_MCLK_COM_FROM_APLLL)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode0 |= (0x11 << 16);
	}
	else if (source == MODE_MCLK_COM_FROM_RX_MCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode0 |= (0x41 << 16);
	}
	else if (source == MODE_MCLK_COME_FROM_EXTERNAL_MCLK)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode0 |= (0x02 << 16);
	}
	else if (source == MODE_SHARE_RX_SCLK_RX_WS)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode0 |= (0x05 << 16);
	}
	else if (source == MODE_SLAVE)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(0x03 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x40 << 16);
		cobra_pcrm->tx_rx_mode0 &= ~(0x10 << 16);
		cobra_pcrm->tx_rx_mode0 |= (0x03 << 16);
	}
}

/*
tx_sclk phase control

0:  phase is 0, 1: phase is 180


*/
void cobra_pcrm_i2s0_rx_sclk_phase(I2S_PHASE phase)
{
	if (phase == PHASE_0)
	{
		cobra_pcrm->tx_rx_mode0 &= ~(1 << 23);
	}
	else if (phase == PHASE_180)
	{
		cobra_pcrm->tx_rx_mode0 |= (1 << 23);
	}
}

void cobra_pcrm_i2s0_receiver_mode(I2S_RECEIVER_MODE work_mode)
{
	if (work_mode == RECEIVER_MODE_PCLK_NO_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_MCLK_COME_FROM_PCLK);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MODE_SHARING_RX_MCLK)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_MCLK_COM_FROM_RX_MCLK);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_TYPICAL_SLAVE_MODE)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_SLAVE);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_SHARE_RXSCK_RX_WS)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_SHARE_RX_SCLK_RX_WS);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(0);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MASTER_MODE_FORM_APLL)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_MCLK_COM_FROM_APLLL);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(1);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
	else if (work_mode == RECEIVER_MASTER_MODE_EXTERNAL_MCLK)
	{
		cobra_pcrm_i2s0_rx_work_mode(MODE_MCLK_COME_FROM_EXTERNAL_MCLK);
		cobra_pcrm_i2s0_pad_rx_sclk_output_enable(1);
		cobra_pcrm_i2s0_pad_rx_mclk_output_enable(0);
		cobra_pcrm_i2s0_rx_sclk_phase(PHASE_0);
	}
}

#endif

void cobra_pcrm_sram0_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->sram_clk_en |= 0x01;
		}
		else
		{
				cobra_pcrm->sram_clk_en &= ~0x01;
		}
}
	
void cobra_pcrm_sram1_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->sram_clk_en |= 0x02;
		}
		else
		{
				cobra_pcrm->sram_clk_en &= ~0x02;
		}
}

void cobra_pcrm_sram2_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->sram_clk_en |= 0x04;
		}
		else
		{
				cobra_pcrm->sram_clk_en &= ~0x04;
		}
}

void cobra_pcrm_sram3_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->sram_clk_en |= 0x08;
		}
		else
		{
				cobra_pcrm->sram_clk_en &= ~0x08;
		}
}

void cobra_pcrm_spwm_clk_div(unsigned int divider)
{
		volatile unsigned int temp;
		temp = cobra_pcrm->spwm_clk_div_dualtimers_clk_div ;
		temp &= ~0x0f;
		temp |=  divider&0x0f;
		cobra_pcrm->spwm_clk_div_dualtimers_clk_div  = temp;
	
//		cobra_pcrm->spwm_clk_div_dualtimers_clk_div &= ~0x0f;
//		cobra_pcrm->spwm_clk_div_dualtimers_clk_div |=  divider&0x0f;
}

void cobra_pcrm_dualtimers_clk_div(unsigned int divider)
{
		volatile unsigned int temp;
		temp = cobra_pcrm->spwm_clk_div_dualtimers_clk_div ;
		temp &= ~(0x01f << 4);
		temp |=   ((divider&0x1f) << 4);
		cobra_pcrm->spwm_clk_div_dualtimers_clk_div  = temp;
	
//		cobra_pcrm->spwm_clk_div_dualtimers_clk_div &= ~(0x01f << 4);
//		cobra_pcrm->spwm_clk_div_dualtimers_clk_div |=  ((divider&0x1f) << 4);
}

void cobra_pcrm_irda_high_cnt_div(unsigned int divider)
{
		volatile unsigned int temp;
		temp = cobra_pcrm->irda_high_low_cnt_div ;
		temp &= ~(0xffff);
		temp |=   ((divider&0xffff));
		cobra_pcrm->irda_high_low_cnt_div = temp;	
}

void cobra_pcrm_irda_low_cnt_div(unsigned int divider)
{
		volatile unsigned int temp;
		temp = cobra_pcrm->irda_high_low_cnt_div ;
		temp &= ~(0xffff << 16);
		temp |=  ((divider&0xffff) << 16);
		cobra_pcrm->irda_high_low_cnt_div = temp;	
}


void cobra_pcrm_i2s0_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->i2s0_i2s1_clk_en |= 0x01;
		}
		else
		{
				cobra_pcrm->i2s0_i2s1_clk_en &= ~0x01;
		}
}

//lianbo 2021/2/5
void cobra_pcrm_i2s1_clk_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->i2s0_i2s1_clk_en |= (0x01 << 16);
	}
	else
	{
		cobra_pcrm->i2s0_i2s1_clk_en &= ~(0x01 << 16);
	}   

}


void cobra_pcrm_m3_lockup_reset(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->m3_lockup_reset |= 0x01;
		}
		else
		{
				cobra_pcrm->m3_lockup_reset &= ~0x01;
		}
}

//unsigned int cobra_pcrm_whether_m3_system_soft_reset(void)
//{
//		if(cobra_pcrm->resetinfo & 0x01)
//				return 1;
//		else	
//				return 0;
//}

///*
//if en = 1, if cpu lockup, then reset CPU

//*/
//unsigned int cobra_pcrm_whether_m3_system_lockup_reset(void)
//{
//		if(cobra_pcrm->resetinfo & 0x02)
//				return 1;
//		else	
//				return 0;
//}

//unsigned int cobra_pcrm_whether_m3_system_watchdog_reset(void)
//{
//		if(cobra_pcrm->resetinfo & 0x04)
//				return 1;
//		else	
//				return 0;
//}

/*

icytrx module isolate control, 0: isolate 0, 1: icytrx signal enable. 

"open opeartion:
1: turn on icytrx power,
2: waiting 80us(?)
3: set icytrx_iso0 to 1
close operation:
1: set icytrx_iso0 to 0
2: turn off icytrx power"
*/

//void cobra_pcrm_icytrx_isolate_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_pcrm->icytrx_iso0 &= ~(1 << 16);
//		}
//		else
//		{
//				cobra_pcrm->icytrx_iso0 |= (1 << 16);
//		}
//	
//}

#if COBRA_MCU_IS_VER1
void	cobra_pcrm_ldoadc_isolate_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->icytrx_iso0 &= ~(1 << 17);
		}
		else
		{
				cobra_pcrm->icytrx_iso0 |= (1 << 17);
		}
}

void cobra_pcrm_ldopll_isolate_en(unsigned int en)
{
		if(en)
		{
				cobra_pcrm->icytrx_iso0 &= ~(1 << 18);
		}
		else
		{
				cobra_pcrm->icytrx_iso0 |= (1 << 18);
		}
	
}
#endif
//PLL
//d2a_pll_cp_en
void cobra_pcrm_d2a_pll_cp_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= 0x01;
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~0x01;
	}
}

//d2a_pll_cp_iupcom
void cobra_pcrm_d2a_pll_cp_iupcom(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 1);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 1);
	}
}

//d2a_pll_cp_sw
void cobra_pcrm_d2a_pll_cp_sw(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 2);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 2);
	}
}

//d2a_pll_en
void cobra_pcrm_d2a_pll_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 3);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 3);
	}
}

//d2a_pll_lct_en
void cobra_pcrm_d2a_pll_lct_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 4);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 4);
	}
}

//d2a_pll_lpf_sms
void cobra_pcrm_d2a_pll_lpf_sms(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 5);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 5);
	}
}

//d2a_pll_pfd_en
void cobra_pcrm_d2a_pll_pfd_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 6);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 6);
	}
}

//d2a_pll_pfd_ms
void cobra_pcrm_d2a_pll_pfd_ms(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 7);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 7);
	}
}

//d2a_pll_retimer_en
void cobra_pcrm_d2a_pll_retimer_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 8);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 8);
	}
}

//d2a_pll_test_en
void cobra_pcrm_d2a_pll_test_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 9);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 9);
	}
}

//d2a_pll_vco_en
void cobra_pcrm_d2a_pll_vco_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->apll_cfg1 |= (0x01 << 10);
	}
	else
	{
		cobra_pcrm->apll_cfg1 &= ~(0x01 << 10);
	}
}

//d2a_pll_lpf_c1<0>
void cobra_pcrm_d2a_pll_lpf_c1(unsigned int value)
{
	volatile unsigned int temp;

	temp = cobra_pcrm->apll_cfg1;
	temp &= ~(0x0f << 11);
	temp |= ((value & 0x0f) << 11);
	cobra_pcrm->apll_cfg1 = temp;
}

//d2a_pll_vco_vbias<0>
void cobra_pcrm_d2a_pll_vco_vbias(unsigned int value)
{
	volatile unsigned int temp;

	temp = cobra_pcrm->apll_cfg1;
	temp &= ~(0x07 << 16);
	temp |= ((value & 0x07) << 16);
	cobra_pcrm->apll_cfg1 = temp;
}

//d2a_pll_pfd_delay<0>
void cobra_pcrm_d2a_pll_pfd_delay(unsigned int value)
{
	volatile unsigned int temp;

	temp = cobra_pcrm->apll_cfg1;
	temp &= ~(0x3f << 19);
	temp |= ((value & 0x3f) << 19);
	cobra_pcrm->apll_cfg1 = temp;
}

//d2a_pll_lpf_c2<0>
void cobra_pcrm_d2a_pll_lpf_c2(unsigned int value)
{
	volatile unsigned int temp;

	temp = cobra_pcrm->apll_cfg1;
	temp &= ~(0x0f << 25);
	temp |= ((value & 0x0f) << 25);
	cobra_pcrm->apll_cfg1 = temp;
}

//d2a_pll_cp_idac<0>
void cobra_pcrm_d2a_pll_cp_idac(unsigned int value)
{
	volatile unsigned int temp;

	temp = cobra_pcrm->apll_cfg1;
	temp &= ~(0x03 << 29);
	temp |= ((value & 0x03) << 29);
	cobra_pcrm->apll_cfg1 = temp;
}

void cobra_pcrm_write_apll_cfg1(unsigned int value)
{
	cobra_pcrm->apll_cfg1 = value;
}

void cobra_pcrm_write_apll_cfg2(unsigned int value)
{
	cobra_pcrm->PLL_PARAM_CTRL0 = value;
}

void cobra_pcrm_write_apll_cfg3(unsigned int value)
{
	cobra_pcrm->PLL_PARAM_CTRL1 = value;
}

void cobra_pcrm_write_apll_cfg4(unsigned int value)
{
	cobra_pcrm->PLL_PARAM_CTRL2 = value;
}

//Ä£Äâ²¿·Ö
//ana_cfg0

//d2a_vdac_en
void cobra_pcrm_d2a_vdac_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= 0x01;
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~0x01;
	}
}

//d2a_cpa_en
void cobra_pcrm_d2a_cpa_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 1);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 1);
	}
}

//d2a_cpb_en
void cobra_pcrm_d2a_cpb_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 2);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 2);
	}
}

//d2a_vdd3_adc_en
void cobra_pcrm_d2a_vdd3_adc_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 3);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 3);
	}
}

//d2a_ldodig_pd0_high_curr
void cobra_pcrm_d2a_ldodig_pd0_high_curr_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 4);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 4);
	}
}

//d2a_ldodig_pd0_vreg_trim<0>
void cobra_pcrm_d2a_ldodig_pd0_vreg_trim(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->ana_cfg0;
	temp &= ~(0x0f << 5);
	temp |= ((value & 0x0f) << 5);
	cobra_pcrm->ana_cfg0 = temp;
}

//d2a_ldodig_pd1_high_curr
void cobra_pcrm_d2a_ldodig_pd1_high_curr_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 9);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 9);
	}
}

//d2a_ldodig_pd1_vreg_trim<0>
void cobra_pcrm_d2a_ldodig_pd1_vreg_trim(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->ana_cfg0;
	temp &= ~(0x0f << 10);
	temp |= ((value & 0x0f) << 10);
	cobra_pcrm->ana_cfg0 = temp;
}

//d2a_efbrwot_en
void cobra_pcrm_d2a_efbrwot_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 14);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 14);
	}
}

//d2a_bgln_t_trim<0>
void cobra_pcrm_d2a_bgln_t_trim(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->ana_cfg0;
	temp &= ~(0x0f << 16);
	temp |= ((value & 0x0f) << 16);
	cobra_pcrm->ana_cfg0 = temp;
}

//d2a_bgln_vref_trim<0>
void cobra_pcrm_d2a_bgln_vref_trim(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->ana_cfg0;
	temp &= ~(0x1f << 20);
	temp |= ((value & 0x1f) << 20);
	cobra_pcrm->ana_cfg0 = temp;
}

//d2a_ldoanaforpll_high_curr
void cobra_pcrm_d2a_ldoanaforpll_high_curr_en(unsigned int en)
{
	if (en)
	{
		cobra_pcrm->ana_cfg0 |= (0x01 << 25);
	}
	else
	{
		cobra_pcrm->ana_cfg0 &= ~(0x01 << 25);
	}
}

//d2a_ldoanaforpll_trim<0>
void cobra_pcrm_d2a_ldoanaforpll_trim(unsigned int value)
{
	volatile unsigned int temp;
	temp = cobra_pcrm->ana_cfg0;
	temp &= ~(0x1f << 26);
	temp |= ((value & 0x1f) << 26);
	cobra_pcrm->ana_cfg0 = temp;
}

//pll_cnt_save[0]
unsigned int cobra_pcrm_pll_cnt_save(void)
{
	return (cobra_pcrm->apll_status & 0x3ff);
}

unsigned int cobra_pcrm_pll_trim_end(void)
{
	if (cobra_pcrm->apll_status & (1 << 10))
		return 1;
	else
		return 0;
}

unsigned int cobra_pcrm_pll_trim_suc(void)
{
	if (cobra_pcrm->apll_status & (1 << 11))
		return 1;
	else
		return 0;
}

//trim_out_o[0]

unsigned int cobra_pcrm_trim_out_o(void)
{
	return ((cobra_pcrm->apll_status >> 16) & 0xff);
}

//try_cnt[0]
unsigned int cobra_pcrm_try_cnt(void)
{
	return ((cobra_pcrm->apll_status >> 24) & 0x0f);
}

void cobra4_pll_debug_information(void)
{
	printf("0x9C: 0x%x \n", cobra_pcrm->apll_cfg1);
	printf("0xA0: 0x%x \n", cobra_pcrm->PLL_PARAM_CTRL0);
	printf("0xA4: 0x%x \n", cobra_pcrm->PLL_PARAM_CTRL1);
	printf("0xA8: 0x%x \n", cobra_pcrm->PLL_PARAM_CTRL2);
	printf("0x100: 0x%x \n", cobra_pcrm->ana_cfg0);
	printf("0x138: 0x%x \n", cobra_pcrm->apll_status);
}

#if (COBRA_POWER_DOWN_ENABLE)
void cobra_pcrm_sleep_mode_clock1_enable(unsigned int en)
{
		cobra_pcrm->module_clock1_sleep_ctrl_en = en;	
}
#endif






#endif














