/**
 ****************************************************************************************
 *
 * @file wscc.h
 *
 * @brief Header file - Weight SCale Profile - Collector Role.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _WSCC_H_
#define _WSCC_H_

/**
 ****************************************************************************************
 * @addtogroup WSCC Weight Scale Profile Collector
 * @ingroup WSCP
 * @brief Weight Scale Profile Collector
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Weight Scale Task instances
#define WSCC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "wsc_common.h"

#if (BLE_WSC_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the WSCC task
enum wscc_states
{
    /// Idle state
    WSCC_FREE,
    /// Connected state
    WSCC_IDLE,
    /// Discovering Services
    WSCC_DISCOVERING_SVC,
    /// Busy state
    WSCC_BUSY,
    /// Number of defined states.
    WSCC_STATE_MAX
};

/// Operation Codes for pending operations in the client.
enum wscc_op_codes
{
    /// Read op-codes
    WSCC_READ_FEAT_OP_CODE = 0,
    /// Read op-codes
    WSCC_READ_CCC_OP_CODE = 1,
    /// Write op-codes.
    WSCC_WRITE_CCC_OP_CODE = 2,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Stored Environment content/context information for a connection
struct wscc_cnx_env
{
    /// Current Operation
    void *operation;

    /// Pending operation
    uint8_t op_pending;

    /// Counter used to check service uniqueness
    uint8_t nb_svc;

    /// Weight Scale Service Characteristics
    struct wscc_wss_content wss;
};

/// Weight Scale environment variable
struct wscc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct wscc_cnx_env* env[WSCC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[WSCC_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Pool of Weight Scale Collector task environments.
extern struct wscc_env_tag **wscc_envs;

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
const struct prf_task_cbs* wscc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send Weight Scale ATT DB discovery results to WSCC host.
 *
 * @param[in] p_wscc_env   The profile environment for the connection
 * @param[in] conidx       Connection Identifier
 * @param[in] status       Indicates the outcome of the operation
 ****************************************************************************************
 */
void wscc_enable_rsp_send(struct wscc_env_tag *p_wscc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send a WSCC_CMP_EVT message to the task which enabled the profile
 *
 * @param[in] p_wscc_env   The profile environment
 * @param[in] conidx       Connection Identifier
 * @param[in] operation    Indicates the operation for which the cmp_evt is being sent.
 * @param[in] status       Indicates the outcome of the operation
 ****************************************************************************************
 */
void wscc_send_cmp_evt(struct wscc_env_tag *p_wscc_env, uint8_t conidx, uint8_t operation, uint8_t status);

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param p_task_desc Task descriptor to fill
 ****************************************************************************************
 */
void wscc_task_init(struct ke_task_desc *p_task_desc);

#endif //(BLE_WSC_CLIENT)

/// @} WSCC

#endif //(_WSCC_H_)
