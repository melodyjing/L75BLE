/**
 ****************************************************************************************
 *
 * @file app_task.h
 *
 * @brief Header file - APPTASK.
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

#ifndef APP_TASK_H_
#define APP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPTASK Task
 * @ingroup APP
 * @brief Routes ALL messages to/from APP block.
 *
 * The APPTASK is the block responsible for bridging the final application with the
 * RWBLE software host stack. It communicates with the different modules of the BLE host,
 * i.e. @ref SMP, @ref GAP and @ref GATT.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "ke_task.h"        // Kernel Task

/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of APP Task Instances
#define APP_IDX_MAX                 (1)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// States of APP task
enum appm_state
{
    /// Initialization state
    APPM_INIT,
    /// Database create state
    APPM_CREATE_DB,
    /// Ready State
    APPM_READY,
    /// Connected state
    APPM_CONNECTED,

    /// Number of defined states.
    APPM_STATE_MAX
};


/// APP Task messages
/*@TRACE*/
enum app_msg_id
{
    APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),

    #if (BLE_APP_HT)
    /// Timer used to refresh the temperature measurement value
    APP_HT_MEAS_INTV_TIMER,
    #endif //(BLE_APP_HT)

    #if (BLE_APP_HID_ENABLE)
    /// Timer used to disconnect the moue if no activity is detecter
    APP_HID_MOUSE_TIMEOUT_TIMER,
    #endif //(BLE_APP_HID)
	
	
		#if (BLE_APP_HRP)
    /// Timer used to refresh the heart rate measurement
    APP_HRP_MEAS_INTV_TIMER,
    #endif //(BLE_APP_HRP)
	  
	 #if (BLE_APP_UART)
	  APP_UART_MEAS_INTV_TIMER,
	
	 #endif
	 
	 #if (BLE_APP_BLOOD)
	  APP_BLOOD_MEAS_INTV_TIMER,
	 #endif
   
	 #if (BLE_APP_HEART_RATE)
	  APP_HRP_MEAS_INTV_TIMER,
   #endif	 
	 
	 #if (BLE_APP_CPPS)	 
	 APP_CPPS_MEAS_INTV_TIMER1,
	 APP_CPPS_MEAS_INTV_TIMER2,
	 
	 #endif
	 
	 #if (BLE_APP_RDW_VOICE)
	  APP_VOICE_KEY_TIMER,
	
	 #endif
	 
	 #if (BLE_APP_PROXR)
		APP_PROXR_TIMER,
	 #endif

#if (APP_AIRCLEAN_ENABLE == 1)
	APP_AIRCLEAN_DELAY_POWER_DOWN_TIMER,
	APP_AIRCLEAN_IP5219_POWER_TIMER,
	APP_AIRCLEAN_FLICKER_LIGHT_TIMER,
	APP_AIRCLEAN_POWER_ON_FLICKER_TIMER,

	APP_AIRCLEAN__TIMER_MAX,
#endif	 

#if (APP_RAINBOW_ENABLE == 1)
	APP_RAINBOW_DEFAULT_TIMER,
	APP_RAINBOW_DELAY_POWER_DOWN_TIMER,

	APP_RAINBOW_TIMER_MAX,
#endif	

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
	APP_MOSQUITO_KILLING_DEFAULT_TIMER,

	APP_MOSQUITO_KILLING_RESTORE_DEFAULT_FLICKER_TIMER,
	APP_MOSQUITO_KILLING_BREATH_FLICKER_TIMER,
	APP_MOSQUITO_KILLING_BREATH_TOP_LED_TIMER,

	APP_MOSQUITO_KILLING_TIMER_MAX,
#endif	

};


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
#if (COBRA_POWER_DOWN_ENABLE == 1) 
#define APP_CON_INT_MIN               (160)  //160  //80*1.25ms(100ms)  6<x<3200
#define APP_CON_INT_MAX               (200)		//200 //80*1.25ms(100ms)		 6<x<3200
#define APP_CON_LATENCY					0
// if APP_CON_TIMEOUT> 3200 ,can't change APP_CON_INT_MIN etc. APP_CON_INT_MIN fit 45ms
#define APP_CON_TIMEOUT					500			//500*10ms(5s)         10<APP_CON_TIMEOUT<3200
#endif
/// @} APPTASK

#endif //(BLE_APP_PRESENT)

#endif // APP_TASK_H_
