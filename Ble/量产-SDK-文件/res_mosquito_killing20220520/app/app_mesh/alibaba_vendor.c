/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	alibaba_vendor.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/8/3

	Description :   和天猫精灵 对接的时候, 需要一个Vendor Model
	
*******************************************************************************/
#if (BLE_APP_MESH == 1)
#include "co_bt.h"
#include "co_utils.h"
#include "prf.h"
#include "m_api.h" 
#include <string.h>
#include "m_api.h" 
#include "m_lay.h"
#include "alibaba_vendor.h"
#include "app_mesh_util.h"
/// Validity of information provided to the Replay Manager
#define MM_TIANMAO_VENDOR_REPLAY_MS               (6000)

typedef struct m_alibaba_vendor_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
	 /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;
	
	  uint32_t  opcode;
	
	  uint8_t   tid;
	  
	  uint16_t  attr_name;
	
	  uint32_t   event;
	
	  /// Address to which a TianMao Status message must be sent (unassigned address if no
    /// status must be sent)
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of TianMao Status message
    m_lid_t status_app_key_lid;
    /// Relaying of sent TianMao Status authorized
    bool status_relay;
	
 
} m_alibaba_vendor_env_t;



//事件上报
static void m_alibaba_vendor_server_send_event(m_alibaba_vendor_env_t *p_env_alibaba, mm_route_buf_env_t *p_route_env,
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

	// printf("vendor .......\n");
    // Check if a transition has been started
    //mm_tb_bind_get_trans_info(p_env_oo->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
    data_length = 7;

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
            p_buf_env->app_key_lid = p_env_alibaba->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_alibaba->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_alibaba->status_relay);

            // Can accept new transitions
            p_env_alibaba->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_alibaba->env.mdl_lid;
        p_buf_env->opcode = p_env_alibaba->opcode;

        // Fill the message
        *(p_data + ALIBABA_SET_TID_POS) = p_env_alibaba->tid++;
        *(p_data + ALIBABA_SET_ATTR_NAME_POS) =  (p_env_alibaba->attr_name&0xff);
        *(p_data + ALIBABA_SET_ATTR_NAME_POS + 1) = ((p_env_alibaba->attr_name>>8)&0xff);
				*(p_data + ALIBABA_SET_PARAMETER_POS) = p_env_alibaba->event;
				*(p_data + ALIBABA_SET_PARAMETER_POS + 1) = p_env_alibaba->event>>8;
				*(p_data + ALIBABA_SET_PARAMETER_POS + 2) = p_env_alibaba->event >> 16;
				*(p_data + ALIBABA_SET_PARAMETER_POS + 3) = p_env_alibaba->event >> 24;
				
        // Send the message
        mm_route_send(p_buf_status);
				
    }
}

static void m_alibaba_vendor_server_handler_set(m_alibaba_vendor_env_t *p_env_alibaba, mesh_tb_buf_t *p_buf,
                                     mm_route_buf_env_t *p_route_env)
{
	      do {
			      uint32_t   param_data;
				  uint16_t   attr_type;
				  // Get pointer to data
				  uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);

								// Extract TID value
                  uint8_t tid = *(p_data + ALIBABA_SET_TID_POS);
							
								 // Extract required attr type value
				  attr_type = *(p_data + ALIBABA_SET_ATTR_NAME_POS + 1);
				  attr_type <<= 8;
				  attr_type += *(p_data + ALIBABA_SET_ATTR_NAME_POS);
							
                  printf("tid = 0x%x attr_type = 0x%x data_len = %d \n", tid,  attr_type, p_buf->data_len);
					
			      if(p_route_env->opcode == ALIBABA_VENDOR_MESSAGE_ATTR_SET)
				  {
                        p_env_alibaba->status_dst_addr = p_route_env->u_addr.src;
						p_env_alibaba->status_app_key_lid = p_route_env->app_key_lid;
                        p_env_alibaba->status_relay = GETB(p_route_env->info, MM_ROUTE_BUF_INFO_RELAY);
                        p_env_alibaba->opcode = 0xD301A8;
                        p_env_alibaba->attr_name = attr_type;
                        p_env_alibaba->tid = tid;
									  
                        param_data = *(p_data + ALIBABA_SET_PARAMETER_POS + 3);
                        param_data <<= 8;
                        param_data += *(p_data + ALIBABA_SET_PARAMETER_POS + 2);
                        param_data <<= 8;
                        param_data += *(p_data + ALIBABA_SET_PARAMETER_POS + 1);
                        param_data <<= 8;
                        param_data += *(p_data + ALIBABA_SET_PARAMETER_POS + 0);
					    printf("linux time = 0x%x \n", param_data);
						app_mesh_util_unix_time(param_data);			
                        p_env_alibaba->event = param_data;
									
                        m_alibaba_vendor_server_send_event(p_env_alibaba, p_route_env, false);
                   }
								
//								if (GETB(p_env_alibaba->env.info, MM_TB_STATE_MDL_INFO_MAIN))
//								{
//										// Update target state
//										p_env_alibaba->tgt_onoff = onoff;

//										// Inform the Binding Manager about new transition
//										mm_tb_bind_trans_new(p_env_alibaba->env.grp_lid, MM_TRANS_TYPE_CLASSIC,
//																				 trans_time, delay);
//								}
//								else
//								{
//										// Inform the Binding Manager
//										mm_tb_bind_trans_req(p_env_alibaba->env.grp_lid, p_env_oo->env.mdl_lid,
//																				 MM_TRANS_TYPE_CLASSIC, onoff, trans_time, delay);
//								}
				
								
					
              }while(0);
	
}


static void m_alibaba_vendor_server_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{
    // Get environment for TianMao Server model
    m_alibaba_vendor_env_t *p_env_alibaba = (m_alibaba_vendor_env_t *)p_env;

    if (p_route_env->opcode == ALIBABA_VENDOR_MESSAGE_ATTR_SET)
    {			
			  m_alibaba_vendor_server_handler_set(p_env_alibaba, p_buf, p_route_env);	
    }
}

static uint16_t m_alibaba_vendor_server_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
    uint16_t status;
	
    printf("vendor mode opcdoe = 0x%x \n", opcode);
	
    if ((opcode == ALIBABA_VENDOR_MESSAGE_ATTR_SET))
    {
        status = MESH_ERR_NO_ERROR;
    }
    else
    {
        status = MESH_ERR_MDL_INVALID_OPCODE;
    }

    return (status);
}


static void m_alibaba_vendor_server_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                          uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}

static void m_alibaba_vendor_server_publish(m_alibaba_vendor_env_t *p_env_alibaba)
{
    // Check if sending of publication is enabled
    if (GETB(p_env_alibaba->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
        m_alibaba_vendor_server_send_event(p_env_alibaba, NULL, true);
    }
}


static void m_alibaba_vendor_server_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    m_alibaba_vendor_env_t *p_env_alibaba = (m_alibaba_vendor_env_t *)p_env;

    if (p_env_alibaba->publi_period_ms)
    {
        // Publish a tianmao event message
        m_alibaba_vendor_server_publish(p_env_alibaba);

        // Restart the timer
        mesh_tb_timer_set(&p_env_alibaba->tmr_publi, p_env_alibaba->publi_period_ms);
    }
}

static uint16_t m_alibaba_vendor_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
    // Register the model
    uint16_t status = m_api_register_model(ALIBABA_VENDOR_SERVER_ID, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, ALIBABA_VENDOR_SERVER_ID, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(m_alibaba_vendor_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            m_alibaba_vendor_env_t *p_env_server_vendor = (m_alibaba_vendor_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_server_vendor->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_server_vendor->tmr_publi.cb = m_alibaba_vendor_server_cb_tmr_publi;
            p_env_server_vendor->tmr_publi.p_env = (void *)p_env_server_vendor;

            // Prepare environment for Replay Manager
            p_env_server_vendor->replay_env.delay_ms = MM_TIANMAO_VENDOR_REPLAY_MS;

            // Set internal callback functions
            p_env_server_vendor->env.cb.cb_rx = m_alibaba_vendor_server_cb_rx;
            p_env_server_vendor->env.cb.cb_opcode_check = m_alibaba_vendor_server_cb_opcode_check;
            p_env_server_vendor->env.cb.cb_publish_param = m_alibaba_vendor_server_publish_param;
            //p_cb_srv->cb_set = mm_gens_oo_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(ALIBABA_VENDOR_SERVER_ID, elmt_idx, *p_mdl_lid);
        }
    } while (0);

    return (status);
}


uint16_t m_alibaba_vendor_register_server(uint8_t elmt_idx, bool main)
{
	// Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, ALIBABA_VENDOR_SERVER_ID) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Alibaba Server model
        status = m_alibaba_vendor_register(elmt_idx, &mdl_lid);

    }

    return (status);
		
}

#endif