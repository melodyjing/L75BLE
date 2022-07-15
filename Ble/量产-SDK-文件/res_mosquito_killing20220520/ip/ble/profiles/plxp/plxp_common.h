/**
 ****************************************************************************************
 *
 * @file plxp_common.h
 *
 * @brief Header File - Pulse Oximeter Profile common types.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 *
 ****************************************************************************************
 */


#ifndef _PLXP_COMMON_H_
#define _PLXP_COMMON_H_

#include "rwip_config.h"

#if (BLE_PLX_CLIENT || BLE_PLX_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */
// Special value for measurements
// NaN (not a number)
#define PLXP_NAN    (0x07FF)
// NRes (not at this resolution)
#define PLXP_NRES   (0x0800)
// + INFINITY
#define PLXP_INFINITY_PLUS  (0x07FE)
// - INFINITY
#define PLXP_INFINITY_MINUS (0x0802)
 
 
// supported_features; -bitfield
//0     Measurement Status support is present
//• Take care of features.measurement_status_supported
//• Measurement_status field present in Measurements
#define PLXP_FEATURE_MEAS_STATUS_PRESENT                (1<<0)
// 1    Device and Sensor Status support is present
// •    Take care of features.device_status_supported
// •    Device_status filed present in Measurements
#define PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT       (1<<1)
// 2    Measurement Storage for Spot-check measurements is supported
// •    Can store Spot-check measurements
// •    Timestamp have to be present in measurements ( so it would overwrite the next feature bit)
#define PLXP_FEATURE_MEAS_STORAGE_SUPPORTED             (1<<2)
// 3    Timestamp for Spot-check measurements is supported 
// •    Timestamp have to be present in measurements
#define PLXP_FEATURE_TIMESTAMP_SUPPORTED                (1<<3)
// 4    SpO2PR-Fast metric is supported
// •    Field is present in continuous measurement
#define PLXP_FEATURE_SPO2PR_FAST_SUPPORTED              (1<<4)
// 5    SpO2PR-Slow metric is supported
// •    Field is present in continuous measurement
#define PLXP_FEATURE_SPO2PR_SLOW_SUPPORTED              (1<<5)
// 6    Pulse Amplitude Index field is supported
// •    Field is present in either measurement
#define PLXP_FEATURE_PULSE_AMPLITUDE_SUPPORTED          (1<<6)
// 7    Multiple Bonds Supported
// •    ???
#define PLXP_FEATURE_MULTIPLE_BONDS_SUPPORTED           (1<<7)

//Uint16 measurement_status_supported; - bitfield
//Bit   Definition
//0-4   Reserved for Future Use
//5     Measurement Ongoing bit supported
#define PLXP_MEAS_STATUS_MEASUREMENT_ONGOING_SUPPORTED                  (1<<5)
//6     Early Estimated Data bit supported
#define PLXP_MEAS_STATUS_EARLY_ESTIMATED_DATA_SUPPORTED                 (1<<6)
//7     Validated Data bit supported
#define PLXP_MEAS_STATUS_VALIDATED_DATA_SUPPORTED                       (1<<7)
//8     Fully Qualified Data bit supported
#define PLXP_MEAS_STATUS_FULLY_QUALIFIED_DATA_SUPPORTED                 (1<<8)
//9     Data from Measurement Storage bit supported
#define PLXP_MEAS_STATUS_DATA_FROM_MEASUREMENT_STORAGE_SUPPORTED        (1<<9)
//10    Data for Demonstration bit supported
#define PLXP_MEAS_STATUS_DATA_FOR_DEMONSTRATION_SUPPORTED               (1<<10)
//11    Data for Testing bit supported
#define PLXP_MEAS_STATUS_DATA_FOR_TESTING_SUPPORTED                     (1<<11)
//12    Calibration Ongoing bit supported
#define PLXP_MEAS_STATUS_CALIBRATION_ONGOING_SUPPORTED                  (1<<12)
//13    Measurement Unavailable bit supported
#define PLXP_MEAS_STATUS_MEASUREMENT_UNAVAILABLE_SUPPORTED              (1<<13)
//14    Questionable Measurement Detected bit supported
#define PLXP_MEAS_STATUS_QUESTIONABLE_MEASUREMENT_DETECTED_SUPPORTED    (1<<14)
//15    Invalid Measurement Detected bit supported
#define PLXP_MEAS_STATUS_INVALID_MEASUREMENT_DETECTED_SUPPORTED         (1<<15)

#define PLXP_MEAS_STATUS_VALID_MASK (0xFFE0)


//Uint24 device_status_supported;
//Bit   Definition
//0     Extended Display Update Ongoing bit supported
#define PLXP_DEVICE_STATUS_EXTENDED_DISPLAY_UPDATE_ONGOING_SUPPORTED            (1<<0)
//1     Equipment Malfunction Detected bit supported
#define PLXP_DEVICE_STATUS_EQUIPMENT_MALFUNCTION_DETECTED_SUPPORTED             (1<<1)
//2     Signal Processing Irregularity Detected bit supported
#define PLXP_DEVICE_STATUS_SIGNAL_PROCESSING_IRREGULARITY_DETECTED_SUPPORTED    (1<<2)
//3     Inadequate Signal Detected bit supported
#define PLXP_DEVICE_STATUS_INADEQUATE_SIGNAL_DETECTE_SUPPORTED                  (1<<3)
//4     Poor Signal Detected bit supported
#define PLXP_DEVICE_STATUS_POOR_SIGNAL_DETECTED_SUPPORTED                       (1<<4)
//5     Low Perfusion Detected bit supported
#define PLXP_DEVICE_STATUS_LOW_PERFUSION_DETECTED_SUPPORTED                     (1<<5)
//6     Erratic Signal Detected bit supported
#define PLXP_DEVICE_STATUS_ERRATIC_SIGNAL_DETECTED_SUPPORTED                    (1<<6)
//7     Nonpulsatile Signal Detected bit supported
#define PLXP_DEVICE_STATUS_NONPULSATILE_SIGNAL_DETECTED_SUPPORTED               (1<<7)
//8     Questionable Pulse Detected bit supported
#define PLXP_DEVICE_STATUS_QUESTIONABLE_PULSE_DETECTED_SUPPORTED                (1<<8)
//9     Signal Analysis Ongoing bit supported
#define PLXP_DEVICE_STATUS_SIGNAL_ANALYSIS_ONGOING_SUPPORTED                    (1<<9)
//10    Sensor Interference Detected bit supported
#define PLXP_DEVICE_STATUS_SENSOR_INTERFERENCE_DETECTED_SUPPORTED               (1<<10)
//11    Sensor Unconnected to User bit supported
#define PLXP_DEVICE_STATUS_SENSOR_UNCONNECTED_TO_USER_SUPPORTED                 (1<<11)
//12    Unknown Sensor Connected bit supported
#define PLXP_DEVICE_STATUS_UNKNOWN_SENSOR_CONNECTED_SUPPORTED                   (1<<12)
//13    Sensor Displaced bit supported
#define PLXP_DEVICE_STATUS_SENSOR_DISPLACED_SUPPORTED                           (1<<13)
//14    Sensor Malfunctioning bit supported
#define PLXP_DEVICE_STATUS_SENSOR_MALFUNCTIONING_SUPPORTED                      (1<<14)
//15    Sensor Disconnected bit supported
#define PLXP_DEVICE_STATUS_SENSOR_DISCONNECTED_SUPPORTED                        (1<<15)
//16-23 Reserved for Future Use

#define PLXP_DEVICE_STATUS_VALID_MASK   (0xFFFF)


#define PLXP_CCC_NTF_ENABLED  (1<<0)
/// Mask to check if Indications are enabled in the ALERT CCC
#define PLXP_CCC_IND_ENABLED  (1<<1)
/// Max value of CCC Field
#define PLXP_MAX_NTF_CFG_VALUE 3


//uint8_t plxp_spot_meas_flag
//0     Timestamp field is present
#define PLXP_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT              (1<<0)
//1     Measurement Status field is present
#define PLXP_SPOT_MEAS_FLAGS_MEAS_STATUS_PRESENT            (1<<1)
//2     Device and Sensor Status field is present
#define PLXP_SPOT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT   (1<<2)
//3     Pulse Amplitude Index field is present
#define PLXP_SPOT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT        (1<<3)
//4     Device Clock is Not Set
#define PLXP_SPOT_MEAS_FLAGS_CLOCK_NOT_SET                  (1<<4)

#define PLXP_SPOT_MEAS_FLAGS_VALID_MASK   (0x1F)


//uint8_t plxp_cont_meas_flag
//0     SpO2PR-Fast field is present
#define PLXP_CONT_MEAS_FLAGS_SPO2PR_FAST_PRESENT            (1<<0)
//1     SpO2PR-Slow field is present
#define PLXP_CONT_MEAS_FLAGS_SPO2PR_SLOW_PRESENT            (1<<1)
//2     Measurement Status field is present
#define PLXP_CONT_MEAS_FLAGS_MEAS_STATUS_PRESENT            (1<<2)
//3     Device and Sensor Status field is present
#define PLXP_CONT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT   (1<<3)
//4     Pulse Amplitude Index field is present
#define PLXP_CONT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT        (1<<4)

#define PLXP_CONT_MEAS_FLAGS_VALID_MASK   (0x1F)

 /*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Control Point OpCodes
enum plxp_cp_opcodes_id
{
    PLXP_OPCODE_REPORT_STORED_RECORDS           = 1,
    PLXP_OPCODE_DELETE_STORED_RECORDS           = 2,
    PLXP_OPCODE_ABORT_OPERATION                 = 3,
    PLXP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS = 4,
    PLXP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP   = 5,
    PLXP_OPCODE_RESPONSE_CODE                   = 6,
};

// Control Point Operators
enum plxp_cp_operator_id
{
    PLXP_OPERATOR_NULL                  = 0,
    PLXP_OPERATOR_ALL_RECORDS           = 1,
};

// Control Point Response Code Values
enum plxp_cp_resp_code_id
{
    PLXP_RESP_SUCCESS                             = 1,
    PLXP_RESP_OP_CODE_NOT_SUPPORTED               = 2,
    PLXP_RESP_INVALID_OPERATOR                    = 3,
    PLXP_RESP_OPERATOR_NOT_SUPPORTED              = 4,
    PLXP_RESP_INVALID_OPERAND                     = 5,
    PLXP_RESP_NO_RECORDS_FOUND                    = 6,
    PLXP_RESP_ABORT_UNSUCCESSFUL                  = 7,
    PLXP_RESP_PROCEDURE_NOT_COMPLETED             = 8,
    PLXP_RESP_OPERAND_NOT_SUPPORTED               = 9,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */




#endif /* (BLE_PLXP_CLIENT || BLE_PLXP_SERVER) */

#endif /* _PLXP_COMMON_H_ */
