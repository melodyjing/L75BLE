/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	tulip_vendor_module.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/3/25

	Description :   
	
*******************************************************************************/
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include "co_bt.h"
#include "co_utils.h"
#include "prf.h"
#include "m_api.h" 
#include <string.h>
#include "m_api.h" 
#include "m_lay.h"
#include "tulip_vendor_module.h"
#include "app_mesh_util.h"
#include "app_tulip_comm.h"
#include "app_tulip_scene.h"

/// Validity of information provided to the Replay Manager
#define MM_TULIPEI_VENDOR_REPLAY_MS               (6000)


typedef struct  {
	
	uint32_t  status;       
	
	
	
}tulipi_status_t ;

typedef __packed struct {
	uint8_t     type;  
  uint16_t    element_address;	
  uint8_t     param_value[50];	
}tulipi_command_t ;


typedef struct{
	  
	  tulipi_status_t  vendor_status;
	
	  tulipi_command_t  vendor_command;
	
}tulipi_vendor_data_t;


typedef struct m_tulipi_vendor_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
	 /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;
	
	  
	  tulipi_vendor_data_t  tulipi_data;
	
	  /// Address to which a tulipi Status message must be sent (unassigned address if no
    /// status must be sent)
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of tulipi Status message
    m_lid_t status_app_key_lid;
    /// Relaying of sent tulipi Status authorized
    bool status_relay;
	
 
} m_tulipi_vendor_env_t;


static void m_tulipi_vendor_server_send_status(m_tulipi_vendor_env_t *p_env_tulipi, mm_route_buf_env_t *p_route_env,
                                     bool publish)
{
// Pointer to the buffer that will contain the message
    mesh_tb_buf_t *p_buf_status;
    // Remaining time
    uint8_t rem_time;
    // Transition type
    uint8_t trans_type;
    // Data length
    uint8_t data_length;
	
    // Check if a transition has been started
    mm_tb_bind_get_trans_info(p_env_tulipi->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
	
    data_length = sizeof(tulipi_status_t);

    if (mm_route_buf_alloc(&p_buf_status, data_length) == MESH_ERR_NO_ERROR)
    {
        // Get pointer to data
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf_status);
        // Get pointer to environment
        mm_route_buf_env_t *p_buf_env = (mm_route_buf_env_t *)&p_buf_status->env;

        // Prepare environment
        if (p_route_env)
        {
            p_buf_env->app_key_lid = p_route_env->app_key_lid;
            p_buf_env->u_addr.dst = p_route_env->u_addr.src;
            p_buf_env->info = p_route_env->info;
        }
        else if (!publish)
        {
            p_buf_env->app_key_lid = p_env_tulipi->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_tulipi->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_tulipi->status_relay);

            // Can accept new transitions
            p_env_tulipi->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_tulipi->env.mdl_lid;
        p_buf_env->opcode = TULIPEI_VENDOR_MESSAGE_ATTR_STATUS;
				
				memcpy(p_data, &p_env_tulipi->tulipi_data.vendor_status, sizeof(tulipi_status_t));
				
        // Send the message
        mm_route_send(p_buf_status);
    }
}

static void m_tulipi_vendor_server_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                          uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}


static void m_tulipi_vendor_server_publish(m_tulipi_vendor_env_t *p_env_tulipi)
{
    // Check if sending of publication is enabled
    if (GETB(p_env_tulipi->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
        m_tulipi_vendor_server_send_status(p_env_tulipi, NULL, true);
    }
}

static void m_tulipi_vendor_server_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    m_tulipi_vendor_env_t *p_env_tulipi = (m_tulipi_vendor_env_t *)p_env;

    if (p_env_tulipi->publi_period_ms)
    {
        // Publish a tianmao event message
        m_tulipi_vendor_server_publish(p_env_tulipi);

        // Restart the timer
        mesh_tb_timer_set(&p_env_tulipi->tmr_publi, p_env_tulipi->publi_period_ms);
    }
}

static uint16_t m_tulipi_vendor_server_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
    uint16_t status = MESH_ERR_MDL_INVALID_OPCODE;

//	  printf("m_tulipi_vendor_server_cb_opcode_check  0x%x \n", opcode);
	
    if (p_env->mdl_id == TULIPEI_VENDOR_SERVER_ID)
    {
        if ((opcode == TULIPEI_VENDOR_MESSAGE_ATTR_GET)
                || (opcode == TULIPEI_VENDOR_MESSAGE_ATTR_SET)
                || (opcode == TULIPEI_VENDOR_MESSAGE_ATTR_SET_UNACK)
                )
        {
            status = MESH_ERR_NO_ERROR;
        }
    }
   

    return (status);
}


//2021/4/14 lichun
/**
 ****************************************************************************************
 * @brief Callback function called by Mesh Model block to inform the application about update
 * of a local state
 *
 * @param[in] state_id          State identifier
 * @param[in] elmt_idx          Element Index
 * @param[in] state             New state or targeted state value depending on the provided
 * transition time
 * @param[in] trans_time_ms     Transition time in milliseconds
 ****************************************************************************************
 */
__STATIC void mm_api_msg_vendor_msg_upd_ind(uint16_t state_id, uint8_t elmt_idx, uint8_t *msg,uint8_t len,
                                              uint32_t trans_time_ms)
{
    // Allocate the indication message
    mm_api_vendor_msg_upd_ind_t *p_ind = MAL_MSG_ALLOC_DYN(MESH_MDL_API_IND, mm_api_vendor_msg_upd_ind_t,len);

    // Fill the content
    p_ind->ind_code = MM_API_VENDOR_MSG_IND;
    p_ind->elmt_idx = elmt_idx;
    memmove(p_ind->msg,msg,len);
    p_ind->state_id = state_id;
    p_ind->trans_time_ms = trans_time_ms;

    // Send the message
    mal_msg_send(mal_app_id_get(), p_ind);
}


__STATIC void mm_tulipi_handler_set(m_tulipi_vendor_env_t *p_env_tulipi, mesh_tb_buf_t *p_buf,
                                        mm_route_buf_env_t *p_route_env, uint16_t opcode)
{
    do
    {
			
			  tulipi_command_t *p_set = (tulipi_command_t *)MESH_TB_BUF_DATA(p_buf);
								 
  
//				printf("cmd = 0x%x  address = 0x%x £¬ 0x%x \n", p_set->type, p_set->element_address, m_tb_mio_get_prim_addr());
//			
//			  {
//					 for(int i = 0; i < 11; i++)
//					 {
//						  printf("0x%x ", p_set->param_value[i]);
//					 }
//					 printf("\n");
//				}
				
			  if(m_tb_mio_get_prim_addr() != p_set->element_address)
				{
					  return;
				}
			  // Inform application about received value
			  if(p_set->type == TULIPEI_OTA_SET_CMD)
				{
             mm_api_send_srv_state_upd_ind(MM_STATE_TULIPXI_VENDOR_OTA,
                                          p_env_tulipi->env.elmt_idx, p_set->element_address, 0);
					  
        } 
				else if(TULIPEI_SCENE_COLOR_VALUE == p_set->type )
				{
					  //printf("edit scene command\n");
					  mm_api_msg_vendor_msg_upd_ind(MM_STATE_TULIPXI_VENDOR_SCENS_SAVE, p_env_tulipi->env.elmt_idx, \
					                p_set->param_value, sizeof(p_set->param_value), 0);
				}
    } while (0);
}


__STATIC void m_tulipi_vendor_server_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{

    m_tulipi_vendor_env_t *p_env_tulipi;
	
	  printf("m_tulipi_vendor_server_cb_rx  0x%x \n", p_route_env->opcode);
   	
	  if(p_env->mdl_id == TULIPEI_VENDOR_SERVER_ID)
		{
			  p_env_tulipi = (m_tulipi_vendor_env_t *)p_env;
			
			  switch(p_route_env->opcode)
				{
						 
						 case TULIPEI_VENDOR_MESSAGE_ATTR_SET:
						 {
							  mm_tulipi_handler_set(p_env_tulipi, p_buf, p_route_env, p_route_env->opcode);
						 }break;
						 default:
							 break;
				}	
		}


}



static uint16_t m_tulipi_vendor_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
    // Register the model
    uint16_t status = m_api_register_model(TULIPEI_VENDOR_SERVER_ID, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, TULIPEI_VENDOR_SERVER_ID, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(m_tulipi_vendor_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            m_tulipi_vendor_env_t *p_env_server_vendor = (m_tulipi_vendor_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_server_vendor->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_server_vendor->tmr_publi.cb = m_tulipi_vendor_server_cb_tmr_publi;
            p_env_server_vendor->tmr_publi.p_env = (void *)p_env_server_vendor;

            // Prepare environment for Replay Manager
            p_env_server_vendor->replay_env.delay_ms = MM_TULIPEI_VENDOR_REPLAY_MS;

            // Set internal callback functions
            p_env_server_vendor->env.cb.cb_rx = m_tulipi_vendor_server_cb_rx;
            p_env_server_vendor->env.cb.cb_opcode_check = m_tulipi_vendor_server_cb_opcode_check;
            p_env_server_vendor->env.cb.cb_publish_param = m_tulipi_vendor_server_publish_param;
            //p_cb_srv->cb_set = mm_gens_oo_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(TULIPEI_VENDOR_SERVER_ID, elmt_idx, *p_mdl_lid);
        }
    } while (0);

    return (status);
}


uint16_t m_tulipi_vendor_register_server(uint8_t elmt_idx, bool main)
{
	// Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, TULIPEI_VENDOR_SERVER_ID) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Tulipsi Server model
        status = m_tulipi_vendor_register(elmt_idx, &mdl_lid);

    }

    return (status);
		
}


#endif