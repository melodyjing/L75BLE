
#include "app_config.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "app_button.h"
#include "app_task.h"
#include "cobra4A_mpwm.h"
#include "cobra_multi_timer.h"
#include "cobra_dualtimer.h"
#include "cobra_timer.h"
#include "cobra4A_mpwm.h"

#include "app_mosquito_killing.h"
#include "mosquito_killing_data.h"
#include "mosquito_killing_disp.h"
#include "mosquito_killing_statistics.h"
#include "mosquito_killing_protocol.h"

uint8_t TRACE_LEVEL = 0;

static uint8_t app_mosquito_killing_power_on_delay_op_key = 0;
#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
static uint32_t reset_default_flag = 0;
#endif
uint8_t app_mosquito_killing_statistics_flag = 0; /*1==表示计数改变，需要保存*/
uint8_t app_mosquito_killing_zero_flag = 0; /*1==表示操作了清零，需要上传数据*/

uint8_t  app_mosquito_killing_bt_addr[6];
uint8_t  app_mosquito_killing_pid[4];
uint8_t  app_mosquito_killing_password[4];
uint8_t  app_mosquito_killing_name[16];
uint8_t  app_mosquito_killing_addr_save_flag = 0;  /*1==随机生成，需要将其保存，以便下次开机其ip地址不变*/

uint8_t  app_mosquito_killing_restore_flag = 0;
/*0~119; total 120 months*/
uint8_t app_mosquito_killing_curr_month = 0;

uint8_t app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;
//uint8_t app_mosquito_killing_work_state __attribute__((section("retention_area"),zero_init));

static uint8_t app_mosquito_killing_work_state_pre = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;

static uint32_t app_mosquito_killing_adc_detect_buf[APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX];
static uint8_t  app_mosquito_killing_adc_buf_index = 0;
static uint32_t app_mosquito_killing_adc_detectInterval =0;

#if (APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE == 1)
static uint8_t  app_mosquito_killing_adc_average_buf_index = 0;
static uint32_t app_mosquito_killing_adc_average_buf[APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX];
#endif
static void app_mosquito_killing_watchdog_init(void);
static void app_mosquito_killing_execute_10ms_period(void);

#if (APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE == 1)
static void app_mosquito_killing_execute_5ms_period(void);
#endif

static int uart_input_mosquito_killing_protocol(uint8_t inChar);
static void app_mosquito_killing_variable_init(void);

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
static DAY_OF_WEEK get_cur_time_day_of_week(void);
static UTCTime convert_time_to_Second(UTCTimeStruct time);
#endif

static cobra_timer mosquito_killing_soft_timer;
#if (APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE == 1)
static cobra_timer mosquito_killing_soft_timer_5ms;
#endif
void app_mosquito_killing_softtimer_init(void)
{
    cobra_softtimer_init(&mosquito_killing_soft_timer, app_mosquito_killing_execute_10ms_period, 10,  10);
    cobra_softtimer_start(&mosquito_killing_soft_timer);

#if (APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE == 1)
    cobra_softtimer_init(&mosquito_killing_soft_timer_5ms, app_mosquito_killing_execute_5ms_period, APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD,  APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD);
    cobra_softtimer_start(&mosquito_killing_soft_timer_5ms);
#endif
}

#if (APP_MOSQUITO_POWER_ORDER_ENABLE == 1)
static uint8_t PowerOnOrderStart = 0;
static uint32_t PowerOnOrderCount = 0;
#endif

#if(APP_MOSQUITO_KILLING_POWER_ON_PWM_ENABLE == 1)
static uint32_t PowerOnCount = 0;
static uint8_t  PowerOnDuty = 0;
static cobra_timer mosquito_killing_power_on_soft_timer;
static void app_mosquito_killing_execute_1ms_period(void);

void app_mosquito_killing_power_on_softtimer_init(void)
{
    PowerOnCount = 0;
//	PowerOnDuty = 0;
    PowerOnDuty = 3;

    cobra_softtimer_init(&mosquito_killing_power_on_soft_timer, app_mosquito_killing_execute_1ms_period, 1,  1);
    cobra_softtimer_start(&mosquito_killing_power_on_soft_timer);
}

#endif
/*
把不用到的 IO 口,设置为输入
启动设置
*/
void app_mosquito_killing_io_init(void)
{
//    app_m3_gpio_set_direction(GPIO_PIN_0, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_1, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_2, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_6, GPIO_HIGH_Z);
//
//    app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_HIGH_Z);
//    app_m3_gpio_set_direction(GPIO_PIN_10, GPIO_HIGH_Z);
//
//    app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_INPUT);
//    app_m3_gpio_set_direction(GPIO_PIN_31, GPIO_INPUT);
//
//    cobra_pin_pullup_enable(PIN0_INDEX, 1);
//    cobra_pin_pullup_enable(PIN1_INDEX, 1);
//    cobra_pin_pullup_enable(PIN2_INDEX, 1);
//    cobra_pin_pullup_enable(PIN3_INDEX, 1);
//    cobra_pin_pullup_enable(PIN4_INDEX, 1);
//    cobra_pin_pullup_enable(PIN6_INDEX, 1);
//
//
//    cobra_pin_pullup_enable(PIN8_INDEX, 1);
//    cobra_pin_pullup_enable(PIN10_INDEX, 1);
//
//    cobra_pin_pullup_enable(PIN28_INDEX, 0);
//
//    cobra_pin_pullup_enable(PIN29_INDEX, 1);
//    cobra_pin_pullup_enable(PIN31_INDEX, 1);
}

static void app_mosquito_killing_uart_receive_init(void)
{
    cobra4_uart_set_receive_callback(uart_input_mosquito_killing_protocol);
}

static void app_mosquito_killing_pin_init(void)
{
    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_BT_LED_PIN_NUM, 1);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_BT_LED_PIN_NUM, GPIO_HIGH);	 /*high==turn off*/

    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, 1);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_LOW); /*low==turn off*/

    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, 1);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_LOW);

    app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_PREVENT_TOP_SWITCH_PIN_NUM, GPIO_OUTPUT);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_LOW);
//    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, 1);

    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, 1);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_LOW);

    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, 1);
    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, GPIO_LOW);


    app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_PREVENT_TOP_SWITCH_PIN_NUM, GPIO_INPUT);
    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_PREVENT_TOP_SWITCH_PIN_NUM, 0);

    app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_PREVENT_BOTTOM_SWITCH_PIN_NUM, GPIO_INPUT);
    cobra_pin_pullup_enable((PINMUX_INDEX)APP_MOSQUITO_KILLING_PREVENT_BOTTOM_SWITCH_PIN_NUM, 0);

    /*按键提前初始化,防止在button初始化时一起操作，会引起瞬间的检测到有效低电平,测试发现还是会出现*/
//	app_m3_gpio_set_direction(GPIO_PIN_15, GPIO_INPUT);
//	cobra_pin_pullup_enable((PINMUX_INDEX)GPIO_PIN_15, 1);
}

void app_mosquito_killing_bt_led_onoff(uint8_t param)
{
    if(param == 0)
    {
        app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_BT_LED_PIN_NUM, GPIO_HIGH);
    }
    else
    {
        app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_BT_LED_PIN_NUM, GPIO_LOW);  /*亮bt灯*/

        app_mosquito_killing_bt_data_init();

        app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_BREATH_FLICKER_TIMER, 1);
    }

//    app_wechat_send_data((uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);
}

/*duty:0~100*/
/*freq unit hz*/
void app_mosquito_killing_pwm_set(uint32_t freq, uint8_t duty)
{
    static uint8_t u8pwm_init_flag = 0;
    uint32_t u32Temp = 0;

    u8pwm_init_flag = 0;
    u32Temp = cobra_system_get_m3_apb2_clk();

//    __direct_printf("pwm duty[%d/100], freq[%d]hz\n",duty, freq);

    freq = u32Temp/freq;

    if((duty > APP_MOSQUITO_KILLING_PWM_LIMIT_MIN) && (duty < APP_MOSQUITO_KILLING_PWM_LIMIT_MAX))
    {
        if(u8pwm_init_flag == 0)
        {
            u8pwm_init_flag = 1;

            cobra_pcrm_mpwm_active_mode_clock_enable(1);
            cobra_pcrm_mpwm_sleep_mode_clock_enable(1);
            mpwm_close(MPWM_CHANNEL1_EN);
            cobra_map_pinmodeset((PINMUX_INDEX)APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, PIN5_MPWM_MCOA1);
            set_tc1(0);
            write_pola1_set();
            set_lim1(freq);
            set_mat1((freq)*duty/100);
            mpwm_en(MPWM_CHANNEL1_EN);
        }
        else
        {
            set_lim1(freq);
            set_mat1((freq)*duty/100);
        }
    }
    else
    {
        u8pwm_init_flag = 0;
        if(duty >= APP_MOSQUITO_KILLING_PWM_LIMIT_MAX)
        {
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_HIGH);
        }
        else
        {
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_LOW);
        }
    }
}

void app_mosquito_killing_pwm_close(void)
{
    mpwm_close(MPWM_CHANNEL1_EN);
    cobra_pcrm_mpwm_active_mode_clock_enable(0);
    cobra_pcrm_mpwm_sleep_mode_clock_enable(0);

}

#if (APP_MOSQUITO_POWER_ORDER_ENABLE == 1)
static void app_mosquito_killing_work_mode_all_on(void)
{
    app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON;

    PowerOnOrderStart = 1;
    PowerOnOrderCount = 0;

    app_mosquito_killing_statistics_flag = 1; /*1==表示计数改变，需要保存*/


    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &app_mosquito_killing_work_state, 1);
}
#else
static void app_mosquito_killing_work_mode_all_on(void)
{
    app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON;

#if(APP_MOSQUITO_KILLING_POWER_ON_PWM_ENABLE == 1)
    app_mosquito_killing_power_on_softtimer_init();

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_HIGH);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_HIGH);
#else

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_HIGH);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_HIGH);

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_HIGH);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_HIGH);
#endif

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, GPIO_HIGH);

    app_mosquito_killing_tm1650_open();
    app_mosquito_killing_tm1650_disp_all_month_count();


    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &app_mosquito_killing_work_state, 1);
}
#endif

static void app_mosquito_killing_work_mode_bot_on(void)
{
    app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_BOT_ON;

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_LOW);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_LOW);

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_HIGH);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_HIGH);

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, GPIO_HIGH);

    app_mosquito_killing_tm1650_open();
    app_mosquito_killing_tm1650_disp_all_month_count();
}
static void app_mosquito_killing_work_mode_all_off(void)
{
#if (APP_MOSQUITO_POWER_ORDER_ENABLE == 1)
    PowerOnOrderStart = 0;
    PowerOnOrderCount = 0;
#endif

    app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;

    app_mosquito_killing_work_state_pre = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;  /*防拆开关保护后不能自动恢复，需要再次按键*/

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_LOW);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_LOW);

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_LOW);
    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_LOW);

    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, GPIO_LOW);

    app_mosquito_killing_tm1650_close();

    app_mosquito_killing_statistics_flag = 1; /*1==表示计数改变，需要保存*/


    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &app_mosquito_killing_work_state, 1);
}

void app_mosquito_killing_work_mode_zero_clear(void)
{
    app_mosquito_killing_zero_flag = 1;

    memset((uint8_t *)app_mosquito_killing_month_count, 0x00, STATISTICS_MONTH_SZIE);
//    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
//    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);

    app_mosquito_killing_tm1650_open();
    app_mosquito_killing_tm1650_disp_all_month_count();

    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);
}

uint8_t app_mosquito_killing_restore_default_setting(void)
{
#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
    /*restore timing setting*/
    app_mosquito_killing_setting_data_default_setting();
#endif

    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
    app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);

//    app_mosquito_killing_variable_init();
	#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
    reset_default_flag = 1;
	#endif
    return 1;
}

void app_mosquito_killing_control(void)
{
    if(0 == app_mosquito_killing_detect_prevent_status())
    {
        /*关闭输出*/
        app_mosquito_killing_work_mode_all_off();

        printf("not permit work...\n");
        return;
    }

#if 0
    /*only for test all on*/
    app_mosquito_killing_work_mode_all_on();
    return;
#endif

    switch(app_mosquito_killing_work_state)
    {
        case APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON:
            app_mosquito_killing_work_mode_all_on();
            break;

        case APP_MOSQUITO_KILLING_WORK_MODE_BOT_ON:
            app_mosquito_killing_work_mode_bot_on();
            break;

        case APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN:
            app_mosquito_killing_work_mode_all_off();
            break;

        default:
            printf("no work state [%d]\n", app_mosquito_killing_work_state);
            break;
    }

}

/*v1.4改为只有全开和关两种控制*/
static void app_mosquito_killing_mode_switch(void)
{
    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
    {
        app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON;
    }
    else if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
    {
        app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;
//        app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_BOT_ON;
    }
//    else if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_BOT_ON)
//    {
//        app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;
//
//        app_mosquito_killing_work_state_pre = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;
//    }

    __direct_printf("work state [%d](1-all on, 2-bot on, 0-off)\n", app_mosquito_killing_work_state);

    app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_POWER_SWITCH_COUNT_DELAY ;

    app_mosquito_killing_control();

    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &app_mosquito_killing_work_state, 1);
}

/*每隔100ms触发一次*/
void app_mosquito_killing_button_press_down_handle(void * param)
{
    uint8_t _u8Res = 0;
    uint8_t u8Second = 0;
    Button *btButton;
    btButton = (Button *)param;
    u8Second = btButton->ticks/50;
    __direct_printf("dw ticks[%d], u8Second[%d]\n", btButton->ticks, u8Second);  /*unit:20ms */

    if(app_mosquito_killing_detect_prevent_status() == 0)
    {
        return;
    }

    if(btButton->ticks == APP_MOSQUITO_KILLING_POWER_ON_KEY_DURATION)
    {
        if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
        {
            app_mosquito_killing_work_mode_zero_clear();

            app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, (uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);
        }
    }
    else if(btButton->ticks == APP_MOSQUITO_KILLING_RESTORE_KEY_DURATION)
    {
        if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
        {
            app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_TIMER, 1);
            _u8Res = app_mosquito_killing_restore_default_setting();

            _u8Res = 2; /*本地触发带参数2返回 */
            app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &_u8Res, 1);
        }
    }
}

void app_mosquito_killing_button_press_up_handle(void * param)
{
    uint8_t u8Second = 0;
    Button *btButton;
    btButton = (Button *)param;
    u8Second = btButton->ticks/50;
    __direct_printf("up ticks[%d], u8Second[%d]\n", btButton->ticks, u8Second);  /*unit:20ms */

    if(app_mosquito_killing_detect_prevent_status() == 0)
    {
        return;
    }

    if(app_mosquito_killing_power_on_delay_op_key == 0)
    {
        return;
    }

    if((btButton->ticks < APP_MOSQUITO_KILLING_POWER_ON_KEY_DURATION) && (btButton->ticks >2))
    {
        if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
        {
            if(app_mosquito_killing_restore_flag == 1)
            {
                app_mosquito_killing_restore_flag = 0;
                app_mosquito_killing_stop_timer(APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_TIMER);
            }
        }
        app_mosquito_killing_mode_switch();
#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
        if(app_mosquito_killing_timing_setting.timing_auto_operate_flag != 0)  /*有定时操作*/
        {
            app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 1;
        }
#endif
    }
    else if((btButton->ticks <APP_MOSQUITO_KILLING_RESTORE_KEY_DURATION) && (btButton->ticks >2))  /*关机状态下，长按小于复位时间也要开机*/
    {
        if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
        {
            app_mosquito_killing_mode_switch();
#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
            if(app_mosquito_killing_timing_setting.timing_auto_operate_flag != 0)  /*有定时操作*/
            {
                app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 1;
            }
#endif
        }
    }

}

void app_mosquito_killing_gpio_set(uint8_t pinNum, uint8_t param)
{
    if(param)
    {
        app_m3_gpio_write_pin((gpio_pin)pinNum, GPIO_HIGH);
    }
    else
    {
        app_m3_gpio_write_pin((gpio_pin)pinNum, GPIO_LOW);
    }
}

uint8_t  app_mosquito_killing_gpio_get(uint8_t pinNum)
{
    uint8_t res0 = 0;
    uint8_t res1 = 0;
    uint8_t res2 = 0;

    do
    {
        res0 = app_m3_gpio_read_pin((gpio_pin)pinNum);
        res1 = app_m3_gpio_read_pin((gpio_pin)pinNum);
        res2 = app_m3_gpio_read_pin((gpio_pin)pinNum);
    }
    while((res0!=res1) || (res1!=res2));

    return res0;
}

/*
static uint8_t app_mosquito_killing_gpio_get(uint8_t pin)
{
    uint8_t res = 0;

    app_m3_gpio_set_direction((gpio_pin)pin, GPIO_INPUT);
    res = app_m3_gpio_read_pin((gpio_pin)pin);

    return res;
}
*/
/*return :1==permit power on;0==not permit power on*/
uint8_t  app_mosquito_killing_detect_prevent_status(void)
{
    uint8_t res0 = 0;
    uint8_t res1 = 0;

#if (APP_MOSQUITO_RTC_ACCURACY_TEST_ENABLE == 1)
    return 1;
#endif

    res0 = app_mosquito_killing_gpio_get(APP_MOSQUITO_KILLING_PREVENT_TOP_SWITCH_PIN_NUM);
    res1 = app_mosquito_killing_gpio_get(APP_MOSQUITO_KILLING_PREVENT_BOTTOM_SWITCH_PIN_NUM);

    if((res0 == GPIO_LOW) && (res1 == GPIO_LOW))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


#if (APP_MOSQUITO_KILLING_DETECT_DIFF_ENABLE == 1)
static uint32_t app_mosquito_killing_adc_diff_count(void)
{
    uint32_t res = 0;

    for(int i =0; i<APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX; i++)
    {
        if(app_mosquito_killing_adc_detect_buf[i] > APP_MOSQUITO_KILLING_HV_ADC_DETECT_LIMIT)

        {
            res++;
//			printf("index [%d],diff[%04d], count[%d]\n", i, app_mosquito_killing_adc_detect_buf[i], res);
        }
    }

    return res;
}

static void app_mosquito_killing_adc_diff_count_debug(void)
{
    uint32_t res = 0;

	__direct_printf("\n");
    for(int i =0; i<APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX; i++)
    {
        if(app_mosquito_killing_adc_detect_buf[i] > APP_MOSQUITO_KILLING_HV_ADC_DETECT_LIMIT)

        {
            res++;
			__direct_printf("index [%d],diff[%04d], count[%d]\n", i, app_mosquito_killing_adc_detect_buf[i], res);
        }
    }
	__direct_printf("\n");
}

/*每2ms采样一次，与前一值的差值绝对值存于一个数组中，当出现差值大于阀值 1~3次视为触发计数，下一计数周期(20次后)再次检测*/
static uint8_t app_mosquito_killing_statistics(void)
{
    uint8_t res = 0;
    uint32_t _u32TempCur = 0;
    static uint32_t _u32TempPre = 0;

    uint32_t _u32Diff = 0;
    uint32_t _u32ValidSum = 0;  /*记录差值超过阀值的次数*/

    static uint8_t _u8Count = 0;

    if(app_mosquito_killing_adc_detectInterval>0)
    {
        app_mosquito_killing_adc_detectInterval--;
    }

    _u32TempCur = app_mosquito_killing_ADC_voltage();
    if(_u32TempCur > _u32TempPre)
    {
        _u32Diff = _u32TempCur - _u32TempPre;
    }
    else
    {
        _u32Diff = _u32TempPre - _u32TempCur;
    }

    _u32TempPre = _u32TempCur;


//    printf("[%04d][%04d]     ", _u32TempCur, _u32Diff);
//    __direct_printf("[%d]", _u32TempCur);
//	  printf("[%04d] ", _u32TempCur);

#if 0
	/*only for test*/
	printf("%d\n", _u32TempCur);
    return 0;
#endif

    app_mosquito_killing_adc_detect_buf[app_mosquito_killing_adc_buf_index++] = _u32Diff;
    if(app_mosquito_killing_adc_buf_index>=APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX)
    {
        app_mosquito_killing_adc_buf_index = 0;

        _u32ValidSum = app_mosquito_killing_adc_diff_count();
    }
    else
    {
        return 0;

    }

//	/*相当于用buff长度的一半作为周期来判断记数*/
//	if((app_mosquito_killing_adc_buf_index == 0) || (app_mosquito_killing_adc_buf_index == APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX/2))
//	{
//		_u32ValidSum = app_mosquito_killing_adc_diff_count();
//	}
//	else
//	{
//		return 0;
//	}
//    __direct_printf("[%04d][%04d]     ", _u32TempCur, _u32Diff);
//    printf("[%04d][%04d]     ", _u32Diff, _u32ValidSum);


    if((_u32ValidSum>=APP_MOSQUITO_KILLING_HV_ADC_DETECT_VALID_COUNT_MIN) && (_u32ValidSum <= APP_MOSQUITO_KILLING_HV_ADC_DETECT_VALID_COUNT_MAX))
    {
        if(app_mosquito_killing_adc_detectInterval == 0)
        {
#if 1  /*only for test*/
		/*打印出当前计数的有效值，方便调试*/
			app_mosquito_killing_adc_diff_count_debug();

#endif			
            if(app_mosquito_killing_timing_setting.time_sync_finished == 1)
            {
                if(app_mosquito_killing_month_count[app_mosquito_killing_curr_month]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                {
                    app_mosquito_killing_month_count[app_mosquito_killing_curr_month]++;
                }
            }
            else
            {
                if(app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                {
                    app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]++;  /*没有同步时间时，计数到暂存区*/
                }
            }

            app_mosquito_killing_statistics_flag = 1;

            app_mosquito_killing_tm1650_disp_all_month_count();

            app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, (uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);

            app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_INTERVAL_MAX ;  /*20*?=500ms,连续放电计数的最小间隔时间*/
        }

    }

    return res;
}
#elif (APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE == 1)
static uint32_t app_mosquito_killing_adc_average_diff_count(void)
{
    uint32_t res = 0;
    uint32_t _u32Diff = 0;

    for(int i =0; i<APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX-1; i++)
    {
        if(app_mosquito_killing_adc_average_buf[i] > app_mosquito_killing_adc_average_buf[i+1])
        {
            return 0;
        }
    }

    if(app_mosquito_killing_adc_average_buf[APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX-1] < app_mosquito_killing_adc_average_buf[0])
    {
        return 0;
    }

    _u32Diff = app_mosquito_killing_adc_average_buf[APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX-1] - app_mosquito_killing_adc_average_buf[0];

    if((_u32Diff>APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MIN) && (_u32Diff<APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MAX))
    {
        //printf("diff[%d]\n", _u32Diff);

        res = 1;
    }
    else
    {
        res = 0;
    }


    return res;
}

static uint32_t app_mosquito_killing_adc_average(void)
{
    uint32_t res = 0;

    for(int i =0; i<APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX; i++)
    {
        res += app_mosquito_killing_adc_detect_buf[i];
    }

    res = res/APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX;

    return res;
}

#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
/*param uinit: 2ms*/
/*结果放大10倍，避免小数*/
/*220121:将结果放大100倍*/

#if 1
/*注意这个采样周期是2ms,时间使用的单位是1ms,需要转换*/
/*220121*/
static uint32_t app_mosquito_killing_get_linear_ratio(uint32_t param)
{
    uint32_t res = 0;

    if(param < (APP_MOSQUITO_KILLING_LINEAR_RATIO_TIME*60*1000)/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = (param*100*APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)/(APP_MOSQUITO_KILLING_LINEAR_RATIO_TIME*60*1000);
    }
    else if(param < APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT)
    {
        res = 100;
    }	

	res = res;  

//	printf("ratio[%d], time[%d]\n", res, param);
	
    return res;
}
#endif


#if 0
/*220118*/
static uint32_t app_mosquito_killing_get_linear_ratio(uint32_t param)
{
    uint32_t res = 0;

    if(param < 60000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 1;
    }
    else if(param < 120000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 2;
    }
    else if(param < 180000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 3;
    }
    else if(param < 240000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 4;
    }
    else if(param < 300000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 5;
    }
    else if(param < 360000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 6;
    }
    else if(param < 420000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 7;
    }
    else if(param < 480000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 8;
    }
    else if(param < 540000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 9;
    }
    else if(param < APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT)
    {
        res = 10;
    }	

	res = res*10;  /*将原来的10作为一次计数，改为100作为一次计数*/

//	printf("ratio[%d], time[%d]\n", res, param);
	
    return res;
}
#endif

#if 0
/*220117*/
static uint32_t app_mosquito_killing_get_linear_ratio(uint32_t param)
{
    uint32_t res = 0;

    if(param < 30000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 1;
    }
    else if(param < 60000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 2;
    }
    else if(param < 90000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 3;
    }
    else if(param < 120000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 4;
    }
    else if(param < 150000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 5;
    }
    else if(param < 180000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 6;
    }
    else if(param < 210000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 7;
    }
    else if(param < 240000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 8;
    }
    else if(param < 270000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 9;
    }
    else if(param < APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT)
    {
        res = 10;
    }	

//	printf("ratio[%d], time[%d]\n", res, param);
	
    return res;
}
#endif

#if 0
/*220115*/
static uint32_t app_mosquito_killing_get_linear_ratio(uint32_t param)
{
    uint32_t res = 0;

    if(param < 23000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 1;
    }
    else if(param < 26000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 2;
    }
    else if(param < 27000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 3;
    }
    else if(param < 29000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 4;
    }
    else if(param < 30000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 5;
    }
    else if(param < 32000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 6;
    }
    else if(param < 33000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 7;
    }
    else if(param < 35000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 8;
    }
    else if(param < 43000/APP_MOSQUITO_KILLING_HV_ADC_DETECT_PERIOD)
    {
        res = 9;
    }
    else if(param < APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT)
    {
        res = 10;
    }	

//	printf("ratio[%d], time[%d]\n", res, param);
	
    return res;
}
#endif

#endif

/*每10ms采样一次，放入到计算平均数的buffer中，用于计算平均值，每次的采样值 ，再与平均值比较来判断有无放电*/
static uint8_t app_mosquito_killing_statistics(void)
{
    uint8_t res = 0;
    uint32_t _u32Temp = 0;
    uint32_t _u32Average = 0;
    uint32_t _u32Diff = 0;

    static uint8_t _u8Count = 0;

#if (APP_MOSQUITO_KILLING_DETECT_LIMIT_ENABLE == 1)
    static uint8_t _u8TimeFlag = 0;
    static uint32_t _u32TimeLimit = 0;
    static uint32_t _u32CountLimit = 0;
#endif

#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
    static uint32_t _u32LinearCount = APP_MOSQUITO_KILLING_LINEAR_BASE;
    static uint32_t _u32LinearTimeLimit = 0;
    static uint32_t _u32LinearRatio = 0;
	if(reset_default_flag == 1)
	{
		_u32LinearCount = APP_MOSQUITO_KILLING_LINEAR_BASE;
		_u32LinearTimeLimit = 0;
		_u32LinearRatio = 0;	
		reset_default_flag =0;
	}
#endif

    if(app_mosquito_killing_adc_detectInterval>0)
    {
        app_mosquito_killing_adc_detectInterval--;
    }

#if (APP_MOSQUITO_KILLING_DETECT_LIMIT_ENABLE == 1)
    if(_u8TimeFlag == 1)
    {
        if(_u32TimeLimit < APP_MOSQUITO_KILLING_HV_ADC_DETECT_TIME_LIMIT)
        {
            _u32TimeLimit++;

            if(_u32CountLimit > APP_MOSQUITO_KILLING_HV_ADC_DETECT_COUNT_LIMIT)
            {
                return 0;
            }
        }
        else
        {
            _u32TimeLimit = 0;
            _u32CountLimit = 0;
            _u8TimeFlag = 0;
        }
    }
#endif

#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
    _u32LinearTimeLimit++;
#endif


    _u32Temp = app_mosquito_killing_ADC_voltage();
//    __direct_printf("[%d]", _u32Temp);
//	  printf("[%04d] ", _u32Temp);

    app_mosquito_killing_adc_detect_buf[app_mosquito_killing_adc_buf_index++] = _u32Temp;
    if(app_mosquito_killing_adc_buf_index>=APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX)
    {
        app_mosquito_killing_adc_buf_index = 0;
    }

    _u32Average = app_mosquito_killing_adc_average();

    app_mosquito_killing_adc_average_buf[app_mosquito_killing_adc_average_buf_index++] = _u32Average;
    if(app_mosquito_killing_adc_average_buf_index>=APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX)
    {
        app_mosquito_killing_adc_average_buf_index = 0;

        res = app_mosquito_killing_adc_average_diff_count();
    }
    else
    {
        return 0;
    }

//    __direct_printf("[%04d][%04d]     ", _u32Temp, _u32Average);
//    printf("[%04d][%04d]     ", _u32Temp, _u32Average);


    if(res ==  1)
    {
        if(app_mosquito_killing_adc_detectInterval == 0)
        {
#if (APP_MOSQUITO_KILLING_DISCHARGE_TIME_DEBUG_220119 == 1)
			/*only for test*/
			printf("count[%d], diff[%d], Ratio[%d], time[%d]ms, Rcount[%d]\n", app_mosquito_killing_month_count[app_mosquito_killing_curr_month], app_mosquito_killing_adc_average_buf[APP_MOSQUITO_KILLING_HV_ADC_DETECT_AVERAGE_BUFFER_MAX-1] - app_mosquito_killing_adc_average_buf[0], _u32LinearRatio, _u32LinearTimeLimit*2, _u32LinearCount);
#endif

#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
            if(_u32LinearTimeLimit < APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT)
            {
                _u32LinearRatio = app_mosquito_killing_get_linear_ratio(_u32LinearTimeLimit);
                _u32LinearCount += _u32LinearRatio;

//                if(_u32LinearCount > APP_MOSQUITO_KILLING_LINEAR_BASE) /*为了比较 放大了100倍,100为1 次计数*/
				if(_u32LinearCount >= APP_MOSQUITO_KILLING_LINEAR_BASE) /*为了比较 放大了100倍,100为1 次计数*/
                {
                    _u32LinearCount = _u32LinearCount - APP_MOSQUITO_KILLING_LINEAR_BASE;
                }
                else
                {
                    _u32LinearTimeLimit = 0; /*每次有效的放电后重新开始计时*/

					app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_INTERVAL_MAX ;  /*20*?=500ms,连续放电计数的最小间隔时间*/

                    return 0;
                }
            }
            else
            {
                _u32LinearCount = 0; /*这里不能赋基准初值，因执行后面就已计数了*/
            }

            _u32LinearTimeLimit = 0; /*每次有效的计数后重新开始计时*/
#endif

            if(app_mosquito_killing_timing_setting.time_sync_finished == 1)
            {
                if(app_mosquito_killing_month_count[app_mosquito_killing_curr_month]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                {
                    app_mosquito_killing_month_count[app_mosquito_killing_curr_month]++;
                }
            }
            else
            {
                if(app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                {
                    app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]++;  /*没有同步时间时，计数到暂存区*/
                }
            }

            app_mosquito_killing_statistics_flag = 1;

            app_mosquito_killing_tm1650_disp_all_month_count();

            app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, (uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);

            app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_INTERVAL_MAX ;  /*20*?=500ms,连续放电计数的最小间隔时间*/

//			printf("count[%d]\n",app_mosquito_killing_month_count[app_mosquito_killing_curr_month]);

#if (APP_MOSQUITO_KILLING_DETECT_LIMIT_ENABLE == 1)
            if(_u8TimeFlag == 0)
            {
                _u8TimeFlag = 1;  /*begin count time*/
                _u32CountLimit = 0;
                _u32CountLimit = 0;
            }
            _u32CountLimit++;
#endif
        }

    }

    return res;
}

#else
static uint32_t app_mosquito_killing_adc_average(void)
{
    uint32_t res = 0;

    for(int i =0; i<APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX; i++)
    {
        res += app_mosquito_killing_adc_detect_buf[i];
    }

    res = res/APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX;

    return res;
}

/*每10ms采样一样，放入到计算平均数的buffer中，用于计算平均值，每次的采样值 ，再与平均值比较来判断有无放电*/
static uint8_t app_mosquito_killing_statistics(void)
{
    uint8_t res = 0;
    uint32_t _u32Temp = 0;
    uint32_t _u32Average = 0;
    uint32_t _u32Diff = 0;

    static uint8_t _u8Count = 0;

#if 0
    /*only for test*/
    static uint8_t _u8Flag = 0;
    if(_u8Flag == 0)
    {
        _u8Flag = 1;
        app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_HIGH);
    }
    else
    {
        _u8Flag = 0;
        app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_LOW);
    }
#endif

    _u32Temp = app_mosquito_killing_ADC_voltage();
//    __direct_printf("[%d]", _u32Temp);
//	  printf("[%04d] ", _u32Temp);

    app_mosquito_killing_adc_detect_buf[app_mosquito_killing_adc_buf_index++] = _u32Temp;
    if(app_mosquito_killing_adc_buf_index>=APP_MOSQUITO_KILLING_HV_ADC_DETECT_BUFFER_MAX)
    {
        app_mosquito_killing_adc_buf_index = 0;
    }

    _u32Average = app_mosquito_killing_adc_average();
//    __direct_printf("[%04d][%04d]     ", _u32Temp, _u32Average);
//    printf("[%04d][%04d]     ", _u32Temp, _u32Average);

#if 0
	printf("%d\n", _u32Average);
	return 0;
#endif

    if(_u32Temp > _u32Average)
    {
        _u32Diff = _u32Temp - _u32Average;

        if(_u32Diff > APP_MOSQUITO_KILLING_HV_ADC_DETECT_LIMIT)
        {
            __direct_printf_statistics("\n1[%03d][%04d][%04d](%03d)(%02d)",_u32Diff, _u32Temp, _u32Average, app_mosquito_killing_adc_detectInterval,_u8Count);

            _u8Count++;
            if(_u8Count>APP_MOSQUITO_KILLING_HV_ADC_DETECT_VALID_COUNT_MAX)
            {
                if(app_mosquito_killing_adc_detectInterval == 0)
                {
                    __direct_printf_statistics("  2[%03d]\n",_u32Temp);
                    if(app_mosquito_killing_timing_setting.time_sync_finished == 1)
                    {
                        if(app_mosquito_killing_month_count[app_mosquito_killing_curr_month]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                        {
                            app_mosquito_killing_month_count[app_mosquito_killing_curr_month]++;
                        }
                    }
                    else
                    {
                        if(app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]<APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                        {
                            app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-1]++;  /*没有同步时间时，计数到暂存区*/
                        }
                    }

                    app_mosquito_killing_statistics_flag = 1;

                    app_mosquito_killing_tm1650_disp_all_month_count();

                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, (uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);

                    app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_INTERVAL_MAX ;  /*20*?=500ms,连续放电计数的最小间隔时间*/
                }
                else
                {
                    __direct_printf_statistics("\n");
                }
            }
            else
            {
                __direct_printf_statistics("\n");
            }
        }
        else
        {
            _u8Count = 0;
        }
    }
    else
    {
        _u32Diff = _u32Average - _u32Temp;

        if(_u32Diff > APP_MOSQUITO_KILLING_HV_ADC_DETECT_LIMIT/2)
        {
            _u8Count = 0;
        }
    }

    if(app_mosquito_killing_adc_detectInterval>0)
    {
        app_mosquito_killing_adc_detectInterval--;
    }

    return res;
}

#endif
static void app_mosquito_killing_jtag_init(void)
{
    cobra_map_pinmodeset(PIN20_INDEX, PIN_MODE_0);
    cobra_map_pinmodeset(PIN21_INDEX, PIN_MODE_0);

    cobra_pin_input_enable(PIN20_INDEX, 1);
    cobra_pin_input_enable(PIN21_INDEX, 1);
}

static void app_mosquito_killing_button_init(void)
{
    app_button_init();
    app_button_sw9_attach_event(PRESS_DOWN,app_mosquito_killing_button_press_down_handle);
    app_button_sw9_attach_event(PRESS_UP,app_mosquito_killing_button_press_up_handle);
    app_button_start();
}


static void app_mosquito_killing_variable_init(void)
{
    /*开机默认的状态:全开*/
    /*从power down到all on时会延时500ms再去检测放电计数*/
    __direct_printf("app_mosquito_killing_work_state[%d]\n", app_mosquito_killing_work_state);
    app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;
    app_mosquito_killing_work_state_pre = APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN;

    app_mosquito_killing_statistics_flag = 0;

    app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_POWER_SWITCH_COUNT_DELAY ;  /*重新开机1s内不计放电数量*/

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
    memset(&app_mosquito_killing_timing_setting, 0x00, sizeof(mosquito_killing_timing_setting_t));
#endif

    app_mosquito_killing_curr_month = APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX - 1; /*开机默认设置为暂存区，同步时间后进行修改*/
}

#if (APP_MOSQUITO_RTC_ACCURACY_TEST_ENABLE == 1)
static void app_mosquito_killing_rtc_test_set_time(void)
{
    COBRA_RTC_TIME_DATE_T time;
    time.SUBSEC = 0;
    time.SEC	= 0;
    time.MIN	= 0;
    time.HOUR	= 0;
    time.DAY	= 1;
    time.MONTH	= 1;
    time.YEAR	= 21;

    cobra_rtc_timer_write_en(1);
    cobra_rtc_set_current_time_date(&time);
    cobra_rtc_timer_write_en(0);
}

static void app_mosquito_killing_rtc_test_get_disp_time(void)
{
    static uint8_t _u8DispFlag = 0;
    static uint32_t _u32DispData = 0;

    COBRA_RTC_TIME_DATE_T time;
    cobra_rtc_get_current_time_date(&time);

    if(_u8DispFlag == 0)
    {
        _u8DispFlag = 1;
        _u32DispData = 900 + time.MIN;
    }
    else
    {
        _u8DispFlag = 0;
        _u32DispData = 800 + time.SEC;
    }

    app_mosquito_killing_tm1650_disp(_u32DispData);
}
#endif

void app_mosquito_killing_watchdog_restore(void)
{
    uint32_t _u32Temp = 0;

    _u32Temp = *(uint32_t*)(COBRA_PINMUX_BASE + 0x42*4);
    __direct_printf("[0x%08x](0x01=hd, 0x0c=softreset, 0x0a=watchdog, 0x00=power on)\n", _u32Temp);

    _u32Temp = _u32Temp>>24;

    if(_u32Temp == 0x0a)
    {
        __direct_printf("watchdog reset\n");

        __direct_printf("bak_work_state[%d], bak_time_sync_finished[%d], res[%d][%d]\n", app_mosquito_killing_setting_bak.bak_work_state, app_mosquito_killing_setting_bak.bak_time_sync_finished, app_mosquito_killing_setting_bak.reserve, app_mosquito_killing_setting_bak.reserve01);

        /*开机状态下watchdog复位才恢复*/
        if(app_mosquito_killing_setting_bak.bak_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
        {
            app_mosquito_killing_work_state = APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON;
        }

        if(app_mosquito_killing_setting_bak.reserve < 0xFF)
        {
            app_mosquito_killing_setting_bak.reserve++;/*用于记录watchdog 复位的次数*/
        }
    }
    else if(_u32Temp == 0x0c)
    {
        __direct_printf("soft reset\n");
    }
    else if(_u32Temp == 0x01)
    {
        __direct_printf("power on/hd  reset\n");
//		app_cobra_rtc_init();
    }

}

void app_mosquito_killing_adjust_rtc(void)
{
    uint32_t _u32Temp = 0;

    _u32Temp = *(uint32_t*)(0x4008B044);
    __direct_printf("1 rtc [0x%08x]\n", _u32Temp);

    _u32Temp = (_u32Temp&0xFFFFFF0F)|0x50;  /*4~7 bits设置为5*/

    __direct_printf("2 rtc [0x%08x]\n", _u32Temp);

    *(uint32_t*)(0x4008B044) = _u32Temp;

}
void app_mosquito_killing_init(void)
{
    __here;

    app_mosquito_killing_softtimer_init();
    app_mosquito_killing_receive_bt_data_softtimer_init();

#if(APP_MOSQUITO_KILLING_POWER_ON_PWM_ENABLE == 1)
//	app_mosquito_killing_power_on_softtimer_init();
#endif

    app_mosquito_killing_variable_init();

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
    app_mosquito_killing_setting_data_load();
#endif
    app_mosquito_killing_watchdog_restore();

    app_mosquito_killing_pin_init();

    app_mosquito_killing_uart_receive_init();

    app_mosquito_killing_disp_init();

    app_mosquito_killing_ADC_Init();

    app_mosquito_killing_jtag_init();

    app_mosquito_killing_control(); /*开机默认控制输出*/

//    app_cobra_rtc_init(); /**common.c初始化已调用*/

#if (APP_MOSQUITO_RTC_ACCURACY_TEST_ENABLE == 1)
    app_mosquito_killing_rtc_test_set_time();
#endif

#if (APP_MOSQUITO_KILLING_WATCHDOG_ENABLE == 1)
    app_mosquito_killing_watchdog_init();
#endif

    app_mosquito_killing_adjust_rtc();

    app_mosquito_killing_button_init();
}

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
#define IsLeapYear(yr) (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))

#define YearLength(yr) (IsLeapYear(yr) ? 366 : 365)

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC

//typedef uint32_t UTCTime; /* used to store the second counts for RTC */

#define BEGYEAR         2000     // UTC started at 00:00:00 January 1, 2000
#define SECOND_OF_DAY             86400UL  // 24 hours * 60 minutes * 60 seconds

//2000-01-01 is sat
#define SYSTEM_ORIGIN_DAY_OF_WEEK (Sat)
#define LENGTH_OF_WEEK      (7)

/* calculate day of week */
DAY_OF_WEEK get_day_of_week(uint32_t secTime)
{
    uint32_t day = secTime / SECOND_OF_DAY;

    DAY_OF_WEEK today = (DAY_OF_WEEK)(((day %LENGTH_OF_WEEK) + SYSTEM_ORIGIN_DAY_OF_WEEK) %LENGTH_OF_WEEK);

    return today;
}

static DAY_OF_WEEK get_cur_time_day_of_week(void)
{
    DAY_OF_WEEK day_of_week = MOn;;
    uint32_t SecondCountRTC = 0;
    UTCTimeStruct tm;

    COBRA_RTC_TIME_DATE_T rtc_time;
    cobra_rtc_get_current_time_date(&rtc_time);

    tm.year = rtc_time.YEAR;
    tm.month = rtc_time.MONTH;
    tm.day = rtc_time.DAY;
    tm.hour = rtc_time.HOUR;
    tm.minutes = rtc_time.MIN;
    tm.seconds = rtc_time.SEC;
    SecondCountRTC = convert_time_to_Second(tm);

    day_of_week = get_day_of_week(SecondCountRTC); // day of week

    return day_of_week;
}

static mosquito_killing_timing_t* app_mosquito_killing_get_timing(uint8_t index)
{
    if(index >= MAX_TIMING_NUM)
    {
        return NULL;
    }

    return &(app_mosquito_killing_data_timing[index]);
}

static int app_mosquito_killing_set_timing(uint8_t index,mosquito_killing_timing_t *timing)
{
    if(index >= MAX_TIMING_NUM)
    {
        return -1;
    }

    if(NULL == timing)
    {
        return -1;
    }

    memcpy(&(app_mosquito_killing_data_timing[index]),timing,sizeof(mosquito_killing_timing_t));

    return 0;
}

/*********************************************************************
 * Get month length
 *********************************************************************/
uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
    uint8_t days = 31;

    if ( mon == 1 ) // feb
    {
        days = ( 28 + lpyr );
    }
    else
    {
        if ( mon > 6 ) // aug-dec
        {
            mon--;
        }

        if ( mon & 1 )
        {
            days = 30;
        }
    }

    return ( days );
}

static UTCTime convert_time_to_Second(UTCTimeStruct time)
{
    uint32_t i = 0;
    UTCTime offset = 0;

    //day time
    offset += time.seconds;
    offset += time.minutes* 60;
    offset += time.hour * 60 * 60;

    uint8_t leapYear = IsLeapYear(time.year + 2000);

    offset += SECOND_OF_DAY * (time.day - 1);

    for(i = 0; i < time.month - 1; ++i)   //month start from 1
    {
        offset += monthLength(leapYear,i) * SECOND_OF_DAY;
    }

    for(i = 0; i< time.year ; ++i)
    {
        if(IsLeapYear(i + 2000))
        {
            offset += SECOND_OF_DAY * 366;
        }
        else
        {
            offset += SECOND_OF_DAY * 365;
        }
    }

    return offset;

}

/**************************************************************************
* Calculte UTCTime
***************************************************************************/
void ConvertToUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
    // calculate the time less than a day - hours, minutes, seconds
    {
        uint32_t day = secTime % SECOND_OF_DAY;
        tm->seconds = day % 60UL;
        tm->minutes = (day % 3600UL) / 60UL;
        tm->hour = day / 3600UL;
    }

    // Fill in the calendar - day, month, year
    {
        uint16_t numDays = secTime / SECOND_OF_DAY;
        tm->year = BEGYEAR;
        while ( numDays >= YearLength( tm->year ) )
        {
            numDays -= YearLength( tm->year );
            tm->year++;
        }

        tm->month = 0;
        while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ))
        {
            numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
            tm->month++;
        }

        tm->day = numDays;
    }
}

static uint32_t app_mosquito_killing_calculate_time_diff(COBRA_RTC_TIME_DATE_T *current_time, COBRA_RTC_TIME_DATE_T *start_time)
{
    uint32_t _u32Res = 0;
    uint32_t _u32TimeCur = 0;
    uint32_t _u32TimeStart = 0;

    UTCTimeStruct tmCurr;
    UTCTimeStruct tmStart;

    tmCurr.year = current_time->YEAR;
    tmCurr.month = current_time->MONTH;
    tmCurr.day = current_time->DAY;
    tmCurr.hour = current_time->HOUR;
    tmCurr.minutes = current_time->MIN;
    tmCurr.seconds = current_time->SEC;

    tmStart.year = start_time->YEAR;
    tmStart.month = start_time->MONTH;
    tmStart.day = start_time->DAY;
    tmStart.hour = start_time->HOUR;
    tmStart.minutes = start_time->MIN;
    tmStart.seconds = start_time->SEC;


    _u32TimeCur = convert_time_to_Second(tmCurr);
    _u32TimeStart = convert_time_to_Second(tmStart);

    if(_u32TimeStart > _u32TimeCur)
    {
        __direct_printf("start time[%d] > current time[%d] error!!!\n", _u32TimeStart, _u32TimeCur);
    }
    else
    {
        _u32Res = _u32TimeCur - _u32TimeStart;
    }

    return _u32Res;
}
static uint8_t app_mosquito_killing_curr_month_update(void)
{
    uint8_t _u8Res = 0;
    uint8_t index = 0;
    COBRA_RTC_TIME_DATE_T rtc_time;
    cobra_rtc_get_current_time_date(&rtc_time);

    /*循环定时判断月份*/
//    index = rtc_time.YEAR - 21;  /*以21年为起始基准，计算其对应的月份；改为存储的第一次同步时间的年份*/

    _u8Res = (uint8_t)(app_mosquito_killing_year_base.year_base - 2000);
    index = rtc_time.YEAR - _u8Res;  /*以21年为起始基准，计算其对应的月份；改为存储的第一次同步时间的年份*/
    if((index>=0) && (index<10))
    {
        app_mosquito_killing_curr_month = index*12+rtc_time.MONTH - 1;

    }
    else
    {
        __direct_printf("period get date error[%d], base y[%d]\n", rtc_time.YEAR, app_mosquito_killing_year_base.year_base);
    }

    return app_mosquito_killing_curr_month;
}

void app_mosquito_killing_timing_check(void)
{
    uint8_t index=0;
    static uint32_t sec_of_day_curr = 0;
    static uint32_t sec_of_day_begin = 0;
    static uint32_t sec_of_day_end = 0;

    static uint32_t _u32TimeDiff = 0;    
	#if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
  	static uint32_t flag_active_one_time = 0;    
	#endif

    DAY_OF_WEEK day_of_week = MOn;;
    uint32_t SecondCountRTC = 0;
    UTCTimeStruct tm;

    COBRA_RTC_TIME_DATE_T rtc_time;
    cobra_rtc_get_current_time_date(&rtc_time);

    tm.year = rtc_time.YEAR;
    tm.month = rtc_time.MONTH;
    tm.day = rtc_time.DAY;
    tm.hour = rtc_time.HOUR;
    tm.minutes = rtc_time.MIN;
    tm.seconds = rtc_time.SEC;
    SecondCountRTC = convert_time_to_Second(tm);

    day_of_week = get_day_of_week(SecondCountRTC); // day of week

//    sec_of_day_curr = rtc_time.HOUR*60*60 + rtc_time.MIN*60 + rtc_time.SEC;
    sec_of_day_curr = SecondCountRTC;

    #if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
		 if(app_mosquito_killing_get_timing(index)->week_day ==0x80)
		 {
			  app_mosquito_killing_get_timing(index)->week_day |= (1 << day_of_week);
			 flag_active_one_time = 1;
		 }
		 if(app_mosquito_killing_get_timing(index)->week_day ==0x00)
			  flag_active_one_time = 0xaa;
		#endif
#if 0
    /*循环定时判断月份*/
    index = rtc_time.YEAR - 21;  /*以21年为起始基准，计算其对应的月份*/
    if((index>=0) && (index<10))
    {
        app_mosquito_killing_curr_month = index*12+rtc_time.MONTH - 1;

    }
    else
    {
        __direct_printf("period get date error[%d]\n", rtc_time.YEAR);
    }
#endif

    __direct_printf(" auto flag[%d], op key flag[%d], cur[%d], end[%d]\n", app_mosquito_killing_timing_setting.timing_auto_operate_flag, app_mosquito_killing_timing_setting.timing_key_operatoe_flag, sec_of_day_curr, sec_of_day_end);

//    if(app_mosquito_killing_timing_setting.timing_auto_operate_flag != 0)
//    {
//        _u32TimeDiff = app_mosquito_killing_calculate_time_diff(&rtc_time, &(app_mosquito_killing_timing_setting.timging_start_time));
//    }

    /*按键操作取消了本次的自动定时功能*/
    /*不能根据每天的定时标志位来处理，因为当结束时间小于开始时间时，是在第二天才结束的，需要从定时开始后通过计算定时的时间来判断本次定时是否已结束*/
    /*定时启动由每天的标志位和启动时间来决定，结束则由定时开始后的时间计数来判断*/
				#if 0
    if(app_mosquito_killing_timing_setting.timing_key_operatoe_flag != 0) /*在定时期间有按键操作，且本次定时还没有结束，则退出定时判断*/
    {
//		_u32TimeDiff = app_mosquito_killing_calculate_time_diff(&rtc_time, &(app_mosquito_killing_timing_setting.timging_start_time));
        /*定时期间有按键操作，timing_auto_operate_flag一定!= 0, 在前面统计计算时间差*/

//        if(app_mosquito_killing_timing_setting.timing_count_second<_u32TimeDiff)
	
        if(sec_of_day_curr <= sec_of_day_end)
        {
            return;
        }
        else
		
        {
            app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
            app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 0;
        }
    }
	#endif//Ganxiaoping debug timer bug 20220329
    for(index =0; index < MAX_TIMING_NUM; index ++)
    {
        if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 0)
        {
            /******************************************************************************************************************************************/
            /*计算定时时间段长度*/
            sec_of_day_begin = app_mosquito_killing_get_timing(index)->hour_begin*60*60 + app_mosquito_killing_get_timing(index)->min_begin*60;
            sec_of_day_end = app_mosquito_killing_get_timing(index)->hour_end*60*60 + app_mosquito_killing_get_timing(index)->min_end*60;

            if(sec_of_day_end > sec_of_day_begin) /*定时结束设置在同一天*/
            {
                app_mosquito_killing_timing_setting.timing_count_second = sec_of_day_end - sec_of_day_begin;
            }
            else
            {
                /*定时结束设置在第二天*/
                app_mosquito_killing_timing_setting.timing_count_second = sec_of_day_end + (23-app_mosquito_killing_get_timing(index)->hour_begin)*60*60 + (60 - app_mosquito_killing_get_timing(index)->min_begin)*60;
            }
            /******************************************************************************************************************************************/

            tm.hour = app_mosquito_killing_get_timing(index)->hour_begin;
            tm.minutes = app_mosquito_killing_get_timing(index)->min_begin;
            tm.seconds = 0;

            sec_of_day_begin = convert_time_to_Second(tm);
            sec_of_day_end = sec_of_day_begin + app_mosquito_killing_timing_setting.timing_count_second;

        }

#if 1
        /*only for test*/
        {
            static uint32_t _u32TestFlag = 0;
            _u32TestFlag++;
            if(_u32TestFlag > 10)  /*10s */
            {
                _u32TestFlag=0;
                __direct_printf("\nsec curr[%d], begin[%d], end[%d]; op flag[%d], count sec[%d], key flag[%d], week[%d], set w[0x%02x]\n", sec_of_day_curr, sec_of_day_begin, sec_of_day_end, app_mosquito_killing_timing_setting.timing_auto_operate_flag, app_mosquito_killing_timing_setting.timing_count_second, app_mosquito_killing_timing_setting.timing_key_operatoe_flag, day_of_week, app_mosquito_killing_get_timing(index)->week_day);
            }

        }
#endif

        if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 0)
        {
            if((sec_of_day_curr >= sec_of_day_begin) && (sec_of_day_curr <= sec_of_day_end))
            {

//            __direct_printf("Global_Time:%d-%d-%d\n",tm.year,tm.month,tm.day);
//            __direct_printf(" %d:%d|||dayofWeek:%d\n",tm.hour,tm.minutes,day_of_week);

                if(app_mosquito_killing_get_timing(index)->week_day & (1 << day_of_week))
                {
                    if((((app_mosquito_killing_get_timing(index)->week_day)>>7) & 0x01) == APP_MOSQUITO_KILLING_TIMING_ON)
                    {
                        if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 0)
                        {
                            /*power on*/
													 #if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
													if(flag_active_one_time < 4)
													{
														flag_active_one_time++;													
													}
													else
													{
														app_mosquito_killing_timing_setting.timging_start_time.YEAR  = tm.year;
														app_mosquito_killing_timing_setting.timging_start_time.MONTH = tm.month;
														app_mosquito_killing_timing_setting.timging_start_time.DAY =   tm.day;
														app_mosquito_killing_timing_setting.timging_start_time.HOUR = app_mosquito_killing_get_timing(index)->hour_begin;
														app_mosquito_killing_timing_setting.timging_start_time.MIN =  app_mosquito_killing_get_timing(index)->min_begin;
														app_mosquito_killing_timing_setting.timging_start_time.SEC = 0;
														return;
													}
													#endif
                            app_mosquito_killing_timing_setting.timing_auto_operate_flag = 1;

                            app_mosquito_killing_work_mode_all_on();
                        }
                    }
                    else
                    {
                        if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 0)
                        {
                            /*power off*/
													 #if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
													if(flag_active_one_time < 4)
													{
														flag_active_one_time++;													
													}
													else
													{
														app_mosquito_killing_timing_setting.timging_start_time.YEAR  = tm.year;
														app_mosquito_killing_timing_setting.timging_start_time.MONTH = tm.month;
														app_mosquito_killing_timing_setting.timging_start_time.DAY =   tm.day;
														app_mosquito_killing_timing_setting.timging_start_time.HOUR = app_mosquito_killing_get_timing(index)->hour_begin;
														app_mosquito_killing_timing_setting.timging_start_time.MIN =  app_mosquito_killing_get_timing(index)->min_begin;
														app_mosquito_killing_timing_setting.timging_start_time.SEC = 0;
														return;
													}
													#endif
                            app_mosquito_killing_timing_setting.timing_auto_operate_flag = 2;

                            app_mosquito_killing_work_mode_all_off();
                        }
                    }

                    app_mosquito_killing_timing_setting.timging_start_time.YEAR  = tm.year;
                    app_mosquito_killing_timing_setting.timging_start_time.MONTH = tm.month;
                    app_mosquito_killing_timing_setting.timging_start_time.DAY =   tm.day;
                    app_mosquito_killing_timing_setting.timging_start_time.HOUR = app_mosquito_killing_get_timing(index)->hour_begin;
                    app_mosquito_killing_timing_setting.timging_start_time.MIN =  app_mosquito_killing_get_timing(index)->min_begin;
                    app_mosquito_killing_timing_setting.timging_start_time.SEC = 0;
                }
            }
        }
        else
        {
//            if(app_mosquito_killing_get_timing(index)->week_day & (1 << day_of_week))
            if(sec_of_day_curr > sec_of_day_end) /*以累计时间来判断定时是否结束*/
            {
                if((((app_mosquito_killing_get_timing(index)->week_day)>>7) & 0x01) == APP_MOSQUITO_KILLING_TIMING_ON) /*每天的定时开或关设置一样*/
                {
                    if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 1)
                    {
//                        app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                        /*power off*/
													 #if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
													if(flag_active_one_time < 4)
													{
														flag_active_one_time++;													
													}
													else
													{
														app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                            app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 0;
														return;
													}
													#endif
                        app_mosquito_killing_work_mode_all_off();
                    }
                }
                else
                {
                    if(app_mosquito_killing_timing_setting.timing_auto_operate_flag == 2)
                    {
//                        app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                        /*power on*/
													 #if(APP_TIMEING_ACTIVE_ONLY_ONE_TIME==1)
													if(flag_active_one_time < 4)
													{
														flag_active_one_time++;													
													}
													else
													{
														app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                            app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 0;
														return;
													}
													#endif
                        app_mosquito_killing_work_mode_all_on();
                    }
                }

                app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 0;
            }
        }
    }

}
#endif

#if(APP_MOSQUITO_KILLING_POWER_ON_PWM_ENABLE == 1)
/*period 10ms, freq:100hz,精度是1ms,只能是10%步进*/
#define POWERON_TIME_MAX  3000
#define POWERON_PERIOD_MAX  10

static void app_mosquito_killing_execute_1ms_period(void)
{
    static uint32_t _u32Peroid =0;
//    static uint32_t _u32DetectInterval =POWERON_TIME_MAX/10;  /*步进一次的保持时间ms*/
    static uint32_t _u32DetectInterval =POWERON_TIME_MAX/7;  /*步进一次的保持时间ms*/


    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
    {
        PowerOnCount++;
        _u32Peroid++;

        if(PowerOnCount < _u32DetectInterval)  /*每个步进占空比保持时间300ms*/
        {
            if(_u32Peroid < POWERON_PERIOD_MAX)  /*10ms*/
            {
                if(_u32Peroid < PowerOnDuty)
                {
                    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_HIGH);
                    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_HIGH);
                }
                else
                {
                    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_LOW);
                    app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_LOW);
                }
            }
            else
            {
                _u32Peroid = 0;
            }
        }
        else
        {
            PowerOnDuty++;
            PowerOnCount = 0;
        }

        if(PowerOnDuty > 9)
        {
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_HIGH);
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_HIGH);

            cobra_softtimer_stop(&mosquito_killing_power_on_soft_timer);
        }
    }
}
#endif

#if (APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE == 1)
static void app_mosquito_killing_execute_5ms_period(void)
{
    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
    {
        app_mosquito_killing_statistics();
    }
}
#endif

static void app_mosquito_killing_execute_10ms_period(void)
{
    static uint32_t _u32Count =0;
    static uint32_t _u32DetectInterval =0;

    static uint8_t  _u8CountPrevent = 0;

    _u32Count++;

#if 0
    cobra_watchdog_irq_clear();
    return; /*only for test*/
#endif

    if(_u32Count%100 == 0)
    {
        __direct_printf(".");
    }

    if(app_mosquito_killing_power_on_delay_op_key == 0)
    {
        if(_u32Count >20)
        {
            app_mosquito_killing_power_on_delay_op_key = 1;
        }
    }

#if (APP_MOSQUITO_RTC_ACCURACY_TEST_ENABLE == 1)
    /*only for test*/
    if(_u32Count%100 == 0)
    {
        app_mosquito_killing_rtc_test_get_disp_time();
    }
#endif

#if (APP_MOSQUITO_POWER_ORDER_ENABLE == 1)
    /*分阶段上电开机*/
    if(PowerOnOrderStart == 1)
    {
        PowerOnOrderCount++;

        if(PowerOnOrderCount == 1)  /*10ms,立即开显示和高压板*/
        {
            app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_POWER_SWITCH_COUNT_DELAY ;

            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM, GPIO_HIGH);
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM, GPIO_HIGH);

            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM, GPIO_HIGH);

            app_mosquito_killing_tm1650_open();
            app_mosquito_killing_tm1650_disp_all_month_count();

        }
        else if(PowerOnOrderCount == 10) /*100ms*/
        {
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM, GPIO_HIGH);
        }
        else if(PowerOnOrderCount == 10+10)
        {
            app_mosquito_killing_gpio_set(APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM, GPIO_HIGH);

            PowerOnOrderStart = 0;
            PowerOnOrderCount = 0;
        }
    }
#endif

    if(_u32Count%100 == 0)
    {

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
        if(app_mosquito_killing_timing_setting.time_sync_finished == 1)
        {
            if(app_mosquito_killing_year_base.valid == 0xAA55)
            {
                app_mosquito_killing_curr_month_update();
            }

            if((((app_mosquito_killing_data_timing[0].week_day)>>7) & 0x01) == APP_MOSQUITO_KILLING_TIMING_ON)  /*使用了定时才去检测*/
            {
                app_mosquito_killing_timing_check();
            }
        }

#endif

        if(app_mosquito_killing_addr_save_flag == 1)
        {
            app_mosquito_killing_addr_save_flag = 0;
            app_mosquito_killing_mac_pid_write();
        }
#if 1
        if(app_mosquito_killing_statistics_flag > 0)
        {
            app_mosquito_killing_statistics_flag++;

            if(app_mosquito_killing_statistics_flag == APP_MOSQUITO_KILLING_SAVE_INTEVAL)
            {
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
            }
            else if(app_mosquito_killing_statistics_flag == APP_MOSQUITO_KILLING_SAVE_INTEVAL*2)
            {
                app_mosquito_killing_statistics_flag = 0;
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);
            }
        }
#endif


#if (APP_MOSQUITO_KILLING_WATCHDOG_ENABLE == 1)
        cobra_watchdog_irq_clear();
#endif
    }

    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
    {
        return;
    }

#if 0
    /*从power down到all on时会延时500ms再去检测放电计数*/
    if((app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON) && (app_mosquito_killing_work_state_pre == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN))
    {
        app_mosquito_killing_work_state_pre = APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON;

        app_mosquito_killing_adc_detectInterval = APP_MOSQUITO_KILLING_HV_ADC_DETECT_POWER_SWITCH_COUNT_DELAY ;  /*重新开机1s内不计放电数量*/
        __direct_printf("\n!!!!!interval [%d]\n", app_mosquito_killing_adc_detectInterval);
    }
#endif

#if 0  /*solved*/
    if(_u32Count%20 == 0)  /*200ms 开屏一次，防止偶尔的关屏(不知为何偶尔会关闭)*/
    {
        app_mosquito_killing_tm1650_open();
    }
#endif

#if (APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE == 1)
#else
    app_mosquito_killing_statistics();
#endif

    /*10*5 50ms至少可以保护动作*/
    if(app_mosquito_killing_detect_prevent_status() == 0)
    {
        if(_u8CountPrevent < 5)
        {
            _u8CountPrevent++;
        }
        else if(_u8CountPrevent == 5)
        {
            _u8CountPrevent++;  /*确保只操作一次*/

            app_mosquito_killing_work_mode_all_off();
        }
    }
    else
    {
        _u8CountPrevent = 0;
    }
}


/**************************************************************************************************************/
/*timer */
void app_mosquito_killing_stop_timer(ke_msg_id_t const timer_id)
{
    __printf("timer_id[%d=0x%x]\n", timer_id, timer_id)

    ke_timer_clear(timer_id, TASK_APP);
}

void app_mosquito_killing_start_timer(ke_msg_id_t const timer_id, uint32_t delay)
{
//    __printf("timer_id[%d=0x%x], delay[%d](10ms)\n", timer_id,timer_id, delay)

    ke_timer_set(timer_id, TASK_APP, delay);
}

static int app_mosquito_killing_msg_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Do nothing


    return (KE_MSG_CONSUMED);
}

static int app_mosquito_killing_default_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
//	__here;
    static uint8_t _u8tmp = 0;


    return (KE_MSG_CONSUMED);
}

static int app_mosquito_killing_restore_default_flicker_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
//	__here;
    static uint8_t _u8tmp = 0;
    static uint8_t _u8tmp1 = 0;

    __direct_printf("restoe timer[%d][%d]\n", _u8tmp, _u8tmp1);

    app_mosquito_killing_restore_flag = 1;

    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN)
    {
        if(_u8tmp == 0)
        {
            _u8tmp = 1;
            app_mosquito_killing_tm1650_open();
            app_mosquito_killing_tm1650_restore_disp();
        }
        else if(_u8tmp == 1)
        {
            _u8tmp = 0;
            app_mosquito_killing_tm1650_disp_clear();
            app_mosquito_killing_tm1650_close();

            _u8tmp1++;
        }

        if(_u8tmp1 < APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_COUNT)
        {
            app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_TIMER, APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_INTERVAL);
        }
        else
        {
            _u8tmp = 0;
            _u8tmp1 = 0;
            app_mosquito_killing_restore_flag = 0;
        }
    }

    return (KE_MSG_CONSUMED);
}

static int app_mosquito_killing_breath_flicker_handler(void)
{
//	__here;
    static uint8_t _u8TmpBreathTime = 0;
    static uint8_t _u8TmpCount = 0;
    static uint8_t _u8TmpDirect = 0;


    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
    {

        if(_u8TmpDirect == 0)
        {
            /*渐变暗*/
            if(_u8TmpCount*APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_STEP < 100)
            {
                app_mosquito_killing_pwm_set(1000, 100 - _u8TmpCount*APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_STEP);
                _u8TmpCount++;
            }
            else
            {
                _u8TmpCount = 0;
                _u8TmpDirect = 1;
            }
        }
        else if(_u8TmpDirect == 1)
        {
            /*渐变亮*/
            if(_u8TmpCount*APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_STEP < 100)
            {
                app_mosquito_killing_pwm_set(1000, _u8TmpCount*APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_STEP);
                _u8TmpCount++;
            }
            else
            {
                _u8TmpCount = 0;
                _u8TmpDirect = 2;
            }
        }
        else
        {
            /*完成一次渐变*/
            _u8TmpCount = 0;
            _u8TmpDirect = 0;
            _u8TmpBreathTime++;
        }

        if(_u8TmpBreathTime < APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_MAX)
        {
            app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_BREATH_FLICKER_TIMER, APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_INTERVAL);
        }
        else
        {
            _u8TmpCount = 0;
            _u8TmpDirect = 0;
            _u8TmpBreathTime=0;
        }
    }

    return (KE_MSG_CONSUMED);
}

static int app_mosquito_killing_top_led_flicker_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
//	__here;
    static uint8_t _u8tmp = 0;
    static uint8_t _u8tmp1 = 0;

    if(app_mosquito_killing_work_state == APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON)
    {
        if(_u8tmp == 0)
        {
            _u8tmp = 1;
            app_mosquito_killing_pwm_set(1000, 0);  /*off*/

        }
        else if(_u8tmp == 1)
        {
            _u8tmp = 0;
            app_mosquito_killing_pwm_set(1000, 100);  /*on*/

            _u8tmp1++;
        }

        if(_u8tmp1 < APP_MOSQUITO_KILLING_TOP_LED_FLICKER_COUNT)
        {
            app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_BREATH_TOP_LED_TIMER, APP_MOSQUITO_KILLING_TOP_LED_FLICKER_INTERVAL);
        }
        else
        {
            _u8tmp = 0;
            _u8tmp1 = 0;
        }
    }

    return (KE_MSG_CONSUMED);
}


/// Default State handlers definition
const struct ke_msg_handler app_mosquito_killing_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER, (ke_msg_func_t)app_mosquito_killing_msg_handler},

    {APP_MOSQUITO_KILLING_DEFAULT_TIMER, (ke_msg_func_t)app_mosquito_killing_default_handler},
    {APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_TIMER, (ke_msg_func_t)app_mosquito_killing_restore_default_flicker_handler},
    {APP_MOSQUITO_KILLING_BREATH_FLICKER_TIMER, (ke_msg_func_t)app_mosquito_killing_breath_flicker_handler},
    {APP_MOSQUITO_KILLING_BREATH_TOP_LED_TIMER, (ke_msg_func_t)app_mosquito_killing_top_led_flicker_handler},

};
const struct app_subtask_handlers app_mosquito_killing_handlers = APP_HANDLERS(app_mosquito_killing);
/**************************************************************************************************************/

void app_mosquito_killing_soft_reset(void)
{
    cobra4_aon_wk_protection_en(1);
    cobra4_aon_wk_aon_soft_sysrst_en(1);
    cobra_pcrm_soft_reset_whole_chip(1);
    cobra4_aon_wk_protection_en(0);
}

/**************************************************************************************************************/
#if (APP_MOSQUITO_KILLING_WATCHDOG_ENABLE == 1)
static void app_mosquito_killing_watchdog_init(void)
{
    __here;
    cobra_watchdog_init();

    /*test result:1s*/
//	app_cobra_watchdog_init(1000, WATCHDOG_RESET);
    /*test result:8s*/
//    app_cobra_watchdog_init(100*1000, WATCHDOG_RESET);
    app_cobra_watchdog_init(320*1000, WATCHDOG_RESET);  /*for debug, swddownload==>Download time:18190ms,*/
}
#endif

/*************************************************************************************/
/*following test code*/
#if 0
#define   M4_EFLASH_TEST_ADDRESS  (0x50000 - 0x3000)
static void app_rainbow_data_write(uint32_t param)
{
    unsigned char  temp_buffer[4];
    temp_buffer[3] = param>>24&0xFF;
    temp_buffer[2] = param>>16&0xFF;
    temp_buffer[1] = param>>8&0xFF;
    temp_buffer[0] = param&0xFF;

//	__direct_printf("[0x%x], [0x%x], [0x%x], [0x%x]\n", temp_buffer[0], temp_buffer[1], temp_buffer[2], temp_buffer[3]);

    cobra_flash_erase(M4_EFLASH_TEST_ADDRESS, 4096);

    cobra_flash_write(M4_EFLASH_TEST_ADDRESS, sizeof(temp_buffer), temp_buffer);

    cobra_cache_invalidate_the_cache();

//    __direct_printf("w M4_EFLASH_TEST_ADDRESS[0x%x], data[0x%x]\n", M4_EFLASH_TEST_ADDRESS, param);
}
static uint32_t app_rainbow_data_read(void)
{
//    uint8_t *  p_count = (uint8_t * )(0x00800000 + M4_EFLASH_TEST_ADDRESS);
    uint32_t * _u32Buf = (uint32_t * )(0x00800000 + M4_EFLASH_TEST_ADDRESS);

//    printf("eflash_test %x %x %x %x\n",*p_count,*(p_count+1),*(p_count+2),*(p_count+3));

//    __direct_printf("r data[0x%x]\n", *_u32Buf);

    return  (*_u32Buf);
}
#endif

static void app_mosquito_killing_printf_switch(uint8_t param)
{
    TRACE_LEVEL = param;
    __direct_printf("TRACE_LEVEL[%d]\n", TRACE_LEVEL);
}


static void app_mosquito_killing_printf(void)
{
    uint8_t  _u8ArrTemp[20];
    uint16_t _u16Temp = 0;

    uint8_t _u8Temp = 0;
    uint32_t _u32Temp = 0;
    __direct_printf("\n********************************************\n");
    __direct_printf("%s %s\n", __DATE__, __TIME__);

    __direct_printf("statistics month count:\n");
    _u32Temp = 0;
    for(int i=0; i<APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX; i++)
    {
        _u32Temp += app_mosquito_killing_month_count[i];
        __direct_printf("m[%03d]=[%04d]  ", i, app_mosquito_killing_month_count[i]);
        if((i+1)%6 == 0)
            __direct_printf("\n");
    }
    __direct_printf("\ntotal count[%d]\n", _u32Temp);


    __direct_printf("work state[%d](1-all on, 2-bot on, 0-off)\n", app_mosquito_killing_work_state);
    __direct_printf("workstate pre[%d]\n", app_mosquito_killing_work_state_pre);
    __direct_printf("interval[%d]\n", app_mosquito_killing_adc_detectInterval);
    __direct_printf("bt_connect_count[%d], conn_bt_flag[%d]\n", app_mosquito_killing_bt_connect_count, app_mosquito_killing_timing_setting.conn_bt_flag);
    __direct_printf("curr_month[%d]\n", app_mosquito_killing_curr_month);

    _u32Temp = app_mosquito_killing_ADC_voltage();
    __direct_printf("adc [%d]\n", _u32Temp);

    _u8Temp = app_mosquito_killing_detect_prevent_status();
    __direct_printf("detect prevent status[%d](1--permit)\n", _u8Temp);

#if 0
    _u32Temp = &app_mosquito_killing_month_count[0];
    __direct_printf("arr addr[0x%x]\n", _u32Temp);
    _u32Temp = &app_mosquito_killing_month_count;
    __direct_printf("arr addr[0x%x]\n", _u32Temp);
    _u32Temp = app_mosquito_killing_month_count;
    __direct_printf("arr addr[0x%x]\n", _u32Temp);
#endif

    __direct_printf("save date [%d-%d-%d %d:%d:%d] w[%d] \n",app_mosquito_killing_save_date.year, app_mosquito_killing_save_date.month, app_mosquito_killing_save_date.day, app_mosquito_killing_save_date.hour, app_mosquito_killing_save_date.min, app_mosquito_killing_save_date.sec, app_mosquito_killing_save_date.week);


    COBRA_RTC_TIME_DATE_T rtc_time;
    cobra_rtc_get_current_time_date(&rtc_time);
    UTCTimeStruct tm;
    tm.year = rtc_time.YEAR;
    tm.month = rtc_time.MONTH;
    tm.day = rtc_time.DAY;
    tm.hour = rtc_time.HOUR;
    tm.minutes = rtc_time.MIN;
    tm.seconds = rtc_time.SEC;
    __direct_printf("rtc time [%d-%d-%d %d:%d:%d] ,week[%d](0~6)\n",rtc_time.YEAR, rtc_time.MONTH, rtc_time.DAY, rtc_time.HOUR, rtc_time.MIN, rtc_time.SEC, get_day_of_week(convert_time_to_Second(tm)));

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
    __direct_printf(" begin/h m end/h min onoff/dayofweek\n");
    for(int i=0; i<MAX_TIMING_NUM; i++)
    {
        __direct_printf("[%02d][%02d]   [%02d][%02d]  [0x%02x]\n",app_mosquito_killing_data_timing[i].hour_begin, app_mosquito_killing_data_timing[i].min_begin, app_mosquito_killing_data_timing[i].hour_end, app_mosquito_killing_data_timing[i].min_end, app_mosquito_killing_data_timing[i].week_day);
    }

    __direct_printf("timing sync flag[%d],auto flag[%d], op key flag[%d], timing count[%d]sec\n",app_mosquito_killing_timing_setting.time_sync_finished, app_mosquito_killing_timing_setting.timing_auto_operate_flag, app_mosquito_killing_timing_setting.timing_key_operatoe_flag, app_mosquito_killing_timing_setting.timing_count_second);
    __direct_printf("start timimg time [%d-%d-%d %d:%d:%d] \n",app_mosquito_killing_timing_setting.timging_start_time.YEAR, app_mosquito_killing_timing_setting.timging_start_time.MONTH, app_mosquito_killing_timing_setting.timging_start_time.DAY, app_mosquito_killing_timing_setting.timging_start_time.HOUR, app_mosquito_killing_timing_setting.timging_start_time.MIN, app_mosquito_killing_timing_setting.timging_start_time.SEC);

#endif


    __direct_printf(" mac[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]\n",app_mosquito_killing_bt_addr[0], app_mosquito_killing_bt_addr[1], app_mosquito_killing_bt_addr[2], app_mosquito_killing_bt_addr[3], app_mosquito_killing_bt_addr[4], app_mosquito_killing_bt_addr[5]);
    __direct_printf(" pid[0x%02x][0x%02x][0x%02x][0x%02x]\n",app_mosquito_killing_pid[0], app_mosquito_killing_pid[1], app_mosquito_killing_pid[2], app_mosquito_killing_pid[3]);
    __direct_printf(" pd[0x%02x][0x%02x][0x%02x][0x%02x]\n",app_mosquito_killing_password[0], app_mosquito_killing_password[1], app_mosquito_killing_password[2], app_mosquito_killing_password[3]);

    __direct_printf("name len[%d]:\n", sizeof(app_mosquito_killing_name));
    for(int i=0; i<sizeof(app_mosquito_killing_name); i++)
    {
        __direct_printf("[0x%02x]",app_mosquito_killing_name[i]);
    }
    __direct_printf("\n");

    __direct_printf("bak_work_state[%d], bak_time_sync_finished[%d], res[%d][%d]\n", app_mosquito_killing_setting_bak.bak_work_state, app_mosquito_killing_setting_bak.bak_time_sync_finished, app_mosquito_killing_setting_bak.reserve, app_mosquito_killing_setting_bak.reserve01);

    _u8Temp = 6;
    if(NVDS_OK == nvds_get(PARAM_ID_BD_ADDRESS, &_u8Temp, (uint8_t *)&_u8ArrTemp))
    {
        __direct_printf("r addr[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]\n",_u8ArrTemp[0], _u8ArrTemp[1], _u8ArrTemp[2], _u8ArrTemp[3], _u8ArrTemp[4], _u8ArrTemp[5]);
    }
    else
    {
        __direct_printf("r addr failed!!!\n");
    }

    __direct_printf("M4_EFLASH_NVDS_ADDRESS[0x%x], M0_M3_NVDS_SIZE[0x%x]\n", M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);
    __direct_printf("M4_EFLASH_MAC_PID_ADDRESS[0x%x]\n", M4_EFLASH_MAC_PID_ADDRESS);
    __direct_printf("FLASH_SETTING_DATA_START_ADDR[0x%x]\n", FLASH_SETTING_DATA_START_ADDR);
    __direct_printf("FLASH_SETTING_DATA_BACKUP_START_ADDR[0x%x]\n", FLASH_SETTING_DATA_BACKUP_START_ADDR);

    __direct_printf("\nfollowing addr must be 4bytes align:\n");
    __direct_printf("TIMING_SETTING_SIZE[%d], STATISTICS_MONTH_SZIE[%d], SAVE_DATE_SIZE[%d],SAVE_BT_CONNECT_COUNT_SIZE[%d]\n", TIMING_SETTING_SIZE, STATISTICS_MONTH_SZIE, SAVE_DATE_SIZE,SAVE_BT_CONNECT_COUNT_SIZE );
    __direct_printf("SETTING_BAK_SIZE[%d]\n", SETTING_BAK_SIZE);
    __direct_printf("FLASH_SETTING_CRC_HEADER_SZIE[%d], FLASH_SETTING_SIZE[%d] \n",FLASH_SETTING_CRC_HEADER_SZIE, FLASH_SETTING_SIZE);
    __direct_printf("YEAR_BASE_SIZE[%d]\n", YEAR_BASE_SIZE);


    __direct_printf("cobra_system_get_m3_apb0_clk[%d]\n", cobra_system_get_m3_apb0_clk());

#if 1
    _u32Temp = *(uint32_t*)(COBRA_PINMUX_BASE + 0x42*4);
    __direct_printf("[0x%08x](0x01=hd, 0x0c=softreset, 0x0a=watchdog, 0x00=power on)\n", _u32Temp);
#endif

    _u32Temp = *(uint32_t*)(0x4008B044);
    __direct_printf("rtc [0x%08x]\n", _u32Temp);

#if (APP_MOSQUITO_KILLING_DETECT_DIFF_ENABLE == 1)
    __direct_printf("APP_MOSQUITO_KILLING_DETECT_DIFF_ENABLE = 1\n");
#else
    __direct_printf("APP_MOSQUITO_KILLING_DETECT_DIFF_ENABLE = 0\n");
#endif

#if (APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE == 1)
    __direct_printf("APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE = 1\n");
    __direct_printf("APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MIN[%d], APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MAX[%d]\n", APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MIN, APP_MOSQUITO_KILLING_HV_ADC_DETECT_DIFF_MAX);
#else
    __direct_printf("APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE = 0\n");
#endif

#if (APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE == 1)
	__direct_printf("APP_MOSQUITO_KILLING_LINEAR_RATIO_TIME[%d]min\n", APP_MOSQUITO_KILLING_LINEAR_RATIO_TIME);
	__direct_printf("APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT[%d]min\n", APP_MOSQUITO_KILLING_HV_ADC_DETECT_LINEAR_TIME_LIMIT*2/1000/60);
#else
	__direct_printf("APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE = 0\n");
#endif

    __direct_printf("year base[%d], valid[0x%x]\n", app_mosquito_killing_year_base.year_base, app_mosquito_killing_year_base.valid);

	_u32Temp = rf_icyv2_reg_rd(0xB1);
	__direct_printf("freq trim reg 0xB1=[0x%0x](%d)\n", _u32Temp, _u32Temp);

#if 0
    /*test overflow*/
    _u16Temp = 0xffff-2;
    _u8Temp = 0xff -2;
//	_u32Temp = 0xffffffff -2;
    _u32Temp = 0x7fffffff -2;
    for(int i=0; i< 10; i++)
    {
        printf("u8[0x%x, %d], u16[0x%x, %d], u32[0x%x, %d, %u]\n", _u8Temp, _u8Temp, _u16Temp, _u16Temp, _u32Temp, _u32Temp, _u32Temp);
        _u8Temp++;
        _u16Temp++;
        _u32Temp++;
    }

	_u32Temp = 0xFFFFFFFF;
	if(_u32Temp > 0x1A000)
	{
		__direct_printf("test compare 0xFFFFFFFF > 0x1A000\n");
	}
#endif

}

static int uart_input_mosquito_killing_protocol(uint8_t inChar)
{
    static uint8_t u8RecCmd = 0;
    static uint8_t u8RecData1 = 0;
    static uint8_t u8RecData2 = 0;

    static uint32_t _u32tmp = 0;

//	printf("[0x%x]\n", inChar);

#if (APP_MOSQUITO_ADD_TEST_CODE_ENABLE == 1)
    app_mosquito_killing_bt_data_assemble(&inChar, 1);
#endif

    if(inChar == 0xAA)
    {
        _u32tmp = 1;
    }
    else if(_u32tmp == 1)
    {
        _u32tmp = 2;
        u8RecCmd = inChar; /*cmd*/
    }
    else if(_u32tmp == 2)
    {
        _u32tmp = 3;
        u8RecData1 = inChar;  /*data1*/
    }
    else if(_u32tmp == 3)
    {
        _u32tmp = 4;
        u8RecData2 = inChar;  /*data2*/
    }

    if(_u32tmp == 4)
    {
        _u32tmp = 0;
    }
    else
    {
        return 0;
    }

//    __direct_printf("cmd[%d], data1[%d], data2[%d]\n", u8RecCmd, u8RecData1, u8RecData2);

    switch(u8RecCmd)
    {
        case 0:
            app_mosquito_killing_printf();
            break;

        case 1:
            if(u8RecData2 == 0)
            {
//                app_mosquito_killing_tm1650_open();
                app_mosquito_killing_tm1650_disp(900+u8RecData1);
            }
            else if(u8RecData2 == 1)
            {
                __direct_printf("app_mosquito_killing_tm1650_close...\n");
                app_mosquito_killing_tm1650_close();
            }
            else if(u8RecData2 == 2)
            {
                __direct_printf("app_mosquito_killing_tm1650_open...\n");
                app_mosquito_killing_tm1650_open();
            }
            else if(u8RecData2 == 3)
            {
                app_mosquito_killing_tm1650_disp(u8RecData1);
//                app_mosquito_killing_tm1650_disp(888);
            }
            else if(u8RecData2 == 5)
            {
                static uint32_t _u32Temp05 = 0;
                _u32Temp05++;
                app_mosquito_killing_tm1650_disp(_u32Temp05%100);
            }
            else if(u8RecData2 == 6)
            {
                static uint32_t _u32Temp06 = 0;
                _u32Temp06++;
                app_mosquito_killing_tm1650_disp(_u32Temp06);
            }
            else if(u8RecData2 == 4)
            {
                app_mosquito_killing_tm1650_disp_debug(u8RecData1);
            }
            else if(u8RecData2 == 7)
            {
                app_mosquito_killing_tm1650_brightness_debug(u8RecData1);
            }

            break;

        case 2:
            app_mosquito_killing_pwm_set(1000, u8RecData1);
            __direct_printf("set pwm 1000Hz, duty[%d]\n",u8RecData1);

            break;

        case 3:
            app_wechat_send_data((uint8_t *)&u8RecData1, sizeof(u8RecData1));
            break;

        case 4:
        {
            COBRA_RTC_TIME_DATE_T time;

            if(u8RecData1 == 0)
            {
                time.DAY	= 22;
            }
            else if(u8RecData1 == 1)
            {
                time.DAY	= 23;
            }
            else
            {
                time.DAY	= 24;
            }

            time.SUBSEC = 0;
            time.SEC    = 0;
            time.MIN    = 10;
            time.HOUR   = 10;
            time.MONTH  = 11;
            time.YEAR   = 21;


            cobra_rtc_timer_write_en(1);
            cobra_rtc_set_current_time_date(&time);
            cobra_rtc_timer_write_en(0);
            __direct_printf("set time y[%d], m[%d], d[%d], h[%d],min[%d], s[%d] \n",time.YEAR, time.MONTH, time.DAY, time.HOUR, time.MIN, time.SEC);

            break;
        }

        case 5:
            if(u8RecData1 == 1)
            {
                __direct_printf("1 timing data flash write test...\n");
                for(int i=0; i<MAX_TIMING_NUM; i++)
                {
                    app_mosquito_killing_data_timing[i].hour_begin = u8RecData2;
                    app_mosquito_killing_data_timing[i].min_begin = u8RecData2;
                    app_mosquito_killing_data_timing[i].hour_end = u8RecData2;
                    app_mosquito_killing_data_timing[i].min_end = u8RecData2;
                    app_mosquito_killing_data_timing[i].week_day = 0x7f;
                }
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);
            }
            else if(u8RecData1 == 2)
            {
                __direct_printf("2 timing data flash write test...\n");
                app_mosquito_killing_timing_setting.time_sync_finished = 1;

                for(int i=0; i<MAX_TIMING_NUM; i++)
                {
                    app_mosquito_killing_data_timing[i].hour_begin = 10;
                    app_mosquito_killing_data_timing[i].min_begin = 11;

                    app_mosquito_killing_data_timing[i].hour_end = 10;
                    app_mosquito_killing_data_timing[i].min_end = 12;

                    app_mosquito_killing_data_timing[i].week_day = 0x7f;
                }
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
                app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_BACKUP_START_ADDR);
            }
            else
            {
                __direct_printf("timing data flash load test...\n");
                app_mosquito_killing_setting_data_load();
            }

            break;

        case 6:
            if(u8RecData1 == 1)
            {
                app_m3_gpio_write_pin(u8RecData2, GPIO_HIGH);
                __direct_printf("set io[%d] GPIO_HIGH\n", u8RecData2);
            }
            else
            {
                app_m3_gpio_write_pin(u8RecData2, GPIO_LOW);
                __direct_printf("set io[%d] GPIO_LOW\n", u8RecData2);
            }
            break;

        case 7:
            if(u8RecData1 == 1)
            {
                app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_BREATH_FLICKER_TIMER, 1);
            }
            else
            {
                app_mosquito_killing_start_timer(APP_MOSQUITO_KILLING_BREATH_TOP_LED_TIMER, 1);
            }
            break;

        case 8:
            if(u8RecData1 == 1)
            {
                app_mosquito_killing_mac_pid_load();
            }
            else if(u8RecData1 == 2)
            {
                app_mosquito_killing_bt_addr[0] = u8RecData2;
                app_mosquito_killing_bt_addr[1] = 0x11;
                app_mosquito_killing_bt_addr[2] = 0x12;
                app_mosquito_killing_bt_addr[3] = 0x13;
                app_mosquito_killing_bt_addr[4] = 0x14;
                app_mosquito_killing_bt_addr[5] = 0x15;

                app_mosquito_killing_pid[0] = u8RecData2+1;
                app_mosquito_killing_pid[1] = 0x62;
                app_mosquito_killing_pid[2]	= 0x63;
                app_mosquito_killing_pid[3] = 0x64;

                app_mosquito_killing_password[0] = u8RecData2+2;
                app_mosquito_killing_password[1] = 0x72;
                app_mosquito_killing_password[2] = 0x73;
                app_mosquito_killing_password[3] = 0x74;

                for(int i=0; i< 16; i++)
                {
                    app_mosquito_killing_name[i] = 0x30+i;
                }

                app_mosquito_killing_mac_pid_write();
            }

            break;

        case 9:
            if(u8RecData1 == 1)
            {
                app_mosquito_killing_restore_default_setting();
            }
            else if(u8RecData1 == 2)
            {
//                app_mosquito_killing_setting_data_load();
                app_mosquito_killing_setting_data_read(FLASH_SETTING_DATA_START_ADDR);
                app_mosquito_killing_setting_data_read(FLASH_SETTING_DATA_BACKUP_START_ADDR);
            }
            break;

        case 10:
            __direct_printf("watchdog test...\n");
            app_cobra_watchdog_init(1000, WATCHDOG_RESET);
            break;

        case 11:
            __direct_printf("sofreset...\n");
            app_mosquito_killing_soft_reset();
            break;

        case 12:
            app_mosquito_killing_printf_switch(u8RecData1);
            break;

        default:
            __direct_printf("uart test error  no cmd[%d]\n", u8RecCmd);
            break;
    }

    return 1;
}
/**************************************************************************************************************/

#endif

