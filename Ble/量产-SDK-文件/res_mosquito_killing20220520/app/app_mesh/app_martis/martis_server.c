/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	File Name	:	  martis_server.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/7/14
	Description :   
	    微断这边的设备
	Changed Log	:
	Liujian  2021/7/14		- Creation
*****************************************************************************/

#include "martis_server.h"

#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions
#include "mm_api_int.h"     // Mesh Model Application Programming Interface Internal Definitions
#include "m_lay.h"
#include <stdio.h>


typedef struct  {
	
	uint32_t  status;       

}micro_breaker_status_t ;


typedef __packed struct {
	uint8_t     type;  
  uint16_t    element_address;	
  uint8_t     param_value[128];	
}micro_breaker_command_t ;


typedef struct{
	  
	  micro_breaker_status_t  vendor_status;
	
	  micro_breaker_command_t  vendor_command;
	
}micro_breaker_data_t;


/// Structure for micro breaker Server model environment
typedef struct mm_micro_breaker_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;

    micro_breaker_data_t micro_data;

    /// Address to which a micro breaker Status message must be sent (unassigned address if no
    /// status must be sent)
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of micro breaker Status message
    m_lid_t status_app_key_lid;
    /// Relaying of sent micro breaker Status authorized
    bool status_relay;
} mm_micro_breaker_env_t;


__STATIC void mm_micro_breaker_send_status(mm_micro_breaker_env_t *p_env_micro_breaker, mm_route_buf_env_t *p_route_env,
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
    mm_tb_bind_get_trans_info(p_env_micro_breaker->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
    data_length = sizeof(micro_breaker_status_t);

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
            p_buf_env->app_key_lid = p_env_micro_breaker->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_micro_breaker->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_micro_breaker->status_relay);

            // Can accept new transitions
            p_env_micro_breaker->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_micro_breaker->env.mdl_lid;
        p_buf_env->opcode = MARTIS_VENDOR_MESSAGE_STATE;

        memcpy(p_data, &p_env_micro_breaker->micro_data.vendor_status, sizeof(micro_breaker_status_t));
        // Send the message
        mm_route_send(p_buf_status);
    }
}

__STATIC void mm_micro_breaker_handler_set(mm_micro_breaker_env_t *p_env_micro_breaker, mesh_tb_buf_t *p_buf,
                                     mm_route_buf_env_t *p_route_env)
{
    do
    {
			
			  micro_breaker_command_t *p_set = (micro_breaker_command_t *)MESH_TB_BUF_DATA(p_buf);
			
			  uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			
			  uint8_t data_lenght = p_buf->data_len;
			
			  for(int i = 0; i < p_buf->data_len; i++)
			  {
					 printf("0x%02x ", p_data[i]);
				}
				printf("\n");
				
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_SERVER_LIGHT == 1))				
				app_micro_breaker_process_rx_data(p_data, data_lenght);
#endif
				
//			  mm_micro_breaker_send_status(p_env_micro_breaker, p_route_env, false);
//				
//			  if(m_tb_mio_get_prim_addr() != p_set->element_address)
//				{
//					  return;
//				}
#if 0			
        // Get pointer to data
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
        // Extract required onoff state value
        uint8_t onoff = *(p_data + MM_GEN_OO_SET_OO_POS);
        // Extract TID value
        uint8_t tid = *(p_data + MM_GEN_OO_SET_TID_POS);
        // Transition time
        uint8_t trans_time = MM_TRANS_TIME_UNKNOWN;
        // Delay
        uint8_t delay = 0;

        // Check received state value
        if (onoff > 1)
        {
            break;
        }

        // Extract and check optional parameters if present
        if (p_buf->data_len == MM_GEN_OO_SET_LEN)
        {
            trans_time = (uint16_t)(*(p_data + MM_GEN_OO_SET_TRANS_TIME_POS));

            // Check received value
            if (GETF(trans_time, MM_TRANS_TIME_STEP_NB) > MM_TRANS_TIME_NB_STEPS_MAX)
            {
                // Drop the message
                break;
            }

            delay = *(p_data + MM_GEN_OO_SET_DELAY_POS);
        }

        // Check if received message is a retransmitted one, if state is modified and if
        // a new transition can be started now
        if ((p_env_oo->status_dst_addr != MESH_UNASSIGNED_ADDR)
                || mm_tb_replay_is_retx(&p_env_oo->replay_env, p_route_env->u_addr.src, tid)
                || (onoff == p_env_oo->onoff))
        {
            if (p_route_env->opcode == MM_MSG_GEN_OO_SET)
            {
                // Send Generic OnOff status message
                mm_gens_oo_send_status(p_env_oo, p_route_env, false);
            }
            break;
        }

        // Keep information for transmission of status if needed
        if (p_route_env->opcode == MM_MSG_GEN_OO_SET)
        {
            p_env_oo->status_dst_addr = p_route_env->u_addr.src;
            p_env_oo->status_app_key_lid = p_route_env->app_key_lid;
            p_env_oo->status_relay = GETB(p_route_env->info, MM_ROUTE_BUF_INFO_RELAY);
        }

        if (GETB(p_env_oo->env.info, MM_TB_STATE_MDL_INFO_MAIN))
        {
     
					// Update target state
            p_env_oo->tgt_onoff = onoff;

            // Inform the Binding Manager about new transition
            mm_tb_bind_trans_new(p_env_oo->env.grp_lid, MM_TRANS_TYPE_CLASSIC,
                                 trans_time, delay);
        }
        else
        {
            // Inform the Binding Manager
            mm_tb_bind_trans_req(p_env_oo->env.grp_lid, p_env_oo->env.mdl_lid,
                                 MM_TRANS_TYPE_CLASSIC, onoff, trans_time, delay);
        }
#endif

    } while (0);
}



__STATIC void mm_micro_breaker_publish(mm_micro_breaker_env_t *p_env_micro_breaker)
{
    // Check if sending of publication is enabled
    if (GETB(p_env_micro_breaker->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
        mm_micro_breaker_send_status(p_env_micro_breaker, NULL, true);
    }
}


__STATIC void mm_micro_breaker_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    mm_micro_breaker_env_t *p_env_micro_breaker = (mm_micro_breaker_env_t *)p_env;

    if (p_env_micro_breaker->publi_period_ms)
    {
        // Publish a Generic OnOff Status message
        mm_micro_breaker_publish(p_env_micro_breaker);

        // Restart the timer
        mesh_tb_timer_set(&p_env_micro_breaker->tmr_publi, p_env_micro_breaker->publi_period_ms);
    }
}

__STATIC void mm_micro_breaker_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{
    // Get environment for micro breaker Server model
    mm_micro_breaker_env_t *p_env_micro_breaker = (mm_micro_breaker_env_t *)p_env;

    if (p_route_env->opcode != MARTIS_VENDOR_MESSAGE_GET)
    {
        // Handle Generic OnOff Set/Set Unacknowledged message
        mm_micro_breaker_handler_set(p_env_micro_breaker, p_buf, p_route_env);
    }
    else
    {
        // Send a Generic micro breaker Status message
        mm_micro_breaker_send_status(p_env_micro_breaker, p_route_env, false);
    }
}



__STATIC uint16_t mm_micro_breaker_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
    uint16_t status;

    if ((opcode == MARTIS_VENDOR_MESSAGE_GET)
            || (opcode == MARTIS_VENDOR_MESSAGE_SET))
    {
        status = MESH_ERR_NO_ERROR;
    }
    else
    {
        status = MESH_ERR_MDL_INVALID_OPCODE;
    }

    return (status);
}

__STATIC void mm_micro_breaker_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                          uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}


uint16_t mm_micro_breaker_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
    // Register the model
    uint16_t status = m_api_register_model(MARTIS_VENDOR_SERVER_ID, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, MARTIS_VENDOR_SERVER_ID, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(mm_micro_breaker_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            mm_micro_breaker_env_t *p_env_micro_breaker = (mm_micro_breaker_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_micro_breaker->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_micro_breaker->tmr_publi.cb = mm_micro_breaker_cb_tmr_publi;
            p_env_micro_breaker->tmr_publi.p_env = (void *)p_env_micro_breaker;

            // Prepare environment for Replay Manager
            p_env_micro_breaker->replay_env.delay_ms = MM_MICRO_BREAKER_REPLAY_MS;

            // Set internal callback functions
            p_env_micro_breaker->env.cb.cb_rx = mm_micro_breaker_cb_rx;
            p_env_micro_breaker->env.cb.cb_opcode_check = mm_micro_breaker_opcode_check;
            p_env_micro_breaker->env.cb.cb_publish_param = mm_micro_breaker_publish_param;
       //     p_cb_srv->cb_set = mm_micro_breaker_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(MARTIS_VENDOR_SERVER_ID, elmt_idx, *p_mdl_lid);
        }
    } while (0);

    return (status);
}

uint16_t m_martis_breaker_register_server(uint8_t elmt_idx, bool main)
{
	// Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, MARTIS_VENDOR_SERVER_ID) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Tulipsi Server model
        status = mm_micro_breaker_register(elmt_idx, &mdl_lid);

    }

    return (status);
		
}



