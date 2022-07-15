/**
 ****************************************************************************************
 *
 * @file cgmc.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile - Collector Role.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _CGMC_H_
#define _CGMC_H_

/**
 ****************************************************************************************
 * @addtogroup CGMC Continuous Glucose Monitoring Profile Collector
 * @ingroup WSCP
 * @brief Continuous Glucose Monitoring Profile Collector
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Continuous Glucose Monitoring Task instances
#define CGMC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "cgmp_common.h"

#if (BLE_CGM_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

//#define CCC_READY

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/// Possible states of the CGMC task
enum cgmc_states
{
    /// Idle state
    CGMC_FREE,
    /// Connected state
    CGMC_IDLE,
    /// Discovering Services
    CGMC_DISCOVERING_SVC,
    /// Busy state
    CGMC_BUSY,
    /// Number of defined states.
    CGMC_STATE_MAX
};

/// Operation Codes for pending operations in the client.
enum cgmc_op_pending
{
    /// Reserved operation code
    CGMC_OP_PENDING_NO_OP = 0x00,

    /// Read op-codes
    // internal state when reading features after discovery
    CGMC_OP_PENDING_READ_FEATURES_AT_INIT,
    // read FEATURES as the result of the command request
    CGMC_OP_PENDING_READ_FEATURES,
    // read STATUS as the result of the command request
    CGMC_OP_PENDING_READ_STATUS,
    // read Session Start Time as the result of the command request
    CGMC_OP_PENDING_READ_SESSION_START_TIME,
    // read Session Run Time as the result of the command request
    CGMC_OP_PENDING_READ_SESSION_RUN_TIME,

    /// Write op-codes.
    CGMC_OP_PENDING_WRITE_RACP,
    CGMC_OP_PENDING_WRITE_SPECIFIC_OPS_CTRL_PT,
    CGMC_OP_PENDING_WRITE_CCC,

};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/**
 *  Stored Environment content/context information for a connection
 */
struct cgmc_cnx_env
{
    uint8_t op_pending;
    //Feature
    /// If enabled in CGM Features @see common cgm_features
    uint32_t cgm_feature;
    /// Define the Type of Sample @see common enum cgmp_type_sample_id
    uint8_t  type_sample;
    /// Define the Location of Sample @see common enum cgmp_sample_location_id
    uint8_t  sample_location;

    /// Counter used to check service uniqueness
    uint8_t nb_svc;

    /// Continuous Glucose Monitoring Service Characteristics
    struct cgmc_cgmp_content cgmc;
};


/// Continuous Glucose Monitoring environment variable
struct cgmc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct cgmc_cnx_env* env[CGMC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[CGMC_IDX_MAX];
};



/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Pool of Continuous Glucose Monitoring Collector task environments.
extern struct cgmc_env_tag **cgmc_envs;

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
const struct prf_task_cbs* cgmc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send Continuous Glucose Monitoring ATT DB discovery results to CGMC host.
 ****************************************************************************************
 */
void cgmc_enable_rsp_send(struct cgmc_env_tag *cgmc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send a CGMC_CMP_EVT message to the task which enabled the profile
 ****************************************************************************************
 */
void cgmc_send_cmp_evt(struct cgmc_env_tag *cgmc_env, uint8_t conidx, uint8_t operation, uint8_t status);



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
void cgmc_task_init(struct ke_task_desc *task_desc);

#endif //(BLE_CGMP_CLIENT)

/// @} CGMC

#endif //(_CGMC_H_)
