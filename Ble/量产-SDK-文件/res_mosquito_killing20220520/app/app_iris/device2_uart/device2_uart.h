/******************************************************************************
	File Name	:	device2_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/8

	Description :   

	Changed Log	:
		Liujian  2019/12/8			- Creation

*****************************************************************************/

#ifndef __DEVICE2_UART_H__
#define __DEVICE2_UART_H__


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_DEVICE2_UART_ENABLE)
#include <stdint.h>
#include <stdbool.h>
//#include "htp_common.h"
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"

/*
 * MACROS
 ****************************************************************************************
 */
#define UART_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define UART_HANDLE(idx) (device2_uart_att_hdl_get(uart_env, (idx)))

#define UART_IDX(hdl)    (device2_uart_att_idx_get(uart_env, (hdl)))


/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of Health thermometer task instances
#define UART_IDX_MAX    (1)







#define UART_IND_NTF_CFG_MAX_LEN          (2)

#define  DEVICE2_UART_PROFILE_MASK               (0x003F)


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


///
enum devvice2_uart_state
{
    /// Idle state
    DEVICE2_UART_IDLE,
    /// Busy state
    DEVICE2_UART_BUSY,

    /// Number of defined states.
    DEVICE2_UART_STATE_MAX
};

///Attributes database elements
enum device2_uart_att_db_list
{
    DEVICE2_UART_IDX_SVC,

	//bluetooth to phone
    DEVICE2_UART_IDX_TX_CHAR,
    DEVICE2_UART_IDX_TX_VAL,
    DEVICE2_UART_IDX_TX_VAL_IND_CFG,
	
	//phone send to bluetooth
	 DEVICE2_UART_IDX_RX_CHAR,
	 DEVICE2_UART_IDX_RX_VAL,  
	//  UART_IDX_RX_VAL_CFG,
		
	
		

   DEVICE2_UART_IDX_NB,
};


/// ongoing operation information
struct device2_uart_op
{
     /// Operation
     uint8_t op;
     /// Cursor on connection
     uint8_t cursor;
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// Task that request the operation that should receive completed message response
     uint16_t dest_id;
     /// Packed notification/indication data size
     uint8_t length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t conidx;
     /// Packed notification/indication data
     uint8_t data[__ARRAY_EMPTY];
};


///Health Thermometer Profile Thermometer Environment Variable
struct device2_uart_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct device2_uart_op * operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;



    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
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
uint8_t device2_uart_get_valid_rge_offset(uint16_t features);

/**
 ****************************************************************************************
 * @brief Retrieve HTS service profile interface
 *
 * @return HTS service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* device2_uart_prf_itf_get(void);



/**
 ****************************************************************************************
 * @brief  This function fully manage notification and indication of health thermometer
 *         to peer(s) device(s) according to on-going operation requested by application:
 *            - Modification of Intermediate Temperature
 *            - Indicate to a known device that Temperature Measured has change
 *            - Indicate to a known device that Measure Interval has change
 ****************************************************************************************
 */
void device2_uart_exe_operation(void);



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
uint8_t device2_uart_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);


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
uint16_t device2_uart_att_hdl_get(struct device2_uart_env_tag* uart_env, uint8_t att_idx);

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
uint8_t device2_uart_att_idx_get(struct device2_uart_env_tag* uart_env, uint16_t handle);




#endif //BLE_HT_THERMOM

/// @} HTPT

#endif // HTPT_H_
