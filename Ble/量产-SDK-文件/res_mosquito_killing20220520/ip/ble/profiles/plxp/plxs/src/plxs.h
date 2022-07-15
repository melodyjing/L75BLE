/**
 ****************************************************************************************
 *
 * @file plxs.h
 *
 * @brief Header file - Pulse Oximeter Profile Service.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _PLXS_H_
#define _PLXS_H_

/**
 ****************************************************************************************
 * @addtogroup PLXS Pulse Oximeter Profile Service
 * @ingroup PLXS
 * @brief Pulse Oximeter Profile Service
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PLX_SERVER)
#include "plxp_common.h"
#include "plxs_task.h"

#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Pulse Oximeter Profile Service role task instances
#define PLXS_IDX_MAX        (BLE_CONNECTION_MAX)

//Mamimum Features size
#define PLXS_IDX_FEATURES_SIZE_MAX (2+2+3)

//Mamimum Control Point size
// OpCode + Operator
#define PLXS_IDX_RACP_SIZE_MAX (2)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
 /// Possible states of the PLXS task
enum plxs_state_id
{
    /// not connected state
    PLXS_FREE,
    /// idle state
    PLXS_IDLE,
    /// read request
    PLXS_OP_READ,
    /// notify 
    PLXS_OP_NOTIFY,
    /// indicate Spot_Measurement
    PLXS_OP_INDICATE_MEAS,
    /// indicate RACP
    PLXS_OP_INDICATE_RACP,
    PLXS_OP_INDICATE_RACP_NO_CMP,

    /// Number of defined states.
    ENVS_STATE_MAX
};



/// Pulse Oximeter Service - Attribute List
enum plxs_att_list
{
    /// Pulse Oximeter Service
    PLXS_IDX_SVC,
    /// PLXP SPOT-Measurement Characteristic
    PLXS_IDX_SPOT_MEASUREMENT_CHAR,
    PLXS_IDX_SPOT_MEASUREMENT_VAL,
    /// CCC Descriptor
    PLXS_IDX_SPOT_MEASUREMENT_CCC,
    /// PLXP Continuous Measurement Characteristic
    PLXS_IDX_CONT_MEASUREMENT_CHAR,
    PLXS_IDX_CONT_MEASUREMENT_VAL,
    /// CCC Descriptor
    PLXS_IDX_CONT_MEASUREMENT_CCC,
    /// PLXP Features Characteristic
    PLXS_IDX_FEATURES_CHAR,
    PLXS_IDX_FEATURES_VAL,
    /// PLXP Record Access Control Point Characteristic
    PLXS_IDX_RACP_CHAR,
    PLXS_IDX_RACP_VAL,
    /// CCC Descriptor
    PLXS_IDX_RACP_CCC,

    /// Number of attributes
    PLXS_IDX_NB,
};


/*
 * STRUCTURES
 ****************************************************************************************
 */



/// Pulse Oximeter Profile Service environment variable
struct plxs_env_tag
{
    /// profile environment
    prf_env_t prf_env;

    /// Pulse Oximeter Service Start Handle
    uint16_t shdl;
    /// Define the Type of Operation @see enum plxs_optype_id
    uint8_t  type_of_operation;
    /// Supported Features @see common supported_features
    uint16_t supported_features;
    /// If enabled in  Supported Features @see common measurement_status_supported
    uint16_t measurement_status_supported;
    /// If enabled in  Supported Features @see common device_status_supported
    uint32_t device_status_supported;
    /// CCC for Spot-Measurement
    uint16_t prfl_ind_cfg_spot[BLE_CONNECTION_MAX];
    /// CCC for Cont-Measurement 
    uint16_t prfl_ind_cfg_cont[BLE_CONNECTION_MAX];
    /// CCC for RACP 
    uint16_t prfl_ind_cfg_racp[BLE_CONNECTION_MAX];
    /// RACP keep state of the procedure
    uint8_t racp_in_progress[BLE_CONNECTION_MAX];

    /// State of different task instances
    ke_state_t state[PLXS_IDX_MAX];

};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 // Convert raw handle to the attribute index from @see enum plxs_att_list
uint8_t plxs_handle2att(uint16_t handle);
// Convert attribute to the raw handle index from @see enum plxs_att_list
uint16_t plxs_att2handle(uint8_t attribute);

/**
 ****************************************************************************************
 * @brief Retrieve PLXP service profile interface
 *
 * @return PLXP service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* plxs_prf_itf_get(void);


/**
 ****************************************************************************************
 * @brief  This function sends control point indication error
 ****************************************************************************************
 */
void plxs_send_rsp_ind(uint8_t conidx, uint8_t req_op_code, uint8_t status);


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
void plxs_task_init(struct ke_task_desc *task_desc);

#endif //(BLE_PLX_SERVER)

/// @} PLXS

#endif //(_PLXS_H_)
