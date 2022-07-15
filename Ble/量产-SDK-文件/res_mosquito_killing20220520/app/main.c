/**@file            main.c
* @brief
* @details
      1. app_config.h ����һЩȫ������, ���� ������, ���д�����Щ profile ��Ӧ��
         Default ��һ�����������豸 �� �¶ȼ�, �ͻ����Բο�ʹ��

      2. cobra_config.h  ��Ҫ����������Щ������� ʹ��, ���õĿ�������Ϊ0 ,�����ʵ����ٴ������

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

    GLOBAL_INT_STOP();            //�ر��ж�

    app_cobra_init();                //ϵͳ��ʼ��, ������������״̬

    if((cobra_get_calculate_mode() == RF_MODE_NORMAL) || (cobra_get_calculate_mode() == RF_MODE_TEST))
    {

        app_cobra_ble_stack_init();

        cobra_system_interrupt_enable();     //ʹ�������ж�

        app_m4_timer_init();

        cobra_app_background_task_init();

#if COBRA_UDMA_EN
        app_rdw_udma_init();
#endif
    }

    GLOBAL_INT_START();             //��ȫ���ж�

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

