#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "cobra_system.h"
#include "cobra_pcrm.h"
#include "sysctrl.h"
#include "arch.h"
#include <math.h>
#include <stdlib.h>
#include "rwip.h"      // RW SW initialization
#include "app.h"
#include "rdw_dma.h"
#include "rdw_mcu.h"
#include "voice_task.h"
#include "app_rdw_voice.h"

#include "cobra_umcu.h"
#include "rdw_mcu.h"
#include "rdw_console.h"
//#include "rdw_ota.h"
#include "app_sleep.h"
#include "new_uart.h"
#include "rdw_gpio.h"
#include "cobra_cortex_m3_nvic.h"
#include "cobra_cortex_m4_nvic.h"
#include "cobra_pcrm.h"
#include "cobra_uart.h"
#include "cobra_pinmux.h"
#include "cobra_i2c.h"
#include "cobra_rtc.h"
#include "cobra_trng.h"
//#include "rdw_ipc.h"
#include "cobra_timer.h"
#include "cobra_dualtimer.h"
#include "cobra_m3_systick.h"
#include "cobra_eflash_ctrl.h"
#include "cobra_soft_timer.h"
#include "cobra_irda.h"
#include "cobra_watchdog.h"
#include "cobra_ecc.h"
#include "cobra_gpadc.h"
#include "app_bk_task.h"
#include "LCD_Z240IT010.h"   
#include "app_nrf24l01.h"
#include "app_csem.h"
#include "app_electricity.h"
#include "reg_blecore.h"      // BLE Core registers
#include "app_network.h"
#include "app_test_csem.h"

void cobra_jtag_enable(void);

uint32_t error = RESET_NO_ERROR;


void app_m3_init(void)
{
	//各种Driver 的初始化, 让变量指向各个driver 的初始地址
		cobra_system_init();
	//为弥补bootload 带来的 JTAG 不能工作的问题, 让JTAG   正常工作
		cobra_jtag_enable();    //cobra2 uesd
	
		cobra_system_m3_init();
	
		cobra_system_uart_init();

#if COBRA_M3_SYSTICK_EN	

		cobra_m3_sys_tick_init();

#endif
	
//		cobra_system_interrupt_enable();
	
}

// Creation of uart external interface api
//const struct rwip_eif_api ipc_api =
//{
//        app_rdw_ipc_host_read,
//				app_rdw_ipc_host_write,
//				app_rdw_ipc_host_flow_on,
//				app_rdw_ipc_host_flow_off,
//};

#if 0
const struct rwip_eif_api uart_api =
{
    cobra_m3_uart1_read,
    cobra_m3_uart1_write,
    cobra_m3_uart1_flow_on,
    cobra_m3_uart1_flow_off,
};


const struct rwip_eif_api* rwip_eif_get(uint8_t type)
{
    const struct rwip_eif_api* ret = NULL;
    switch(type)
    {
        case RWIP_EIF_AHI:
        {
         //   ret = &uart_api;
        }
        break;
        #if (BLE_EMB_PRESENT) || (BT_EMB_PRESENT)
        case RWIP_EIF_HCIC:
        {
        //    ret = &uart_api;
        }
        break;
        #elif !(BLE_EMB_PRESENT) || !(BT_EMB_PRESENT)
        case RWIP_EIF_HCIH:
        {
        //    ret = &uart_api;
        }
        break;
        #endif // (BLE_EMB_PRESENT)			
//				case RWIP_EIF_HCIC:   //RWIP_EIF_IPC:   //liujian
//				{
//		//				ret = &ipc_api;
//				}
        default:
        {
            ASSERT_INFO(0, type, 0);
        }
        break;
    }
    return ret;
}
#endif

void cobra_jtag_enable(void)
{
		cobra_pcrm_pinmux_active_mode_clock_enable(1);
	
		cobra_map_pinmodeset(PIN0_INDEX, PIN0_JTAG_TDO);
		cobra_map_pinmodeset(PIN1_INDEX, PIN1_JTAG_TCK);
		cobra_map_pinmodeset(PIN2_INDEX, PIN2_JTAG_TDI);
	
//		cobra_map_pinmodeset(PIN4_INDEX, PIN4_JTAG_TCK1);
		cobra_map_pinmodeset(PIN5_INDEX, PIN5_JTAG_NTRST0);
//		cobra_map_pinmodeset(PIN6_INDEX, PIN6_JTAG_NTRST1);
	
//		cobra_map_pinmodeset(PIN8_INDEX, PIN8_JTAG_TDI1);
	
//		cobra_map_pinmodeset(PIN10_INDEX, PIN10_JTAG_TDO1);
	
		cobra_map_pinmodeset(PIN15_INDEX, PIN15_JTAG_TMS0_SWDO_SWDOEN);
//		cobra_map_pinmodeset(PIN16_INDEX, PIN16_JTAG_TMS1_SWDO_SWDOEN);
//		cobra_map_pinmodeset(PIN17_INDEX, PIN17_JTAG_SEL0);
//		cobra_map_pinmodeset(PIN18_INDEX, PIN18_JTAG_SEL1);
}


int main(void)
{
		GLOBAL_INT_STOP();
	
		app_m3_init();
	
		app_m3_gpio_init();

#ifndef CFG_NRF24L01_DEMO
	//test
//		cobra_map_pinmodeset(PIN11_INDEX, PIN11_GPIO1_PIN3);
		app_m3_gpio_set_direction(GPIO_PIN_11, GPIO_OUTPUT);
		app_m3_gpio_write_pin(GPIO_PIN_11, GPIO_HIGH);
#endif
		app_m3_i2c_init();  
	
	//	rdw_init_i2s();  //need add 2019/4/2
	//	rdw_init_pdm();	   //need add 2019/4/2
		
//		app_m3_ipc_host_init();
	
		app_rdw_udma_init();  

#if (CFG_LCD_DEMO)
		app_cobra_lcd_init();
#endif	


	//	cobra_dualtimer_set_frequency(DUALTIMER2, 10);
	

	
		srand(1);
		
		rwip_init(error);

		cobra_system_interrupt_enable();
		
#if BLE_APP_RDW_VOICE
	rdw_voice_task_init();
#endif

#if 	CFG_APP_CONSOLE_CMD
	 rdw_console_task_init();
#endif

	 cobra_app_background_task_init();

#if (COBRA_ECC_EN)
	 cobra_app_eccmicro_task_init();
#endif

//	 app_cobra_watchdog_init(3000, WATCHDOG_RESET);
#if (APP_COBRA_UMCU_WAKEUP_M0_EN)
		app_cobra_umcu_wakeup_m0_init();
//		app_cobra_umcu_wakeup_m0_run();
#endif

#if 1
#if (APP_COBRA_RTC_EN) && (COBRA_RTC_EN)
	 app_cobra_rtc_init();
#endif

#if (APP_COBRA_IRDA_EN) && (COBRA_IRDA_EN)
	 app_cobra_irda_rx_init();
#endif

#if (APP_COBRA_VOICE_WAKEUP_EN) && (COBRA_UMCU_EN)
	// app_cobra_voice_wakeup_init();	 
#endif
	
	 app_cobra_gpadc_init();

//LCD 和 NRF24L01 没有使用的时候,可以使用这个函数来测试 
//#ifndef CFG_LCD_DEMO 
//#ifndef CFG_NRF24L01_DEMO
//	 app_sleep_key_setting();
//#endif
//#endif	 

	 app_cobra_init_systick();

	 app_cobra_soft_timer_init();
		
#endif


#if CFG_NRF24L01_DEMO
		//app_cobra_nRF24L01_init();
		app_state_grid_gateway_init();
#endif

//#if (CFG_LCD_DEMO)
//		app_cobra_lcd_init();
//#endif	

#if CFG_CSEM_RF_PROTOCOL
		 app_csem_init();
#endif

#if CFG_CSEM_RF_PROTOCOL
    		app_cobra_electricity_gate_task_init();
				printf("This is gateway ......... \n");
	//			app_test_send_callback(0);
//			app_cobra_electricity_device_task_init();
#endif

#if CFG_TEST_CSEM_ENABLE
			app_test_csem_init();
#endif

	 printf("RDW  %s \n", __DATE__);

	
	 GLOBAL_INT_START();
	 

	 while(1)
    {
        // schedule all pending events
        rwip_schedule(); 
			
			
				GLOBAL_INT_DISABLE();
				switch(rwip_sleep())
        {
            case RWIP_DEEP_SLEEP:
            {
              // printf("enter deep sleep \n");
							// app_sleep_require_cmd(0x55);
							// app_test_clear_deep_sleep_require();	
							//	WFI();
            }
						break;
            case RWIP_CPU_SLEEP:
            {
						//	 printf("cpu sleep \n");
						//	uint32_t irq_stat      = ble_intcntl_get();
						//	printf("0x%04x \n", irq_stat);
						//	cobra_system_interrupt_enable();
							WFI();
            }
						break;
            case RWIP_ACTIVE:
					
						break;
            default:
            {
                // nothing to do.
            } break;
        }
				//WFI();
			  GLOBAL_INT_RESTORE();    
    }
	
}

/*
  这个函数主要用于profile 内部调用, 找到 自定义的 pforile.
  自己写的Profile 请添加到这里


*/
const struct prf_task_cbs* rdw_custom_define_profile(uint32_t task_id)
{
		const struct prf_task_cbs* prf_cbs = NULL;
#if BLE_APP_RDW_VOICE
		if(task_id == TASK_ID_VOICE)
		{
				prf_cbs = voice_prf_itf_get();
		}
#endif

#if BLE_APP_OTA
		if(task_id == TASK_ID_OTA)
		{
				prf_cbs = ota_prf_itf_get();
		}
#endif		
		
#if BLE_APP_UART
		if(task_id == TASK_ID_UART)
		{
				prf_cbs = uart_prf_itf_get();
		}
#endif	
		
		return prf_cbs;
	
}

