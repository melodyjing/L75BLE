/**
 ****************************************************************************************
 *
 * @file cgms.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile Service.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _CGMS_H_
#define _CGMS_H_

/**
 ****************************************************************************************
 * @addtogroup CGMS Continuous Glucose Monitoring Profile Service
 * @ingroup CGMS
 * @brief Continuous Glucose Monitoring Profile Service
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_CGM_SERVER)
#include "cgmp_common.h"
#include "cgms_task.h"

#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Continuous Glucose Monitoring Profile Service role task instances
#define CGMS_IDX_MAX        (BLE_CONNECTION_MAX)

//Maximum Features size
// uint24 + 2x 4bit + crc
#define CGMS_IDX_FEATURES_SIZE_MAX (3+1+2)

//Maximum Status size
#define CGMS_IDX_STATUS_SIZE_MAX (2+3+2)


//Maximum Session Run Time size
#define CGMS_IDX_SESSION_RUN_TIME_SIZE_MAX (2+2)

//Maximum Control Point size
// OpCode + Operator+ Filter + FilterParameter
#define CGMS_IDX_RACP_SIZE_MAX (2+5+2)

//Maximum Specific OPs Control Point size
// OpCode + Operator+ Operand
#define CGMS_IDX_OPS_CTRL_PT_SIZE_MAX (2+sizeof(struct cgm_calibration_operand) + 2)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
 /// Possible states of the CGMS task
enum cgms_state_id
{
    /// not connected state
    CGMS_FREE,
    /// idle state
    CGMS_IDLE,
    /// read request
    CGMS_OP_READ,
    /// notify 
    CGMS_OP_NOTIFY,
    /// indicate Spot_Measurement
    CGMS_OP_INDICATE_MEAS,
    /// indicate RACP
    CGMS_OP_INDICATE_RACP,
    CGMS_OP_INDICATE_RACP_NO_CMP,
    
    /// application request
    CGMS_OP_WRITE_OPS_CTRL_PT,
    CGMS_OP_INDICATE_OPS_CTRL_PT,

    /// Number of defined states.
    CGMS_STATE_MAX
};



/// Pulse Oximeter Service - Attribute List
enum cgms_att_list
{
    /// Pulse Oximeter Service
    CGMS_IDX_SVC,
    /// CGM Measurement Characteristic
    CGMS_IDX_MEASUREMENT_CHAR,
    CGMS_IDX_MEASUREMENT_VAL,
    /// CCC Descriptor
    CGMS_IDX_MEASUREMENT_CCC,
    /// CGM Features Characteristic
    CGMS_IDX_FEATURES_CHAR,
    CGMS_IDX_FEATURES_VAL,
    /// CGM Status Characteristic
    CGMS_IDX_STATUS_CHAR,
    CGMS_IDX_STATUS_VAL,
    /// CGM Session Start Time Characteristic
    CGMS_IDX_SESSION_START_TIME_CHAR,
    CGMS_IDX_SESSION_START_TIME_VAL,
    /// CGM Session Run Time Characteristic
    CGMS_IDX_SESSION_RUN_TIME_CHAR,
    CGMS_IDX_SESSION_RUN_TIME_VAL,
    /// CGM Record Access Control Point Characteristic
    CGMS_IDX_RACP_CHAR,
    CGMS_IDX_RACP_VAL,
    /// CCC Descriptor
    CGMS_IDX_RACP_CCC,
    /// CGM Specific Ops Control Point Characteristic
    CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CHAR,
    CGMS_IDX_SPECIFIC_OPS_CTRL_PT_VAL,
    /// CCC Descriptor
    CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC,

    /// Number of attributes
    CGMS_IDX_NB,
};

// prfl_state
// keep internal state of CCC enabled per Characteristic and RACP process
#define CGMS_PRFL_STATE_CLEAR            (0)
#define CGMS_NTF_MEAS_ENABLED_BIT        (1<<0)
#define CGMS_NTF_MEAS_ENABLED_POS        (0)
#define CGMS_IND_RACP_ENABLED_BIT        (1<<1)
#define CGMS_IND_RACP_ENABLED_POS        (1)
#define CGMS_IND_SOPS_ENABLED_BIT        (1<<2)
#define CGMS_IND_SOPS_ENABLED_POS        (2)
#define CGMS_RACP_IN_PROGRESS_BIT        (1<<3)
#define CGMS_RACP_IN_PROGRESS_POS        (3)

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Continuous Glucose Monitoring Profile Service environment variable
struct cgms_env_tag
{
    /// profile environment
    prf_env_t prf_env;

    /// Pulse Oximeter Service Start Handle
    uint16_t shdl;
    /// CGM Features @see common cgm_features
    uint32_t cgm_feature;
    /// CCC for Measurement
    /// CCC for RACP 
    /// CCC for Specific OPS Control Point
    /// RACP keep state of the procedure
    uint8_t prfl_state[CGMS_IDX_MAX];
    /// State of different task instances
    ke_state_t state[CGMS_IDX_MAX];

};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 // Convert raw handle to the attribute index from @see enum cgms_att_list
uint8_t cgms_handle2att(uint16_t handle);
// Convert attribute to the raw handle index from @see enum cgms_att_list
uint16_t cgms_att2handle(uint8_t attribute);

/**
 ****************************************************************************************
 * @brief Retrieve CGMP service profile interface
 *
 * @return CGMP service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* cgms_prf_itf_get(void);


/**
 ****************************************************************************************
 * @brief  This function sends control point indication error
 ****************************************************************************************
 */
void cgms_send_rsp_ind(uint8_t conidx, uint8_t req_op_code, uint8_t status);


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
void cgms_task_init(struct ke_task_desc *task_desc);

#endif //(BLE_CGM_SERVER)

/// @} CGMS

#endif //(_CGMS_H_)
