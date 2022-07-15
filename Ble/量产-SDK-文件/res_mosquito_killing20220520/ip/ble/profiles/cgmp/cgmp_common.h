/**
 ****************************************************************************************
 *
 * @file cgmp_common.h
 *
 * @brief Header File - Continuous Glucose Monitoring Profile common types.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 *
 ****************************************************************************************
 */


#ifndef _CGMP_COMMON_H_
#define _CGMP_COMMON_H_

#include "rwip_config.h"

#if (BLE_CGM_CLIENT || BLE_CGM_SERVER)

#include "prf_types.h"

/*
 * MACROS
 ****************************************************************************************
 */
/// macro to check the bits are set in the test value
/// @param[in] __r value to check
/// @param[in] __b bit field to check
/// @return the bool of the check
#define CMPF(__r, __b)      (( (__r) & (__b) ) == (__b))

/*
 * DEFINES
 ****************************************************************************************
 */
// Special value for measurements
// NaN (not a number)
#define CGMP_NAN    (0x07FF)
// NRes (not at this resolution)
#define CGMP_NRES   (0x0800)
// + INFINITY
#define CGMP_INFINITY_PLUS  (0x07FE)
// - INFINITY
#define CGMP_INFINITY_MINUS (0x0802)

// Size of the E2E CRC 
#define CGMP_CRC_LEN (2)

// Minimum Size of the Control Point Request 
#define CGMP_CP_MIN_LEN (2)

//Maximum Session Start Time size
#define CGMP_IDX_SESSION_START_TIME_SIZE_MAX (7+1+1+2)

// size of the calibration block in the Sp Ops requests
#define CGMP_OPS_CALIBRATION_SIZE (11)

/// ERROR CODES
#define CGM_ERROR_MISSING_CRC     (0x80)
#define CGM_ERROR_INVALID_CRC     (0x81)
 
/// cgm_features: -bitfield
#define CGM_FEATURE_CALIBRATION_SUPPORTED                                           (1<<0)
#define CGM_FEATURE_PATIENT_HIGH_LOW_ALERTS_SUPPORTED                               (1<<1)
#define CGM_FEATURE_HYPO_ALERTS_SUPPORTED                                           (1<<2)
#define CGM_FEATURE_HYPER_ALERTS_SUPPORTED                                          (1<<3)
#define CGM_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS_SUPPORTED                      (1<<4)
#define CGM_FEATURE_DEVICE_SPECIFIC_ALERT_SUPPORTED                                 (1<<5)
#define CGM_FEATURE_SENSOR_MALFUNCTION_DETECTION_SUPPORTED                          (1<<6)
#define CGM_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION_SUPPORTED                 (1<<7)
#define CGM_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION_SUPPORTED                      (1<<8)
#define CGM_FEATURE_LOW_BATTERY_DETECTION_SUPPORTED                                 (1<<9)
#define CGM_FEATURE_SENSOR_TYPE_ERROR_DETECTION_SUPPORTED                           (1<<10)
#define CGM_FEATURE_GENERAL_DEVICE_FAULT_SUPPORTED                                  (1<<11)
#define CGM_FEATURE_E2E_CRC_SUPPORTED                                               (1<<12)
#define CGM_FEATURE_MULTIPLE_BOND_SUPPORTED                                         (1<<13)
#define CGM_FEATURE_MULTIPLE_SESSIONS_SUPPORTED                                     (1<<14)
#define CGM_FEATURE_CGM_TREND_INFORMATION_SUPPORTED                                 (1<<15)
#define CGM_QUALITY_SUPPORTED                                           (1<<16)

// time zone defines
#define CGM_TIME_ZONE_MIN (-48)
#define CGM_TIME_ZONE_MAX (56)
#define CGM_TIME_ZONE_UNKNOWN (-128)


/// enum type_sample
enum cgmp_type_sample_id
{
    CGM_TYPE_SMP_CAPILLARY_WHOLE_BLOOD     = 1,
    CGM_TYPE_SMP_CAPILLARY_PLASMA          = 2,
    CGM_TYPE_SMP_CAPILLARY_WHOLE_BLOOD_2   = 3,
    CGM_TYPE_SMP_VENOUS_PLASMA             = 4,
    CGM_TYPE_SMP_ARTERIAL_WHOLE_BLOOD      = 5,
    CGM_TYPE_SMP_ARTERIAL_PLASMA           = 6,
    CGM_TYPE_SMP_UNDETERMINED_WHOLE_BLOOD  = 7,
    CGM_TYPE_SMP_UNDETERMINED_PLASMA       = 8,
    CGM_TYPE_SMP_INTERSTITIAL_FLUID        = 9,
    CGM_TYPE_SMP_CONTROL_SOLUTION          = 10,
};

/// enum type_sample_location
enum cgmp_sample_location_id
{
    CGM_SMP_LOC_FINGER                    = 1,
    CGM_SMP_LOC_ALTERNATE_SITE_TEST       = 2,
    CGM_SMP_LOC_EARLOBE                   = 3,
    CGM_SMP_LOC_CONTROL_SOLUTION          = 4,
    CGM_SMP_LOC_SUBCUTANEOUS_TISSUE       = 5,
    CGM_SMP_LOC_LOCATION_NOT_AVAILABLE    = 15,
};

/// cgms_meas_flags - bitfield:
#define CGMS_MEAS_FLAGS_CGM_TREND_INFORMATION_PRESENT                     (1<<0)
#define CGMS_MEAS_FLAGS_CGM_QUALITY_PRESENT                               (1<<1)
#define CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_WARNING_PRESENT  (1<<5)
#define CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_CAL_TEMP_PRESENT (1<<6)
#define CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_STATUS_PRESENT   (1<<7)
#define CGMS_MEAS_FLAGS_VALID                                             (0xE3)


/// Sensor Status Annunciation Field, Status-Octet present
#define ANNUNC_STATUS_SESSION_STOPPED                                     (1<<0)
#define ANNUNC_STATUS_DEVICE_BATTERY_LOW                                  (1<<1)
#define ANNUNC_STATUS_SENSOR_TYPE_INCORRECT                               (1<<2)
#define ANNUNC_STATUS_SENSOR_MALFUNCTION                                  (1<<3)
#define ANNUNC_STATUS_DEVICE_SPECIFIC_ALERT                               (1<<4)
#define ANNUNC_STATUS_GENERAL_DEVICE_FAULT                                (1<<5)

/// Sensor Status Annunciation Field, Cal/Temp-Octet present
#define ANNUNC_CAL_TEMP_TIME_SYNCHRONIZATION_REQUIRED                     (1<<0)
#define ANNUNC_CAL_TEMP_CALIBRATION_NOT_ALLOWED                           (1<<1)
#define ANNUNC_CAL_TEMP_CALIBRATION_RECOMMENDED                           (1<<2)
#define ANNUNC_CAL_TEMP_CALIBRATION_REQUIRED                              (1<<3)
#define ANNUNC_CAL_TEMP_SENSOR_TEMPERATURE_TOO_HIGH                       (1<<4)
#define ANNUNC_CAL_TEMP_SENSOR_TEMPERATURE_TOO_LOW                        (1<<5)

/// Sensor Status Annunciation Field, Warning-Octet present
#define ANNUNC_WARNING_SENSOR_RESULT_LOWER_THAN_THE_PATIENT_LOW_LEVEL     (1<<0)
#define ANNUNC_WARNING_SENSOR_RESULT_HIGHER_THAN_THE_PATIENT_HIGH_LEVEL   (1<<1)
#define ANNUNC_WARNING_SENSOR_RESULT_LOWER_THAN_THE_HYPO_LEVEL            (1<<2)
#define ANNUNC_WARNING_SENSOR_RESULT_HIGHER_THAN_THE_HYPER_LEVEL          (1<<3)
#define ANNUNC_WARNING_SENSOR_RATE_OF_DECREASE_EXCEEDED                   (1<<4)
#define ANNUNC_WARNING_SENSOR_RATE_OF_INCREASE_EXCEEDED                   (1<<5)
#define ANNUNC_WARNING_SENSOR_RESULT_LOWER_THAN_THE_DEVICE_CAN_PROCESS    (1<<6)
#define ANNUNC_WARNING_SENSOR_RESULT_HIGHER_THAN_THE_DEVICE_CAN_PROCESS   (1<<7)


// Daylight Saving Time Offset
enum cgmp_dst_offset_id
{
    CGM_DST_OFFSET_STANDARD_TIME              = 0,   //
    CGM_DST_OFFSET_HALF_AN_HOUR_DAYLIGHT_TIME = 2,   //(+0.5h)
    CGM_DST_OFFSET_DAYLIGHT_TIME              = 4,   //(+1h)
    CGM_DST_OFFSET_DOUBLE_DAYLIGHT_TIME       = 8,   //(+2h)
    CGM_DST_OFFSET_UNKNOWN_DAYLIGHT_TIME      = 255, //
};


/// Calibration Status - bitfield cgmp_cal_status_xx
#define CGMP_CAL_STATUS_DATA_REJECTED          (1<<0)
#define CGMP_CAL_STATUS_DATA_OUT_OF_RANGE      (1<<1)
#define CGMP_CAL_STATUS_PROCESS_PENDING        (1<<2)



#define CGMP_CCC_NTF_ENABLED  (1<<0)
/// Mask to check if Indications are enabled
#define CGMP_CCC_IND_ENABLED  (1<<1)
/// Max value of CCC Field
#define CGMP_MAX_NTF_CFG_VALUE 3


 /*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Control Point OpCodes
enum cgmp_cp_opcodes_id
{
    CGMP_OPCODE_REPORT_STORED_RECORDS           = 1,
    CGMP_OPCODE_DELETE_STORED_RECORDS           = 2,
    CGMP_OPCODE_ABORT_OPERATION                 = 3,
    CGMP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS = 4,
    CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP   = 5,
    CGMP_OPCODE_RESPONSE_CODE                   = 6,
};

// Control Point Operators
enum cgmp_cp_operator_id
{
    CGMP_OP_NULL                  = 0,
    /// All records
    CGMP_OP_ALL_RECS              = 1,
    /// Less than or equal to
    CGMP_OP_LT_OR_EQ              = 2,
    /// Greater than or equal to
    CGMP_OP_GT_OR_EQ              = 3,
    /// Within range of (inclusive)
    CGMP_OP_WITHIN_RANGE_OF       = 4,
    /// First record(i.e. oldest record)
    CGMP_OP_FIRST_REC             = 5,
    /// Last record (i.e. most recent record)
    CGMP_OP_LAST_REC              = 6,
};


/// record access control filter type
enum cgmp_racp_filter
{
    /// Filter using Time Offset
    CGMP_FILTER_TIME_OFFSET = (1),
};


// Control Point Response Code Values
enum cgmp_cp_resp_code_id
{
    CGMP_RESP_SUCCESS                             = 1,
    CGMP_RESP_OP_CODE_NOT_SUPPORTED               = 2,
    CGMP_RESP_INVALID_OPERATOR                    = 3,
    CGMP_RESP_OPERATOR_NOT_SUPPORTED              = 4,
    CGMP_RESP_INVALID_OPERAND                     = 5,
    CGMP_RESP_NO_RECORDS_FOUND                    = 6,
    CGMP_RESP_ABORT_UNSUCCESSFUL                  = 7,
    CGMP_RESP_PROCEDURE_NOT_COMPLETED             = 8,
    CGMP_RESP_OPERAND_NOT_SUPPORTED               = 9,
};


// CGM OPS CONTROL POINT
// cgmp_ops_opcodes_id
enum cgmp_ops_opcodes_id
{
    CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL          = 1,
    CGMP_OPS_CODE_GET_CGM_COMMUNICATION_INTERVAL          = 2,
    CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE     = 3,
    CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE           = 4,
    CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE           = 5,
    CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE      = 6,
    CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL            = 7,
    CGMP_OPS_CODE_GET_PATIENT_HIGH_ALERT_LEVEL            = 8,
    CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE       = 9,
    CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL             = 10,
    CGMP_OPS_CODE_GET_PATIENT_LOW_ALERT_LEVEL             = 11,
    CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE        = 12,
    CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL                    = 13,
    CGMP_OPS_CODE_GET_HYPO_ALERT_LEVEL                    = 14,
    CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE               = 15,
    CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL                   = 16,
    CGMP_OPS_CODE_GET_HYPER_ALERT_LEVEL                   = 17,
    CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE              = 18,
    CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL        = 19,
    CGMP_OPS_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL        = 20,
    CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE   = 21,
    CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL        = 22,
    CGMP_OPS_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL        = 23,
    CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE   = 24,
    CGMP_OPS_CODE_RESET_DEVICE_SPECIFIC_ALERT             = 25,
    CGMP_OPS_CODE_START_THE_SESSION                       = 26,
    CGMP_OPS_CODE_STOP_THE_SESSION                        = 27,
    CGMP_OPS_CODE_RESPONSE_CODE                           = 28,

};

// cgmp_ops_response_id
enum cgmp_ops_response_id
{
    CGMP_OPS_RESPONSE_SUCCESS                            = 1,
    CGMP_OPS_RESPONSE_OP_CODE_NOT_SUPPORTED              = 2,
    CGMP_OPS_RESPONSE_INVALID_OPERAND                    = 3,
    CGMP_OPS_RESPONSE_PROCEDURE_NOT_COMPLETED            = 4,
    CGMP_OPS_RESPONSE_PARAMETER_OUT_OF_RANGE             = 5,

};


/*
 * STRUCTURES
 ****************************************************************************************
 */
// compacted size pf the struct cgm_calibration_operand
#define CGMP_CALIBR_SIZE                                 (11)
struct cgm_calibration_operand
{
    /// Glucose Concentration at Calibration
    prf_sfloat cal_glucose_concentration;
    /// Calibration Time in minutes
    uint16_t cal_time;
    /// Calibration Type of Sample @see common enum cgmp_type_sample_id
    uint8_t cal_type_sample;
    /// Calibration Location of Sample @see common enum cgmp_sample_location_id
    uint8_t cal_sample_location;
    /// Next Calibration Time in minutes
    uint16_t next_cal_time;
    /// Calibration Record Number
    uint16_t cal_record_number;
    /// Calibration Status - bitfield @see common CGMP_CAL_STATUS_xx
    uint8_t cal_status;
};



union cgmp_ops_operand
{
    // 0     Reserved for future use 
    // 1     Communication interval in minutes
    uint16_t interval;
    // 2     N/A 
    // 3     Communication Interval in minutes 
    // 4     Operand value as defined in the Calibration Value Fields. 
    struct cgm_calibration_operand calibration;
    // 5     Calibration Data Record Number 
    uint16_t cal_data_record_number;
    // 6     Calibration Data 
    //???
    // 7     Patient High bG value in mg/dL 
    prf_sfloat patient_high_bg_value;
    // 8     N/A 
    // 9     Patient High bG value in mg/dL 
    //10     Patient Low bG value in mg/dL 
    prf_sfloat patient_low_bg_value;
    //11     N/A 
    //12     Patient Low bG value in mg/dL 
    //13     Hypo Alert Level value in mg/dL 
    prf_sfloat hypo_alert_level_value;
    //14     N/A 
    //15     Hypo Alert Level value in mg/dL 
    //16     Hyper Alert Level value in mg/dL 
    prf_sfloat hyper_alert_level_value;
    //17     N/A 
    //18     Hyper Alert Level value in mg/dL 
    //19     Rate of Decrease Alert Level value in mg/dL/min 
    prf_sfloat rate_of_decrease_alert_level_value;
    //20     N/A 
    //21     Rate of Decrease Alert Level value in mg/dL/min 
    //22     Rate of Increase Alert Level value in mg/dL/min 
    prf_sfloat rate_of_increase_alert_level_value;
    //23     N/A 
    //24     Rate of Increase Alert Level value in mg/dL/min 
    //25     N/A 
    //26     N/A 
    //27     N/A 
    //28     Request Op Code, Response Code Value
    struct 
    {
        uint8_t request_op_code;
        uint8_t response_code_value;
    } response_code;
};



/// Record access control point response
struct cgmp_racp_rsp
{
    /// operation code
    uint8_t op_code;

    /// operator (ignored, always set to null)
    uint8_t operator;

    /// response operand
    union
    {
        /// Number of record (if op_code = CGMP_REQ_NUM_OF_STRD_RECS_RSP)
        uint16_t num_of_record;

        /// Command Status (if op_code = CGMP_REQ_RSP_CODE)
        struct
        {
            /// Request Op Code
            uint8_t op_code_req;
            /// Response Code status
            uint8_t status;
        } rsp;
    } operand;
};
  


#endif /* (BLE_CGMP_CLIENT || BLE_CGMP_SERVER) */

#endif /* _CGMP_COMMON_H_ */
