#ifndef _MOSQUITO_KILLING_DISPH_H
#define _MOSQUITO_KILLING_DISPH_H

#include "app_config.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "cobra_system.h"

/*error code*/
#define   APP_MOSQUITO_KILLING_TM1650_ERROR_PROTECT_SWITCH  0x00


/*0~7, 0=max, 1~7依次增加*/
#define   APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS  0x00
/*0=8段，1-7段*/
#define   APP_MOSQUITO_KILLING_TM1650_DEF_SEG  0x00
/*0=正常工作 ，1=待机*/
#define   APP_MOSQUITO_KILLING_TM1650_DEF_WORK_MODE  0x00
/*0=关屏显示，1=开屏显示*/
#define   APP_MOSQUITO_KILLING_TM1650_DEF_ONOFF_MODE  0x01

#define   APP_MOSQUITO_KILLING_TM1650_CMD_ADDR  0x48
/*一次最大通讯的字节数，4个位，每位两个字节*/
#define   APP_MOSQUITO_KILLING_TM1650_COMM_LEN_MAX  (4*2)

#define  APP_MOSQUITO_KILLING_TM1650_DISP_COUNT_MAX 999
/*new hardware */
#define APP_MOSQUITO_KILLING_CLK_PIN_NUMBER		    GPIO_PIN_3
#define APP_MOSQUITO_KILLING_CLK_PIN_MODE			PIN_MODE_13
#define APP_MOSQUITO_KILLING_CLK_PIN_INDEX			PIN3_INDEX

#define APP_MOSQUITO_KILLING_DAT_PIN_NUMBER		    GPIO_PIN_4
#define APP_MOSQUITO_KILLING_DAT_PIN_MODE			PIN_MODE_13
#define APP_MOSQUITO_KILLING_DAT_PIN_INDEX			PIN4_INDEX

#define SET_CLK_HIGH  app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_CLK_PIN_NUMBER, GPIO_HIGH)
#define SET_CLK_LOW   app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_CLK_PIN_NUMBER, GPIO_LOW)

#define SET_DAT_HIGH    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_HIGH)
#define SET_DAT_LOW     app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_LOW)
#define SET_DAT_OUTPUT  app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_OUTPUT)
#define SET_DAT_INPUT  app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_INPUT)
#define GET_DAT_STATUS  app_m3_gpio_read_pin(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER)

//#define COMM_DELAY  __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop()
#define COMM_DELAY  __nop();__nop();__nop();__nop();__nop();__nop()
//#define COMM_DELAY  __nop();__nop();__nop()
//#define COMM_DELAY  __nop()

void app_mosquito_killing_tm1650_init(uint8_t paraBrightness, uint8_t paramSeg, uint8_t paramWorkMode, uint8_t paramOnOffMode);
void app_mosquito_killing_tm1650_disp(uint32_t paramDataDisp);
void app_mosquito_killing_disp_init(void);
void app_mosquito_killing_tm1650_disp_all_month_count(void);

void app_mosquito_killing_tm1650_restore_disp(void);
void app_mosquito_killing_tm1650_disp_clear(void);
void app_mosquito_killing_tm1650_ota_disp(void);

void app_mosquito_killing_tm1650_open(void);
void app_mosquito_killing_tm1650_close(void);

void app_mosquito_killing_tm1650_disp_debug(uint8_t param);
void app_mosquito_killing_tm1650_brightness_debug(uint8_t param);

#endif  /* APP_MOSQUITO_KILLING_ENABLE */

#endif  /* _MOSQUITO_KILLING_DISPH_H */
