/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_rdw_common.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/7/9
	Description :
          把一部分 cobra4_common  关于非蓝牙的函数移到这个文件中
	Changed Log	:
		Liujian  2021/7/9			- Creation

*****************************************************************************/
#include "rwip_config.h"
#include "cobra4_rdw_common.h"
#include "cobra4_common.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "cobra_uart.h"
#include "rwip.h"
#include "arch.h"
#include "cobra_watchdog.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra4_aon_top.h"

#if (COBRA_POWER_DOWN_ENABLE==1)
static uart_id  module_printuart_index __attribute__((section("retention_area"),zero_init));
static unsigned int module_work_mode __attribute__((section("retention_area"),zero_init));
#else
static uart_id  module_printuart_index;
static unsigned int module_work_mode;
#endif


/*
  2021/6/8
  添加 LDO trim
*/
void app_m4_trim_init(void)
{
    unsigned int chipid[4];
    uint8_t trim_sta = 0;
    uint8_t trim_value;

    cobra_information_flash_read(0x0040, 16, (uint8_t *)chipid);

    trim_sta = (chipid[1]>>2)&0x1;
    if(trim_sta == 0)
    {
        trim_value = (chipid[1]>>9)&0x1f;
        cobra4_aon_pd_top_protection_en(1);
        cobra4_aon_pd_top_d2a_bgln_vref_trim(trim_value);
        cobra4_aon_pd_top_protection_en(0);
    }

    trim_sta = (chipid[1]>>0)&0x1;
    if(trim_sta == 0)
    {
        trim_value = (chipid[1]>>3)&0x7;
        cobra4_aon_pd_top_protection_en(1);
        cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(trim_value);
        cobra4_aon_pd_top_protection_en(0);
    }

    trim_sta = (chipid[0]>>26)&0x1;
    if(trim_sta == 0)
    {
        trim_value = (chipid[0]>>27)&0x1f;
        cobra4_aon_pd_top_protection_en(1);
        cobra4_aon_pd_top_d2a_ldoanaforpll_trim(trim_value);
        cobra4_aon_pd_top_protection_en(0);
    }
}

#if (APP_SLEEP_MODE_ENABLE == 1)

void app_common_led_init(void)
{
    app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_OUTPUT);
}

void app_common_led_on_off(unsigned int en)
{
    if(en)
    {
        app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_HIGH);
    }
    else
    {
        app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_LOW);
    }
}
#endif

/*
 设置PIN5 为输入, 判断是否为 0
 如果为 0 , 说明要串口进入 HTL 模式, 使用中测仪 来测试模块
 如果为 1, 正常工作模式
 IO 口为输入上拉
*/
void app_common_io_init(void)
{
    app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_INPUT);
    cobra_pin_pullup_enable(PIN5_INDEX, 1);
    app_m3_gpio_disable_interrupt(GPIO_PIN_5);
}

BREAKER_WORK_MODE app_common_module_mode(void)
{

    if(app_m3_gpio_read_pin(GPIO_PIN_5))
        return MODE_NORMAL;
    else
        return MODE_HCITL;

//		return MODE_HCITL;    // 先强制让设备处于 中测仪 工作状态
}

BREAKER_WORK_MODE app_common_get_work_mode(void)
{
    return module_work_mode;
}

uart_id app_common_get_uart_print_index(void)
{
    return module_printuart_index;
}

void app_common_uart_hci_mode_init(void)
{
#if (CFG_PRODUCT_HCI_MODE_ENABLE == 1)
    app_common_io_init();

    module_work_mode = MODE_HCITL; //app_common_module_mode();

    if(module_work_mode == MODE_NORMAL)
    {
        module_printuart_index = M3_UART0;
    }
    else
    {
        module_printuart_index = M4_NULL;
    }
#else

    module_work_mode = MODE_NORMAL;
    module_printuart_index = M3_UART0;

#endif

}







const struct rwip_eif_api uart_api_normal =
{
    cobra4_uart_dummy_read,
    cobra4_uart_dummy_write,
    cobra4_uart_dummy_flow_on,
    cobra4_uart_dummy_flow_off,
};


#if  H4TL_SUPPORT

const struct rwip_eif_api uart_api =
{
    cobra4_uart0_read,
    cobra4_uart0_write,
    cobra4_uart0_flow_on,
    cobra4_uart0_flow_off,
};

const struct rwip_eif_api* rwip_eif_get(uint8_t type)
{
    const struct rwip_eif_api* ret = NULL;
    switch(type)
    {
    case 0:   //一般是CFG_EMB 模式,
    {
        if(app_common_get_work_mode() == MODE_NORMAL)
        {
            ret = &uart_api_normal;
        }
        else
        {
            ret = &uart_api;
        }
    }
    break;

    case 1:  //一般是 AHI 模式
    {

    }
    break;
    default:
    {
        ASSERT_INFO(0, type, 0);
    }
    break;
    }
    return ret;
}
#else
const struct rwip_eif_api* rwip_eif_get(uint8_t type)
{
	 const struct rwip_eif_api* ret = NULL;
	 ret = &uart_api_normal;
	 return ret;
}
#endif


void HardFault_Handler(void)
{
    printf("HardFault_Handler \n");
    app_cobra_watchdog_init(20, WATCHDOG_RESET);
    while(1);
}

void MemManage_Handler(void)
{
    printf("MemManage_Handler ....... \n");
    app_cobra_watchdog_init(20, WATCHDOG_RESET);
    while(1);
}

void BusFault_Handler(void)
{
    printf("BusFault_Handler ....... \n");
    app_cobra_watchdog_init(20, WATCHDOG_RESET);
    while(1);
}

void UsageFault_Handler(void)
{
    printf("UsageFault_Handler ....... \n");
    app_cobra_watchdog_init(20, WATCHDOG_RESET);
    while(1);
}

void DebugMon_Handler(void)
{
    printf("DebugMon_Handler ....... \n");
    app_cobra_watchdog_init(20, WATCHDOG_RESET);
    while(1);
}

#if (APP_METER_DEBUG_ENABLE)
//测试使用,系统重新启动的时候, P28  的 LED 闪烁
void app_meter_debug(void)
{
    unsigned int i;

    app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_28, 1);
    cobra_pin_pullup_enable(PIN28_INDEX, 1);
}

void app_debug_led(void)
{
    static unsigned int debug_flag = 0;
    if(debug_flag)
    {
        app_m3_gpio_write_pin(GPIO_PIN_28, 0);
        debug_flag = 0;
    }
    else
    {
        app_m3_gpio_write_pin(GPIO_PIN_28, 1);
        debug_flag = 1;
    }

}
#endif




