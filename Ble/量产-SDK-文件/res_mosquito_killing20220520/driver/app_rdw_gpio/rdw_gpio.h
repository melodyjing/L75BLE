//liujian add 
//2018/8/6
//GPIO DRIVER

#ifndef __RDW_GPIO_H__
#define __RDW_GPIO_H__


#ifdef __cplusplus
extern "C"
{
#endif
#include "cobra_m4_gpio.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
	
/// Enumeration of GPIO pins
//enum gpio_pin
//{
//    GPIO_PIN_0 = 0,
//    GPIO_PIN_1 = 1,
//    GPIO_PIN_2 = 2,
//    GPIO_PIN_3 = 3,
//    GPIO_PIN_4 = 4,
//    GPIO_PIN_5 = 5,
//    GPIO_PIN_6 = 6,
//    GPIO_PIN_7 = 7,
//	
//    GPIO_PIN_8 = 8,
//    GPIO_PIN_9 = 9,
//    GPIO_PIN_10 = 10,
//    GPIO_PIN_11 = 11,
//    GPIO_PIN_12 = 12,
//    GPIO_PIN_13 = 13,
//    GPIO_PIN_14 = 14,
//    GPIO_PIN_15 = 15,
//	
//	/*
//    GPIO_PIN_16 = 16,
//    GPIO_PIN_17 = 17,
//    GPIO_PIN_18 = 18,
//    GPIO_PIN_19 = 19,
//    GPIO_PIN_20 = 20,
//    GPIO_PIN_21 = 21,
//    GPIO_PIN_22 = 22,
//    GPIO_PIN_23 = 23,
//    GPIO_PIN_24 = 24,
//    GPIO_PIN_25 = 25,
//    GPIO_PIN_26 = 26,
//    GPIO_PIN_27 = 27,
//    GPIO_PIN_28 = 28,
//    GPIO_PIN_29 = 29,
//    GPIO_PIN_30 = 30,
//    GPIO_PIN_31 = 31,
//		*/
//    GPIO_PIN_MAX = 16
//};



///GPIO states (low/high)
//enum gpio_level
//{
//    GPIO_LOW = 0,
//    GPIO_HIGH  = 1
//};

//typedef enum 
//{
//		GPIO_OUTPUT = 0,
//		GPIO_INPUT = 1
//}gpio_dir;

//typedef enum 
//{
//		GPIO_LEVEL_HIGH = 0, //高电平触发
//	  GPIO_LEVEL_LOW  = 1, //低电平触发
//	  GPIO_EDGE_RISE  = 2, //上升缘触发
//	  GPIO_EDGE_FALL  = 3, //下降缘触发
//	  GPIO_EDGE_BOTH  = 4, //边缘都触发
//}gpio_interrupt_type;

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Callback function pointer type for level detection
typedef void (*gpio_callback_t)(uint8_t);


/**
 ****************************************************************************************
 * @brief Initialize and configure the reference GPIO.
 * This function configures the GPIO according to the system needs.
 ****************************************************************************************
 */
void app_m3_gpio_init(void);


/**
 ****************************************************************************************
 * @brief Read GPIO pin level.
 * This function get selected GPIO level.
 *
 * @param[in]  pin     pin to get
 * @return     level   level
 ****************************************************************************************
 */
gpio_level app_m3_gpio_read_pin(gpio_pin pin);


/**
 ****************************************************************************************
 * @brief Write on GPIO pin.
 * This function set selected GPIO to the selected level.
 *
 * @param[in]  pin     pin to set
 * @param[in]  level   level
 ****************************************************************************************
 */
void app_m3_gpio_write_pin(gpio_pin pin,  gpio_level level);

/**
 ****************************************************************************************
 * @brief Toggle a GPIO pin.
 * This function set selected GPIO to the opposite level that is currently applied.
 * @param[in]  pin     pin to toggle
 ****************************************************************************************
 */
void app_m3_gpio_toggle_pin(gpio_pin pin);

/**
 ****************************************************************************************
 * @brief Modify direction of a set of GPIOs.
 * It writes on direction register without impacting unselected GPIOs.
 *
 * @param[in]  mask     pins to set
 * @param[in]  value    value to set
 ****************************************************************************************
 */
void  app_m3_gpio_set_direction(gpio_pin pin, gpio_dir dir);


/**
 ****************************************************************************************
 * @brief Enable interrupt on GPIO pin.
 *
 * This function enables the detection of selected level on the selected pin.
 *
 * @param[in]  pin          pin to detect
 * @param[in]  p_callback   callback function pointer
 ****************************************************************************************
 */
void  app_m3_gpio_enable_interrupt(gpio_pin pin,  gpio_interrupt_type type, gpio_callback_t p_callback);

/**
 ****************************************************************************************
 * @brief Disable interrupt on GPIO pin.
 *
 * This function disables the level detection on selected pin.
 *
 * @param[in]  pin     pin to detect
 ****************************************************************************************
 */
void app_m3_gpio_disable_interrupt(gpio_pin pin);

/**
 ****************************************************************************************
 * @brief Handles GPIO interrupt.
 ****************************************************************************************
 */
void gpio_isr(void);	



#ifdef __cplusplus
}
#endif

#endif 
