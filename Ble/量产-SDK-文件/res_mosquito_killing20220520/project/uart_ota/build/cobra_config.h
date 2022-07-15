/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_config.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   cobra define 
					


	Changed Log	:

		Liujian  2019/3/8			- Creation


*****************************************************************************/

#ifndef __COBRA_CONFIG_H__
#define __COBRA_CONFIG_H__

#define MCU_M0									0
#define MCU_M3									1
#define MCU_M4									2

#define MCU_RUN_SYSTEM					MCU_M4

#define COBRA4_SYSTICK_EN				1
#define COBRA_M3_SYSTICK_EN			0             //使能在M3 上的systick 功能, 在M3 上运行的时候,这个必须为 1

#define COBRA_SPI_FLASH_CTRL_EN 1
#define COBRA_PCRM_EN						1
#define COBRA_I2S1_EN						1
#define COBRA_I2S0_EN						1

#if MCU_RUN_SYSTEM == MCU_M4
#define COBRA4_AON_PD_TOP_EN		1
#define COBRA4_D_VAD_EN					1
#define COBRA4_AON_WK_EN				1
#else
#define COBRA_AON_PMU_EN				1

#endif

#define COBRA_ADC_EN_VERSION2   1
#define COBRA_AON_DCDC_EN				1

#define COBRA_EFLASH_CTRL_EN		1
#define COBRA_CACHE_CTRL_EN			1

#define COBRA_M0_PATCH_EN				1

#define COBRA_M0_SYSCTRL_EN			0

#define COBRA_PINMUX_EN					1

#define COBRA_UMCU_EN						1

#define COBRA_CEVA_BASEBADN_EN	1
#define COBRA_UART_EN						1		

#define COBRA_UDMA_EN						1

//#define COBRA_I2C_EN						0

#define COBRA_M3_GPIO_EN				0       //M3 UART0 UART1 enable

#define COBRA4_GPIO_EN					1

#define COBRA_TRNG_EN						1



#define COBRA_MCU_IS_VER1  			0

#define COBRA_IPC_EN						0   
#define COBRA_IPC_HOST_EN				0     //如果运行在 M3 上,这个要使能, 如果 IPC 驱动用在 M0 上, 这个要Disable
#define COBRA_IPC_EMB_EN				0			//如果运行在 M0 上,这个要Enable, 如果IPC 驱动运行在 M3 上, 定义 Disable 


//#define COBRA_I2S_EN						0

//#define COBRA_PDM_EN						0

#define COBRA_TIMER_EN					1
#define COBRA_DUALTIMER_EN			0
#define COBRA_SPI_EN						0           //SPI0, SPI1 enable

#define COBRA_SOFT_TIMER_EN			1

#define COBRA_RTC_EN						1

#define COBRA_IRDA_EN						0

#define COBRA_GPADC_EN					0
#define COBRA4_GPADC_EN					1
#define APP_COBRA4_GPADC_EN			0   //测试GPADC 

#define COBRA_TRIM_EN						0

#define COBRA_M0_GPIO_EN				0

#define COBRA_ECC_EN						0


//#define APP_COBRA_RTC_EN						1
#define APP_COBRA_IRDA_EN						0
#define APP_COBRA_UMCU_EN						0
#define APP_COBRA_VOICE_WAKEUP_EN		0
#define APP_COBRA_STANDBY_EN					0

#define APP_COBRA_UMCU_WAKEUP_M0_EN		0   // 用于UMCU 唤醒M0, M3 发送数据前,先把M0 唤醒



#define COBRA_WATCHDOG_EN				1

#define APP_COBRA_TRIM_EN				0

#define COBRA_SM4_EN						  0

#define COBRA4_SM4_EN							0

#define USE_RDW_HARDWARE_SUPPORT  1	

#define COBRA_AES_EN					0    // 2019/8/10  come from coney 

#define COBRA4_DRIVER_TEST_ENABLE		1

#define COBRA4_NEW_UART_DRIVER			0

//#define COBRA_SPWM_EN					   1
//#define COBRA_MPWM_EN            1

//#define COBRA_SRC_EN             0     //lianbo
//#define COBRA_ECC_NEW_EN         0     //lianbo 
//#define COBRA_SHA_EN             0     //lianbo
//#define COBRA_WM8978_EN          0     //lianbo

#if(COBRA_WM8978_EN == 1)
#undef  COBRA_I2C_EN
#define COBRA_I2C_EN             1
#define COBRA_I2C_NEW            1
#endif




//#define COBRA4_AUDIO_EN                  0
//#define COBRA4_PLL_EN                    0
//#define COBRA_PLL_VER2_EN                0

#if((COBRA4_PLL_EN == 1) && (COBRA_PLL_VER2_EN == 1))
#error "err pll config"
#endif


//#define COBRA4_BLE_WHITELIST_EN          0


//#define COBRA4_MMU_ENABLE            1             //内存管理使能

//#define COBRA_LINK_LIBRARY_ENABLE         1
//#define COBRA_LINKEN_SOFT_TIMER_ENABLE    1

#if (COBRA_LINK_LIBRARY_ENABLE == 1)
#undef   COBRA4_MMU_ENABLE
#define  COBRA4_MMU_ENABLE     1
#endif

#if((COBRA_MULTI_TIMER_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))
#error "不能使用两种 softtimer 实现方式"
#endif



#endif

