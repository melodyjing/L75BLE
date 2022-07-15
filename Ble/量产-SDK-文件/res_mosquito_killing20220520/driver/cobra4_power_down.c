/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_power_down.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/5/17

	Description :   


	Changed Log	:

		Liujian  2021/5/17		- Creation

*****************************************************************************/
#include "cobra4_power_down.h"
#include <stdbool.h>
#include "cobra_system.h"
#include "cobra_uart.h"
#include "cobra_timer.h"
#if (COBRA_POWER_DOWN_ENABLE)

#include "rf.h"
#if (USE_LIB_ENABLE == 1)
volatile bool wakeupFlag     __attribute__((section("retention_area"),zero_init));
volatile bool ble_deep_sleep __attribute__((section("retention_area"),zero_init));
#else
extern volatile bool wakeupFlag, ble_deep_sleep;
#endif

extern void pop_ble_reg(void);
extern void rf_init_apb(void);
extern void isdeep_sleep(void);

extern volatile bool ble_deep_sleep;
extern void cobra_pcrm_sleep_mode_clock1_enable(unsigned int en);

static void cobra_system_m4_init_pd(void)
{
	volatile int i;

	cobra4_systemInit();

	cobra4_aon_pd_top_protection_en(1);
	cobra4_aon_pd_top_rtc0_pclk_en(1); //rtc pclk
	cobra4_aon_pd_top_rtc0_clk_en(1);  //rtc clk
//	cobra4_aon_pd_top_rdw_rf_pclk_en(1);
	cobra4_aon_pd_top_csem_rf_pclk_en(1);
	cobra4_aon_pd_top_ucpu_pclk_en(1);
	cobra4_aon_pd_top_ucpu_mask_en(1);
	cobra4_aon_pd_top_dualtimers_pclk_en(1);
	cobra4_aon_pd_top_watchdog_pclk_en(1);
	cobra4_aon_pd_top_dcdc_pmu_pclk_en(1);
	cobra4_aon_pd_top_pinmux_pclk_en(1);
	cobra4_aon_pd_top_an_low_power_clk_gate_en(1);
	cobra4_aon_pd_top_protection_en(0);

//	cobra_pcrm_uart1_active_mode_clock_enable(1);
	cobra_pcrm_pdm0_active_mode_clock_enable(1);
	cobra_pcrm_pdm1_active_mode_clock_enable(1);
//	cobra_pcrm_i2c0_active_mode_clock_enable(1);

//	cobra_pcrm_apll_active_mode_clock_enable(1);
	cobra_pcrm_sleep_mode_clock1_enable(0x0005C89D);
//	cobra_pcrm_sleep_mode_clock2_enable(0xffffffff);
//	cobra_pcrm_deep_sleep_mode_clock1_enable(0xffffffff);
//	cobra_pcrm_deep_sleep_mode_clock2_enable(0xffffffff);

	cobra_pcrm_p1_power_ctrl(1);
	cobra_pcrm_p2_power_ctrl(1);


	cobra_cache_normal_mode_enable(1); //cache enable

#if (!HZ32000)	
	//开启32K  时钟
	cobra4_aon_wk_protection_en(1);
	//cobra4_aon_wk_aon_rcosc32k_en(1);
	cobra4_aon_wk_aon_xtal32k_en(1);
	//cobra4_aon_wk_aon_rcosc24m_en(1);

	cobra4_aon_wk_aon_rdw_clk_selection(rdw_CLK_XTAL32K);

	cobra4_aon_wk_protection_en(0);
	cobra4_aon_pd_top_protection_en(1);
	cobra4_aon_pd_top_lf_clock_selection(LF_XTAL_32K);

	cobra4_aon_pd_top_xtal32_trim(0x0f);
	cobra4_aon_pd_top_protection_en(0);
#endif

	if(ble_deep_sleep==false)
		cobra_ceva_baseband_csem_init();
	
	cobra_system_switch_to_48m(1); //开启48M 晶振
	// cobra_spi_flash_set_rate(RATE_4MHZ);      //SPI flash need to modified to 4M, because the system clock
	cobra_system_clock_setting(); //分频设定
								  //	 cobra_spi_flash_set_rate_hz(4000000);
  cobra_uart_clear_tx_buffer(M3_UART0);
	cobra4_aon_pd_top_protection_en(1);
	cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);
	cobra4_aon_pd_top_aon_pclk_divider(0);
	cobra_system_set_m3_apb0_clk(4000000);
	cobra4_aon_pd_top_protection_en(0);
	

	cobra_system_uart_init();

	cobra4_d_vad_send_sigal_to_ppu(0);
	cobra4_d_vad_bt_clock_sel(24);
#if  (RDW_CGF_NVDS_SRAM)
#if (COBRA_POWER_DOWN_ENABLE==1)	
	if(ble_deep_sleep==false)
#endif	
	cobra_nvds_init();
#endif
}



void app_poweron_lowpower_init(void)
{
	  REG_BLE_WR(COBRA_PCRM_BASE+0x7C,0x0005C89D);  //open some important active clk firt for mcu_power_down

#if	(USE_LIB_ENABLE==1)
		wakeupFlag = 0;
		isdeep_sleep();	
#endif	  
	  //各种Driver 的初始化, 让变量指向各个driver 的初始地址
    if(ble_deep_sleep==0)
		{
	     cobra_system_init();
    }
		
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_aon_hclk_divider(0);
		cobra4_aon_pd_top_protection_en(0);
		
		cobra_system_m4_init_pd();
		
    if(ble_deep_sleep == false)
		{
	     app_m3_gpio_init();
		}
}

int app_cobra4_power_down_process(void)
{
	if(ble_deep_sleep==true)
	 {
	      while(ip_deepslstat_get()!=ip_deepslwkup_get());

		  
#if COBRA_UDMA_EN					
          while(cobra_udma_get_channel_mode(0) != 0);                           
          while(cobra_udma_get_channel_mode(1) != 0);                                   
#endif					
          while(wakeupFlag==0)
		      {
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
						__nop();
		    }
		    return  1;
	}	
	return 0;
	
}

void app_cobra4_power_down_process_48m(void)
{
	     if(ble_deep_sleep==true)
		   {				
#if (!FPGA)			
			     REG_BLE_WR(ICYV2_BASE+0xb4,0xc3841580);		//hpb
#else			
			     rf_icyv2_reg_wr_32(0xb4,0xc3841580);		 
#endif			
				   rf_init_apb();
				   pop_ble_reg();
		   }
		   else
			 {
					 cobra_ceva_baseband_icytrx_xtal_48m_enable(1);
			 }
			 
			 while(!(REG_BLE_RD(ICYV2_BASE+0x12C)&0x0800));
		   while((cobra_uart_get_interrupt_statue(M3_UART0)&RDW_UART_TX_EMPTY) != RDW_UART_TX_EMPTY);
	
}

#if LOW_POWER_WAKE_UP_BY_UART

void cobra_low_power_timer0_init(void)
{
	  cobra_pcrm_timer0_active_mode_clock_enable(1);
	  cobra_pcrm_timer0_sleep_mode_clock_enable(1);

	  cobra_timer_interrupt_set(APB2_TIMER0, 1);	//Enale interrupt

	  NVIC_SetPriority((IRQn_Type)COBRA4_TIMER0_IRQn, 2);
	  NVIC_ClearPendingIRQ((IRQn_Type)COBRA4_TIMER0_IRQn);
	  NVIC_EnableIRQ((IRQn_Type)COBRA4_TIMER0_IRQn);
}

void  cobra_low_power_timer0_start(void)
{
	  volatile COBRA_TIMER_TypeDef * p_cobra_apb2_timer0 = cobra_timer_get_apb2_timer0();
	
	  rdw_timer_set_value(p_cobra_apb2_timer0, cobra_system_get_m3_apb2_clk()/300);
	
	  cobra_timer_enable_set(APB2_TIMER0, 1);
	

}
bool get_timer0_status(void)
{
	  if(!((cobra_uart_get_interrupt_statue(M3_UART0)& 0x0008)==0x0008))
		   return false;
	  else 
		   return true;
}


void cobra_low_power_uart0_receive_handle(void)
{
    if(!get_uart0_rx_doing_status())
    {
        low_power_uart_start_receive();
    }
		
    if(get_uart0_tx_doing_status() || get_uart0_rx_doing_status())
	  {
	     cobra_low_power_timer0_start();         //有数据发送，继续延时
    }
	  else
	  {
       rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
	  }
}


void init_wake_up(void)
{
		NVIC_DisableIRQ((IRQn_Type)PAD_TRIGGER_SINGLE_IRQ);	
	
		cobra_pin_pullup_enable(PIN9_INDEX,1);
	
		app_m3_gpio_set_direction(GPIO_PIN_9,GPIO_INPUT);
	
		cobra_m4_gpio_interrupt_enable(GPIO_PIN_9);
	
		cobra_m4_gpio_interrupt_leve(GPIO_PIN_9,0);
	
		NVIC_SetPriority((IRQn_Type)PAD_TRIGGER_SINGLE_IRQ,1);
	
		NVIC_EnableIRQ((IRQn_Type)PAD_TRIGGER_SINGLE_IRQ);	
}

#endif

#endif