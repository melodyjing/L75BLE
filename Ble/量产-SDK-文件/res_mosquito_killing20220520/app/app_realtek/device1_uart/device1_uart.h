/******************************************************************************
	File Name	:	device1_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/27

	Description :   

	Changed Log	:
		Liujian  2019/11/27			- Creation

*****************************************************************************/

#ifndef __DEVICE1_UART_H__
#define __DEVICE1_UART_H__


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_DEVICE1_UART_ENABLE)
#include <stdint.h>
#include <stdbool.h>
//#include "htp_common.h"
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"
#include "def_self.h"
/*
 * MACROS
 ****************************************************************************************
 */
#define UART_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define UART_HANDLE(idx) (device1_uart_att_hdl_get(uart_env, (idx)))

#define UART_IDX(hdl)    (device1_uart_att_idx_get(uart_env, (hdl)))


/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of Health thermometer task instances
#define UART_IDX_MAX    (1)







#define UART_IND_NTF_CFG_MAX_LEN          (2)

#define  UART_PROFILE_MASK               (0x003F)


//#define HTPT_TEMP_TYPE_MASK               (0x0030)
//#define HTPT_INTM_TEMP_MASK               (0x01C0)
//#define HTPT_MEAS_INTV_MASK               (0x0600)
//#define HTPT_MEAS_INTV_CCC_MASK           (0x0800)
//#define HTPT_MEAS_INTV_VALID_RGE_MASK     (0x1000)

#define UART_TX_ATT_NB             (4)

#define HTPT_TEMP_TYPE_ATT_NB             (2)
#define HTPT_INTERM_MEAS_ATT_NB           (3)
#define HTPT_MEAS_INTV_ATT_NB             (2)
#define HTPT_MEAS_INTV_CCC_ATT_NB         (1)
#define HTPT_MEAS_INTV_RNG_ATT_NB         (1)


/// Possible states of the HTPT task
enum uart_state
{
    /// Idle state
    UART_IDLE,
    /// Busy state
    UART_BUSY,

    /// Number of defined states.
    UART_STATE_MAX
};

///Attributes database elements
enum uart_att_db_list
{
    UART_IDX_SVC,

	//bluetooth to phone
    UART_IDX_TX_CHAR,
    UART_IDX_TX_VAL,
    UART_IDX_TX_VAL_IND_CFG,
	
	//phone send to bluetooth
		UART_IDX_RX_CHAR,
	  UART_IDX_RX_VAL,  
	//  UART_IDX_RX_VAL_CFG,
		
	
		

    UART_IDX_NB,
};


/// ongoing operation information
struct device1_uart_op
{
     /// Operation
     UINT8 op;
     /// Cursor on connection
     UINT8 cursor;
     /// Handle of the attribute to indicate/notify
     UINT16 handle;
     /// Task that request the operation that should receive completed message response
     UINT16 dest_id;
     /// Packed notification/indication data size
     UINT8 length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     UINT8 conidx;
     /// Packed notification/indication data
     UINT8 data[__ARRAY_EMPTY];
};


///Health Thermometer Profile Thermometer Environment Variable
struct device1_uart_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct device1_uart_op * operation;
    /// Service Start Handle
    UINT16 shdl;
    /// Database configuration
    UINT16 features;



    /// Notification and indication configuration of peer devices.
    UINT8 ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[UART_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Compute the offset of the valid range descriptor.
 * The Measurement Interval characteristic has two optional descriptors. In the database,
 * the Client Characteristic Configuration descriptor will always be placed just after the
 * characteristic value. Thus, this function checks if the CCC descriptor has been added.
 * @return     0 if Measurement Interval Char. is not writable (no Valid Range descriptor)
 *             1 if Measurement Interval Char. doesn't support indications (no CCC descriptor)
 *             2 otherwise
 ****************************************************************************************
 */
UINT8 uart_get_valid_rge_offset(UINT16 features);

/**
 ****************************************************************************************
 * @brief Retrieve HTS service profile interface
 *
 * @return HTS service profile interface
 ****************************************************************************************
 */
CONST struct prf_task_cbs* device1_uart_prf_itf_get(void);



/**
 ****************************************************************************************
 * @brief  This function fully manage notification and indication of health thermometer
 *         to peer(s) device(s) according to on-going operation requested by application:
 *            - Modification of Intermediate Temperature
 *            - Indicate to a known device that Temperature Measured has change
 *            - Indicate to a known device that Measure Interval has change
 ****************************************************************************************
 */
void device1_uart_exe_operation(void);



/**
 ****************************************************************************************
 * @brief Update Notification, Indication configuration
 *
 * @param[in] conidx    Connection index
 * @param[in] cfg       Indication configuration flag
 * @param[in] valid_val Valid value if NTF/IND enable
 * @param[in] value     Value set by peer device.
 *
 * @return status of configuration update
 ****************************************************************************************
 */
UINT8 device1_uart_update_ntf_ind_cfg(UINT8 conidx, UINT8 cfg, UINT16 valid_val, UINT16 value);


/**
 ****************************************************************************************
 * @brief Retrieve attribute handle from attribute index
 *
 * @param[in] htpt_env   Environment variable
 * @param[in] att_idx    Attribute Index
 *
 * @return attribute Handle
 ****************************************************************************************
 */
UINT16 device1_uart_att_hdl_get(struct device1_uart_env_tag* htpt_env, UINT8 att_idx);

/**
 ****************************************************************************************
 * @brief Retrieve attribute index from attribute handle
 *
 * @param[in] htpt_env   Environment variable
 * @param[in] handle     Attribute Handle
 *
 * @return attribute Index
 ****************************************************************************************
 */
UINT8 device1_uart_att_idx_get(struct device1_uart_env_tag* htpt_env, UINT16 handle);



void uart_temp_send_rsp_send(UINT8 conidx, UINT8 status);

#endif //BLE_HT_THERMOM

/// @} HTPT

#endif // HTPT_H_
