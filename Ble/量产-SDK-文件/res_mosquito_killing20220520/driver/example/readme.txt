注意事项：
1，ADC驱动例程  app_adc_sample.c
请打开宏COBRA_MULTI_TIMER_ENABLE APP_ADC_SAMPLE_COMMON_ENABLE和COBRA_APP_ADC_DATA_EN

2，EFLASH驱动例程  app_eflash_test.c
请打开宏COBRA_APP_EFLASH_TEST

3,SPI 驱动例程 app_spimaster_test.c
请打开宏COBRA_SPI_EN COBRA_APP_SPI_TEST和APP_LCD_ST7735S_R096HQ1101FL_ENABLE

4，RTC驱动例程 app_rtc_test.c
请打开宏COBRA_RTC_TEST

5,WDG驱动例程 app_wdg_test.c
请打开宏APP_ENABLE_WATCHDOAG和COBRA_APP_WDG_TEST 

6,RNG驱动例程 app_trn_test.c
请打开宏COBRA_APP_TRN_TEST

7,6路TIMER例程 app_timer_test.c
请打开宏COBRA_DUALTIMER_EN COBRA_TIMER_TEST和COBRA_APP_SIXTIMER_ENABLE，关闭COBRA_SOFT_TIMER_EN


备注：蓝牙透传需要打开UART_MSG_HANDLE_DWT。各项的宏不能同时打开，因为会有冲突。
