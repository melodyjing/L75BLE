/**
 ****************************************************************************************
 *
 * @file cgmc_task.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile Collector/Client Role Task.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _CGMC_TASK_H_
#define _CGMC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup CGMCTASK  Pulse Oximeter Collector Task
 * @ingroup CGMC
 * @brief  Continuous Glucose Monitoring ProfileProfile Collector Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" /// Task definitions
#include "prf_types.h"

#include "cgmp_common.h"
/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// 30 seconds control point timeout timer
#define CGMC_CP_TIMEOUT                  0x0BB8

/// Message IDs
enum cgmc_msg_ids
{
    /// Enable the Profile Collector task - at connection
    CGMC_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_CGMC),
    /// Response to Enable the Profile Collector task - at connection
    CGMC_ENABLE_RSP,
    ///*** CGMC CHARACTERISTIC REQUESTS
    /// Read Characteristic
    CGMC_READ_CMD,
    /// Write Session Start Time Characteristic
    CGMC_WRITE_SESS_START_TIME_CMD,
    /// Configure Characteristic's CCC descriptor
    CGMC_CFG_CCC_CMD,
    /// Write Command to the Control Point
    CGMC_WRITE_RACP_CMD,
    /// Write Command to the Specific OPS Control Point
    CGMC_WRITE_OPS_CTRL_PT_CMD,
    /// Receive
    /// *   Continuous Measurement Notification
    /// *   Control Point Response Indication RACP_RESP
    /// *   Specific OPS Control Point Response Indication
        /// Measurement
        /// Record Access Control Point response
        /// CGM Feature
        /// CGM Status
        /// CGM Session Start Time
        /// CGM Session Run Time
        /// CGM Specific OPS Control Point
    CGMC_VALUE_IND,

    /// Complete event for the Application commands
    CGMC_CMP_EVT,

    /// Control Point Request Timeout
    CGMC_CP_REQ_TIMEOUT,

};


/// Pulse Oximeter Service Characteristics
enum cgmc_char_ids
{
    /// CGM Measurement Characteristic
    CGMC_CHAR_MEASUREMENT = 0,
    /// CGM Features Characteristic
    CGMC_CHAR_FEATURES,
    /// CGM Status Characteristic
    CGMC_CHAR_STATUS,
    /// CGM Session Start Time Characteristic
    CGMC_CHAR_SESSION_START_TIME,
    /// CGM Session Run Time Characteristic
    CGMC_CHAR_SESSION_RUN_TIME,
    /// CGM Record Access Control Point Characteristic
    CGMC_CHAR_RACP,
    /// CGM Specific Ops Control Point Characteristic
    CGMC_CHAR_SPECIFIC_OPS_CTRL_PT,

    CGMC_CHAR_MAX,
};

/// Pulse Oximeter Service Characteristic Descriptors
enum cgmc_desc_ids
{
    /// CGM Measurement Characteristic Clien Configuration
    CGMC_DESC_MEASUREMENT_CCC = 0,
    /// CGM Record Access Control Point Characteristic Clien Configuration
    CGMC_DESC_RACP_CCC,
    /// CGM Specific Ops Control Point Characteristic Clien Configuration
    CGMC_DESC_SPECIFIC_OPS_CTRL_PT_CCC,

    CGMC_DESC_MAX,
};


/// Define command operation
enum cgmc_op_codes
{
    CGMC_NO_OP                           = 0,
// Read Operations                       
    CGMC_OP_CODE_READ_FEATURES           = 1,
    CGMC_OP_CODE_READ_STATUS             = 2,
    CGMC_OP_CODE_READ_SESSION_START_TIME = 3,
    CGMC_OP_CODE_READ_SESSION_RUN_TIME   = 4,
// Write Operations
    CGMC_OP_CODE_WRITE_SESSION_START_TIME= 5,
    CGMC_OP_CODE_WRITE_CCC               = 6,
    CGMC_OP_CODE_WRITE_CTRL_PT           = 7,
    CGMC_OP_CODE_WRITE_OPS_CTRL_PT       = 8,
    CGMC_OP_CODE_GATTC_REGISTER          = 9,
// Indications
    CGMC_OP_CODE_MEASUREMENT             = 10,
    CGMC_OP_CODE_RACP_CMD                = 11,
    CGMC_OP_CODE_SPECIFIC_OPS_CTRL_PT    = 12,
};




/*
 * API MESSAGE STRUCTURES
 ****************************************************************************************
 */

/**
 * Structure containing the characteristics handles, value handles and descriptors for
 * the Pulse Oximeter Service
 */
struct cgmc_cgmp_content
{
    /// service info
    struct prf_svc svc;

    /// Characteristic info:
    ///  - Feature
    ///  - Measurement
    struct prf_char_inf chars[CGMC_CHAR_MAX];

    /// Descriptor handles:
    ///  - Client cfg
    struct prf_char_desc_inf descs[CGMC_DESC_MAX];
};

/// Parameters of the @see CGMC_ENABLE_REQ message
struct cgmc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// Existing handle values CGMC
    struct cgmc_cgmp_content cgmc;
};

/// Parameters of the @see CGMC_ENABLE_RSP message
struct cgmc_enable_rsp
{
    /// status
    uint8_t status;
    /// Existing handle values CGMC
    struct cgmc_cgmp_content cgmc;
};

/// CGM Feature
struct cgmc_read_feature_value
{
    /// If enabled in CGM Features @see common cgm_features
    uint32_t cgm_feature;
    /// Define the Type of Sample @see common enum cgmp_type_sample_id
    uint8_t  type_sample;
    /// Define the Location of Sample @see common enum cgmp_sample_location_id
    uint8_t  sample_location;
};

/// CGM Status
struct cgmc_read_status
{
    /// The following values are defined for the Time Offset Field, specifying 
    /// the relative time difference of the single CGM values to the session 
    /// start time. Value: (0x0000 - 0xFFFF), 
    /// Description: (Time offset in minutes as offset to the Session Start Time). 
    uint16_t time_offset;
    /// The Sensor Status Annunciation Field. 
    /// The Sensor Status Annunciation Field shall be attached to every CGM Measurement Record to which the status applies. 
    uint8_t warning;
    /// The Sensor Status Annunciation Field. - bitfield @see common Sensor Status Annunciation Field CAL_TEMP
    uint8_t cal_temp;
    /// The Sensor Status Annunciation Field. - bitfield @see common Sensor Status Annunciation Field STATUS
    uint8_t annunc_status;
};

/// CGM Session Start Time
struct cgmc_read_sess_start_time
{
    /// time of Session Start Time
    struct prf_date_time session_start_time;
    /// (-48..+56) Offset from UTC in number of 15 minutes increments. A value of -128 means that the time zone offset is not known. The offset defined in this characteristic is constant, regardless whether daylight savings is in effect. 
    int8_t        time_zone;
    /// DSTOOffset  @see common enum cgmp_dst_offset_id
    uint8_t       dst_offset;
};

/// CGM Session Run Time
struct cgmc_rd_sess_run_time
{
    /// Run time in minutes
    uint16_t run_time;
};


///*** CGMC CHARACTERISTIC READ REQUESTS
/// Parameters of the @see CGMC_READ_CMD message
/// operations: CGMC_OP_CODE_READ_FEATURES           = 1,
///             CGMC_OP_CODE_READ_STATUS             = 2,
///             CGMC_OP_CODE_READ_SESSION_START_TIME = 3,
///             CGMC_OP_CODE_READ_SESSION_RUN_TIME   = 4,

struct  cgmc_read_cmd
{
    /// Operation - select characteristic @see enum cgmc_op_codes
    uint8_t operation;
};




/// Parameters of the @see CGMC_WRITE_SESS_START_TIME_CMD message
struct cgmc_write_sess_start_time_cmd
{
    /// time of Session Start Time
    struct prf_date_time session_start_time;
    /// (-48..+56) Offset from UTC in number of 15 minutes increments. A value of -128 means that the time zone offset is not known. The offset defined in this characteristic is constant, regardless whether daylight savings is in effect. 
    int8_t        time_zone;
    /// DSTOOffset  @see common enum cgmp_dst_offset_id
    uint8_t       dst_offset;
};

///*** CGMC CHARACTERISTIC/CCC DESCRIPTOR  WRITE REQUESTS
/// Parameters of the @see CGMC_CFG_CCC_CMD message
/// operation
struct  cgmc_cfg_ccc_cmd
{
    /// operation = char_opcode - select characteristic @see enum cgmc_op_codes
    uint8_t operation;
    /// The Client Characteristic Configuration Value
    uint16_t ccc;
};

/// Write Opertion Command to the Control Point forward to Application
/// Parameters of the @see CGMC_WRITE_RACP_CMD,
struct cgmc_write_racp_cmd
{
    /// Operation = CGMC_OP_CODE_RACP_CMD @see enum cgmc_op_codes
    uint8_t operation;
    /// Control Point OpCode @see enum cgmp_cp_opcodes_id
    uint8_t cp_opcode;
    /// Operator  @see enum cgmp_cp_operator_id
    uint8_t cp_operator;
    // no need Filter Type as it is selected automatically
    /// Filter parameter
    uint16_t min_time_offset;
    /// Filter parameter
    uint16_t max_time_offset;
};

/// Receive Features
/// *   Receive Measurement Notification 
/// *   Receive Control Point Response Indication RACP_RESP
///    CGMC_VALUE_IND,

/// Indication Record Access Control Point Response
struct cgmc_racp_resp
{
    /// Control Point OpCode @see enum cgmp_cp_opcodes_id
    uint8_t cp_opcode;
    /// Request Control Point OpCode @see enum cgmp_cp_opcodes_id
    uint8_t request_cp_opcode;
    /// Response Code @see enum cgmp_cp_resp_code_id
    uint8_t response_code;
    /// Number of Records
    uint16_t number_of_records;
};


/// CGM Measurement Value
struct cgmc_meas_value
{
//    /// The Size Field represents the size of the CGM Measurement record.
//    uint8_t size;
    /// CGM Trend Information Present - bitfield @see common cgms_meas_flags
    uint8_t flags;
    /// The CGM Glucose Concetration Field contains the CGM glucose concentration in mg/dL as a SFLOAT data type.
    prf_sfloat glucose_concentration;
    /// The following values are defined for the Time Offset Field, specifying 
    /// the relative time difference of the single CGM values to the session 
    /// start time. Value: (0x0000 - 0xFFFF), 
    /// Description: (Time offset in minutes as offset to the Session Start Time). 
    uint16_t time_offset;
    /// The Sensor Status Annunciation Field. 
    /// The Sensor Status Annunciation Field shall be attached to every CGM Measurement Record to which the status applies. 
    uint8_t warning;
    /// The Sensor Status Annunciation Field. - bitfield @see common Sensor Status Annunciation Field CAL_TEMP
    uint8_t cal_temp;
    /// The Sensor Status Annunciation Field. - bitfield @see common Sensor Status Annunciation Field STATUS
    uint8_t status;
    /// The CGM Trend Information Field contains the CGM Trend information in (mg/dL)/min as an SFLOAT data type. This field is optional if the device supports CGM Trend information (Bit 15 in CGM Feature is set to 1) otherwise excluded. 
    prf_sfloat trend_information;
    /// This field contains the CGM Quality information in % as an SFLOAT data type. The SFLOAT-Type is a 16-bit word comprising a signed 4-bit integer exponent followed by a signed 12-bit Mantissa, each in twos-complement form. This field is optional if the device supports CGM Quality (Bit 16 in CGM Feature is set to 1) otherwise excluded. 
    prf_sfloat quality;
};

/// Specific OPS Control Point Response
struct cgmc_ops_ctrl_pt_resp
{
    /// Control Point OpCode @see enum cgmp_ops_opcodes_id
    uint8_t opcode;
    /// Op-code Response  @see enum cgmp_ops_response_id
    uint8_t response;
    /// operand specific to opcode
    union cgmp_ops_operand operand;
};

/// Indication the Record Access Control Point response
/// Indication CGM Measurement
/// Parameters of the @see CGMC_VALUE_IND,
struct cgmc_value_ind
{
    /// Operation @see enum cgmc_op_codes
    uint8_t operation;
    union cgmc_meas_value_tag
    {
        /// Measurement
        struct cgmc_meas_value               meas;
        /// Record Access Control Point response
        struct cgmc_racp_resp                racp_resp;
        /// Specific Ops Control Point response
        struct cgmc_ops_ctrl_pt_resp         ops_cp_resp;
        /// CGM Feature
        struct cgmc_read_feature_value       feature;
        /// CGM Status
        struct cgmc_read_status              status;
        /// CGM Session Start Time
        struct cgmc_read_sess_start_time     start_time;
        /// CGM Session Run Time
        struct cgmc_rd_sess_run_time         run_time;
    } value;
};


/// Write Opertion Command to the Specific Ops Control Point forward to Application
/// Parameters of the @see CGMC_WRITE_OPS_CTRL_PT_CMD,
struct cgmc_write_ops_ctrl_pt_cmd
{
    /// Operation (_cmd specific format) = CGMC_OP_CODE_RACP_CMD @see enum cgmc_op_codes
    uint8_t operation;
    /// Control Point OpCode @see enum cgmp_ops_opcodes_id
    uint8_t opcode;
    /// Op-code Response  @see enum cgmp_ops_response_id
    uint8_t response;
    /// operand specific to opcode
    union cgmp_ops_operand operand;
};



/// Complete event for the Application commands
///    CGMC_CMP_EVT,
/// Parameters of the @see CGMC_CMP_EVT message
/// Complete Event Information
struct  cgmc_cmp_evt
{
    /// Operation  @see enum cgmc_op_codes
    uint8_t operation;
    /// Status
    uint8_t status;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/// @} CGMCTASK

#endif //(_CGMC_TASK_H_)
