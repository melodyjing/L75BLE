/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_time.c
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/1

	Description :   


	Changed Log	:

		Liujian  2021/2/1		- Creation

*****************************************************************************/
#include "mm_tscns_time.h"
#include "mm_tscns_util.h"
#include <stdio.h>

typedef struct  {
	
	uint32_t  tai_seconds;        // 32bit is enough for 2000 ~ 2099 year
	
	uint8_t   tai_seconds_rsv;
	
	uint8_t   subsecond;
	
	uint8_t   uncertainty;
	
	uint16_t  time_authority		:1;
	
	uint16_t  tai_utc_delta	    :15;
	
	int8_t   time_zone_offset;
	
}time_status_t __ALIGN1;


typedef struct{
	
	time_status_t   time_status;
	
	uint8_t         zone_offset_new;
	
	uint8_t         tai_zone_change[5];
	
	uint16_t        delta_new	:15;
	
	uint16_t        rsv				:1;
	
	uint8_t         tai_delta_change[5];
	
	uint8_t         role;
	
}mesh_time_t;


/// Structure for Time Server model environment
typedef struct mm_tscns_time_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;

    mesh_time_t   mesh_time;

    /// Address to which a Time Status message must be sent (unassigned address if no
    /// status must be sent)
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of Time Status message
    m_lid_t status_app_key_lid;
    /// Relaying of sent Time Status authorized
    bool status_relay;
} mm_tscns_time_env_t;


/// Structure for Time Setup Server model environment
typedef struct mm_tscns_times_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Pointer to environment of associated Time Server model
    mm_tscns_time_env_t *p_env_time;
} mm_tscns_times_env_t;


__STATIC uint16_t mm_tscns_time_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
    uint16_t status = MESH_ERR_MDL_INVALID_OPCODE;

    if (p_env->mdl_id == MM_ID_TSCNS_TIM)
    {
        if ((opcode == MM_MSG_TIME_GET)
                || (opcode == MM_MSG_TIME_STATUS)
                || (opcode == MM_MSG_ZONE_GET)
                || (opcode == MM_MSG_TAI_UTC_DELTA_GET))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }
    else if (p_env->mdl_id == MM_ID_TSCNS_TIMS)    //Setup
    {
        if ((opcode == MM_MSG_TIME_SET)
                || (opcode == MM_MSG_ZONE_SET)
                || (opcode == MM_MSG_TAI_UTC_DELTA_SET)
				        || (opcode == MM_MSG_TIME_ROLS_SET)
                || (opcode == MM_MSG_TIME_ROLE_GET))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }

    return (status);
}


__STATIC void mm_tscns_time_send_status(mm_tscns_time_env_t *p_env_time, mm_route_buf_env_t *p_route_env,
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

	  printf("mm_tscns_time_send_status  \n");
	
    // Check if a transition has been started
    mm_tb_bind_get_trans_info(p_env_time->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
    data_length = sizeof(time_status_t);

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
            p_buf_env->app_key_lid = p_env_time->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_time->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_time->status_relay);

            // Can accept new transitions
            p_env_time->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_time->env.mdl_lid;
        p_buf_env->opcode = MM_MSG_TIME_STATUS;
				
				memcpy(p_data, &p_env_time->mesh_time.time_status, sizeof(time_status_t));
				
        // Send the message
        mm_route_send(p_buf_status);
    }
}

__STATIC void mm_tscns_time_publish(mm_tscns_time_env_t *p_env_time)
{
    // Check if sending of publication is enabled
    if (GETB(p_env_time->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
        mm_tscns_time_send_status(p_env_time, NULL, true);
    }
}

__STATIC void mm_tscns_time_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    mm_tscns_time_env_t *p_env_time = (mm_tscns_time_env_t *)p_env;

    if (p_env_time->publi_period_ms)
    {
        // Publish a Generic OnOff Status message
        mm_tscns_time_publish(p_env_time);

        // Restart the timer
        mesh_tb_timer_set(&p_env_time->tmr_publi, p_env_time->publi_period_ms);
    }
}

__STATIC void mm_tscns_time_handler_set(mm_tscns_time_env_t *p_env_time, mesh_tb_buf_t *p_buf,
                                        mm_route_buf_env_t *p_route_env, uint16_t opcode)
{
    do
    {
			
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			  
			  time_status_t *p_set = (time_status_t *)MESH_TB_BUF_DATA(p_buf);
			
			  if((((0 == p_set->tai_seconds_rsv) || p_set->time_authority) && is_valid_tai_second(p_set->tai_seconds)))
				{
					  printf("OK ...... \n");
			      memcpy(&p_env_time->mesh_time.time_status, p_data, sizeof(time_status_t));
					

						
					  // Inform application about received value
            mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_TAI_SECONDS,
                                          p_env_time->env.elmt_idx, p_set->tai_seconds, 0);
					  
					  mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_ZONE_OFFSET,
                                          p_env_time->env.elmt_idx, p_set->time_zone_offset, 0);
					
			  }
				
				
				
				if(opcode == MM_MSG_TIME_SET) 
				{
				   mm_tscns_time_send_status(p_env_time, p_route_env, false);
				}
				
		{	
        uint32_t   tai;		
        int32_t    temp;			
				mesh_utc_t utc_now = {0};
				
				get_utc(p_env_time->mesh_time.time_status.tai_seconds, &utc_now);
				
				printf("year = %d \n",  utc_now.year);
				printf("month = %d \n", utc_now.month);
				printf("day = %d \n",   utc_now.day);
				printf("hour = %d \n",  utc_now.hour);
				printf("minute = %d \n",  utc_now.minute);
				printf("second = %d \n",  utc_now.second);
				printf("week = %d \n",  utc_now.week);
				
				tai = p_env_time->mesh_time.time_status.tai_seconds;
				tai += p_env_time->mesh_time.time_status.tai_utc_delta;
				temp = p_env_time->mesh_time.time_status.time_zone_offset * 15 * 60;
				tai += temp;
				get_utc(tai, &utc_now);
				
				printf("year = %d \n",  utc_now.year);
				printf("month = %d \n", utc_now.month);
				printf("day = %d \n",   utc_now.day);
				printf("hour = %d \n",  utc_now.hour);
				printf("minute = %d \n",  utc_now.minute);
				printf("second = %d \n",  utc_now.second);
				printf("week = %d \n",  utc_now.week);
				
		}		
				
			
    } while (0);
}



__STATIC void mm_tscns_time_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{

    mm_tscns_time_env_t *p_env_time;
	
	  printf("mm_tscns_time_cb_rx  0x%x \n", p_route_env->opcode);
   	
	  if(p_env->mdl_id == MM_ID_TSCNS_TIM)
		{
			  p_env_time = (mm_tscns_time_env_t *)p_env;
			
			  switch(p_route_env->opcode)
				{
						 
						 case MM_MSG_TIME_STATUS:
						 {
							  mm_tscns_time_handler_set(p_env_time, p_buf, p_route_env, p_route_env->opcode);
						 }break;
						 case MM_MSG_TIME_GET:
						 {
							 mm_tscns_time_send_status(p_env_time, p_route_env, false);
						 }break;
						 case MM_MSG_ZONE_GET:
						 case MM_MSG_TAI_UTC_DELTA_GET:
						 default:
							 break;
				}	
		}
		else
		{
			    // Environment for Time Server model
            mm_tscns_times_env_t *p_env_times = (mm_tscns_times_env_t *)p_env;
			
            p_env_time = p_env_times->p_env_time;
			
			     switch(p_route_env->opcode)
					 {
						 case MM_MSG_TIME_SET:
						 {
							  mm_tscns_time_handler_set(p_env_time, p_buf, p_route_env, p_route_env->opcode);
						 }break;
						 
						 case MM_MSG_ZONE_SET:
						 case MM_MSG_TAI_UTC_DELTA_SET:
						 case MM_MSG_TIME_ROLS_SET:
						 case MM_MSG_TIME_ROLE_GET:
						 default:
							 break;
					 }
			
		}

}

__STATIC void mm_tscns_time_cb_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                            uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}

__STATIC uint16_t mm_tscns_time_cb_set(mm_tb_state_mdl_env_t *p_env, uint16_t state_id, uint32_t state)
{
    // Returned status
    uint16_t status = MESH_ERR_NO_ERROR;
	
    // Get environment for the Time Server model
    mm_tscns_time_env_t *p_env_time = (mm_tscns_time_env_t *)p_env;
	
	  printf("mm_tscns_time_cb_set  \n");

    switch (state_id)
    {
//        case (MM_STATE_TSCNS_TAI_SECONDS):
//        {
//					  uint32_t g_time = state;
//					  printf("g_time = %d \n", g_time);
//        }break;

        default:
        {
            status = MESH_ERR_INVALID_PARAM;
        } break;
    }

    return (status);
}



uint16_t mm_tscns_time_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
	  // Model local index
    m_lid_t mdl_lid;
	
    // Register the model
    uint16_t status = m_api_register_model(MM_ID_TSCNS_TIM, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_TIM, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(mm_tscns_time_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            mm_tscns_time_env_t *p_env_time = (mm_tscns_time_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_time->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_time->tmr_publi.cb = mm_tscns_time_cb_tmr_publi;
            p_env_time->tmr_publi.p_env = (void *)p_env_time;

            // Prepare environment for Replay Manager
            p_env_time->replay_env.delay_ms = MM_TIME_REPLAY_MS;

            // Set internal callback functions
            p_env_time->env.cb.cb_rx = mm_tscns_time_cb_rx;
            p_env_time->env.cb.cb_opcode_check = mm_tscns_time_cb_opcode_check;
            p_env_time->env.cb.cb_publish_param = mm_tscns_time_cb_publish_param;
            p_cb_srv->cb_set = mm_tscns_time_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(MM_ID_TSCNS_TIM, elmt_idx, *p_mdl_lid);
					
					  // Register Time Setup Server model
            status = m_api_register_model(MM_ID_TSCNS_TIMS, elmt_idx, 0, &mm_route_cb, &mdl_lid);

            if (status != MESH_ERR_NO_ERROR)
            {
                break;
            }
						
						// Inform the Model State Manager about registered model
           status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_TIMS, mdl_lid,
                                      MM_TB_STATE_ROLE_SRV, sizeof(mm_tscns_times_env_t));

           if (status == MESH_ERR_NO_ERROR)
           {
               // Get allocated environment
               mm_tscns_times_env_t *p_env_times = (mm_tscns_times_env_t *)mm_tb_state_get_env(mdl_lid);

               // Set internal callback functions
               p_env_times->env.cb.cb_rx = mm_tscns_time_cb_rx;
               p_env_times->env.cb.cb_opcode_check = mm_tscns_time_cb_opcode_check;

               // Link environment
               p_env_times->p_env_time = p_env_time;

               // Inform application about registered model
               mm_api_send_register_ind(MM_ID_TSCNS_TIMS, elmt_idx, mdl_lid);
           }
				
						
						
        }
    } while (0);

    return (status);
}


void mm_tscns_time_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info)
{
    // Get environment for Time Server model
    mm_tscns_time_env_t *p_env_time = (mm_tscns_time_env_t *)mm_tb_state_get_env(mdl_lid);

    switch (event)
    {
        case (MESH_MDL_GRP_EVENT_TRANS_REJECTED):
        {
            // Send a response to the node that has required the transition
           // mm_gens_oo_check_status_rsp(p_env_oo);
        } break;

        case (MESH_MDL_GRP_EVENT_TRANS_DELAY_EXPIRED):
        {
            // Start the transition
           // mm_tb_bind_trans_start(p_env_oo->env.grp_lid);
        } break;

        case (MESH_MDL_GRP_EVENT_TRANS_IMMEDIATE):
        {
           // p_env_oo->onoff = p_env_oo->tgt_onoff;
        } // no break;

        default:
        {
        } break;
    }
}



