/**
 ****************************************************************************************
 *
 * @file plf.h
 *
 * @brief Declaration of the PLF API.
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

#ifndef _PLF_H_
#define _PLF_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>            // standard integer functions
#include <stdbool.h>           // standard boolean functions



/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// PS2 connector switch interface
enum PS2_CONNECTOR_SWITCH
{
    PS2_CONNECTOR_SWITCH_UART,
    PS2_CONNECTOR_SWITCH_PS2,
};


/// IcyTRx RF interface selection
enum RF_INTF_SEL
{
	RF_INTF_V1X_V2X,
	RF_INTF_V3X_V4X,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Definition of RiCow platform version structure
struct plf_version
{
    uint8_t version;
    uint8_t plf_type;
    uint8_t rf_type;
    uint8_t ip_type;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

/**
 ****************************************************************************************
 * @defgroup PLF
 * @ingroup DRIVERS
 *
 * @brief Platform register driver
 *
 * @{
 *
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize and configure Platform registers.
 ****************************************************************************************
 */
void plf_init(void);

/**
 ****************************************************************************************
 * @brief Raise SW trigger.
 * This function generates a pulse on the platform SW trigger line.
 ****************************************************************************************
 */
void plf_trig(void);

/**
 ****************************************************************************************
 * @brief Reset platform.
 * This function performs a HW reset.
 ****************************************************************************************
 */
void plf_reset(void);

/**
 ****************************************************************************************
 * @brief Read RF board ID.
 *
 * @return    RF board ID number
 ****************************************************************************************
 */
uint16_t plf_read_rf_board_id(void);

/**
 ****************************************************************************************
 * @brief Read RF board ID.
 *
 * @param[out]  version   RiCow version informations (plf type, version, date, time, ...)
 ****************************************************************************************
 */
void plf_read_version(struct plf_version * plfversion);

/**
 ****************************************************************************************
 * @brief Get the UART clock speed in Hz.
 *
 * @return   clock     UART input clock speed
 ****************************************************************************************
 */
uint32_t plf_uart_clock_speed(void);

/**
 ****************************************************************************************
 * @brief Get the COUNTER clock speed in Hz.
 *
 * @return   clock     COUNTER input clock speed
 ****************************************************************************************
 */
uint32_t plf_counter_clock_speed(void);


/**
 ****************************************************************************************
 * @brief Check if RF clock has to be switched.
 *
 * @return   rf_switch     true: RF clock to be switched / false: no switch
 ****************************************************************************************
 */
bool plf_rf_switch(void);

/**
 ****************************************************************************************
 * @brief Switch PS2 connector to PS2 or UART interface
 *****************************************************************************************
 */
void plf_switch_ps2_connector(enum PS2_CONNECTOR_SWITCH interface);

/**
 ****************************************************************************************
 * @brief IcyTRx RF interface selection on Bubble boards
 *****************************************************************************************
 */
void plf_rf_interface_sel(enum RF_INTF_SEL interface);

/**
 ****************************************************************************************
 * @brief BTIPT sysclk_en activation
 *****************************************************************************************
 */
void plf_rf_sysclk_cntl_rf_sysclk_en(uint8_t sysclk_en);
/**
 ****************************************************************************************
 * @brief Codec interface clock on Bubble boards
 *****************************************************************************************
 */
void plf_pcm_codec_init(void);

/// @} PLF

#endif // _PLF_H_
