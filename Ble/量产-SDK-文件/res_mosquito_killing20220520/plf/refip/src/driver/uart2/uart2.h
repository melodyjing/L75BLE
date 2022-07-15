/**
 ****************************************************************************************
 *
 * @file uart2.h
 *
 * @brief UART2 Driver for HCI over UART2 operation.
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

#ifndef _UART2_H_
#define _UART2_H_

/**
 ****************************************************************************************
 * @defgroup UART2 UART2
 * @ingroup DRIVERS
 * @brief UART2 driver
 *
 * @{
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the UART2 to default values.
 *****************************************************************************************
 */
void uart2_init(void);

#ifndef CFG_ROM
/**
 ****************************************************************************************
 * @brief Enable UART2 flow.
 *****************************************************************************************
 */
void uart2_flow_on(void);

/**
 ****************************************************************************************
 * @brief Disable UART2 flow.
 *****************************************************************************************
 */
bool uart2_flow_off(void);
#endif //CFG_ROM

/**
 ****************************************************************************************
 * @brief Finish current UART2 transfers
 *****************************************************************************************
 */
void uart2_finish_transfers(void);

/**
 ****************************************************************************************
 * @brief Starts a data reception.
 *
 * @param[out] bufptr   Pointer to the RX buffer
 * @param[in]  size     Size of the expected reception
 * @param[in]  callback Pointer to the function called back when transfer finished
 * @param[in]  dummy    Dummy data pointer returned to callback when reception is finished
 *****************************************************************************************
 */
void uart2_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);

/**
 ****************************************************************************************
 * @brief Starts a data transmission.
 *
 * @param[in] bufptr   Pointer to the TX buffer
 * @param[in] size     Size of the transmission
 * @param[in] callback Pointer to the function called back when transfer finished
 * @param[in] dummy    Dummy data pointer returned to callback when transmission is finished
 *****************************************************************************************
 */
void uart2_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);

#if defined(CFG_ROM)
/**
 ****************************************************************************************
 * @brief Poll UART2 on reception and transmission.
 *
 * This function is used to poll UART2 for reception and transmission.
 * It is used when IRQ are not used to detect incoming bytes.
 *****************************************************************************************
 */
void uart2_poll(void);
#endif //CFG_ROM

/**
 ****************************************************************************************
 * @brief Serves the data transfer interrupt requests.
 *
 * It clears the requests and executes the appropriate callback function.
 *****************************************************************************************
 */
void uart2_isr(void);

/// @} UART2
#endif /* _UART2_H_ */
