/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_system.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/29

	Description :   
	

	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/

#ifndef __COBRA_SYSTEM_H__
#define __COBRA_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>
#include "reg_blecore.h"      // BLE Core registers
#if COBRA_POWER_DOWN_ENABLE
#include "reg_ipcore.h" 
#endif
#include "cobra4C_eflash_ctrl.h"
#include "rdw_mcu.h"
#include "cobra4_systick.h"
#include "cobra_timer.h"
#include "cobra_rtc.h"
#include "cobra_soft_timer.h"
#include "app_bk_task.h"
#include "cobra4_aes.h"
#if(COBRA4_DRIVER_TEST_ENABLE)
#include "cobra4_driver_test.h"
#endif
#include <stdio.h>
#include "nvds.h"
#include "rdw_mcu.h"
#include "cobra_system.h"
#include "cobra_aon_dcdc.h"
#include "cobra_aon_pmu.h"
#include "cobra4_aon_top.h"
#include "cobra4_d_vad.h"
#include "cobra4_aon_wk.h"
#include "cobra4_aes.h"
#include "cobra4_gpadc.h"
#include "cobra_cache_ctrl.h"
#include "cobra_pcrm.h"
#include "cobra_pinmux.h"
#include "cobra_spi_flash_ctrl.h"
#include "cobra_ceva_baseband.h"
#include "cobra_m0_sysctrl.h"
#include "cobra_uart.h"
#include "cobra_m3_gpio.h"
#include "cobra_m4_gpio.h"
#include "cobra_udma.h"
#include "cobra_watchdog.h"
#include "cobra_i2c.h"
#include "cobra_ipc.h"
#include "cobra_pdm.h"
#include "cobra_i2s.h"
#include "cobra_spi.h"
#include "cobra_rtc.h"
#include "cobra_umcu.h"
#include "cobra_ecc.h"
#include "cobra_timer.h"
#include "cobra_irda.h"
#include "cobra_dualtimer.h"
#include "cobra_soft_timer.h"
#include "cobra_trng.h"
#include "cobra_gpadc.h"
#include "cobra_m0_gpio.h"
#include "cobra_sm4.h"
#include "cobra4_sm4.h"
//#include "cobra4A_mpwm.h"
#include "cobra4A_spwm.h"
#include "cobra4_audio.h"
#include "cobra_sleep.h"
//bluetooth
#include "rwip_config.h"
#include "app.h"
#include "co_bt.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"

#if APP_SCAN_DEVICE_ENABLE
#include "device1_scan.h"
#endif

#include "cobra_src.h"
#include "cobra_agc.h"


typedef enum
{
	 RF_MODE_NORMAL  = 0,
   RF_MODE_RECEIVE = 1,	
	 RF_MODE_SEND    = 2,
	 RF_MODE_TEST    = 3,
}RF_MODE;

typedef enum
{
	  RESET_PIN_MODE,
	  RESET_WATCHDOG_MODE,
	  RESET_SOFT_MODE,
	  RESET_PD_POR_MODE,
	  RESET_ERROR,
	
}RESET_TYPE;

void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);

void cobra_system_init(void);
void cobra_system_power_clk_init(void);

void cobra_system_icytrx_xtal_48m_enable(unsigned int en);
void cobra_system_switch_to_48m(unsigned int en);
unsigned int cobra_system_get_cpu_frequency(void);
void cobra_system_reboot_jump(uint32_t sp,uint32_t pc);

unsigned int cobra_system_get_m3_ahb_clk(void);

unsigned int cobra_system_get_m0_ahb_clk(void);

unsigned int cobra_system_get_m3_apb0_clk(void);
unsigned int cobra_system_get_m3_apb1_clk(void);
unsigned int cobra_system_get_m3_apb2_clk(void);
unsigned int cobra_system_get_m3_apb3_clk(void);

void cobra_system_set_m3_apb0_clk(unsigned int clock_value);

unsigned int cobra_system_get_m0_apb10_clk(void);

void cobra4_systemInit (void);

void cobra_reset_information(void);

void cobra_system_clock_setting(void);

void cobra_system_clock_reset_setting(void);

void cobra_nvds_init(void);

void cobra_system_uart_init(void);

void platform_reset(uint32_t error);

unsigned int cobra_system_ipc_speed(void);

void cobra_system_ipc_speed_set(unsigned int speed);

void cobra_system_interrupt_enable(void);

void app_key_setting(void);

void cobra4_jtag_enable(void);

void app_deep_sleep_disable_interrupt(void);

void app_deep_sleep_enable_interrupt(void);

unsigned int cobra_system_get_cpu_frequency(void);

RF_MODE cobra_get_calculate_mode(void);

void cobra_set_calculate_mode(RF_MODE mode);

void cobra_system_nvds_save_to_eflash(void);

void cobra_system_watchdog_process(void);

void cobra_meter_clock_setting(void);

uint16_t cobra_get_calculate_frequency(void);

uint32_t cobra_get_calculate_address(void);

uint8_t cobra_get_calculate_pluse_type(void);

uint8_t cobra_nvds_param_ok(void);

#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
ble_meter_struct * cobra_get_calculate_param(void);
#endif

unsigned int cobra_system_nvds_is_modified(void);

unsigned char * cobra_nvds_get_prov_uuid(void);

void cobra_system_nvds_reset(void);

void cobra_nvds_produce_debug(void);

void ota_platform_reset(uint32_t error);

void platform_watchdog_reset(uint32_t error);

RESET_TYPE cobra_reset_type(void);

#endif

