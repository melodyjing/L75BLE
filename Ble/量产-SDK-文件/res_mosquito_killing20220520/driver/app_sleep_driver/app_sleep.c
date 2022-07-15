/*
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
//liujian
//2019/5/10 add for cobra sleep control
*/
#include <cobra_config.h>
#include "rdw_mcu.h"
#include "app_bk_task.h"
#include "rdw_dma.h"
#include "cobra_udma.h"
#include "cobra_umcu.h"
#include "cobra_pcrm.h"
#include "rwip_config.h"  
#include "ke_event.h"
#include "rwip_int.h"  
#include "app_sleep.h"
#include "cobra_pinmux.h"
#include "rdw_gpio.h"
#include "cobra_m3_systick.h"
#include "cobra_soft_timer.h"
#include "cobra_aon_pmu.h"
#include "cobra_pcrm.h"
#include "cobra_rtc.h"
#include "cobra_ipc.h"
#include "cobra_watchdog.h"
//#include "rdw_ipc.h"
#include "app_system_trim.h"
#include "cobra_system.h"
#include "cobra_standby_program.h"
#include "cobra_ecc.h"
#include "cobra_cache_ctrl.h"
static int32_t key_timer_index;
static int32_t deep_sleep_require = 0;
extern struct rwip_env_tag        rwip_env;


bool app_cobra_kernel_sleep_check(void)
{
    return (ke_event_get_all() == 0);
}

unsigned int  app_cobra_sleep(void)
{
    uint8_t sleep_res = RWIP_ACTIVE;

    do
    {
				if(deep_sleep_require)
				{
						sleep_res = RWIP_DEEP_SLEEP;
						break;
				}
				
				//printf("rwip_env.prevent_sleep = %x \n", rwip_env.prevent_sleep);
       //检查内核是否有事件阻止它睡觉
        // Check if some kernel processing is ongoing (during wakeup, kernel events are not processed)
        if (((rwip_env.prevent_sleep & RW_WAKE_UP_ONGOING) == 0) && !app_cobra_kernel_sleep_check())
            break;

        // Processor sleep can be enabled
        sleep_res = RWIP_CPU_SLEEP;

        /************************************************************************
         **************              CHECK RW FLAGS                **************
         ************************************************************************/
        // First check if no pending procedure prevent from going to sleep
        if (rwip_env.prevent_sleep != 0)
            break;
				
			

    } while(0);

    if(sleep_res != RWIP_DEEP_SLEEP)
    {
        
    }
    return sleep_res;
}

//要放在保护位打开的程序中
void app_cobra_wakeup_pin_init(void)
{
	//2019/8/6 还需要更改
#if 0	
		//假设 IO2 有唤醒功能
			cobra_aon_pmu_io2_negedge_detect_enable(1);
	//		cobra_aon_pmu_io2_posedge_detect_enable(1);

			cobra_aon_pmu_mcu_io_posedge_detect_enable(0);
			cobra_aon_pmu_mcu_io_negedge_detect_enable(0);
	
		
			
			cobra_aon_pmu_wakeup_source0_selection(IO2_EDGE_INT_EVENT);
			cobra_aon_pmu_aon2mcu_source0_selection(IO2_EDGE_INT_EVENT);
			
			cobra_map_pinmodeset(PIN29_INDEX, PIN29_M0_GPIO_PIN13);
			cobra_pin_input_enable(PIN29_INDEX, 1);
			cobra_pin_pullup_enable(PIN29_INDEX, 1);
	
			cobra_aon_pmu_edge_irq_en(1);  //上拉电阻等要在前面配置好
			cobra_aon_pmu_io_lat_en(1);
#endif	
		
}

void app_cobra_enter_deep_sleep(void)
{
	 //liujian 2019/8/6
	//还需要更改
#if 0	
		//	app_cobra_rtc_uninit();
			cobra_aon_pmu_protection_en(1);   
			cobra_pcrm_shutdown_mode_en(1);
			cobra_aon_pmu_protection_en(0);  
	
			cobra_watchdog_disable();
			app_cobra_soft_timer_uninit();
#if COBRA_M3_SYSTICK_EN
			cobra_m3_sys_tick_uninit();
#endif
			
	
			app_cobra_wakeup_pin_init();
		//	app_cobra_voice_wakeup_init();  //voice wakeup enable
	
			cobra_aon_pmu_protection_en(1);             //保护位打开, 可以操作 aon PMU
			cobra_pcrm_pmu_clk_divider(0);
	
			cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_RCOSC8M);
	
			cobra_aon_pmu_shutdown_mode_enable(1);     //进入shutdown mode
	
	

//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG0, 0x124, 0x0004, 0);  //LF gated
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG1, 0x104, 0x0404, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG2, 0x106, 0x0000, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG3, 0x000, 0x0301, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG4, 0x003, 0x0001, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG5, 0x140, 0x0081, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG6, 0x11c, 0x003e, 1);   //last command
			
			cobra_aon_pmu_aon_xtal32k_enable(1);    		//open xtal32k
			cobra_aon_pmu_aon_csem_clk_cfg(CSEM_XTAL32K);
								
			cobra_aon_pmu_protection_en(0);						//关闭保护位
			
			cobra_pcrm_shutdown_mode_en(1);
			
			cobra_pcrm_p1_power_ctrl(0);
			cobra_pcrm_p2_power_ctrl(0);
			
//			cobra_pcrm_m3_power_enable_ctrl(0);
//			cobra_pcrm_rfm0_power_enable_ctrl(0);
			NVIC_SetDeepSleep();
			
			WFI();
#endif

}

//#pragma arm section code = ".relocate_code"
static unsigned int standby_en = 0;
void app_cobra_enter_standby(void)
{
	//liujian 2019/8/6
	//还需要更改
	
#if 0
			cobra_watchdog_disable();	
			app_cobra_soft_timer_uninit();
	
#if COBRA_M3_SYSTICK_EN
			cobra_m3_sys_tick_uninit();
#endif
	
			
			cobra_aon_pmu_protection_en(1);             //保护位打开, 可以操作 aon PMU
	
			cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_RCOSC8M);
	
			cobra_aon_pmu_d_vddio_postdrv_pd1_en(0);
			cobra_aon_d_vdd3_adc_en(1);
			cobra_aon_pmu_protection_en(0); 
	
//			cobra_pcrm_icytrx_isolate_en(1);
	
			cobra_pcrm_m3_sleep_mode_clock_enable(1);
//			cobra_pcrm_rfm0_sleep_mode_clock_enable(1);
			cobra_pcrm_bluetooth_sleep_mode_clock_enable(1);
			cobra_pcrm_eflash_ctrl_sleep_mode_clock_enable(1);
			cobra_pcrm_vim_sleep_mode_clock_enable(1);
			cobra_pcrm_spiflash_ctrl_sleep_mode_clock_enable(1);
			cobra_pcrm_system_bus_sleep_mode_clock_enable(1);
			cobra_pcrm_bt_cpu_sleep_mode_clock_enable(1);
			cobra_pcrm_vim_sys_bus_sleep_mode_clock_enable(1);
			cobra_pcrm_m3_sleep_mode_clock_enable(1);
			cobra_pcrm_uart0_sleep_mode_clock_enable(1);
			cobra_pcrm_uart1_sleep_mode_clock_enable(1);
			cobra_pcrm_vim_cache_sleep_mode_clock_enable(1);
			cobra_pcrm_pinmux_sleep_mode_clock_enable(1);
//			cobra_pcrm_uart10_sleep_mode_clock_enable(1);
//			cobra_pcrm_uart11_sleep_mode_clock_enable(1);
//			cobra_pcrm_timer10_sleep_mode_clock_enable(1);
			cobra_pcrm_apll_dclk_sleep_mode_clock_enable(1);
	//		cobra_pcrm_gpio0_sleep_mode_clock_enable(1);
	//		cobra_pcrm_gpio1_sleep_mode_clock_enable(1);
	
			cobra_aon_pmu_protection_en(1);
			cobra_aon_pmu_standby_mode_enable(1);
			
			app_cobra_wakeup_pin_init();
			
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG0, 0x124, 0x0010, 0); //close mcu_vd HF_CLK,LF_CLK(gating),irda_pclken=1
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG1, 0x104, 0x0000, 0); //close mcu_vd ldo_pd0dig1,ldo_pd1ldo1, open ldo_ret
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG2, 0x106, 0x0000, 0); //close cu_bgp_en
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG3, 0x000, 0x0301, 0); //set dcdc csem sleep_mode
//			//cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG4, 0x003, 0x0001, 0);
//			//cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG5, 0x140, 0x0081, 0);
//			cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG4, 0x11c, 0x000a, 1);  //close aon hf_clk, close rcosc32k, close rco8m
			
			cobra_aon_pmu_protection_en(0);
			
			cobra_pcrm_standby_mode_en(1);
			
			cobra_pcrm_p1_power_ctrl(0);
			cobra_pcrm_p2_power_ctrl(0);
			
//			cobra_pcrm_m3_power_enable_ctrl(0);
//			cobra_pcrm_rfm0_power_enable_ctrl(0);
			
			NVIC_ClearPendingIRQ(AON_EVENT0_IRQn);
			NVIC_EnableIRQ(AON_EVENT0_IRQn);
			
			standby_en = 1;
			NVIC_SetDeepSleep();
			GLOBAL_INT_STOP();			
			WFI();			

			cobra_aon_pmu_init();
			cobra_aon_pmu_protection_en(1);
			cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_XTAL48M);
			cobra_aon_pmu_protection_en(0);	
				
			cobra_pcrm_p1_power_ctrl(1);
			cobra_pcrm_p2_power_ctrl(1);
			
//			cobra_system_uart_init();
//			app_cobra_rtc_init();
			app_cobra_soft_timer_init();
#if COBRA_M3_SYSTICK_EN	
			cobra_m3_sys_tick_init();
#endif
#endif

}
//#pragma arm section code

unsigned int cobra_standby_mode_get(void)
{
			return standby_en;
}

void cobra_standby_mode_clear(void) 
{
			standby_en = 0;
}

void cobra_standby_wakeup_interrupt(void)
{
			cobra_pcrm_standby_mode_en(0);
	//liujian 2019/8/6 
//			cobra_aon_pmu_protection_en(1);
//			cobra_aon_pmu_edge_irq_en(0);
//			cobra_aon_pmu_io_lat_en(0);	
//			cobra_aon_pmu_protection_en(0);	
}




void app_key_pin10_debounce_callback(unsigned int param)
{
				printf("key 10 pressed \n");
#if (COBRA_ECC_EN)	
				app_cobra_eccmicro_test();   //测试, 按键之后, ECC 计算一次
#endif
	
//#if (APP_COBRA_TRIM_EN)
//				app_trim_init();   // 测试
//#else
//				deep_sleep_require = 1;
//#endif
	
}


void app_test_clear_deep_sleep_require(void)
{
			deep_sleep_require = 0;
}


void app_key_pin10_function(unsigned char  param)
{
			cobra_soft_timer_start(key_timer_index, 10, 0);
}


void app_sleep_key_setting(void)
{

//		cobra_map_pinmodeset(PIN10_INDEX, PIN10_GPIO1_PIN2);
		cobra_pin_pullup_enable(PIN10_INDEX, 1);
		cobra_pin_input_enable(PIN10_INDEX, 1);
		app_m3_gpio_set_direction(GPIO_PIN_10, GPIO_INPUT);
		app_m3_gpio_enable_interrupt(GPIO_PIN_10, GPIO_LEVEL_LOW, app_key_pin10_function);		

		key_timer_index = cobra_soft_timer_create(app_key_pin10_debounce_callback);		
}


//static unsigned int cobra_sleep_mode;
//void app_sleep_require_cmd(const uint8_t sleep_mode)
//{
//    struct hci_sleep_cmd *cmd = KE_MSG_ALLOC(HCI_COMMAND, 0, HCI_COBRA_SLEEP_CMD_OPCODE, hci_sleep_cmd);
//		cmd->sleep_mode = sleep_mode;
//		cobra_sleep_mode = sleep_mode;
//    hci_send_2_controller(cmd);
//}

//int hci_cobra_sleep_cmd_cmp_evt_handler(uint16_t opcode, struct hci_basic_cmd_cmp_evt const *event)
//{
//		volatile int i;
//		printf("M0 sleep opcode = %x...... \n", cobra_sleep_mode);
//		if((cobra_sleep_mode == 0xA0) || (cobra_sleep_mode == 0xA1))
//			return (KE_MSG_CONSUMED);
//		
//		cobra_aon_pmu_protection_en(1); 
//		cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_RCOSC8M);
//		cobra_pcrm_pmu_clk_divider(0);
//		cobra_aon_pmu_protection_en(0); 
//	
//	  cobra_ipc_m3_app2emb_trigger_set(IPC_M3_RESET_M0);

//	  //app_cobra_enter_deep_sleep();
//		//  app_cobra_enter_standby();
//	
//#if (APP_COBRA_STANDBY_EN)
//	  app_cobra_enter_standby_retention();
//#endif	
//	  printf("M3 wakeup ...... \n");
//	
//    return (KE_MSG_CONSUMED);
//}


#if (APP_COBRA_STANDBY_EN)


/*
切换到VIS_RAM  中, 在程序中进入standby 模式

*/
void app_cobra_enter_standby_retention(void)
{
			//cobra_aon_pmu_protection_en(1);   
			cobra_pcrm_standby_mode_en(1);
			//cobra_aon_pmu_protection_en(0);   
			standby_en = 1;
//			cobra_pcrm_vim_sram1_retention_en(1);
//			cobra_pcrm_vim_sram2_retention_en(1);
//			cobra_pcrm_vim_sram3_retention_en(1);
//			cobra_pcrm_vim_sram4_retention_en(1);
			copy_standby_program_to_vim_ram();
			cobra_enter_vim_run();
}

#endif




