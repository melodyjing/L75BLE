/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_scene.c
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/7

	Description :   


	Changed Log	:

		Liujian  2021/2/7	- Creation

*****************************************************************************/

#include "mm_tscns_scene.h"
#include <stdio.h>

#define   MAX_SCENE_COUNT  16
typedef struct {
	 
	 uint8_t    statues_code;
	 
	 uint16_t   current_scene_index;
	
	 uint16_t   target_scene_index;
	
	 uint8_t    remaining_time;
		
}scene_status_t;


typedef struct {
	 
	 uint8_t    statues_code;
	 
	 uint16_t   current_scene;       //当前场景的 index
	
	 uint16_t   scene[MAX_SCENE_COUNT];
		
}scene_reg_status_t;


typedef struct mm_tscns_scene_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;

 
	  scene_reg_status_t  reg_scene_status;
	
    /// Source address of set message that has triggered last or current transition
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of Schdule Status

    m_lid_t status_app_key_lid;
	
    /// Relaying of sent Time Status authorized
    bool status_relay;
	
}mm_tscns_scene_env_t;


/// Structure for Scene Setup Server model environment
typedef struct mm_tscns_scenes_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
    /// Pointer to environment of associated Scene Server model
    mm_tscns_scene_env_t *p_env_scene;
	
} mm_tscns_scenes_env_t;

uint8_t mesh_cmd_sig_scene_set_ll(mm_tscns_scene_env_t *p_env_scene, uint16_t scene_id, uint16_t op_code);


__STATIC uint16_t mm_tscns_scene_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
	
	  printf("mm_tscns_scene_cb_opcode_check  0x%x .... \n", opcode);
	
	  uint16_t status = MESH_ERR_MDL_INVALID_OPCODE;

    if (p_env->mdl_id == MM_ID_TSCNS_SCN)
    {
        if ((opcode == MM_MSG_SCENE_GET)
					       || (opcode == MM_MSG_SCENE_REG_GET)
				         || (opcode == MM_MSG_SCENE_RECALL)
                 || (opcode == MM_MSG_SCENE_RECALL_NOACK))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }
    else if (p_env->mdl_id == MM_ID_TSCNS_SCNS)    //Setup
    {
        if ((opcode == MM_MSG_SCENE_STORE)
					      || (opcode == MM_MSG_SCENE_STORE_NOACK)
				        || (opcode == MM_MSG_SCENE_DEL)
                || (opcode == MM_MSG_SCENE_DEL_NOACK))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }

    return (status);
}

__STATIC void mm_tscns_scene_send_status(mm_tscns_scene_env_t *p_env_scene, mm_route_buf_env_t *p_route_env,
                                     bool publish, uint16_t target_scene_index)
{
    // Pointer to the buffer that will contain the message
    mesh_tb_buf_t *p_buf_status;
    // Remaining time
    uint8_t rem_time;
    // Transition type
    uint8_t trans_type;
    // Data length
    uint8_t data_length;

	  printf("mm_tscns_scene_send_status  \n");
	
	  scene_status_t  rsp;
	
    // Check if a transition has been started
    mm_tb_bind_get_trans_info(p_env_scene->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
    data_length = sizeof(scene_status_t);

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
            p_buf_env->app_key_lid = p_env_scene->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_scene->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_scene->status_relay);

            // Can accept new transitions
            p_env_scene->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_scene->env.mdl_lid;
        p_buf_env->opcode = MM_MSG_SCENE_STATUS;
				
				rsp.current_scene_index = p_env_scene->reg_scene_status.current_scene;
				
				rsp.statues_code = SCENE_ST_SUCCESS;
				
				rsp.target_scene_index = target_scene_index;
				
				rsp.remaining_time = 1;
				
				memcpy(p_data, &rsp, sizeof(scene_status_t));
				
        // Send the message
        mm_route_send(p_buf_status);
    }
}

__STATIC void mm_tscns_scene_send_reg_status(mm_tscns_scene_env_t *p_env_scene, mm_route_buf_env_t *p_route_env,
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
	
	  uint8_t  i;
	
	  uint8_t cnt;

	  printf("mm_tscns_scene_send_reg_status  \n");
	
    // Check if a transition has been started
    mm_tb_bind_get_trans_info(p_env_scene->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
	  cnt = 0;
	  for(i = 0; i < MAX_SCENE_COUNT; i++)
	  {
			  if(p_env_scene->reg_scene_status.scene[i])
				{
					 cnt++;
				}
		}
    data_length = offsetof(scene_reg_status_t, scene) + cnt*2;

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
            p_buf_env->app_key_lid = p_env_scene->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_scene->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_scene->status_relay);

            // Can accept new transitions
            p_env_scene->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_scene->env.mdl_lid;
        p_buf_env->opcode = MM_MSG_SCENE_REG_STATUS;
				
				memcpy(p_data, &p_env_scene->reg_scene_status, data_length);
				
        // Send the message
        mm_route_send(p_buf_status);
    }
}


__STATIC void mm_tscns_scene_publish(mm_tscns_scene_env_t *p_env_scene)
{
	  printf("mm_tscns_scene_publish .... \n");
    // Check if sending of publication is enabled
    if (GETB(p_env_scene->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
    //    mm_tscns_schdule_send_status(p_env_schdule, NULL, true);
    }
}



__STATIC void mm_tscns_scene_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    mm_tscns_scene_env_t *p_env_scene = (mm_tscns_scene_env_t *)p_env;

    if (p_env_scene->publi_period_ms)
    {
        
        mm_tscns_scene_publish(p_env_scene);

        // Restart the timer
        mesh_tb_timer_set(&p_env_scene->tmr_publi, p_env_scene->publi_period_ms);
    }
}

/////////////////////////////////////////////////////////////
//Scene set
__STATIC void mm_tscns_scene_action_handler_scene_set(mm_tscns_scene_env_t *p_env_scene, mesh_tb_buf_t *p_buf,
                                        mm_route_buf_env_t *p_route_env, uint16_t opcode)
{
    do
    {
			
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			
			  uint16_t  scene_id = p_data[0] + (p_data[1] << 8);
			
			  printf("scene_id = %d \n", scene_id);
			
			  if(SCENE_ID_INVALID == scene_id)
				{
    	     return ;
        }
			
		//	 p_env_scene->reg_scene_status.current_scene_index = scene_id;
				
			 p_env_scene->reg_scene_status.statues_code = mesh_cmd_sig_scene_set_ll(p_env_scene, scene_id, opcode);
			
      				
			 if(opcode == MM_MSG_SCENE_STORE)
			 {				 
		         mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_SCENE_SAVE,
                                          p_env_scene->env.elmt_idx, scene_id, 0);
				
	     }
			 
       if((opcode == MM_MSG_SCENE_STORE) || (opcode == MM_MSG_SCENE_DEL))
			 {
			    mm_tscns_scene_send_reg_status(p_env_scene, p_route_env, false);		
			 }
			 
    } while (0);
		
}


__STATIC void mm_tscns_scene_action_handler_scene_recall(mm_tscns_scene_env_t *p_env_scene, mesh_tb_buf_t *p_buf,
                                        mm_route_buf_env_t *p_route_env, uint16_t opcode)
{
    do
    {
			  //scene_status_t   rsp;
			
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			
			  uint16_t  scene_id = p_data[0] + (p_data[1] << 8);
			
			  printf("recall scene_id = %d \n", scene_id);
			
			  if(SCENE_ID_INVALID == scene_id)
				{
    	     return ;
        }
			
//			 p_env_scene->scene_status.current_scene_index = p_env_scene->reg_scene_status.current_scene;
//				
//			 p_env_scene->scene_status.statues_code = SCENE_ST_SUCCESS;
//			
//       p_env_scene->scene_status.target_scene_index = scene_id;		
//				
//			 p_env_scene->scene_status.remaining_time = 5;  //500ms
				
			 if(opcode == MM_MSG_SCENE_RECALL)
			 {
			    mm_tscns_scene_send_status(p_env_scene, p_route_env, false, scene_id);		
			 }
			 
			 if(opcode == MM_MSG_SCENE_RECALL)
			 {				 
		         mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_SCENE_RECALL,
                                          p_env_scene->env.elmt_idx, scene_id, 0);
				
	     }
			 
       p_env_scene->reg_scene_status.current_scene = scene_id;
			 
    } while (0);
		
}


__STATIC void mm_tscns_scene_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{
	  mm_tscns_scene_env_t *p_env_scene;
	
	  printf("mm_tscns_scene_cb_rx  0x%x \n", p_route_env->opcode);
	
	  if(p_env->mdl_id == MM_ID_TSCNS_SCN)
		{
			  p_env_scene = (mm_tscns_scene_env_t *)p_env;
			
			  switch(p_route_env->opcode)
				{
						 case MM_MSG_SCENE_GET:
						 {
							  
						 }break;
						 case MM_MSG_SCENE_REG_GET:
						 {
							 mm_tscns_scene_send_reg_status(p_env_scene, p_route_env, false);		
						 }break;
						 case MM_MSG_SCENE_RECALL:
						 case MM_MSG_SCENE_RECALL_NOACK:
						 {
							  mm_tscns_scene_action_handler_scene_recall(p_env_scene, p_buf, p_route_env, p_route_env->opcode);
						 }
						 default:
							 break;
				}	
		}
		else
		{
			    // Environment for Scene Server setup model
            mm_tscns_scenes_env_t *p_env_scenes = (mm_tscns_scenes_env_t *)p_env;
			
            p_env_scene = p_env_scenes->p_env_scene;
			
			     switch(p_route_env->opcode)
					 {
						 case MM_MSG_SCENE_STORE:
						 case MM_MSG_SCENE_STORE_NOACK:
						 case MM_MSG_SCENE_DEL:
						 case MM_MSG_SCENE_DEL_NOACK:
							 mm_tscns_scene_action_handler_scene_set(p_env_scene, p_buf, p_route_env, p_route_env->opcode);
						 default:
							 break;
					 }
			
		}
		
}


__STATIC void mm_tscns_scene_cb_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                            uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}


__STATIC uint16_t mm_tscns_scene_cb_set(mm_tb_state_mdl_env_t *p_env, uint16_t state_id, uint32_t state)
{
	
	   // Returned status
    uint16_t status = MESH_ERR_NO_ERROR;
	
    // Get environment for the Scene Server model
    mm_tscns_scene_env_t *p_env_schdule = (mm_tscns_scene_env_t *)p_env;
	
	  printf("mm_tscns_scene_cb_set  \n");

//    switch (state_id)
//    {
//        case (MM_STATE_TSCNS_TAI_SECONDS):
//        {
//					  uint32_t g_time = state;
//					  //printf("g_time = %d \n", g_time);
//					  mesh_scheduler_proc(p_env, g_time);
//        }break;

//        default:
//        {
//            status = MESH_ERR_INVALID_PARAM;
//        } break;
//    }

    return (status);
}



uint16_t mm_tscns_scene_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
	 	  // Model local index
    m_lid_t mdl_lid;
	
    // Register the model
    uint16_t status = m_api_register_model(MM_ID_TSCNS_SCN, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_SCN, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(mm_tscns_scene_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            mm_tscns_scene_env_t *p_env_scene = (mm_tscns_scene_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_scene->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_scene->tmr_publi.cb = mm_tscns_scene_cb_tmr_publi;
            p_env_scene->tmr_publi.p_env = (void *)p_env_scene;

            // Prepare environment for Replay Manager
            p_env_scene->replay_env.delay_ms = MM_SCENE_REPLAY_MS;

            // Set internal callback functions
            p_env_scene->env.cb.cb_rx = mm_tscns_scene_cb_rx;
            p_env_scene->env.cb.cb_opcode_check = mm_tscns_scene_cb_opcode_check;
            p_env_scene->env.cb.cb_publish_param = mm_tscns_scene_cb_publish_param;
            p_cb_srv->cb_set = mm_tscns_scene_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(MM_ID_TSCNS_SCN, elmt_idx, *p_mdl_lid);
					
					  // Register Time Setup Server model
            status = m_api_register_model(MM_ID_TSCNS_SCNS, elmt_idx, 0, &mm_route_cb, &mdl_lid);

            if (status != MESH_ERR_NO_ERROR)
            {
                break;
            }
						
						// Inform the Model State Manager about registered model
           status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_SCNS, mdl_lid,
                                      MM_TB_STATE_ROLE_SRV, sizeof(mm_tscns_scenes_env_t));

           if (status == MESH_ERR_NO_ERROR)
           {
               // Get allocated environment
               mm_tscns_scenes_env_t *p_env_scenes = (mm_tscns_scenes_env_t *)mm_tb_state_get_env(mdl_lid);

               // Set internal callback functions
               p_env_scenes->env.cb.cb_rx = mm_tscns_scene_cb_rx;
               p_env_scenes->env.cb.cb_opcode_check = mm_tscns_scene_cb_opcode_check;

               // Link environment
               p_env_scenes->p_env_scene = p_env_scene;

               // Inform application about registered model
               mm_api_send_register_ind(MM_ID_TSCNS_SCNS, elmt_idx, mdl_lid);
           }
										
        }
				
    } while (0);

    return (status);
	
}


void mm_tscns_scene_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info)
{
	
}


uint8_t mesh_cmd_sig_scene_set_ll(mm_tscns_scene_env_t *p_env_scene, uint16_t scene_id, uint16_t op_code)
{
    uint8_t st = SCENE_ST_SUCCESS;
	
	  int  i;
	
    int  save = 0;
	
	  if( (op_code == MM_MSG_SCENE_STORE) ||  (op_code == MM_MSG_SCENE_STORE_NOACK))
		{
	         for(i = 0; i < MAX_SCENE_COUNT; i++)
           {
                 if(p_env_scene->reg_scene_status.scene[i] == scene_id)
				         {
					           // p_env_scene->reg_scene_status.scene[i] = scene_id;
					           break;
				         }
				         else if(p_env_scene->reg_scene_status.scene[i] == 0)
				         {
					           p_env_scene->reg_scene_status.scene[i] = scene_id;
					           break;
				         }
	         }	

           if(i >= MAX_SCENE_COUNT)
		       {
                st = SCENE_ST_REG_FULL;
		       }		
	   }		
	  
     return st;
}


