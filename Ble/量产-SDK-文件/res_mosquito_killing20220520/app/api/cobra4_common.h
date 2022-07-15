/******************************************************************************
	File Name	:	cobra4_common.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/19

	Description :   
				…Ó€⁄
	Changed Log	:
		Liujian  2019/12/19		- Creation

*****************************************************************************/
#ifndef __COBRA4_COMMON_H__
#define __COBRA4_COMMON_H__
#include <stdint.h>

#include "cobra_uart.h"

/** @defgroup BLE_APPEARANCES Bluetooth Appearance values
 *  @note Retrieved from http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
 * @{ */
#define BLE_APPEARANCE_UNKNOWN                                0 /**< Unknown. */
#define BLE_APPEARANCE_GENERIC_PHONE                         64 /**< Generic Phone. */
#define BLE_APPEARANCE_GENERIC_COMPUTER                     128 /**< Generic Computer. */
#define BLE_APPEARANCE_GENERIC_WATCH                        192 /**< Generic Watch. */
#define BLE_APPEARANCE_WATCH_SPORTS_WATCH                   193 /**< Watch: Sports Watch. */
#define BLE_APPEARANCE_GENERIC_CLOCK                        256 /**< Generic Clock. */
#define BLE_APPEARANCE_GENERIC_DISPLAY                      320 /**< Generic Display. */
#define BLE_APPEARANCE_GENERIC_REMOTE_CONTROL               384 /**< Generic Remote Control. */
#define BLE_APPEARANCE_GENERIC_EYE_GLASSES                  448 /**< Generic Eye-glasses. */
#define BLE_APPEARANCE_GENERIC_TAG                          512 /**< Generic Tag. */
#define BLE_APPEARANCE_GENERIC_KEYRING                      576 /**< Generic Keyring. */
#define BLE_APPEARANCE_GENERIC_MEDIA_PLAYER                 640 /**< Generic Media Player. */
#define BLE_APPEARANCE_GENERIC_BARCODE_SCANNER              704 /**< Generic Barcode Scanner. */
#define BLE_APPEARANCE_GENERIC_THERMOMETER                  768 /**< Generic Thermometer. */
#define BLE_APPEARANCE_THERMOMETER_EAR                      769 /**< Thermometer: Ear. */
#define BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR            832 /**< Generic Heart rate Sensor. */
#define BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT    833 /**< Heart Rate Sensor: Heart Rate Belt. */
#define BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE               896 /**< Generic Blood Pressure. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_ARM                   897 /**< Blood Pressure: Arm. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_WRIST                 898 /**< Blood Pressure: Wrist. */
#define BLE_APPEARANCE_GENERIC_HID                          960 /**< Human Interface Device (HID). */
#define BLE_APPEARANCE_HID_KEYBOARD                         961 /**< Keyboard (HID Subtype). */
#define BLE_APPEARANCE_HID_MOUSE                            962 /**< Mouse (HID Subtype). */
#define BLE_APPEARANCE_HID_JOYSTICK                         963 /**< Joystick (HID Subtype). */
#define BLE_APPEARANCE_HID_GAMEPAD                          964 /**< Gamepad (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITIZERSUBTYPE                 965 /**< Digitizer Tablet (HID Subtype). */
#define BLE_APPEARANCE_HID_CARD_READER                      966 /**< Card Reader (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITAL_PEN                      967 /**< Digital Pen (HID Subtype). */
#define BLE_APPEARANCE_HID_BARCODE                          968 /**< Barcode Scanner (HID Subtype). */
#define BLE_APPEARANCE_GENERIC_GLUCOSE_METER               1024 /**< Generic Glucose Meter. */
#define BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR      1088 /**< Generic Running Walking Sensor. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE      1089 /**< Running Walking Sensor: In-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE      1090 /**< Running Walking Sensor: On-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP       1091 /**< Running Walking Sensor: On-Hip. */
#define BLE_APPEARANCE_GENERIC_CYCLING                     1152 /**< Generic Cycling. */
#define BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER            1153 /**< Cycling: Cycling Computer. */
#define BLE_APPEARANCE_CYCLING_SPEED_SENSOR                1154 /**< Cycling: Speed Sensor. */
#define BLE_APPEARANCE_CYCLING_CADENCE_SENSOR              1155 /**< Cycling: Cadence Sensor. */
#define BLE_APPEARANCE_CYCLING_POWER_SENSOR                1156 /**< Cycling: Power Sensor. */
#define BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR        1157 /**< Cycling: Speed and Cadence Sensor. */
#define BLE_APPEARANCE_GENERIC_PULSE_OXIMETER              3136 /**< Generic Pulse Oximeter. */
#define BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP            3137 /**< Fingertip (Pulse Oximeter subtype). */
#define BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN           3138 /**< Wrist Worn(Pulse Oximeter subtype). */
#define BLE_APPEARANCE_GENERIC_WEIGHT_SCALE                3200 /**< Generic Weight Scale. */
#define BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT          5184 /**< Generic Outdoor Sports Activity. */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP         5185 /**< Location Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP 5186 /**< Location and Navigation Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD          5187 /**< Location Pod (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD  5188 /**< Location and Navigation Pod (Outdoor Sports Activity subtype). */


#if (BLE_APP_HID_ENABLE)
#define   APP_HID_ADV_DATA_APPEARANCE   "\x03\x19\xC1\x03"
#define   APP_ADV_DATA_APPEARANCE_LEN   (4)
#endif //(BLE_APP_HID)

/**
 * @brief Boolean Type definition
 */
typedef enum 
{
   FALSE = 0, 
   TRUE = !FALSE
} Bool;

/**
 * @brief Flag Status and Interrupt Flag Status type definition
 */
typedef enum 
{
   RESET = 0, 
   SET = !RESET
} FlagStatus, IntStatus, SetState;

#define PARAM_SETSTATE(State) ((State==RESET) || (State==SET))


/**
 * @brief Functional State Definition
 */
typedef enum 
{
   DISABLE = 0, 
   ENABLE = !DISABLE
} FunctionalState;

#define PARAM_FUNCTIONALSTATE(State) ((State==DISABLE) || (State==ENABLE))

/**
 * @ Status type definition
 */
typedef enum 
{
    ERROR = 0, 
    SUCCESS = !ERROR
} Status;



typedef enum {
		MODE_NORMAL,
	  MODE_HCITL,
}BREAKER_WORK_MODE;


void app_cobra_init(void);

void appm_build_adv_data(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf);

void appm_build_scan_rsp_data(uint16_t *p_length, uint8_t *p_buf);

uint8_t appm_get_dev_name(uint8_t* name);

void app_m4_custom_application_init(void);

void app_m4_trim_init(void);

#if (APP_METER_DEBUG_ENABLE)
void app_meter_debug(void);
void app_debug_led(void);
#endif

void app_common_led_init(void);

void app_common_led_on_off(unsigned int en);
	
void app_common_io_init(void);
BREAKER_WORK_MODE app_common_module_mode(void);
BREAKER_WORK_MODE app_common_get_work_mode(void);
uart_id app_common_get_uart_print_index(void);
void app_meter_io_init(void);
void app_ble_mesh_update_param(void);

void app_m4_timer_init(void);

void app_cobra_ble_stack_init(void);

void app_custom_application_loop(void);

const struct prf_task_cbs* rdw_custom_define_profile(uint32_t task_id);

#endif
