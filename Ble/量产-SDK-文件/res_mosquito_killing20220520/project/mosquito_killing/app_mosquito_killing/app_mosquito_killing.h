
#ifndef _APP_MOSQUITO_KILLING_H
#define _APP_MOSQUITO_KILLING_H

#include "app_config.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "cobra_system.h"
#define  APP_TIMEING_ACTIVE_ONLY_ONE_TIME     1
#define  APP_MOSQUITO_KILLING_WORK_MODE_ALL_ON             0x01
#define  APP_MOSQUITO_KILLING_WORK_MODE_BOT_ON             0x02
#define  APP_MOSQUITO_KILLING_WORK_MODE_POWER_DOWN         0x00

#define  APP_MOSQUITO_KILLING_WORK_MODE_ZERO_CLEAR         0x04

#define APP_MOSQUITO_KILLING_BT_LED_PIN_NUM      GPIO_PIN_13

#define APP_MOSQUITO_KILLING_TOP_UV_LED_PIN_NUM    GPIO_PIN_5

#define APP_MOSQUITO_KILLING_PREVENT_TOP_SWITCH_PIN_NUM     GPIO_PIN_29
#define APP_MOSQUITO_KILLING_PREVENT_BOTTOM_SWITCH_PIN_NUM  GPIO_PIN_14

#define APP_MOSQUITO_KILLING_TOP_FAN_PIN_NUM     GPIO_PIN_28
#define APP_MOSQUITO_KILLING_BOTTOM_FAN_PIN_NUM  GPIO_PIN_27

#define APP_MOSQUITO_KILLING_BOTTOM_UV_LED_V_OUT_CON_PIN_NUM     GPIO_PIN_31
#define APP_MOSQUITO_KILLING_HV_OUT_PIN_NUM    GPIO_PIN_8

#define  APP_MOSQUITO_KILLING_PWM_LIMIT_MIN  1
#define  APP_MOSQUITO_KILLING_PWM_LIMIT_MAX  99

/*unit 10ms*/
#define APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_INTERVAL    25
#define APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_COUNT    3

#define APP_MOSQUITO_KILLING_TOP_LED_FLICKER_INTERVAL  25
#define APP_MOSQUITO_KILLING_TOP_LED_FLICKER_COUNT  10

/*修改后5秒才执行保持操作*/
#define APP_MOSQUITO_KILLING_SAVE_INTEVAL  5 

/*unit 10ms*/
/*慢:呼吸灯效果，0~100%强度，变亮 变暗各1秒,共变化100次(亮灭各50次),每次调2%*/
#define  APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_INTERVAL  (3)  /*每次渐变的时间*10ms*/
//#define  APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_COUNT  (100)  /*一共渐变的次数*/
#define  APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_STEP  (2)   /*每次渐变的步长*/
#define  APP_MOSQUITO_KILLING__LIGHT_FLICK_SLOW_MAX  (3)   /*一共呼吸的次数*/


//#define  APP_MOSQUITO_KILLING_POWER_ON_KEY_DURATION  3
//#define  APP_MOSQUITO_KILLING_RESTORE_KEY_DURATION   4

/*140*20ms=2800ms=2.8s*/
#define  APP_MOSQUITO_KILLING_POWER_ON_KEY_DURATION  145
#define  APP_MOSQUITO_KILLING_RESTORE_KEY_DURATION   230

extern void app_mosquito_killing_init(void);
extern void app_mosquito_killing_io_init(void);

extern void app_mosquito_killing_gpio_set(uint8_t pinNum, uint8_t param);
extern uint8_t  app_mosquito_killing_gpio_get(uint8_t pinNum);
extern uint8_t  app_mosquito_killing_detect_prevent_status(void);
extern uint8_t  app_mosquito_killing_statistics(void);
extern void app_mosquito_killing_bt_led_onoff(uint8_t param);

/*************************************************************************************/
/*timer*/
extern const struct app_subtask_handlers app_mosquito_killing_handlers;

extern void app_mosquito_killing_stop_timer(ke_msg_id_t const timer_id);
extern void app_mosquito_killing_start_timer(ke_msg_id_t const timer_id, uint32_t delay);
/*************************************************************************************/

#endif  /* APP_MOSQUITO_KILLING_ENABLE */

#endif  /* _APP_MOSQUITO_KILLING_H */


