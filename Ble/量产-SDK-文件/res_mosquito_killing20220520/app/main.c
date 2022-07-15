/**@file            main.c
* @brief
* @details
      1. app_config.h 中有一些全局配置, 蓝牙 的配置, 还有打开了哪些 profile 的应用
         Default 有一个串口蓝牙设备 和 温度计, 客户可以参考使用

      2. cobra_config.h  主要是配置有哪些外设程序 使能, 不用的可以配置为0 ,可以适当减少代码体积

* @author      			Liujian
* @date             2020-04-09
* @version			    V1.0
* @copyright		    Copyright (c)	2014-2021 Radiawave Ltd.
*/
#include "cobra_system.h"
#include "cobra4_new_uart.h"

#if(DIANBIAO_MODULE_ENABLE == 1)
#include "app_iris_calculate.h"
#include "app_iris_console.h"
#endif

#include "cobra4_common.h"
#include "rdw_dma.h"

#if(COBRA_POWER_DOWN_ENABLE)
#include "cobra4_power_down.h"
#endif

int main(int argc, char * argv[])
{

    GLOBAL_INT_STOP();            //关闭中断

    app_cobra_init();                //系统初始化, 进入正常工作状态

    if((cobra_get_calculate_mode() == RF_MODE_NORMAL) || (cobra_get_calculate_mode() == RF_MODE_TEST))
    {

        app_cobra_ble_stack_init();

        cobra_system_interrupt_enable();     //使能蓝牙中断

        app_m4_timer_init();

        cobra_app_background_task_init();

#if COBRA_UDMA_EN
        app_rdw_udma_init();
#endif
    }

    GLOBAL_INT_START();             //开全局中断

    app_m4_custom_application_init();
		
    while(1)
    {
        rwip_schedule();    // schedule all pending events

        app_custom_application_loop();

        GLOBAL_INT_DISABLE();

        switch(rwip_sleep())
        {
        case RWIP_DEEP_SLEEP:
        {
#if (APP_SLEEP_MODE_ENABLE == 1)
            app_common_led_on_off(1);
            cobra_sleep_enter();
            WFI();
            cobra_sleep_exit();
            app_common_led_on_off(0);
#else
            WFI();
#endif
            break;
        }

        case RWIP_CPU_SLEEP:
        {
            WFI();
            break;
        }
        case RWIP_ACTIVE:
            break;
        default:
        {
            // nothing to do.
        } break;
        }

        GLOBAL_INT_RESTORE();
    }
}

