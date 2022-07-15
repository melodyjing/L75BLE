/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_system.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/29

	Description :   


	Changed Log	:

		Liujian  2019/3/29			- Creation



*****************************************************************************/
#include "cobra_system.h"
#include "cobra4_common.h"
#include "cobra_dualtimer_aon.h"
#include "cobra4C_eflash_ctrl.h"

#if ((BLE_APP_MESH == 1) && (APP_MESH_CWHSL_LIGHT_FUNTION == 1))
#include "app_ate_parameter.h"
#endif

#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)
#include  "app_sfud.h"
#endif

#if (COBRA_POWER_DOWN_ENABLE )
#include "cobra4_power_down.h"
#endif

#define RDW_BLE_DEFAULT_BDADDR  {{0x68, 0x00, 0x00, 0x00, 0x00, 0xC6}}

static struct bd_addr  rdw_default_bdaddr = RDW_BLE_DEFAULT_BDADDR;

void copy_m0_boot_to_rambank0(void);  //test......

#if (COBRA_POWER_DOWN_ENABLE)
static unsigned int system_frequency  __attribute__((section("retention_area"),zero_init));

static unsigned int cpu_frequency  __attribute__((section("retention_area"),zero_init));

static unsigned int m0_ahb_frequency  __attribute__((section("retention_area"),zero_init));

static unsigned int m3_apb0_frequency  __attribute__((section("retention_area"),zero_init));
static unsigned int m3_apb1_frequency  __attribute__((section("retention_area"),zero_init));
static unsigned int m3_apb2_frequency  __attribute__((section("retention_area"),zero_init));
static unsigned int m3_apb3_frequency  __attribute__((section("retention_area"),zero_init));

extern volatile bool ble_deep_sleep;
#else
static unsigned int system_frequency = 0;

static unsigned int cpu_frequency = 0;

static unsigned int m0_ahb_frequency = 0;
//static unsigned int m3_ahb_frequency = 0;

static unsigned int m3_apb0_frequency = 0;
static unsigned int m3_apb1_frequency = 0;
static unsigned int m3_apb2_frequency = 0;
static unsigned int m3_apb3_frequency = 0;
#endif

static uint8_t   calculate_mode = RF_MODE_NORMAL;
static uint16_t  calculate_frequency;     //测量是2.4G 的带外频率
static uint32_t  calculate_address;       //测量是2.4G 的同步字
static uint8_t   calculate_pluse_type;    //测量的脉冲类型
static uint32_t  system_reboot_flag = 0;

static uint8_t   nvds_param_flag = 0;

static uint8_t   reset_type;



#if (COBRA_FLASH_POWERON_CRASE == 1)
static uint8_t mesh_poweronoff_number;

static uint8_t mesh_poweron_timeout;

static uint32_t mesh_address_flag;

static uint8_t flash_light_number;

uint32_t read_meshaddress_flag(void);
uint8_t read_flash_light_number(void);
uint8_t read_poweronoff_number(void);
uint8_t read_poweron_timeout(void);
#endif

#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
static ble_meter_struct calculate_meter_param;
#endif

void cobra_nvds_produce_random_uuid(void);

void cobra4_system_init_clk (void)
{
		system_frequency = 24000000;   //24Mhz  default
		cpu_frequency = system_frequency;
	//	m3_ahb_frequency = system_frequency;
	
		m3_apb0_frequency = system_frequency;
		m3_apb1_frequency = system_frequency;
		m3_apb2_frequency = system_frequency;
		m3_apb3_frequency = system_frequency;
	
		m0_ahb_frequency = system_frequency;
	
}

void cobra_clock_default_setting(void)
{
		cpu_frequency = system_frequency;

		m3_apb0_frequency = system_frequency;
		m3_apb1_frequency = system_frequency;
		m3_apb2_frequency = system_frequency;
		m3_apb3_frequency = system_frequency;
	
		m0_ahb_frequency = system_frequency;
}

unsigned int cobra_system_get_cpu_frequency(void)
{
		return system_frequency;	
}

void cobra_cpu_set_frequency(unsigned char divider)
{
		cpu_frequency = system_frequency/(divider+1);
		cobra_pcrm_system_clk_divider(divider&0xff);
}

unsigned int cobra_system_get_m0_ahb_clk(void)
{
		return m0_ahb_frequency;	
}

unsigned int cobra_system_get_m3_apb0_clk(void)
{
		return m3_apb0_frequency;	
}

unsigned int cobra_system_get_m3_apb1_clk(void)
{
		return m3_apb1_frequency;	
}

unsigned int cobra_system_get_m3_apb2_clk(void)
{
		return m3_apb2_frequency;	
}

unsigned int cobra_system_get_m3_apb3_clk(void)
{
		return m3_apb3_frequency;	
}

void cobra_system_set_m3_apb0_clk(unsigned int clock_value)
{
	  m3_apb0_frequency = clock_value;
}

void cobra_system_clock_setting(void)
{
		 cobra_cpu_set_frequency(0);
#if 1	
#if (APP_COBRA_IRDA_EN || APP_COBRA_IRDA_WAKEUP_EN || APP_COBRA_IRDATX_EN)
	    cobra_pcrm_shift_pattern_length(0x0b);
#else
      cobra_pcrm_shift_pattern_length(0x0f);
#endif
	
		 cobra_pcrm_m0_system_clk_shift_pattern(0x5555);   //二分频率 	
		 m0_ahb_frequency = cpu_frequency>>1;
		
		// m3_ahb_frequency = cpu_frequency;
	
#if (APP_COBRA_IRDA_EN || APP_COBRA_IRDA_WAKEUP_EN || APP_COBRA_IRDATX_EN)
        cobra_pcrm_apb1_clk_shift_pattern(0x249);
        m3_apb1_frequency = cpu_frequency / 3;
#else
        cobra_pcrm_apb1_clk_shift_pattern(0x4444);
        m3_apb1_frequency = cpu_frequency >> 2;
#endif
		
		 cobra_pcrm_apb2_clk_shift_pattern(0x4444);
		 m3_apb2_frequency = cpu_frequency >> 2;
		 
		 cobra_pcrm_apb3_clk_shift_pattern(0x4444);
		 m3_apb3_frequency = cpu_frequency >> 2;
#else
		 cobra_pcrm_shift_pattern_length(11);
	
		 cobra_pcrm_m0_system_clk_shift_pattern(0x924);   //三分频率 	
		 m0_ahb_frequency = cpu_frequency/3;
		
//		 cobra_pcrm_m3_system_clk_shift_pattern(0x924);   //三制德?
		 m3_ahb_frequency = cpu_frequency;
	
		 cobra_pcrm_rfm0_apb_cfg_shift_pattern(0x820);   //二分频率
		 m0_apb10_frequency = m0_ahb_frequency >> 2;
	
		 cobra_pcrm_apb0_clk_shift_pattern(0x820);
		 //m3_apb0_frequency = m3_ahb_frequency >> 1;
	
		 cobra_pcrm_apb1_clk_shift_pattern(0x820);
		 m3_apb1_frequency = m3_ahb_frequency >> 1 ;
		
		 cobra_pcrm_apb2_clk_shift_pattern(0x820);
		 m3_apb2_frequency = m3_ahb_frequency >> 1 ;
		 
		 cobra_pcrm_apb3_clk_shift_pattern(0x820);
		 m3_apb3_frequency = m3_ahb_frequency >> 1 ;

#endif

}


void cobra_system_clock_reset_setting(void)
{
		 cobra_cpu_set_frequency(0);
		 cobra_pcrm_shift_pattern_length(0x0f);
		 cobra_pcrm_m0_system_clk_shift_pattern(0xffff);    	
		 cobra_pcrm_apb1_clk_shift_pattern(0xffff);
		 cobra_pcrm_apb2_clk_shift_pattern(0xffff);
		 cobra_pcrm_apb3_clk_shift_pattern(0xffff);
}

//设置中断向量表地址
void cobra_system_set_vector(void)
{
#if ((APP_AIRCLEAN_ENABLE == 1) || (APP_RAINBOW_ENABLE == 1) || (ESD_TEST_ENABLE == 1) ||(APP_TEST_USE_MPU == 1))
	
	 NVIC_SetVectorTable(0x00800000, 0);
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)

#if(APP_MOSQUITO_KILLING_OTA_ENABLE == 1)
	 NVIC_SetVectorTable(0x00830000, 0);
#else
	 NVIC_SetVectorTable(0x00800000, 0);	
#endif

#elif (APP_MESH_LIGHT_ENABLE == 1)
	

#if APP_MESH_CWHSL_LIGHT_FUNTION	
   NVIC_SetVectorTable(0x00804000, 0);
#else	
	 NVIC_SetVectorTable(0x00004000, 0);
#endif	
	
#elif ((APP_RUN_FROM_SPI_FLASH_ENABLE == 1) && (COBRA4C_INSIDE_SPI_FLASH_ENABLE))
	
   NVIC_SetVectorTable(0x890000, 0);	
	
#else  /////////////////////////////////////////////////
	
#ifdef COBRA_OTA_ENABLE
	  NVIC_SetVectorTable((0x00830000 - 0x6000), 0);
#else	
#if(COBRA_POWER_DOWN_ENABLE)
		if(ble_deep_sleep == false)
		{			
	      NVIC_SetVectorTable(0x00800000, 0);
		}
#else	
#if APP_MESH_CWHSL_LIGHT_FUNTION | RADAR_MODUL_ENABLE
    NVIC_SetVectorTable(0x00800000, 0);    
#else
		NVIC_SetVectorTable(0x00000000, 0);    
#endif		
#endif
#endif
#endif	
	
}


void cobra_system_init(void)
{
     cobra_system_set_vector();
	
		 cobra_aon_dcdc_init();
	
#if COBRA4_AON_PD_TOP_EN
			cobra4_aon_pd_top_init();
#endif

#if COBRA_AON_PMU_EN
		 cobra_aon_pmu_init();
#endif

#if COBRA4_D_VAD_EN	
		cobra4_d_vad_init();
#endif

#if COBRA4_AON_WK_EN
		cobra4_aon_wk_init();
#endif

#if COBRA4_AUDIO_EN
		cobra4_audio_init();
#endif	
		 cobra_pinmux_init();
		 cobra_pcrm_init();
		 cobra_eflash_init();	
		 cobra_cache_init();
	   cobra_ceva_baseband_init();

		 cobra_uart_init();

#if COBRA4_GPIO_EN
#if (COBRA_POWER_DOWN_ENABLE==1)
		  if(ble_deep_sleep == false)
		  {
			    cobra_m4_gpio_init();
		  }
#else
       cobra_m4_gpio_init();
#endif			
		
#endif
#if COBRA_UDMA_EN		
		 cobra_udma_init();
#endif

#if COBRA_UMCU_EN	
		cobra_umcu_init();	
#endif
	
	
#if COBRA_RTC_EN
		cobra_rtc_init();	
#endif
	
#if COBRA_DUALTIMER_EN	
		cobra_dualtimer_init();
#endif

#if COBRA_DUALTIMER_AON_EN	
		cobra_aon_dualtimer_init();
#endif
	
#if COBRA_PDM_EN	
		 cobra_pdm_init();
#endif
	
#if COBRA_SRC_EN
	cobra_src_init();
#endif

#if COBRA_AGC_EN
    cobra_agc_init();
#endif    
    
#if COBRA_I2S_EN	
		 cobra_i2s_init();
#endif

#if 	COBRA_TIMER_EN
		 cobra_timer_init();
#endif

#if COBRA_I2C_EN
		 cobra_i2c_init();
#endif

#if COBRA_SPI_EN
		cobra_spi_init();
#endif

#if COBRA_SOFT_TIMER_EN
		cobra_soft_timer_init();
#endif

#if COBRA_IRDA_EN
		cobra_irda_init();
#endif

#if COBRA_GPADC_EN
		cobra_gpadc_init();
#endif

#if COBRA_WATCHDOG_EN
		cobra_watchdog_init();
#endif

#if	(COBRA_TRNG_EN)
		cobra_trng_init();
#endif

#if (COBRA_M0_GPIO_EN)
		cobra_m0_gpio_init();
#endif

#if (COBRA_ECC_EN)
		cobra_ecc_init();
#endif

#if COBRA_SM4_EN
	cobra_sm4_init();
#endif

#if COBRA_AES_EN
	cobra4_aes_init();
#endif

#if COBRA4_GPADC_EN
	 cobra4_gpadc_init();
#endif

#if COBRA4_SM4_EN
		cobra4_sm4_init();
#endif

#if (COBRA_SPWM_EN)
	   cobra_spwm_init();
#endif
	
#if (COBRA_MPWM_EN)
	  cobra_mpwm_init();
#endif
	
}

/*
初始化 NVDS, 读取EFLASH 或者 SPI FLASH 里面保存的数据
初始化一些全局变量
*/
void cobra_system_nvds_init(void)
{
#if(APP_OTABOOTLOAD_ENABLE)
     cobra_eflash_parameter_init();
#endif

//#if (BLE_APP_MESH == 1) 
////2021/7/20 修改, 因为 MESH 需要加快擦除时间
//      #if(APP_RUN_FROM_SPI_FLASH_ENABLE == 1)
//      cobra_eflash_parameter_init();
//			#endif
//#endif	

#if ((COBRA4C_INSIDE_SPI_FLASH_ENABLE) && (APP_RUN_FROM_SPI_FLASH_ENABLE == 0))  // 只有程序在 EFLASH 运行时,才执行这个
     app_sfud_init();
#endif

	  		
#if ((APP_ADC_SAMPLE_ENABLE == 0) && (APP_MOUSE_ENABLE == 0) && (APP_OTABOOTLOAD_ENABLE == 0) && (ESD_TEST_ENABLE == 0))   //不要使用NVDS)
		 cobra_nvds_init();
#endif		 
		 
#if (BLE_APP_MESH == 1) 	 
	  cobra_nvds_produce_random_uuid();
#endif	
	
}


/*
时钟和电源初始化
*/
void cobra_system_power_clk_init(void)
{
			cobra4_system_init_clk();
	
	    cobra4_aon_pd_top_watchdog_presetn_en(1);
	    cobra4_aon_pd_top_watchdog_presetn_en(0);
			cobra4_aon_pd_top_protection_en(1);
			cobra4_aon_pd_top_rtc0_pclk_en(1);  //rtc pclk
			cobra4_aon_pd_top_rtc0_clk_en(1);   //rtc clk
			cobra4_aon_pd_top_csem_rf_pclk_en(1);
			cobra4_aon_pd_top_ucpu_pclk_en(1);
			cobra4_aon_pd_top_ucpu_mask_en(1);
			cobra4_aon_pd_top_dualtimers_pclk_en(1);
			cobra4_aon_pd_top_watchdog_pclk_en(1);
			cobra4_aon_pd_top_dcdc_pmu_pclk_en(1);
			cobra4_aon_pd_top_pinmux_pclk_en(1);
			cobra4_aon_pd_top_protection_en(0);
	

	
		 cobra_pcrm_m3_active_mode_clock_enable(1);
		 cobra_pcrm_bluetooth_active_mode_clock_enable(1);
		 cobra_pcrm_eflash_ctrl_active_mode_clock_enable(1);
		 cobra_pcrm_vim_active_mode_clock_enable(1);
		 cobra_pcrm_system_bus_active_mode_clock_enable(1);
		 cobra_pcrm_vim_sys_bus_active_mode_clock_enable(1);
		 cobra_pcrm_spiflash_ctrl_active_mode_clock_enable(1);
		 cobra_pcrm_udma_active_mode_clock_enable(1);
		 cobra_pcrm_sha_active_mode_clock_enable(1);
		 cobra_pcrm_aon_inf_bridge_hclk_active_mode_clock_enable(1);
		 cobra_pcrm_vim_cache_active_mode_clock_enable(1);
		 cobra_pcrm_eflash_ctrl_active_mode_clock_enable(1);
		 cobra_pcrm_pinmux_active_mode_clock_enable(1);
		 cobra_pcrm_bt_cpu_active_mode_clock_enable(1);
		 cobra_pcrm_apll_dclk_active_mode_clock_enable(1);
		 cobra_pcrm_uart1_active_mode_clock_enable(1);	
		 cobra_pcrm_bluetooth_sleep_mode_clock_enable(1);
		 cobra_pcrm_bt_cpu_sleep_mode_clock_enable(1);
		 cobra_pcrm_uart1_sleep_mode_clock_enable(1);
		 cobra_pcrm_uart0_sleep_mode_clock_enable(1);
		 cobra_pcrm_udma_sleep_mode_clock_enable(1);
		 cobra_pcrm_pdm0_sleep_mode_clock_enable(1);
		 cobra_pcrm_eflash_ctrl_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_vim_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_spiflash_ctrl_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_system_bus_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_bt_cpu_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_vim_sys_bus_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_m3_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_uart0_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_uart1_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_vim_cache_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_pinmux_deep_sleep_mode_clock_enable(1);
		 cobra_pcrm_apll_dclk_deep_sleep_mode_clock_enable(1);
		 
		 cobra_pcrm_irda_active_mode_clock_enable(1);
		 cobra_pcrm_irda_sleep_mode_clock_enable(1);
		 cobra_pcrm_irda_deep_sleep_mode_clock_enable(1);

#if((USE_PDM_SAMPLE) && BLE_APP_RDW_VOICE)		 
		 cobra_pcrm_pdm0_active_mode_clock_enable(1);
	   cobra_pcrm_pdm1_active_mode_clock_enable(1);
	   cobra_pcrm_i2c0_active_mode_clock_enable(1);
#endif

		  cobra_pcrm_p1_power_ctrl(1);
		  cobra_pcrm_p2_power_ctrl(1);
			
#if(BLE_APP_RDW_VOICE)			 
		  cobra_pcrm_apll_active_mode_clock_enable(1);
	    cobra_pcrm_apll_sleep_mode_clock_enable(1);
#endif
		 
		  cobra_cache_normal_mode_enable(1);  //cache enable
			
			cobra4_aon_wk_protection_en(1);
			
			cobra4_aon_wk_aon_rcosc32k_en(1);
			cobra4_aon_wk_aon_xtal32k_en(1);
			cobra4_aon_wk_aon_rcosc24m_en(1);
#if (APP_SLEEP_MODE_ENABLE == 1)			
			cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_XTAL32K);
#else
      cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_RCOSC32K);  
#endif
			cobra4_aon_wk_protection_en(0);
			cobra4_aon_pd_top_protection_en(1);
			//cobra4_aon_pd_top_lf_clock_selection(LF_XTAL_32K);      //old   
			
			#if (COBRA_POWER_DOWN_ENABLE == 1)
			cobra4_aon_pd_top_lf_clock_selection(LF_XTAL_32K);
			#else
			cobra4_aon_pd_top_lf_clock_selection(LF_RC_OSC32K);      //2020/7/1 杨东
			#endif
			
			cobra4_aon_pd_top_protection_en(0);
			
		  cobra_ceva_baseband_csem_init();

		  cobra_system_switch_to_48m(1);
		 
#if (COBRA_POWER_DOWN_ENABLE == 0)
		 cobra4_aon_wk_aon_rcosc24m_en(0);
#endif
  
#if ((APP_ADC_SAMPLE_ENABLE == 1) || ((APP_MOUSE_ENABLE  == 1)&(APP_MOUSE_TEST_EMI_ENABLE == 0)))
//不分频, 全速运行
#else
	 	  cobra_system_clock_setting();  //分频设定 
#endif			

	
			cobra4_aon_pd_top_protection_en(1);
#if ((APP_ADC_SAMPLE_ENABLE == 1)	|| ((APP_MOUSE_ENABLE  == 1)&(APP_MOUSE_TEST_EMI_ENABLE == 0)))		           //2020/7/1 杨东   HCLK_XTAL48M
			cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);  
			cobra4_aon_pd_top_aon_hclk_divider(11);
			cobra4_aon_pd_top_aon_pclk_divider(0);
			m3_apb0_frequency = 4000000;
#elif (TUYA_BLE_SDK_ENABLE == 1)
      cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);  
			cobra4_aon_pd_top_aon_hclk_divider(1);
			cobra4_aon_pd_top_aon_pclk_divider(0);
			m3_apb0_frequency = 24000000;
#elif (APP_RAINBOW_ENABLE == 1) 			

      cobra_eflash_rainbow_parameter_init();
    
      cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);
			cobra4_aon_pd_top_aon_hclk_divider(1);
			cobra4_aon_pd_top_aon_pclk_divider(0);
			m3_apb0_frequency = 24000000;	
#elif (APP_MOSQUITO_KILLING_ENABLE == 1) 			
      cobra_eflash_mosquito_killing_parameter_init();
    
      cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);
			cobra4_aon_pd_top_aon_hclk_divider(1);
			cobra4_aon_pd_top_aon_pclk_divider(0);
			m3_apb0_frequency = 24000000;			
#else
			cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_CSEM32K);  //2020/7/1 杨东   HCLK_XTAL48M
			cobra4_aon_pd_top_aon_hclk_divider(0);
			cobra4_aon_pd_top_aon_pclk_divider(0);
			m3_apb0_frequency = 32768;			   
#endif

			cobra4_aon_pd_top_protection_en(0);
	
		  cobra4_d_vad_send_sigal_to_ppu(0);

      cobra4_d_vad_bt_clock_sel(24);      //二分频, 射频基带使用 24Mhz
			
      cobra_system_nvds_init();
			
}


/*
  2020/9/11
  为了在校表模式下, 可以达到更高的速率, 要切换 Aon clock
*/

void cobra_meter_clock_setting(void)
{
	  cobra4_aon_pd_top_protection_en(1);
	
	  cobra4_aon_pd_top_lf_clock_selection(LF_XTAL_32K);
	  cobra4_aon_pd_top_aon_hclk_clock_selection(HCLK_XTAL48M);  

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
	  cobra4_aon_pd_top_aon_hclk_divider(11);
	  cobra4_aon_pd_top_aon_pclk_divider(0);
	  m3_apb0_frequency = 4000000;
#elif (TUYA_BLE_SDK_ENABLE == 1)
		cobra4_aon_pd_top_aon_hclk_divider(1);
		cobra4_aon_pd_top_aon_pclk_divider(0);
		m3_apb0_frequency = 24000000;
#else	
	  cobra4_aon_pd_top_aon_hclk_divider(23);
	  cobra4_aon_pd_top_aon_pclk_divider(0);
	  m3_apb0_frequency = 2000000;
#endif
	
	  cobra4_aon_pd_top_protection_en(0);
}


/*
change cobra system to 48M
if en, need to modified csem mode, and change to 48M
if en = 0, need to change to RC 8M, then close csem mode
*/
void cobra_system_switch_to_48m(unsigned int en)
{
		volatile unsigned int i;
		if(en)
		{
				cobra4_aon_wk_protection_en(1);
				cobra4_aon_wk_icytrx_iso_en(0);
				cobra4_aon_wk_protection_en(0);
			
				cobra_aon_dcdc_ldo_vddd_en(1);
				cobra_aon_dcdc_bgp_en(1);
				cobra_aon_dcdc_ptat_enb(1);
				//cobra_pcrm_icytrx_soft_reset();
				cobra4_aon_pd_top_protection_en(1);
				cobra4_aon_pd_top_icytrx_reset_en(0);  //reset
				cobra4_aon_pd_top_protection_en(0);	
			
#if (COBRA_POWER_DOWN_ENABLE)
        app_cobra4_power_down_process_48m();
#else
				cobra_ceva_baseband_icytrx_xtal_48m_enable(1);
#endif			

				cobra4_aon_pd_top_protection_en(1);
				cobra4_aon_pd_top_hf_clock_selection(HF_XTAL48M);
				cobra4_aon_pd_top_protection_en(0);	
				system_frequency = 48000000;
				cobra_clock_default_setting();
		}
		else
		{
				 cobra4_aon_pd_top_protection_en(1);
				 cobra4_aon_pd_top_hf_clock_selection(HF_RC_OSC24M);
				 cobra4_aon_pd_top_protection_en(0);
				 cobra_ceva_baseband_icytrx_xtal_48m_enable(0);
				 system_frequency = 24000000;
				 cobra_clock_default_setting();
		}
}


void cobra_system_uart_init(void)
{
		//Enable cobra4 UART0	
		cobra_pcrm_uart0_active_mode_clock_enable(1);
	  cobra_pcrm_uart0_soft_reset();
	
		cobra_pin_pullup_enable(PIN9_INDEX, 1);

#if (APP_AIRCLEAN_ENABLE == 1)	
	   cobra_pin_pullup_enable(PIN7_INDEX, 1);
	   cobra_map_pinmodeset(PIN7_INDEX, PIN7_UART0_TXD);	
	   cobra_pin_drain_source_enable(PIN7_INDEX, 1);
#elif (APP_RAINBOW_ENABLE == 1)
	
	   cobra_pin_pullup_enable(PIN7_INDEX, 1);
	   cobra_map_pinmodeset(PIN7_INDEX, PIN7_UART0_TXD);	
	   cobra_pin_drain_source_enable(PIN7_INDEX, 1);
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)
	   cobra_pin_pullup_enable(PIN7_INDEX, 1);
	   cobra_map_pinmodeset(PIN7_INDEX, PIN7_UART0_TXD);	
	   cobra_pin_drain_source_enable(PIN7_INDEX, 1);	
#else   ////////////////////////////////////////////////////
	
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))	
	
#else	
		app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_OUTPUT);
	  app_m3_gpio_write_pin(GPIO_PIN_27, 1);
	  cobra_pin_pullup_enable(PIN27_INDEX, 1);
	
	  cobra_map_pinmodeset(PIN27_INDEX, PIN27_UART0_TXD);
	  cobra_pin_drain_source_enable(PIN27_INDEX, 1);
#endif
#endif
	
	
		cobra_map_pinmodeset(PIN9_INDEX, PIN9_UART0_RXD);
	  cobra_pin_drain_source_enable(PIN9_INDEX, 1);
	

    if(app_common_get_work_mode() == MODE_NORMAL)
		{
			  #if (DIANBIAO_MODULE_ENABLE == 1)
				  cobra_uart_init_rate(M3_UART0, 115200, PARITY_EVEN);
			  #elif(TUYA_BLE_SDK_ENABLE == 1)
			    cobra_uart_init_rate(M3_UART0, 230400, PARITY_NULL);
			  #elif(APP_MOSQUITO_KILLING_BAUDRATE_FIX == 1)
//			    cobra_uart_init_rate(M3_UART0, 230400, PARITY_NULL);
			    cobra_uart_init_rate(M3_UART0, 460800, PARITY_NULL);
			  #else
			    cobra_uart_init_rate(M3_UART0, 115200, PARITY_NULL);
			  #endif
    }
		else
		{
				cobra_uart_init_rate(M3_UART0, 57600, PARITY_NULL);
		}

		
}
/*
 得到系统复位的原因

*/
RESET_TYPE cobra_reset_type(void)
{
		if(cobra4_aon_wk_m4_pin_or_power_up_reset())
		{
				return RESET_PIN_MODE ;   //pin reset
		}
		else if(cobra4_aon_wk_m4_system_lockup_reset())   //watchdog
		{
				return RESET_WATCHDOG_MODE;    
		}
		else if(cobra4_aon_wk_m4_system_soft_reset())    
		{
				return RESET_SOFT_MODE;    
		}
		else if(cobra4_aon_wk_m4_system_mcu_pd_por_reset())
		{			
			  return RESET_PD_POR_MODE;    
		}
		else
		{
			  return RESET_ERROR;    
		}
	
}


RF_MODE cobra_get_calculate_mode(void)
{
		return calculate_mode;	
}

void cobra_set_calculate_mode(RF_MODE mode)
{
		calculate_mode = mode;	
}

uint16_t cobra_get_calculate_frequency(void)
{
	  return calculate_frequency;	
}

uint32_t cobra_get_calculate_address(void)
{
	  return calculate_address;	
}

uint8_t cobra_get_calculate_pluse_type(void)
{
	  return  calculate_pluse_type;
}

#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
ble_meter_struct * cobra_get_calculate_param(void)
{
	  return  &calculate_meter_param;
}
#endif


#if (BLE_APP_MESH == 1)   
static unsigned char mesh_uuid[16];
unsigned char * cobra_nvds_get_prov_uuid(void)
{	
	 return mesh_uuid;
}

#define CLIENT_NODE_UUID {0xA8, 0x01, 0x71, 0x0C, 0x2F, 0x00, 0x00, 0xCA, 0xF6, 0x21, 0x7A, 0xFA, 0x28, 0x02, 0x10, 0x00}

void cobra_nvds_produce_random_uuid(void)
{
			uint8_t   temp, i;
			temp = MESH_DEVICE_UUID_LENGTH;
			
			if(nvds_get(MESH_DEVICE_UUID_FLAG, (uint8_t *)&temp, mesh_uuid) != NVDS_OK)
			{
#if (APP_MESH_CWHSL_LIGHT_FUNTION == 1)
				    mesh_uuid[0] = 0x00;
				    mesh_uuid[1] = 0x8f;    //CID
				   
            mesh_uuid[2] = 0xea;    //PID
				    
				    mesh_uuid[3] = 0x56;     //Product ID
				    mesh_uuid[4] = 0xba;

            mesh_uuid[5] = 0x01; 		 //feature
            
            mesh_uuid[6] = 0x01;      //module
           
            mesh_uuid[7] = rdw_default_bdaddr.addr[0];
				    mesh_uuid[8] = rdw_default_bdaddr.addr[1];			   
            mesh_uuid[9] = rdw_default_bdaddr.addr[2]; 			    
				    mesh_uuid[10] = rdw_default_bdaddr.addr[3]; 
				    mesh_uuid[11] = rdw_default_bdaddr.addr[4];
            mesh_uuid[12] = rdw_default_bdaddr.addr[5];	
				
				    mesh_uuid[13] = 0x00;     //RFU
				    mesh_uuid[14] = 0x00;
            mesh_uuid[15] = 0x01; 
#else				
#if (BLE_APP_MESH_TULIPE_LIGHT == 1)
				    mesh_uuid[0] = 0xE1;
				    mesh_uuid[1] = 0x56;    //CID
				   
            mesh_uuid[2] = 0x05;    //PID
				    
				    mesh_uuid[3] = 0x0C;     //Product ID
				    mesh_uuid[4] = 0x2F;

            mesh_uuid[5] = 0x2F; 		 //feature
            
            mesh_uuid[6] = 0x01;      //module
           
            mesh_uuid[7] = rdw_default_bdaddr.addr[5];
				    mesh_uuid[8] = rdw_default_bdaddr.addr[4];			   
            mesh_uuid[9] = rdw_default_bdaddr.addr[3]; 			    
				    mesh_uuid[10] = rdw_default_bdaddr.addr[2]; 
				    mesh_uuid[11] = rdw_default_bdaddr.addr[1];
            mesh_uuid[12] = rdw_default_bdaddr.addr[0];	
				
				    mesh_uuid[13] = 0x00;     //RFU
				    mesh_uuid[14] = 0x00;
            mesh_uuid[15] = 0x00; 		 
				
#else				
				   for(i = 0; i< 16; i++)
			     {
				       mesh_uuid[i] = cobra_trng_rand_read();
			     }
#endif
#endif
					 
					 nvds_put(MESH_DEVICE_UUID_FLAG, MESH_DEVICE_UUID_LENGTH, (uint8_t *)mesh_uuid);
			}
						
}

void cobra_nvds_produce_debug(void)
{
	    uint8_t   i;
	 		for(i = 0; i< 16; i++)
			{
				 printf("0x%02x ", mesh_uuid[i]);
			}
}

#endif

void cobra_nvds_read_param(void)
{
	const char *p_buffer;
	
	p_buffer = (const char *)M4_EFLASH_PARAM_ADDRESS;
	if(strncmp(p_buffer, (const char *)"RDW2150", sizeof("RDW2150")-1) == 0)
	{
		 nvds_param_flag = 1;
	}

}


void cobra_nvds_mac_address_init(void)
{
	  
	
	  unsigned char  temp_buffer[7];
#if ((APP_RUN_FROM_SPI_FLASH_ENABLE == 0) && (COBRA4C_INSIDE_SPI_FLASH_ENABLE))  //EFLASH 运行
	
	 cobra_inside_spi_flash_read(INSIDE_SPI_FLASH_NVDS + 2*SEC_SIZE,  temp_buffer, 7);
	 
	 if(temp_buffer[0] != 0x66)
	 {
		   temp_buffer[0] = 0x66;
		   temp_buffer[1] = cobra_trng_rand_read();
		   temp_buffer[2] = cobra_trng_rand_read();
		   temp_buffer[3] = cobra_trng_rand_read();
		   temp_buffer[4] = cobra_trng_rand_read();
		   temp_buffer[5] = 0x55;
		   temp_buffer[6] = 0xC0;
		   cobra_inside_spi_flash_erase(INSIDE_SPI_FLASH_NVDS + 2*SEC_SIZE,  8);
		   cobra_inside_spi_flash_write(INSIDE_SPI_FLASH_NVDS + 2*SEC_SIZE,  temp_buffer, 7);	
		   cobra_cache_invalidate_the_cache(); 
	 }
	 
    
#else	
	 
	 const char *p_buffer;
	 
	 p_buffer = (const char *)(M4_EFLASH_PARAM_ADDRESS + 100);
	
	 if(*p_buffer != 0x55)
	 {
		   temp_buffer[0] = 0x55;
		   temp_buffer[1] = cobra_trng_rand_read();
		   temp_buffer[2] = cobra_trng_rand_read();
		   temp_buffer[3] = cobra_trng_rand_read();
		   temp_buffer[4] = cobra_trng_rand_read();
		   temp_buffer[5] = 0x11;
		   temp_buffer[6] = 0xC0;
//		   cobra_flash_erase(M4_EFLASH_PARAM_ADDRESS, 4096);
//		   cobra_flash_write((unsigned int)p_buffer, 7,	(unsigned char *)temp_buffer);	
		   cobra_cache_invalidate_the_cache(); 
	 }
#endif	 
	 
}



uint8_t cobra_nvds_param_ok(void)
{
	 return nvds_param_flag;
}

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
extern uint8_t  app_mosquito_killing_bt_addr[];
extern uint8_t  app_mosquito_killing_pid[];
extern uint8_t  app_mosquito_killing_password[];
extern uint8_t  app_mosquito_killing_name[];
extern uint8_t  app_mosquito_killing_addr_save_flag;

#define   M4_EFLASH_MAC_PID_ADDRESS  (0x50000 - 0x4000)
#define  APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET  (0x00800000)

void app_mosquito_killing_cobra_nvds_mac_address_init(void)
{
    uint8_t * _u8Buf = (uint8_t * )(APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET + M4_EFLASH_MAC_PID_ADDRESS );

    if((_u8Buf[0] == 0xAA) && (_u8Buf[1] == 0x55))
    {
        if(_u8Buf[9] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_pid, _u8Buf+10, 4);
        }
		
        if(_u8Buf[14] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_password, _u8Buf+15, 4);
        }

        if(_u8Buf[19] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_name, _u8Buf+20, 16);
        }
		
        if(_u8Buf[2] == 0xA5)
        {
            memcpy((uint8_t *)&rdw_default_bdaddr.addr, _u8Buf+3, BD_ADDR_LEN);
            memcpy((uint8_t *)&app_mosquito_killing_bt_addr, (uint8_t *)&rdw_default_bdaddr.addr, BD_ADDR_LEN);
			

			return;
        }
    }

	rdw_default_bdaddr.addr[0] = 0x89;
	rdw_default_bdaddr.addr[1] = cobra_trng_rand_read();
	rdw_default_bdaddr.addr[2] = cobra_trng_rand_read();
	rdw_default_bdaddr.addr[3] = cobra_trng_rand_read();
	rdw_default_bdaddr.addr[4] = cobra_trng_rand_read();
	rdw_default_bdaddr.addr[5] = 0xC8;

	app_mosquito_killing_addr_save_flag = 1;  /*随机生成地址，需要保存*/
	
	memcpy((uint8_t *)&app_mosquito_killing_bt_addr, (uint8_t *)&rdw_default_bdaddr.addr, BD_ADDR_LEN);
}
#endif

void cobra_nvds_init(void)
{
#if((APP_OTABOOTLOAD_ENABLE == 0) && (ESD_TEST_ENABLE == 0))	
	
	 volatile uint8_t   temp;
	 volatile uint16_t   temp16;
	 uint32_t  temp_32;
	 uint32_t  i;
   uint8_t   temp_breaker_addr[6];
	 uint8_t   breaker_en;
	 
	  cobra_nvds_read_param();
	
#if (((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1)) || (APP_RANDOM_ADDRESS_ENABLE == 1))
	  cobra_nvds_mac_address_init();
#endif
	
	
#if ((BLE_APP_MESH == 1) || (BLE_APP_HID_ENABLE == 1))   

#if ((COBRA4C_INSIDE_SPI_FLASH_ENABLE) && (APP_RUN_FROM_SPI_FLASH_ENABLE == 0))  // 只有程序在 EFLASH 运行时,才执行这个
	
	 uint8_t *p_temp = (uint8_t *)M0_M3_SHARE_ADDRESS;
	
	 cobra_inside_spi_flash_read_nvds(p_temp);
	
#else	

   volatile uint8_t *p_temp = (volatile uint8_t *)M0_M3_SHARE_ADDRESS;
	 volatile uint8_t *p_eflash = (volatile uint8_t *)M4_EFLASH_NVDS_ADDRESS;
	 for(i = 0; i < M0_M3_NVDS_SIZE; i++)
	 {
			 *p_temp++ = *p_eflash++;
	 }
	 cobra_cache_invalidate_the_cache();	 
	 
#endif	 
	 
#endif
	 
		nvds_init((uint8_t *)M0_M3_SHARE_ADDRESS, M0_M3_NVDS_SIZE);
	 
#if NVDS_SUPPORT		
		// 设置 M0 的蓝牙地址
	
		//nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&rdw_default_bdaddr);
		
		//设置  M0  是否bonding
#if ((BLE_APP_HID_ENABLE == 1) || (BLE_APP_MESH_TULIPE_LIGHT == 1)) 

#else	 
   
  	temp = false;	  
		nvds_put(NVDS_TAG_PERIPH_BONDED, 1, (uint8_t *)&temp);
#endif		


		temp = IRIS_CALCULATE_LENGTH;
		
    if(nvds_get(IRIS_CALCULATE_SETFLAG, (uint8_t *)&temp, &calculate_mode) != NVDS_OK)
		{
					calculate_mode = 0;
		}
		

#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
		temp = CALIBRATION_METER_PARAM_LENGTH;
		if(nvds_get(CALIBRATION_METER_PARAM_FLAG, (uint8_t *)&temp, (uint8_t *)&calculate_meter_param) != NVDS_OK)
		{
			  memset(&calculate_meter_param, 0, sizeof(calculate_meter_param));
			 // calculate_mode = 0;
		}
#endif
		
		//只有在电表模式下处理
#if (APP_SCAN_DEVICE_ENABLE)		
		temp = METER_REBOOT_FLAG_LEN;
		if(nvds_get(METER_REBOOT_FLAG, (uint8_t *)&temp, (uint8_t *)&system_reboot_flag) != NVDS_OK)
		{
					system_reboot_flag = 0;
		}
		
    if(system_reboot_flag == METER_REBOOT_MAGIC)
		{
					temp = BREAKER_ADDR1_LEN;
					if(nvds_get(BREAKER_ADDR1_FLAG, (uint8_t *)&temp, temp_breaker_addr) != NVDS_OK)
		      {
					     memset(temp_breaker_addr, 0, 6);
		      }
					app_set_new_duanluqi1_address(temp_breaker_addr);
					
					temp = BREAKER1_ENABLE_LEN;
					if(nvds_get(BREAKER1_ENABLE_FLAG, (uint8_t *)&temp, &breaker_en) != NVDS_OK)
		      {
					     app_device_connect_to_duanluqi_1_enable(0);
		      }
					else
					{
							app_device_connect_to_duanluqi_1_enable(breaker_en);
					}
					
					
					temp = BREAKER_ADDR2_LEN;
					if(nvds_get(BREAKER_ADDR2_FLAG, (uint8_t *)&temp, temp_breaker_addr) != NVDS_OK)
		      {
					     memset(temp_breaker_addr, 0, 6);
		      }
					app_set_new_duanluqi2_address(temp_breaker_addr);
					
					temp = BREAKER2_ENABLE_LEN;
					if(nvds_get(BREAKER2_ENABLE_FLAG, (uint8_t *)&temp, &breaker_en) != NVDS_OK)
		      {
					     app_device_connect_to_duanluqi_2_enable(0);
		      }
					else
					{
							app_device_connect_to_duanluqi_2_enable(breaker_en);
					}
					
					
					temp = BREAKER_ADDR3_LEN;
					if(nvds_get(BREAKER_ADDR3_FLAG, (uint8_t *)&temp, temp_breaker_addr) != NVDS_OK)
		      {
					     memset(temp_breaker_addr, 0, 6);
		      }
				  app_set_new_duanluqi3_address(temp_breaker_addr);
					
					temp = BREAKER3_ENABLE_LEN;
					if(nvds_get(BREAKER3_ENABLE_FLAG, (uint8_t *)&temp, &breaker_en) != NVDS_OK)
		      {
					     app_device_connect_to_duanluqi_3_enable(0);
		      }
					else
					{
							app_device_connect_to_duanluqi_3_enable(breaker_en);
					}
					
          temp_32 = 0xffffffff;
		      nvds_put(METER_REBOOT_FLAG, METER_REBOOT_FLAG_LEN, (unsigned char *)&temp_32);
					
					cobra_system_nvds_save_to_eflash();	
          
		}
		else
		{
				  nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&rdw_default_bdaddr);
			
			    temp = 0;
					nvds_put(PARAM_ID_SLEEP_ENABLE, PARAM_LEN_SLEEP_ENABLE, (uint8_t *)&temp);
					
					temp = 0;
					nvds_put(IRIS_MAC_SETFLAG, IRIS_MAC_SETFLAG_LENGTH, (uint8_t *)&temp);
			
		}
#else

#if (BLE_APP_HID_ENABLE)
    nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&rdw_default_bdaddr);
#else	

#if (APP_METER_AT_COMMAND == 0)	
		rdw_default_bdaddr.addr[0] = cobra_trng_rand_read();
		rdw_default_bdaddr.addr[1] = cobra_trng_rand_read();
		rdw_default_bdaddr.addr[2] = cobra_trng_rand_read();
		rdw_default_bdaddr.addr[3] =  0x77;
		rdw_default_bdaddr.addr[4] =  0x55;   //cobra_trng_rand_read
		rdw_default_bdaddr.addr[5] =  0xC0;
#endif
		
#if (APP_WECHAT_ENABLE == 1)
#if (APP_MOSQUITO_KILLING_ENABLE == 1)
		app_mosquito_killing_cobra_nvds_mac_address_init();
#else
    rdw_default_bdaddr.addr[0] =  0x22;
//		rdw_default_bdaddr.addr[0] =  0x11;
		rdw_default_bdaddr.addr[1] =  0x35;
		rdw_default_bdaddr.addr[2] =  0x65;
		rdw_default_bdaddr.addr[3] =  0x73;
		rdw_default_bdaddr.addr[4] =  0x54;   
		rdw_default_bdaddr.addr[5] =  0xC0;
#endif

#endif
		
		
#if (((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1)) || (APP_RANDOM_ADDRESS_ENABLE == 1))
		
#if ((APP_RUN_FROM_SPI_FLASH_ENABLE == 0) && (COBRA4C_INSIDE_SPI_FLASH_ENABLE))  //EFLASH 运行
    unsigned char p_mac_address[7];		
    cobra_inside_spi_flash_read(INSIDE_SPI_FLASH_NVDS + 2*SEC_SIZE, p_mac_address, 7);
		rdw_default_bdaddr.addr[0] = p_mac_address[1];
		rdw_default_bdaddr.addr[1] = p_mac_address[2];
		rdw_default_bdaddr.addr[2] = p_mac_address[3];
		rdw_default_bdaddr.addr[3] = p_mac_address[4];
		rdw_default_bdaddr.addr[4] = p_mac_address[5];
		rdw_default_bdaddr.addr[5] = p_mac_address[6];
#else		

///大错误................................
    const unsigned char *p_mac_address = (const unsigned char *)(M4_EFLASH_PARAM_ADDRESS + 101);
		rdw_default_bdaddr.addr[0] = 0x44;
		rdw_default_bdaddr.addr[1] = p_mac_address[1];
		rdw_default_bdaddr.addr[2] = p_mac_address[2];
		rdw_default_bdaddr.addr[3] = p_mac_address[3];
		rdw_default_bdaddr.addr[4] = 0x55;
		rdw_default_bdaddr.addr[5] = 0xC0;
#endif		
   
		
#endif
		
		nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&rdw_default_bdaddr);
		
#if ((BLE_APP_MESH == 1) && (APP_MESH_CWHSL_LIGHT_FUNTION == 1)) 		

      lt_load_nv();
			
			if(lt_nv_is_valid()&&(lt_nv_mac_is_valid()))
			{
				 unsigned char *p_temp = lt_nv_get_macaddress();
				
			   nvds_put(PARAM_ID_BD_ADDRESS, PARAM_LEN_BD_ADDRESS, (uint8_t *)p_temp);
									
			}
				
#endif
		
#endif
		
#endif


		
#if (APP_SLEEP_MODE_ENABLE == 1)
		temp = 1;
		nvds_put(PARAM_ID_SLEEP_ENABLE, PARAM_LEN_SLEEP_ENABLE, (uint8_t *)&temp);	
#else
		temp = 0;
		nvds_put(PARAM_ID_SLEEP_ENABLE, PARAM_LEN_SLEEP_ENABLE, (uint8_t *)&temp);		
#endif		
		
#endif

#if (COBRA_FLASH_POWERON_CRASE == 1)
//		cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);
#if (BLE_APP_MESH == 1)
    mesh_poweronoff_number = read_poweronoff_number();
    mesh_poweron_timeout   = read_poweron_timeout();
		if(mesh_poweron_timeout == 0x88)
			mesh_poweronoff_number = 1;
    mesh_address_flag      = read_meshaddress_flag();
		flash_light_number     = read_flash_light_number();
		read_mesh_screen();
		set_mesh_scene_data(read_mesh_scene_data());
		
		cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, 4096);	
//	  cobra_flash_erase(M4_EFLASH_PARAM_ADDRESS, 4096);		
#if (BLE_APP_MESH_TULIPE_LIGHT == 1)
//    set_mesh_scene_data(read_mesh_scene_data());
		cobra_flash_erase(APP_MESH_TULIPX_SCENS_DATA, 4096);
#endif		
#endif
#endif

#endif
		
}

//判断是否是人为需要复位
void cobra_system_watchdog_process(void)
{
	  if(system_reboot_flag == METER_REBOOT_MAGIC)
		{
#if( APP_TEMPLATE_ENABLE || APP_OTABOOTLOAD_ENABLE)
#else			
			  appm_send_gapm_reset_cmd();
#endif			
		}
	
}

//把 SPI flash 里面的数据擦掉
void cobra_system_nvds_reset(void)
{
#if (APP_RUN_FROM_SPI_FLASH_ENABLE == 1)		
		cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);	
#else
	  #if(COBRA4C_INSIDE_SPI_FLASH_ENABLE)
       cobra_inside_spi_flash_erase_nvds();
	  #endif
#endif	
}

void cobra_system_nvds_save_to_eflash(void)
{
#if ((COBRA4C_INSIDE_SPI_FLASH_ENABLE) && (APP_RUN_FROM_SPI_FLASH_ENABLE == 0))  // 只有程序在 EFLASH 运行时,才执行这个
	
	  uint8_t *p_temp = (uint8_t *)M0_M3_SHARE_ADDRESS;
	
	  cobra_inside_spi_flash_save_nvds(p_temp);
	
#else	
		uint8_t *p_temp;
	  uint8_t *p_nvds_magic_buffer = (uint8_t *)(M4_EFLASH_NVDS_ADDRESS);
		p_temp = (uint8_t *)M0_M3_SHARE_ADDRESS;
	
	#if (COBRA_FLASH_POWERON_CRASE == 0)
	  if(*p_nvds_magic_buffer != 0xFF)
		{
			  printf("erase .... \n");
		    cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);	
		}	
	#endif
		cobra_cache_invalidate_the_cache();
		cobra_flash_write(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE, p_temp);	
#endif
}


unsigned int cobra_system_cmp(volatile uint8_t *p_str1, volatile uint8_t *p_str2, unsigned int size)
{
		unsigned int i;
	  unsigned int total_number = 0;
	  for(i = 0; i < size; i++)
	  {
		   if(p_str1[i] != p_str2[i])
			 {
				 //printf("i = %d:  0x%02x 0x%02x \n", i, p_str1[i], p_str2[i]);
				 total_number++;
			 }
	  }
		printf("total number = %d \n", total_number);
		
		if(total_number > 0) return 1;
		
		return 0;
	
}
//判断NVDS 是否有更改   
//返回值 
//  1    有更改
//  0    没有更改
unsigned int cobra_system_nvds_is_modified(void)
{
#if ((APP_RUN_FROM_SPI_FLASH_ENABLE == 0) && (COBRA4C_INSIDE_SPI_FLASH_ENABLE))	 
	//如果程序在 EFLASH 上运行, 数据保存在 SPI Flash 上
	   uint8_t * p_sram = (uint8_t *)M0_M3_SHARE_ADDRESS;
	   return cobra_inside_spi_flash_compare_nvds(p_sram, M0_M3_NVDS_SIZE);
#else	
	  volatile uint8_t * p_sram = (volatile uint8_t *)M0_M3_SHARE_ADDRESS;
	  volatile uint8_t * p_eflash = (volatile uint8_t *)M4_EFLASH_NVDS_ADDRESS;
	  cobra_cache_invalidate_the_cache();
	  if(!cobra_system_cmp(p_sram, p_eflash, M0_M3_NVDS_SIZE))
		{
			  // 相同, 不需要保存
			  return 0;
		}
		return 1;
#endif	
}

void cobra_system_interrupt_enable(void)
{
#if ((APP_RF_NORMAL_PROTOCOL_ENABLE == 0) && (APP_ADC_SAMPLE_ENABLE == 0) && (APP_MOUSE_ENABLE == 0))
		NVIC_EnableIRQ(COBRA4_BT_IRQn);
		NVIC_SetPriority(COBRA4_BT_IRQn, 0);
#endif
	
}

__asm void cobra_system_reboot_jump(uint32_t sp,uint32_t pc)
{
	msr 	msp, r0
	msr 	psp, r0
	bx 		r1
}

void ota_platform_reset(uint32_t error)
{
		uint32_t  pc, sp;
	
		app_deep_sleep_disable_interrupt();
	
		NVIC_DisableIRQ(CSEM_IRQ_RECEIVED);
		NVIC_DisableIRQ(CSEM_IRQ_TX);
	  NVIC_DisableIRQ(COBRA4_BT_IRQn);
	
		GLOBAL_INT_STOP();
	
		cobra_cache_invalidate_the_cache();
#if 1	
	  app_cobra_watchdog_init(200, WATCHDOG_RESET);
	  while(1);
#else	
		uint32_t *rebootaddr =(uint32_t *)(0x00000000);		//CACHE_ADDR  FLASH_ADDR
	
		sp = rebootaddr[0];
		pc = rebootaddr[1];
	
	
		cobra_system_reboot_jump(sp,pc);
#endif	
}

//系统 LIBARAY 会调用这个函数
void rf_init_callback(void)
{
#if ((BLE_APP_MESH == 1) && (APP_MESH_CWHSL_LIGHT_FUNTION == 1)) 		
    lt_frequence_power_init();
#endif
	
}


void platform_reset(uint32_t error)
{
		uint32_t  pc, sp;
	
		app_deep_sleep_disable_interrupt();
	
		NVIC_DisableIRQ(CSEM_IRQ_RECEIVED);
		NVIC_DisableIRQ(CSEM_IRQ_TX);
	  NVIC_DisableIRQ(COBRA4_BT_IRQn);
	
		GLOBAL_INT_STOP();
	
		cobra_cache_invalidate_the_cache();
	
#ifdef COBRA_OTA_ENABLE
	uint32_t *rebootaddr =(uint32_t *)(0x0030000 - 0x6000);	
#else
		uint32_t *rebootaddr =(uint32_t *)(0x00000000);		//CACHE_ADDR  FLASH_ADDR
#endif		
		sp = rebootaddr[0];
		pc = rebootaddr[1];

#if(DIANBIAO_MODULE_ENABLE )
    cobra_system_reboot_jump(sp,pc);
#else
	  app_cobra_watchdog_init(200, WATCHDOG_RESET);
	  while(1);
#endif	

		cobra_system_reboot_jump(sp,pc);
	
}

void platform_watchdog_reset(uint32_t error)
{
		uint32_t  pc, sp;
	
		app_deep_sleep_disable_interrupt();
	
		NVIC_DisableIRQ(CSEM_IRQ_RECEIVED);
		NVIC_DisableIRQ(CSEM_IRQ_TX);
	  NVIC_DisableIRQ(COBRA4_BT_IRQn);
	
		GLOBAL_INT_STOP();
	
		cobra_cache_invalidate_the_cache();
	

	  app_cobra_watchdog_init(200, WATCHDOG_RESET);
	  while(1);

}


#if ((COBRA_RTC_TEST == 0) && (COBRA_RTC_DRIVER_TEST == 0))

void cobra_aon_event3_interrupt(void)
{
#if (APP_COBRA_VOICE_WAKEUP_EN) && (COBRA_UMCU_EN)
		cobra_m3_voice_wakeup_interrupt();
#endif

#if (APP_COBRA_UMCU_EN) && (COBRA_UMCU_EN)
		cobra_m3_umcu_interrupt();
#endif	

#if APP_COBRA_RTC_EN
		cobra_rtc_interrupt();
#endif	
}

#endif


#if (COBRA_APP_SIXTIMER_ENABLE == 0)
void cobra_aon_event4_interrupt(void)
{

//#if (APP_COBRA_IRDA_EN) && (COBRA_IRDA_EN)
//		cobra_irda_interrupt();
//#endif
#if (COBRA_DUALTIMER_AON_EN==1)
	 cobra_aon_dualtimer_interrupt();
#endif	
   printf("cobra_aon_event4_interrupt ....... \n");
}
#endif
/**
  * @brief  Sets the vector table location and Offset.
  * @param  NVIC_VectTab: specifies if the vector table is in RAM or FLASH memory.
  *   This parameter can be one of the following values:
  *     @arg NVIC_VectTab_RAM
  *     @arg NVIC_VectTab_FLASH
  * @param  Offset: Vector Table base offset field. This value must be a multiple 
  *         of 0x200.
  * @retval None
  */
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  /* Check the parameters */
//  assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
//  assert_param(IS_NVIC_OFFSET(Offset));  
   
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}


void cobra4_aes_sm4_interrupt(void)
{
#if COBRA_AES_EN	
		cobra4_aes_interrupt_function();
#endif
	
#if COBRA_SM4_EN
#if USE_RDW_HARDWARE_SUPPORT
		cobra_sm4_IRQHandler();
#endif
	
#if COBRA4_SM4_EN
		cobra4_sm4_interrupt_function();
#endif
#endif
}
#if (COBRA_FLASH_POWERON_CRASE == 1)
uint8_t read_poweronoff_number(void)
{
	uint8_t *  p_count = (uint8_t * )M4_EFLASH_COUNT_ADDRESS;
	return *p_count;
}
uint8_t get_poweronoff_number(void)
{
	return mesh_poweronoff_number;
}

uint8_t read_poweron_timeout(void)
{
	uint8_t *  p_flag = (uint8_t * )M4_EFLASH_COUNT_TIMEOUT_ADDRESS;
	return *p_flag;
}
uint8_t get_poweron_timeout(void)
{
	return mesh_poweron_timeout;
}

uint32_t read_meshaddress_flag(void)
{
	uint32_t *p_flag = (uint32_t * )M4_EFLASH_MESH_ADDRESS;
	return *p_flag;
}
uint32_t get_meshaddress_flag(void)
{
	return mesh_address_flag;
}

uint8_t read_flash_light_number(void)
{
	uint8_t *p_flag = (uint8_t * )M4_EFLASH_FLASH_NUMBERS_ADDRESS;
	return *p_flag;
}
uint32_t get_mesh_flash_light_number(void)
{
	return flash_light_number;
}


#endif


















