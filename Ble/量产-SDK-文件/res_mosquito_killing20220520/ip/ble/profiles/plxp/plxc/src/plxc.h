/**
 ****************************************************************************************
 *
 * @file plxc.h
 *
 * @brief Header file - Pulse Oximeter Profile - Collector Role.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _PLXC_H_
#define _PLXC_H_

/**
 ****************************************************************************************
 * @addtogroup PLXC Pulse Oximeter Profile Collector
 * @ingroup WSCP
 * @brief Pulse Oximeter Profile Collector
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Pulse Oximeter Task instances
#define PLXC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "plxp_common.h"

#if (BLE_PLX_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

//#define CCC_READY

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/// Possible states of the PLXC task
enum plxc_states
{
    /// Idle state
    PLXC_FREE,
    /// Connected state
    PLXC_IDLE,
    /// Discovering Services
    PLXC_DISCOVERING_SVC,
    /// Busy state
    PLXC_BUSY,
    /// Number of defined states.
    PLXC_STATE_MAX
};

/// Operation Codes for pending operations in the client.
enum plxc_op_pending
{
    /// Reserved operation code
    PLXC_OP_PENDING_NO_OP = 0x00,

    /// Read op-codes
    // internal state when reading features after discovery
    PLXC_OP_PENDING_READ_FEATURES_AT_INIT,
    // read FEATURES as the result of the command request
    PLXC_OP_PENDING_READ_FEATURES,

    /// Write op-codes.
    PLXC_OP_PENDING_WRITE_RACP,
    PLXC_OP_PENDING_WRITE_CCC,

};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/**
 *  Stored Environment content/context information for a connection
 */
struct plxc_cnx_env
{
    /// Current Operation
//    void *operation;

    uint8_t op_pending;

    /// Counter used to check service uniqueness
    uint8_t nb_svc;

    /// Pulse Oximeter Service Characteristics
    struct plxc_plxp_content plxc;
};


/// Pulse Oximeter environment variable
struct plxc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct plxc_cnx_env* env[PLXC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[PLXC_IDX_MAX];
};



/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Pool of Pulse Oximeter Collector task environments.
extern struct plxc_env_tag **plxc_envs;

/*
 * GLOBAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve WSCP client profile interface
 *
 * @return WSCP client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* plxc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send Pulse Oximeter ATT DB discovery results to PLXC host.
 ****************************************************************************************
 */
void plxc_enable_rsp_send(struct plxc_env_tag *plxc_env, uint8_t conidx, uint8_t status, \
    uint16_t supported_features, uint16_t measurement_status_supported, uint32_t device_status_supported);

/**
 ****************************************************************************************
 * @brief Send a PLXC_CMP_EVT message to the task which enabled the profile
 ****************************************************************************************
 */
void plxc_send_cmp_evt(struct plxc_env_tag *plxc_env, uint8_t conidx, uint8_t operation, uint8_t status);



/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void plxc_task_init(struct ke_task_desc *task_desc);

#endif //(BLE_PLXP_CLIENT)

/// @} PLXC

#endif //(_PLXC_H_)
