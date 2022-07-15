/******************************************************************************
	File Name	:	device1_uart_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/27

	Description :   

	Changed Log	:
		Liujian  2019/11/27			- Creation

*****************************************************************************/

#include "rwip_config.h"

#if (BLE_DEVICE2_UART_ENABLE)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "device2_uart.h"
#include "device2_uart_task.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include <string.h>

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UART_ENABLE_REQ message.
 * The handler enables the Health Thermometer Profile Thermometer Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int device2_uart_enable_req_handler(ke_msg_id_t const msgid,
                                   struct device2_uart_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);
        uart_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;

    }

    // send response
    struct device2_uart_enable_rsp *rsp = KE_MSG_ALLOC(DEVICE2_UART_ENABLE_RSP, src_id, dest_id, device2_uart_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UART_TEMP_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int device2_uart_temp_send_req_handler(ke_msg_id_t const msgid,
                                      struct device2_uart_data_send_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == DEVICE2_UART_IDLE)
    {
        // Get the address of the environment
        struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);

        {
            // allocate operation to execute
            uart_env->operation    = (struct device2_uart_op *) ke_malloc(sizeof(struct device2_uart_op) + sizeof(struct device2_uart_data_send_req ), KE_MEM_ATT_DB);

            // Initialize operation parameters
            uart_env->operation->cursor  = 0;
            uart_env->operation->dest_id = src_id;
            uart_env->operation->conidx  = GAP_INVALID_CONIDX;
        
            uart_env->operation->op      = DEVICE2_UART_CFG_STABLE_MEAS_IND;
            uart_env->operation->handle  = UART_HANDLE(DEVICE2_UART_IDX_TX_VAL);
           
            uart_env->operation->length  = param->length; 
			      memcpy(&(uart_env->operation->data[0]), param->tx_data, param->length);   
            // put task in a busy state
            ke_state_set(dest_id, DEVICE2_UART_BUSY);

            // execute operation
            device2_uart_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}



/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int device2_gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);
    uint8_t att_idx = UART_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;
	
	  printf("device2 att_idx = %d \n", att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        case DEVICE2_UART_IDX_TX_VAL_IND_CFG:
        {
            cfm->length = UART_IND_NTF_CFG_MAX_LEN;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;
				
				case DEVICE2_UART_IDX_RX_VAL:
        {
            cfm->length = MAX_RX_UART;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;
				

        default:
        {
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GL2C_CODE_ATT_WR_CMD_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int device2_gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);
	
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_NO_ERROR;
    int msg_status = KE_MSG_CONSUMED;
	
		ke_task_id_t  temp_dest_id;

    // to check if confirmation message should be send
    bool send_cfm = true;
	
//	 printf("gattc_write_req_ind_handler ..... \n");

    // retrieve handle information
    uint8_t att_idx = UART_IDX(param->handle);

   
		switch(att_idx)
		{
			 
				case DEVICE2_UART_IDX_TX_VAL_IND_CFG:
				{
					
							if(param->length != UART_IND_NTF_CFG_MAX_LEN)
							{
									status = PRF_ERR_UNEXPECTED_LEN;
							}
							else
							{

				//    status = uart_update_ntf_ind_cfg(conidx, UART_CFG_STABLE_MEAS_IND, PRF_CLI_START_IND, co_read16p(param->value));   //liujian indication
			
							status = device2_uart_update_ntf_ind_cfg(conidx, DEVICE2_UART_CFG_STABLE_MEAS_IND, PRF_CLI_START_NTF, co_read16p(param->value));   //liujian notify
							}
				}break;
				
				case DEVICE2_UART_IDX_RX_VAL:
				{
		
					  struct device2_uart_rx_data * rx_data = KE_MSG_ALLOC(DEVICE2_UART_RX_DATA_IND,
                                                               prf_dst_task_get(&(uart_env->prf_env), conidx),
                                                               dest_id, device2_uart_rx_data);
		
						
//						printf("conidx = %d \n",  conidx);
//						printf("dest_id = %d \n", prf_dst_task_get(&(uart_env->prf_env), conidx));
//						printf("src_id = %d \n",  dest_id);
//						
            rx_data->conidx = conidx;
						rx_data->length = param->length;
						memcpy(rx_data->rx_data, param->value, rx_data->length);

            ke_msg_send(rx_data);
            status = GAP_ERR_NO_ERROR;
						
				}		
					break;

		
				default:
				{
						status = ATT_ERR_REQUEST_NOT_SUPPORTED;
				}break;
		}
   

    if(send_cfm)
    {
        //Send write response
        struct gattc_write_cfm * cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return (msg_status);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int device2_gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);
	
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t value[10];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = UART_IDX(param->handle);
	
	  printf("device2 att_idx = %d \n", att_idx);

    switch(att_idx)
    {



        case DEVICE2_UART_IDX_TX_VAL_IND_CFG:
        {
            value_size = UART_IND_NTF_CFG_MAX_LEN;
          //  co_write16p(value, ((uart_env->ntf_ind_cfg[conidx] & UART_CFG_INTERM_MEAS_NTF) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);       //liujian indication
					
						 co_write16p(value, ((uart_env->ntf_ind_cfg[conidx] & DEVICE2_UART_CFG_INTERM_MEAS_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);    //liujian notify 
					
        }break;


        default:
        {
            status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    // Send data to peer device
    struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, value_size);
    cfm->length = value_size;
    memcpy(cfm->value, value, value_size);
    cfm->handle = param->handle;
    cfm->status = status;

    // Send value to peer device.
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY and GATT_INDICATE message meaning
 * that Measurement notification/indication has been correctly sent to peer device
 *
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int device2_gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
    device2_uart_exe_operation();

    return (KE_MSG_CONSUMED);
}

__STATIC int device2_gattc_write_without_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_cmd const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	
	 return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
KE_MSG_HANDLER_TAB(device2_uart)
{
    {DEVICE2_UART_ENABLE_REQ,            (ke_msg_func_t) device2_uart_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) device2_gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) device2_gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) device2_gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) device2_gattc_cmp_evt_handler},
		{GATTC_WRITE_CMD,            (ke_msg_func_t) device2_gattc_write_without_handler},

    {DEVICE2_UART_TEMP_SEND_REQ,         (ke_msg_func_t) device2_uart_temp_send_req_handler},
 
};

void device2_uart_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
  //  struct device1_uart_env_tag *uart_env = PRF_ENV_GET(UART, uart);
		struct device2_uart_env_tag* uart_env =  PRF_ENV_GET(DEVICE2_UART, device2_uart);
	
    task_desc->msg_handler_tab = device2_uart_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(device2_uart_msg_handler_tab);
    task_desc->state           = uart_env->state;
    task_desc->idx_max         = UART_IDX_MAX;
}

#endif 


