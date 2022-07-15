/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_schdule.c
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/2

	Description :   


	Changed Log	:

		Liujian  2021/2/2	- Creation

*****************************************************************************/

#include "mm_tscns_schdule.h"
#include "mm_tscns_util.h"
#include <stdio.h>

void mesh_scheduler_proc(mm_tb_state_mdl_env_t *p_env, uint32_t seconds);

typedef struct{
	uint64_t valid_flag_or_idx	: 4;	// flag: when save;  index: in message
	uint64_t year		      : 7;
	uint64_t month		    : 12;
	uint64_t day			    : 5;
	uint64_t hour		      : 5;
	uint64_t minute		    : 6;
	uint64_t second		    : 6;
	uint64_t week         : 7;    // bit0 means monday,
	uint64_t action		    : 4;
	uint64_t trans_t	    : 8;    // transition time
	uint16_t scene_id;
	uint8_t rand_hour;
	uint8_t rand_min;
	uint8_t rand_sec;
	uint8_t rsv;
}scheduler_t;

typedef struct mm_tscns_schdule_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;


    scheduler_t  schdule[16];
	  uint8_t      schdule_flag[16];
	
    /// Source address of set message that has triggered last or current transition
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of Schdule Status

    m_lid_t status_app_key_lid;
	
    /// Relaying of sent Time Status authorized
    bool status_relay;
	
}mm_tscns_schdule_env_t;


/// Structure for Schdule Setup Server model environment
typedef struct mm_tscns_schdules_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
    /// Pointer to environment of associated Schdule Server model
    mm_tscns_schdule_env_t *p_env_schdule;
	
} mm_tscns_schdules_env_t;


__STATIC uint16_t mm_tscns_schdule_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
	
	  printf("mm_tscns_schdule_cb_opcode_check  0x%x .... \n", opcode);
	
	  uint16_t status = MESH_ERR_MDL_INVALID_OPCODE;

    if (p_env->mdl_id == MM_ID_TSCNS_SCH)
    {
        if ((opcode == MM_MSG_SHCDULE_GET)
                || (opcode == MM_MSG_SHCDULE_ACTION_GET))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }
    else if (p_env->mdl_id == MM_ID_TSCNS_SCHS)    //Setup
    {
        if ((opcode == MM_MSG_SHCDULE_ACTION_SET)
                || (opcode == MM_MSG_SHCDULE_ACTION_SET_NOACK))
        {
            status = MESH_ERR_NO_ERROR;
        }
    }

    return (status);
}


__STATIC void mm_tscns_schdule_send_status(mm_tscns_schdule_env_t *p_env_schdule, mm_route_buf_env_t *p_route_env,
                                     bool publish, uint8_t schd_idx)
{
	      // Pointer to the buffer that will contain the message
    mesh_tb_buf_t *p_buf_status;
    // Remaining time
    uint8_t rem_time;
    // Transition type
    uint8_t trans_type;
    // Data length
    uint8_t data_length;


	//  printf("mm_tscns_schdule_send_status  \n");
	
    // Check if a transition has been started
    mm_tb_bind_get_trans_info(p_env_schdule->env.grp_lid, &trans_type, &rem_time);

    // Deduce deduce data length
    data_length = offsetof(scheduler_t, rand_hour);

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
            p_buf_env->app_key_lid = p_env_schdule->status_app_key_lid;
            p_buf_env->u_addr.dst = p_env_schdule->status_dst_addr;
            SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RELAY, p_env_schdule->status_relay);

            // Can accept new transitions
            p_env_schdule->status_dst_addr = MESH_UNASSIGNED_ADDR;
        }

        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_RX, 0);
        SETB(p_buf_env->info, MM_ROUTE_BUF_INFO_PUBLISH, publish);
        p_buf_env->mdl_lid = p_env_schdule->env.mdl_lid;
        p_buf_env->opcode = MM_MSG_SHCDULE_ACTION_STATUS;
				
				memcpy(p_data, &p_env_schdule->schdule[schd_idx], offsetof(scheduler_t, rand_hour));
				
        // Send the message
        mm_route_send(p_buf_status);
    }
	
}


__STATIC void mm_tscns_schdule_publish(mm_tscns_schdule_env_t *p_env_schdule)
{
	  printf("mm_tscns_schdule_publish .... \n");
    // Check if sending of publication is enabled
    if (GETB(p_env_schdule->env.info, MM_TB_STATE_MDL_INFO_PUBLI))
    {
    //    mm_tscns_schdule_send_status(p_env_schdule, NULL, true);
    }
}


__STATIC void mm_tscns_schdule_cb_tmr_publi(void *p_env)
{
    // Get allocated environment
    mm_tscns_schdule_env_t *p_env_schdule = (mm_tscns_schdule_env_t *)p_env;

    if (p_env_schdule->publi_period_ms)
    {
        // Publish a Generic OnOff Status message
        mm_tscns_schdule_publish(p_env_schdule);

        // Restart the timer
        mesh_tb_timer_set(&p_env_schdule->tmr_publi, p_env_schdule->publi_period_ms);
    }
}

__STATIC void mm_tscns_schdule_action_handler_set(mm_tscns_schdule_env_t *p_env_schdule, mesh_tb_buf_t *p_buf,
                                        mm_route_buf_env_t *p_route_env)
{
    do
    {
			  uint8_t schd_idx;
			
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			  
			  scheduler_t *p_set = (scheduler_t *)MESH_TB_BUF_DATA(p_buf);
			
			  schd_idx = p_set->valid_flag_or_idx;
			
			  printf("sch idx = %d \n", p_set->valid_flag_or_idx);
			  printf("year = %d \n", p_set->year);
			  printf("month = %d \n", p_set->month);
			  printf("day = %d \n", p_set->day);
			  printf("hour = %d \n", p_set->hour);
			  printf("minute = %d \n", p_set->minute);
			  printf("second = %d \n", p_set->second);
			  printf("week = %d \n", p_set->week);
			  printf("action = %d \n", p_set->action);
			  printf("trans_t = %d \n", p_set->trans_t);
        printf("scene_id = %d \n", p_set->scene_id);
			
			
			  memcpy(&p_env_schdule->schdule[schd_idx], p_set, offsetof(scheduler_t, rand_hour));
				p_env_schdule->schdule_flag[schd_idx] = 1;
							
				mm_tscns_schdule_send_status(p_env_schdule, p_route_env, false, schd_idx);
						
    } while (0);
}



__STATIC void mm_tscns_schdule_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{
	  mm_tscns_schdule_env_t *p_env_schdule;
	
	  printf("mm_tscns_schdule_cb_rx  0x%x \n", p_route_env->opcode);
	
	  if(p_env->mdl_id == MM_ID_TSCNS_SCN)
		{
			  p_env_schdule = (mm_tscns_schdule_env_t *)p_env;
			
			  switch(p_route_env->opcode)
				{
						 case MM_MSG_SHCDULE_GET:
						 case MM_MSG_SHCDULE_ACTION_GET:
						 default:
							 break;
				}	
		}
		else
		{
			    // Environment for Schdule Server model
            mm_tscns_schdules_env_t *p_env_schduls = (mm_tscns_schdules_env_t *)p_env;
			
            p_env_schdule = p_env_schduls->p_env_schdule;
			
			     switch(p_route_env->opcode)
					 {
						 case MM_MSG_SHCDULE_ACTION_SET:
						 {
							  mm_tscns_schdule_action_handler_set(p_env_schdule, p_buf, p_route_env);
						 }
						 case MM_MSG_SHCDULE_ACTION_SET_NOACK:
						 default:
							 break;
					 }
			
		}
		
}



__STATIC void mm_tscns_schdule_cb_publish_param(mm_tb_state_mdl_env_t *p_env, uint16_t addr,
                                            uint32_t period_ms)
{
    // Inform the state manager about received publication parameters
    mm_tb_state_publish_param_ind((mm_tb_state_mdl_publi_env_t *)p_env, addr, period_ms);
}


__STATIC uint16_t mm_tscns_schdule_cb_set(mm_tb_state_mdl_env_t *p_env, uint16_t state_id, uint32_t state)
{
	
	   // Returned status
    uint16_t status = MESH_ERR_NO_ERROR;
	
    // Get environment for the Schdule Server model
    mm_tscns_schdules_env_t *p_env_schdule = (mm_tscns_schdules_env_t *)p_env;
	
	 // printf("mm_tscns_schdule_cb_set  \n");

    switch (state_id)
    {
        case (MM_STATE_TSCNS_TAI_SECONDS):
        {
					  uint32_t g_time = state;
					  //printf("g_time = %d \n", g_time);
					  mesh_scheduler_proc(p_env, g_time);
        }break;

        default:
        {
            status = MESH_ERR_INVALID_PARAM;
        } break;
    }

    return (status);
}

uint16_t mm_tscns_schdule_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid)
{
	 	  // Model local index
    m_lid_t mdl_lid;
	
    // Register the model
    uint16_t status = m_api_register_model(MM_ID_TSCNS_SCH, elmt_idx, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, p_mdl_lid);

    do
    {
        // Check if model has been properly registered
        if (status != MESH_ERR_NO_ERROR)
        {
            break;
        }

        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_SCH, *p_mdl_lid,
                                      MM_TB_STATE_ROLE_SRV | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(mm_tscns_schdule_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            mm_tscns_schdule_env_t *p_env_schdule = (mm_tscns_schdule_env_t *)mm_tb_state_get_env(*p_mdl_lid);
            // Get server-specific callback functions
            mm_srv_cb_t *p_cb_srv = p_env_schdule->env.cb.u.p_cb_srv;

            // Prepare timer for publication
            p_env_schdule->tmr_publi.cb = mm_tscns_schdule_cb_tmr_publi;
            p_env_schdule->tmr_publi.p_env = (void *)p_env_schdule;

            // Prepare environment for Replay Manager
            p_env_schdule->replay_env.delay_ms = MM_SCHDULE_REPLAY_MS;

            // Set internal callback functions
            p_env_schdule->env.cb.cb_rx = mm_tscns_schdule_cb_rx;
            p_env_schdule->env.cb.cb_opcode_check = mm_tscns_schdule_cb_opcode_check;
            p_env_schdule->env.cb.cb_publish_param = mm_tscns_schdule_cb_publish_param;
            p_cb_srv->cb_set = mm_tscns_schdule_cb_set;

            // Inform application about registered model
            mm_api_send_register_ind(MM_ID_TSCNS_SCH, elmt_idx, *p_mdl_lid);
					
					  // Register Time Setup Server model
            status = m_api_register_model(MM_ID_TSCNS_SCHS, elmt_idx, 0, &mm_route_cb, &mdl_lid);

            if (status != MESH_ERR_NO_ERROR)
            {
                break;
            }
						
						// Inform the Model State Manager about registered model
           status = mm_tb_state_register(elmt_idx, MM_ID_TSCNS_SCHS, mdl_lid,
                                      MM_TB_STATE_ROLE_SRV, sizeof(mm_tscns_schdules_env_t));

           if (status == MESH_ERR_NO_ERROR)
           {
               // Get allocated environment
               mm_tscns_schdules_env_t *p_env_schdules = (mm_tscns_schdules_env_t *)mm_tb_state_get_env(mdl_lid);

               // Set internal callback functions
               p_env_schdules->env.cb.cb_rx = mm_tscns_schdule_cb_rx;
               p_env_schdules->env.cb.cb_opcode_check = mm_tscns_schdule_cb_opcode_check;

               // Link environment
               p_env_schdules->p_env_schdule = p_env_schdule;

               // Inform application about registered model
               mm_api_send_register_ind(MM_ID_TSCNS_SCHS, elmt_idx, mdl_lid);
           }
										
        }
				
    } while (0);

    return (status);
	
}


void mm_tscns_schdule_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info)
{
	
}

//////////////////////////////////////////////////////////

int schd_random_rebuild_sec(mm_tscns_schdule_env_t *p_env_schdule);
int schd_random_rebuild_min(mm_tscns_schdule_env_t *p_env_schdule);
int schd_random_rebuild_hour(mm_tscns_schdule_env_t *p_env_schdule);
void rebulid_schd_nearest_and_check_event(mm_tscns_schdule_env_t *p_env_schdule, int rebuild, uint32_t TAI_local);
int update_schd_nearest_TAI_today(mm_tscns_schdule_env_t *p_env_schdule, uint32_t TAI_local);
int schd_event_check(mm_tscns_schdule_env_t *p_env_schdule, uint32_t TAI_local);
static inline void schd_action_cb(mm_tscns_schdule_env_t *p_env_schdule, scheduler_t *p_schd, int ele_idx);
uint32_t get_local_TAI(uint32_t seconds);

static schd_nearest_t g_schd_nearest = {0};

void mesh_scheduler_proc(mm_tb_state_mdl_env_t *p_env, uint32_t seconds)
{
//    uint32_t TAI_local = seconds; //get_local_TAI(seconds);
	
	  uint32_t TAI_local = get_local_TAI(seconds);
	
    mm_tscns_schdule_env_t *p_env_schdule = (mm_tscns_schdule_env_t *)p_env;
	
    int rebuild_nearest = 0;
	
    if(0 == TAI_local % 60)
		{
        rebuild_nearest = schd_random_rebuild_sec(p_env_schdule);
        if(0 == TAI_local % (60*60))
				{
            rebuild_nearest |= schd_random_rebuild_min(p_env_schdule);
            if(0 == TAI_local % (60*60*24))
						{
                schd_random_rebuild_hour(p_env_schdule);
                rebuild_nearest = 1;        // rebuild every day
            }
        }
    }

    rebulid_schd_nearest_and_check_event(p_env_schdule, rebuild_nearest, TAI_local);
}

static inline int is_schd_alive(scheduler_t *p_schd)
{
   // if((0 == p_schd->valid_flag_or_idx) || (SCHD_ACTION_NONE == p_schd->action))
	
	  if(SCHD_ACTION_NONE == p_schd->action)
		{
        return 0;
    }
    return 1;
}


static inline uint8_t get_random_min_or_sec()
{
    return rand() % 60;
}

static inline uint8_t get_random_hour()
{
    return rand() % 24;
}


int schd_random_rebuild_sec(mm_tscns_schdule_env_t *p_env_schdule)
{
    uint32_t update = 0;
	  uint32_t i;
	  scheduler_t *p_schd;
    for(i = 0; i < 16; i++)
	  {
			  p_schd = &p_env_schdule->schdule[i];
        if(is_schd_alive(p_schd))
				{
           if((SCHD_SEC_RANDOM == p_schd->second)&&(SCHD_HOUR_RANDOM != p_schd->hour)&&(SCHD_MIN_RANDOM != p_schd->minute))
					 {
                    update = 1;
                    p_schd->rand_sec = get_random_min_or_sec();
           }
        }
				
    }
    return update;
}

int schd_random_rebuild_min(mm_tscns_schdule_env_t *p_env_schdule)
{
    uint32_t update = 0;
	  uint32_t i;
	  scheduler_t *p_schd;
    for(i = 0; i < 16; i++)
	  {
			      p_schd = &p_env_schdule->schdule[i];
            if(is_schd_alive(p_schd))
						{
                if((SCHD_MIN_RANDOM == p_schd->minute)&&(SCHD_HOUR_RANDOM != p_schd->hour))
								{
                    update = 1;
                    p_schd->rand_min = get_random_min_or_sec();
									
                    if(SCHD_SEC_RANDOM == p_schd->second)
										{
                        p_schd->rand_sec = get_random_min_or_sec();
                    }
                }
            }
    }		 
    return update;
}


int schd_random_rebuild_hour(mm_tscns_schdule_env_t *p_env_schdule)
{
    uint32_t update = 0;
	  uint32_t i;
	  scheduler_t *p_schd;
	
    for(i = 0; i < 16; i++)
	  {
			      p_schd = &p_env_schdule->schdule[i];

            if(is_schd_alive(p_schd) && (p_env_schdule->schdule_flag[i]))
						{
                if(SCHD_HOUR_RANDOM == p_schd->hour)
							  {
                    update = 1;
                    p_schd->rand_hour = get_random_hour();
									
                    if(SCHD_MIN_RANDOM == p_schd->minute)
										{
                        p_schd->rand_min = get_random_min_or_sec();
                    }
										
                    if(SCHD_SEC_RANDOM == p_schd->second)
										{
                        p_schd->rand_sec = get_random_min_or_sec();
                    }
                }
            }

    }
    return update;
		
}

void rebulid_schd_nearest_and_check_event(mm_tscns_schdule_env_t *p_env_schdule, int rebuild, uint32_t TAI_local)
{

  //  if(rebuild)
		{
        update_schd_nearest_TAI_today(p_env_schdule, TAI_local);
    }
    
    schd_event_check(p_env_schdule, TAI_local);  // must after:  get schd nearest TAI_today()
		
//		printf("schdule ......  \n");
		
}

int schd_event_check(mm_tscns_schdule_env_t *p_env_schdule, uint32_t TAI_local)
{
    int act = 0;
	
	  uint32_t i;
	
    if(!g_schd_nearest.TAI_local)
		{
        return 0;
    }

    if(g_schd_nearest.TAI_local == TAI_local)
		{
        act = 1;
			
			  uint32_t entry_bit = g_schd_nearest.entry_bit;
			
        for(i = 0; i < 16; i++)
			  {
              if(entry_bit & BIT(i))
							{
                    entry_bit &= ~BIT(i);
									
                    scheduler_t *p_schd = &p_env_schdule->schdule[i];
									
                    schd_action_cb(p_env_schdule, p_schd, i);
									
                    if(0 == entry_bit)
										{
                        break;
                    }
              }
        }
        
        update_schd_nearest_TAI_today(p_env_schdule, TAI_local + 1);		// check next second
				
    }
		
    return act;
}


uint32_t get_min_sec_step(uint8_t val)
{
    if(val < 60){
        return 0;
    }else if(SCHD_MIN_ANY == val){
        return 1;
    }else if(SCHD_MIN_EVERY15 == val){
        return 15;
    }else if(SCHD_MIN_EVERY20 == val){
        return 20;
    }else{
        return 0;
    }
}

/*
first_flag: get the first match value, when the upper unit is larger.
*/
uint8_t get_nearest_hour(uint8_t hour_schd, uint8_t hour_now, uint8_t hour_rand, int first_flag)
{
    if(hour_schd < 24)
		{
        if(first_flag || (hour_now <= hour_schd))
				{
            return hour_schd;
        }
				else
				{
            return NO_NEAREST;
        }
    }
		else if(SCHD_HOUR_ANY == hour_schd)
		{
        return hour_now;
    }
		else if(SCHD_HOUR_RANDOM == hour_schd)
		{
        if(first_flag || (hour_now <= hour_rand))
				{
            return hour_rand;
        }
				else
				{
            return NO_NEAREST;
        }
    }
		else
	  {
        return NO_NEAREST;
    }
}

/*
first_flag: get the first match value, when the upper unit is larger.
*/
uint8_t get_nearest_min_or_sec(uint8_t val_schd, uint8_t val_now, uint8_t val_rand, int first_flag)
{
	  int i;
    if(val_schd < 60)
		{
        if(first_flag || (val_now <= val_schd))
				{
            return val_schd;
        }
				else
				{
            return NO_NEAREST;
        }
    }
		else if(SCHD_MIN_ANY == val_schd)
		{
        return first_flag ? 0 : val_now;
    }
		else if(SCHD_MIN_RANDOM == val_schd)
		{
        if(first_flag || (val_now <= val_rand))
				{
            return val_rand;
        }
				else
				{
            return NO_NEAREST;
        }
    }
		else if((SCHD_MIN_EVERY15 == val_schd))
		{
        if(first_flag)
				{
            return 0;
        }
				else
				{
            for(i = 0; i < 4; i++ )
					  {
                if(val_now <= i*15)
								{
                    return i*15;
                }
            }
            return NO_NEAREST;
        }
    }
		else if((SCHD_MIN_EVERY20 == val_schd))
		{
        if(first_flag)
				{
            return 0;
        }
				else
				{
            for(i = 0; i < 3; i++)
					  {
                if(val_now <= i*20)
								{
                    return i*20;
                }
            }
            return NO_NEAREST;
        }
    }
		else
		{
        return NO_NEAREST;
    }
}





int update_schd_nearest_TAI_today(mm_tscns_schdule_env_t *p_env_schdule, uint32_t TAI_local)
{
    if(!TAI_local)
		{
        return -1;
    }
    memset(&g_schd_nearest, 0, sizeof(g_schd_nearest)); // init
    
    mesh_utc_t UTC_now = {0};
		
    if(0 != get_utc(TAI_local, &UTC_now))
		{   // action for current TAI
        return 0;
    }
		
	//	printf("%d %d %d %d %d %d \n", UTC_now.year, UTC_now.month, UTC_now.day, UTC_now.hour, UTC_now.minute, UTC_now.second);

    uint32_t days_max = get_days_one_month(UTC_now.year, UTC_now.month);

    uint32_t month_bit = (uint32_t)BIT(UTC_now.month-1);
    uint32_t week_bit = (uint32_t)BIT(UTC_now.week);

    uint32_t nearest_TAI = -1;
		uint32_t i;
		
    for(i = 0; i < 16; i++)
		{
        
            scheduler_t *p_schd = &p_env_schdule->schdule[i];
			
            if((0 == p_env_schdule->schdule_flag[i]) || (SCHD_ACTION_NONE == p_schd->action))
						{
                continue;
            }

            // rebuild nearest_TAI every day in mesh_scheduler_proc_()
            if(((SCHD_YEAR_ANY == p_schd->year)||((UTC_now.year-YEAR_BASE) == p_schd->year))
            && ((SCHD_MONTH_ANY == p_schd->month)||(month_bit & p_schd->month))
            && (((SCHD_DAY_ANY == p_schd->day)&&((SCHD_WEEK_ANY == p_schd->week)||(week_bit & p_schd->week)))
                 ||((UTC_now.day == p_schd->day)||((p_schd->day > days_max) && (UTC_now.day == days_max)))))
						{ // single_day_match
                // today match
            }
						else
						{
                continue;
            }

            uint32_t step_hh = (SCHD_HOUR_ANY == p_schd->hour) ? 1 : 0;
            uint32_t step_mm = get_min_sec_step(p_schd->minute);

            uint8_t nearst_hh = NO_NEAREST, nearst_mm = NO_NEAREST, nearst_ss = NO_NEAREST;

            nearst_hh = get_nearest_hour(p_schd->hour, UTC_now.hour, p_schd->rand_hour, 0);
						
            if(NO_NEAREST == nearst_hh)
						{
                continue;
            }
						else
						{
                int check_next_hour = 0;
                int big_hour = nearst_hh > UTC_now.hour;
                nearst_mm = get_nearest_min_or_sec(p_schd->minute, UTC_now.minute, p_schd->rand_min, big_hour);
							
                if(NO_NEAREST == nearst_mm)
								{
                    check_next_hour = 1;
                }
								else
								{
                    int big_min = big_hour || (nearst_mm > UTC_now.minute);
                    nearst_ss = get_nearest_min_or_sec(p_schd->second, UTC_now.second, p_schd->rand_sec, big_min);
									
                    if(NO_NEAREST == nearst_ss)
										{
                        if(step_mm)
												{
                            nearst_mm += step_mm;
                            if(nearst_mm < 60)
														{
                                nearst_ss = get_nearest_min_or_sec(p_schd->second, 0, p_schd->rand_sec, 1);
                            }
														else
														{
                                check_next_hour = 1;
                            }
                        }
												else
												{
                            check_next_hour = 1;
                        }
                    }
                }
                
                if(check_next_hour && step_hh)
								{
                    nearst_hh += step_hh;
                    if(nearst_hh < 24)
										{
                        nearst_mm = get_nearest_min_or_sec(p_schd->minute, 0, p_schd->rand_min, 1);
                        nearst_ss = get_nearest_min_or_sec(p_schd->second, 0, p_schd->rand_sec, 1);
                    }
                }
            }

            if((nearst_hh < 24)&&(nearst_mm < 60)&&(nearst_ss < 60))
						{
                uint32_t temp = nearst_hh * 3600 + nearst_mm * 60 + nearst_ss;
                if(temp < nearest_TAI)
								{
                    g_schd_nearest.entry_bit = 0;
                    g_schd_nearest.entry_bit |= BIT(i);
                    nearest_TAI = temp;
                }
								else if(temp == nearest_TAI)
								{
                    g_schd_nearest.entry_bit |= BIT(i);
                }
            }
    }
    

    if(nearest_TAI != -1)
		{
        uint32_t temp = (TAI_local / SECOND_ONE_DAY)*SECOND_ONE_DAY;
        nearest_TAI += temp;
        if(TAI_local <= nearest_TAI)
				{
            g_schd_nearest.TAI_local = nearest_TAI;
            return 0;
        }
    }
		
    return -1;
}


uint32_t get_local_TAI(uint32_t seconds)
{
    return (seconds + get_time_zone_offset_min(get_time_zone_offset(8*60))*60);
}

static inline void schd_action_cb(mm_tscns_schdule_env_t *p_env_schdule, scheduler_t *p_schd, int ele_idx)
{
 //   transition_par_t trs_par = {{0}};
 //   trs_par.transit_t = p_schd->trans_t;
	//  printf("p_schd->action = %d \n", p_schd->action);
    switch(p_schd->action){
        case SCHD_ACTION_OFF:
           // access_cmd_onoff(ele_adr_primary, 0, 0, 0, &trs_par);
        //    printf("SCHD_ACTION_OFF \n");
				    mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_ACTION_OFF,
                                          p_env_schdule->env.elmt_idx, SCHD_ACTION_OFF, 0);
				
            break;
				
        case SCHD_ACTION_ON:
        //    printf("SCHD_ACTION_ON \n");
				    mm_api_send_srv_state_upd_ind(MM_STATE_TSCNS_ACTION_ON,
                                          p_env_schdule->env.elmt_idx, SCHD_ACTION_ON, 0);
            break;
				
        case SCHD_ACTION_SCENE:
            #if MD_SCENE_EN
       //     access_cmd_scene_recall(ele_adr_primary, 0, p_schd->scene_id, 0, &trs_par);
			//LOG_MSG_INFO(TL_LOG_COMMON,0,0,"SCHD_ACTION_SCENE", 0);
            #endif
            break;
        default:
            break;
    }

 
//    uint32_t t_hh = p_schd->hour;    // can't use bit-field as parameter in LOG_MSG_INFO()
//    uint32_t t_mm = p_schd->minute;
//    uint32_t t_ss = p_schd->second;
//    
 //   printf("current HH:%2d, MM:%2d, SS:%2d", g_UTC.hour, g_UTC.minute, g_UTC.second);
 //   printf("random  HH:%2d, MM:%2d, SS:%2d", p_schd->rand_hour, p_schd->rand_min, p_schd->rand_sec);
 //   printf("schedul HH:%2d, MM:%2d, SS:%2d", t_hh, t_mm, t_ss);
  
}















