#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "reg_blecore.h"      // BLE Core registers
#include "cobra_system.h"
#include "cobra_timer.h"
#include "cobra_uart.h"
#include "cobra_soft_timer.h"
#include "cobra4A_linked_softtimer.h"
#include "cobra_multi_timer.h"
#include "cobra_button.h"
#include "app_button.h"
#include "rdw_gpio.h"
#include "prf.h"
#include "cobra4_rdw_common.h"
#include "cobra4_ble_common.h"

#if(APP_WECHAT_ENABLE == 1)
#include "app_wechat.h"
#endif

#if RADAR_MODUL_ENABLE
#include "unit_init.h"
#include "cobra_radar.h"
#endif

#if(APP_LAB_TEST== 1)
#include "app_lab_test.h"
#endif

#if (BLE_DEVICE1_UART_ENABLE)
#include "device1_uart.h"
#endif
#if (BLE_DEVICE1_UART_CLIENT_ENABLE)
#include "device1_client_uart.h"
#endif
//#include "device2_client_uart.h"
//#include "device3_client_uart.h"
#include "cobra4_common.h"
#include "rdw_console.h"
#include "rdw_uart_tx.h"

#if (APP_FLASH_LIGHT_NUMBERS)
#if (APP_MESH_CWHSL_LIGHT_FUNTION == 1)
#include "app_mesh_cwhsv_light.h"
#else
#include "app_tulip_light.h"
#endif
#endif

#if (COBRA_APP_ADC_DATA_EN)
#include "rdw_adc.h"
#endif

#if (COBRA_SOFT_UART_ENABLE == 1)
#include "cobra_soft_uart.h"
#endif

#if(DIANBIAO_MODULE_ENABLE == 1)
#include "app_iris_console.h"
#include "app_iris_calculate.h"
#include "app_realtek_console.h"
#include "app_calculate_meter.h"
#include "app_calculate_box.h"
#endif

#if (BLE_BP_SENSOR)
#include "blps.h"
#endif

#if defined(CFG_PRF_UART)
#include "new_uart.h"
#endif

#if (BLE_HR_SENSOR)
#include "hrps.h"
#endif

#include "cobra_mmu.h"
#include "app_rdw_shell.h"
#include "cobra_sleep.h"

#if(COBRA_POWER_DOWN_ENABLE)
#include "cobra4_power_down.h"
#endif

#if(APP_RF_NORMAL_PROTOCOL_ENABLE == 1)
#if ((APP_RF_MODULE_SEND_ADC_DEMO == 1)||(APP_RF_MODULE_RECV_ADC_DEMO == 1))
#include "rf_demo_adc_task.h"
#else
#include "app_wireless_test.h"
#endif

#include "app_wireless_mcu.h"
#endif

#if (APP_METER_AT_COMMAND == 1)
#include "app_meter_common.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan.h"
#endif

#if (COBRA_LOGGER_ENABLE == 1)
#include "app_easylogger.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_com.h"
#endif

#if(APP_AT_COMMAND_ENABLE == 1)
#include "app_at_cmd.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
#include "slave_com.h"
#endif

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include "app_mesh_tscns.h"
#include "app_mesh_util.h"
#endif

#if (BLE_APP_LED_BUTTON == 1)
#include "led_button.h"
#endif

#if (BLE_APP_RDW_VOICE)
#include "voice_task.h"
#include "app_voice.h"
#include "voice_master.h"
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
#include "tuya_ble_app_demo.h"
#include "tuya_ble_event.h"
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
#include "tuya_ble_services.h"
#endif

#if (APP_OTA_MATSER_ENABLE == 1)
#include "ota_master.h"
#endif

#if (APP_ADC_SAMPLE_ENABLE == 1)
#include "app_adc_sample.h"
#endif

#if (APP_MOUSE_ENABLE == 1)
#include "app_mouse.h"
#endif



#if (BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
void  app_m4_enter_clacluter_mode(void);
#endif

#if (BLE_MESH)
extern const struct prf_task_cbs* mal_prf_itf_get(void);
#endif // (BLE_MESH)

#ifdef COBRA_OTA_ENABLE
#include "custom_lib.h"
#endif

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include "cobra_light.h"

#if (BM_DETECT == 1)
#include "cobra_bm_detect.h"
#endif

#endif

#if(APP_WAKEUP_DEMO == 1)
#include "app_wakeup_demo.h"
#endif

#if (APP_AIRCLEAN_ENABLE == 1)
#include "app_airclean.h"
#endif

#if (APP_RAINBOW_ENABLE == 1)
#include "app_rainbow.h"
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "app_mosquito_killing.h"
#endif

#if(APP_DWT_MASTER == 1)
#include "master.h"
#endif

#if(APP_TUYA_BEACON_SDK_ENABLE)
#include "app_main.h"
#endif
/////////////////////////////////////


void app_m4_custom_application_init(void)
{
#if APP_LCD_ST7735S_R096HQ1101FL_ENABLE
    app_cobra_lcd_st7735s_init();
#endif

#if (COBRA_SOFT_UART_ENABLE == 1)
    cobra_soft_uart_init();
#endif

#if CFG_APP_CONSOLE_CMD
    rdw_console_task_init();
#endif


#if(BLE_APP_RDW_VOICE)
    rdw_voice_task_init();       //初始化录音
#endif

#if((BLE_APP_RECEIVE_VOICE) && (BLE_VOICE_RECOGNIZE_IS_ENABLE == 0))
    app_audio_drv_init();
#endif

#if(BLE_APP_RECEIVE_VOICE && (BLE_VOICE_RECOGNIZE_IS_ENABLE == 0))
    app_voice_play_task_init();   //I2S Play
#endif

// 电表的校表模式使能
#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)
    app_m4_enter_clacluter_mode();
#endif

#if (APP_COBRA_RTC_EN && !APP_SLEEP_MODE_ENABLE)
    app_cobra_rtc_init();
#endif

#if COBRA_AES_EN
    app_cobra4_aes_event_callback_init();
#endif

#if APP_COBRA4_GPADC_EN
    app_cobra4_gpadc_init();
#endif

#if (COBRA4_MMU_ENABLE == 1)
    cobra4_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)COBRA_SRAM_END);
//		cobra4_test_mmu();
#endif


#if (APP_METER_DEBUG_ENABLE)
    app_meter_debug();
#endif

#if((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))
    cobra_linked_softtimer_init_timer_lib();
#endif

#if(COBRA_MULTI_TIMER_ENABLE == 1)
    app_cobra_link_soft_timer_init();
#endif

#if((COBRA_BUTTON_EN == 1))
    cobra4A_button_init();
#endif



#if (APP_AIRCLEAN_ENABLE == 1)
    app_ariclean_init();
#endif

#if (APP_RAINBOW_ENABLE == 1)
    app_rainbow_init();
#endif

//#if (APP_MOSQUITO_KILLING_ENABLE == 1)
//    app_mosquito_killing_init();
//#endif

#if((DIANBIAO_MODULE_ENABLE == 1) && (CFG_APP_CONSOLE_CMD == 1))
    iris_dianbiao_console_init();   //用于判断串口是否超时
#endif

#if(APP_RDW_SHELL_ENABLE == 1)
    app_rdw_shell_init();
#endif

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
    app_mesh_tscns_init();
#endif
    cobra_system_watchdog_process();


#if (APP_SLEEP_MODE_ENABLE == 0 && COBRA_POWER_DOWN_ENABLE == 0)
    cobra_meter_clock_setting();
#endif

#if (APP_ENABLE_WATCHDOAG)
//2020/6/30 WatchDog enable
    app_cobra_watchdog_init(20000, WATCHDOG_RESET);
#endif


#if (APP_FLASH_LIGHT_NUMBERS)
    app_mesh_prov_init_timer();
    app_tulip_light_init();
#endif

#if ((BLE_APP_MESH == 1) && ((BLE_APP_MESH_TULIPE_LIGHT == 1)||(APP_MESH_CWHSL_LIGHT_FUNTION == 1)))
    app_mesh_util_init_mesh_eflash();
#endif

#if (APP_SLEEP_MODE_ENABLE == 1)
    app_common_led_init();
    cobra_sleep_init();
#endif

#if(APP_AT_COMMAND_ENABLE == 1)
    app_at_cmd_init();
#endif

#if (COBRA_LOGGER_ENABLE == 1)
    app_easylogger_init();
#endif

#if (APP_ADC_SAMPLE_ENABLE == 1||APP_ADC_SAMPLE_COMMON_ENABLE == 1)
    app_adc_sample_init();
#endif

#if (APP_MOUSE_ENABLE == 1)
    app_mouse_init();
#endif

#if(APP_RF_NORMAL_PROTOCOL_ENABLE == 1)
    app_wireless_mcu_init();

#if(APP_RF_MODULE_SEND_ENABLE == 1)
    printf("This is rf send ......... \n");
    app_cobra_electricity_gate_task_init();
#endif

#if(APP_RF_MODULE_RECEIVE_ENABLE == 1)
    printf("This is rf receive ......... \n");
    app_cobra_electricity_device_task_init();
#endif

#if(APP_RF_MODULE_SEND_ADC_DEMO == 1)
    printf("This is rf send adc demo ......... \n");
    rf_demo_adc_send_task_init();
#endif
#if(APP_RF_MODULE_RECV_ADC_DEMO == 1)
    printf("This is rf receive adc demo ......... \n");
    rf_demo_adc_receive_task_init();
#endif

#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    tuya_ble_app_init();
#endif

#if(DIANBIAO_MODULE_ENABLE == 1)
#if(DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1)
    printf("Meter ZT      %s \n", __TIMESTAMP__);
#else
    printf("Meter Realtek %s \n", __TIMESTAMP__);
#endif
#else
#if (COBRA_POWER_DOWN_ENABLE)
    if(app_cobra4_power_down_process())
        return;
#endif

#if(APP_WAKEUP_DEMO == 1)
    printf("This is a wakeup demo ......... \n");
    app_wakeup_demo_init();
#endif

#if (COBRA_APP_TRN_TEST)
    trng_test_init();
#endif

    printf("%s %s, memory: %d Bytes \n", RDW_SDK_VERSION, __TIMESTAMP__,  COBRA_SRAM_SIZE -( (unsigned int)((void*)&Image$$RW_IRAM1$$ZI$$Limit) - 0x20000000));

#endif

#if (APP_ADC_SAMPLE_COMMON_ENABLE)
    start_adc_sample();
#endif

#if (COBRA_APP_EFLASH_TEST)
    cobra_test_flash();
#endif

#if (COBRA_APP_TRN_TEST)
    trng_test();
#endif

#if (COBRA_RTC_DRIVER_TEST)
    rdw_rtc_test();
#endif

#if (COBRA_APP_SIXTIMER_ENABLE)
    cobra_timers_test();
#endif

#if(APP_TUYA_BEACON_SDK_ENABLE)
    app_tuya_beacon_init();
#endif

#if (CFG_PRODUCT_HCI_MODE_ENABLE == 1)
    if(app_common_get_work_mode() == MODE_HCITL)
    {
        hci_cmd_received(HCI_RESET_CMD_OPCODE, 0, NULL);
    }
#endif

#if ((APP_COBRA_RTC_EN == 0) && ((BLE_APP_MESH_TULIPE_LIGHT == 1)||(APP_MESH_CWHSL_LIGHT_FUNTION == 1)))
    app_cobra_tuiple_light_timer_init();
#endif

#if RADAR_MODUL_ENABLE	
    radar_bsp_init();
#endif

#if(APP_WECHAT_ENABLE == 1)		
   app_wechat_ble_init();
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
    app_mosquito_killing_init();
#endif		
}


void app_cobra_init(void)
{
    //各种Driver 的初始化, 让变量指向各个driver 的初始地址

#if (COBRA_POWER_DOWN_ENABLE)
    app_poweron_lowpower_init();
#else
    cobra_system_init();

    cobra_system_power_clk_init();

    app_m3_gpio_init();
#endif


#if (APP_AIRCLEAN_ENABLE == 1)

    app_airclean_io_init();

#elif (APP_RAINBOW_ENABLE == 1)

    app_rainbow_io_init();
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)
    app_mosquito_killing_io_init();
#else
    app_meter_io_init();    //将不用的IO 口都设置为不漏电流的方式
#endif

    app_common_uart_hci_mode_init();

    cobra_system_uart_init();

#ifdef COBRA_OTA_ENABLE
    custom_appm_init(appm_init);
    custom_platform_reset(platform_reset);
    custom_rdw_custom_define_profile(rdw_custom_define_profile);
//	 custom_rdw_rwip_eif_get(rwip_eif_get);
#endif

#if ((APP_RF_NORMAL_PROTOCOL_ENABLE == 1)	 || (APP_ADC_SAMPLE_ENABLE == 1) || (APP_MOUSE_ENABLE == 1))
    cobra_set_calculate_mode(RF_MODE_TEST);
#endif

#if (COBRA_POWER_DOWN_ENABLE == 0)
    //trim value
    app_m4_trim_init();    //低功耗时, 不能使用, 否则重起
#endif


#if ((COBRA4C_INSIDE_SPI_FLASH_ENABLE) && (APP_RUN_FROM_SPI_FLASH_ENABLE == 0))  // 只有程序在 EFLASH 运行时,才执行这个
    app_cobra4c_spi_flash_read_id();
#endif

}




void app_m4_timer_init(void)
{
    app_cobra_init_timer0_clock();

#if LOW_POWER_WAKE_UP_BY_UART
    cobra_low_power_timer0_init();
#else
#if (COBRA_SOFT_TIMER_EN==1)
    app_cobra_soft_timer_init();          //使用软件定时器
#endif
#endif
}

void app_cobra_ble_stack_init(void)
{
    uint32_t error = RESET_NO_ERROR;

#if (COBRA_POWER_DOWN_ENABLE)
    if(ble_deep_sleep == false)
    {
        rwip_init(error);	            //蓝牙协议初始化
    }
#else
    rwip_init(error);	            //蓝牙协议初始化
#endif
}


/*
2020/10/26
把不用到的 IO 口,设置为输入
GPIO5 , GPIO31  GPIO9, GPIO11 GPIO12  都有使用
*/
void app_meter_io_init(void)
{
    app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_HIGH_Z);   //J20
    app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_HIGH_Z);   //J3
    app_m3_gpio_set_direction(GPIO_PIN_30, GPIO_HIGH_Z);   //J9


//		app_m3_gpio_set_direction(GPIO_PIN_2, GPIO_HIGH_Z);
//		app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_HIGH_Z);

    app_m3_gpio_set_direction(GPIO_PIN_6, GPIO_HIGH_Z);
    //app_m3_gpio_set_direction(GPIO_PIN_7, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_10, GPIO_HIGH_Z);

    app_m3_gpio_set_direction(GPIO_PIN_11, GPIO_INPUT);
    app_m3_gpio_set_direction(GPIO_PIN_12, GPIO_INPUT);

    app_m3_gpio_set_direction(GPIO_PIN_13, GPIO_INPUT);
    app_m3_gpio_set_direction(GPIO_PIN_14, GPIO_INPUT);

    app_m3_gpio_set_direction(GPIO_PIN_15, GPIO_HIGH_Z);

    app_m3_gpio_set_direction(GPIO_PIN_18, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_19, GPIO_HIGH_Z);

    app_m3_gpio_set_direction(GPIO_PIN_22, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_23, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_24, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_25, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_26, GPIO_HIGH_Z);

    cobra_pin_pullup_enable(PIN22_INDEX, 1);
    cobra_pin_pullup_enable(PIN23_INDEX, 1);
    cobra_pin_pullup_enable(PIN24_INDEX, 1);
    cobra_pin_pullup_enable(PIN25_INDEX, 1);
    cobra_pin_pullup_enable(PIN26_INDEX, 1);



#if(COBRA4C_INSIDE_SPI_FLASH_ENABLE == 0)
    app_m3_gpio_set_direction(GPIO_PIN_0, GPIO_HIGH_Z);  //J18
    app_m3_gpio_set_direction(GPIO_PIN_1, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_2, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_16, GPIO_HIGH_Z);
    app_m3_gpio_set_direction(GPIO_PIN_17, GPIO_HIGH_Z);

    cobra_pin_pullup_enable(PIN0_INDEX, 1);
    cobra_pin_pullup_enable(PIN1_INDEX, 1);
    cobra_pin_pullup_enable(PIN2_INDEX, 1);
    cobra_pin_pullup_enable(PIN3_INDEX, 1);
    cobra_pin_pullup_enable(PIN16_INDEX, 1);
    cobra_pin_pullup_enable(PIN17_INDEX, 1);

#endif




    cobra_pin_pullup_enable(PIN4_INDEX, 1);

    cobra_pin_pullup_enable(PIN6_INDEX, 1);
    cobra_pin_pullup_enable(PIN7_INDEX, 1);
    cobra_pin_pullup_enable(PIN8_INDEX, 1);
    cobra_pin_pullup_enable(PIN10_INDEX, 1);
    cobra_pin_pullup_enable(PIN15_INDEX, 1);

    cobra_pin_pullup_enable(PIN18_INDEX, 1);
    cobra_pin_pullup_enable(PIN19_INDEX, 1);



    cobra_pin_pullup_enable(PIN8_INDEX, 1);
    cobra_pin_pullup_enable(PIN28_INDEX, 1);
    cobra_pin_pullup_enable(PIN29_INDEX, 1);
    cobra_pin_pullup_enable(PIN30_INDEX, 1);

}

void app_deep_sleep_disable_interrupt(void)
{
    NVIC_DisableIRQ(COBRA4_UART0_IRQn);
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(COBRA4_TIMER1_IRQn);
    NVIC_DisableIRQ(COBRA4_TIMER0_IRQn);
    NVIC_DisableIRQ(AON_EVENT0_IRQn);
    NVIC_DisableIRQ(AON_EVENT3_IRQn);
    NVIC_DisableIRQ(AON_EVENT4_IRQn);
    NVIC_DisableIRQ(PAD_TRIGGER_SINGLE_IRQ);


}

void app_deep_sleep_enable_interrupt(void)
{
    NVIC_EnableIRQ(COBRA4_UART0_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(COBRA4_TIMER1_IRQn);
    NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);

    // NVIC_EnableIRQ(PAD_TRIGGER_SINGLE_IRQ);
    //	NVIC_EnableIRQ(AON_EVENT0_IRQn);
    NVIC_EnableIRQ(AON_EVENT3_IRQn);
    NVIC_EnableIRQ(AON_EVENT4_IRQn);
}


void app_custom_application_loop(void)
{
#if(COBRA_MULTI_TIMER_ENABLE == 1)
    cobra_softtimer_loop();
#endif

#if(APP_AT_COMMAND_ENABLE == 1) 

#if(APP_MESH_CWHSL_LIGHT_FUNTION == 1)	
	  if(lt_is_ate_mode())
		{
			while(1)
			{
        app_at_cmd_loop();
			  rwip_schedule();
      }			
		}
		else
#endif			
		{
			 app_at_cmd_loop();
		}
#endif

#if (APP_ENABLE_WATCHDOAG)
    cobra_watchdog_set(30000);   //喂狗
#endif

#if ((BLE_APP_MESH == 1) && ((BLE_APP_MESH_TULIPE_LIGHT == 1)||(APP_MESH_CWHSL_LIGHT_FUNTION == 1)))
    app_mesh_tscns_loop_process();
#endif


#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1) && (BM_DETECT == 1))
#if(APP_TULIP_BATTER_DETECT_VERSION2 == 0)
    bm_detect();
#endif
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    tuya_sched_execute();
#endif

#if(APP_RF_MODULE_SEND_ADC_DEMO == 1)
    rf_demo_adc_send_task_loop();
#endif

#if (COBRA_APP_WDG_TEST)
    app_wdg_test();
#endif

#if(APP_TUYA_BEACON_SDK_ENABLE)
    app_tuya_beacon_loop();
#endif

#if APP_MESH_CWHSL_LIGHT_FUNTION
    publish_loop_handler();
#endif

#if CHECK_SCAN_FOR_RESTARE
    restart_loop_handler();
#endif

#if RADAR_MODUL_ENABLE
    loop_event();
#endif

#if(APP_WECHAT_ENABLE == 1)		
   app_wechat_ble_loop();
#endif

}



#if(BLE_DIANBIAO_CALCUALTE_ENABLE == 1)

/*
Nordic 设置时, 把 address  的最高位放在 prefix
base = address
prefix = address  的最低Byte

0x11223344  ----> 0x44112233
0x11111122  ----> 0x22111111
0x33111122  ----> 0x22331111
*/
unsigned int app_calcultae_convert_sync_word(unsigned int address)
{
    unsigned int temp1;
    unsigned int temp2;

    temp1 = address; //address;
    temp1 <<= 24;
    temp1 &= 0xff000000;

    temp2 = address;
    temp2 >>= 8;
    temp2 &= 0x00ffffff;

    temp1 += temp2;

    return temp1;
}


void  app_m4_enter_clacluter_mode(void)
{
    uint16_t  frequency;
    uint32_t  address;
    uint8_t   pluse_type;
    uint32_t  i;
    uint8_t mode = 0;

    APP_CAL_FREQUENCY app_calculate_param;
    struct bd_addr  temp_bdaddr;
    unsigned char temp_length ;
    long long calculate_comm_address;
    unsigned char temp_address[4];
    unsigned char temp;
    unsigned char  *p ;
    unsigned char  real_frequency_number;

    if(cobra_get_calculate_mode() == RF_MODE_SEND)
    {

        nvds_put(IRIS_CALCULATE_SETFLAG, IRIS_CALCULATE_LENGTH, &mode);

        printf("send mode \n");

#if (APP_ENABLE_WATCHDOAG)
//2020/6/30 WatchDog enable
        app_cobra_watchdog_init(5000, WATCHDOG_NO_ACTION);
#endif
        cobra_meter_clock_setting();
        ble_meter_struct * p_param = cobra_get_calculate_param();

        app_calculate_param.frequency_number = p_param->frequency_number;
        for(i = 0; i < p_param->frequency_number; i++)
        {
            app_calculate_param.frequecny[i] = p_param->frequency[i];
        }


        pluse_type = p_param->pluse_type;

        app_calculate_param.power_level = p_param->power_level;

//2021/5/14  杭州 刘健 修改

        p = p_param->comm_address;

        printf("address 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", p[0], p[1], p[2], p[3], p[4], p[5]);

        address = p[2];
        address <<= 8;
        address += p[3];
        address <<= 8;
        address += p[4];
        address <<= 8;
        address += p[5];

        printf("type = %d ,address = %x\n", pluse_type, address);

        address = app_calcultae_convert_sync_word(address);


        real_frequency_number = 0;

        for(i = 0; i < app_calculate_param.frequency_number; i++)
        {
            if(app_calculate_param.frequecny[i] != 0xFFFF)
            {
                real_frequency_number++;
            }
        }

        app_calculate_param.frequency_number = real_frequency_number;

        app_iris_calculate_meter_task(&app_calculate_param, address, pluse_type);

        GLOBAL_INT_START();
        NVIC_DisableIRQ(COBRA4_UART0_IRQn);
        iris_calibration_loop();
    }
}

#endif





