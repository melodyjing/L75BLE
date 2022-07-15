/**
 ****************************************************************************************
 *
 * @file thpp_task.h
 *
 * @brief Header file - THPPTASK.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 *
 ****************************************************************************************
 */

#ifndef THPP_TASK_H_
#define THPP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup THPPTASK Throughput test profile Task
 * @ingroup THPP
 * @briefhroughput test profile
 *
 * The THPPTASK is responsible for handling the APi messages from the application or
 * internal tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include "rwip_task.h" // Task definitions

/*
 * DEFINES
 ****************************************************************************************
 */


/// Messages for Throughput test profile
/*@TRACE*/
enum thpp_msg_id
{
    /// Operation Completed event
    THPP_CMP_EVT = TASK_FIRST_MSG(TASK_ID_THPP),
    /// Throughput test configuration
    THPP_CONFIGURE_CMD,
    /// Throughput test start
    THPP_START_CMD,
    /// Trigger a message with statistics information
    THPP_STAT_IND,


    /// Internal API timer to send statistics
    THPP_STAT_TIMEOUT_IND,
};

/// Operation code
enum thpp_op_id
{
    /// No operation
    THPP_OP_NONE      = 0,
    /// Configure throughput test operation
    THPP_OP_CONFIGURE = 1,
    /// Start throughput test operation
    THPP_OP_START     = 2,
};

/// Parameters of the @ref THPP_CMP_EVT message
struct thpp_cmp_evt
{
    /// Operation code
    uint8_t op;
    /// Status code of operation
    uint8_t status;
};

/// Parameters of the @ref THPP_CONFIGURE_CMD message
struct thpp_configure_cmd
{
    /// Operation code:
    ///  - THPP_OP_CONFIGURE: Configure throughput test operation
    uint8_t op;
    /// Number of packet to transmit
    uint16_t nb_pkt_tx;
    /// Size of transmitted packet
    uint16_t tx_size;
    /// Number of packet to receive
    uint16_t nb_pkt_rx;
    /// Size of expected reception packets
    uint16_t rx_size;
};

/// Parameters of the @ref THPP_START_CMD message
struct thpp_start_cmd
{
    /// Operation code:
    ///  - THPP_OP_START: Start throughput test operation
    uint8_t op;
};

/// Parameters of the @ref THPP_STAT_IND message
struct thpp_stat_ind
{
    /// Number of bytes transmitted
    uint32_t nb_byte_tx;
    /// Number of packet received
    uint32_t nb_byte_rx;
    /// Number of packet transmitted
    uint16_t nb_tx;
    /// Number of packet received
    uint16_t nb_rx;
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters for the database creation
struct thpp_db_cfg
{
    /// Database configuration
    uint8_t dummy;
};

/// @} THPPTASK
#endif // THPP_TASK_H_
