/******************************************************************************
	File Name	:	host_advertiser1.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/8

	Description :   

	Changed Log	:
		Liujian  2019/12/8			- Creation

*****************************************************************************/
#include "host_advertiser1.h"
#include "rdw_mcu.h"
#include "cobra_uart.h"
#if (APP_HOST2_ENABLE && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)
#include "app_host2_uart.h"
#include "cobra4_common.h"
#include "device1_scan.h"

extern const struct ke_task_desc TASK_DESC_DEVICE1;

struct adv_env_tag  adv_device1_env;


static unsigned int collection_1_ready = 0;

static struct bd_addr collection_address1 = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};   //采集器1 地址

void app_collection_1_ready(void)
{
		collection_1_ready = 1;
}

void app_collection_1_not_ready(void)
{
		collection_1_ready = 0;
}

unsigned int app_collection_1_is_ready(void)
{
	
	return collection_1_ready;
}


void app_iris_collection1_address(struct bd_addr * p_collection_address)
{
		memcpy(collection_address1.addr, p_collection_address->addr, BD_ADDR_LEN);	
}

//比较地址是否是采集器1 的地址
unsigned int app_device_compare_collection1_address(unsigned char * source_addr)
{
	if(!memcmp(source_addr, collection_address1.addr, BD_ADDR_LEN))
	{
			return 1;
	}
	return 0;
}

void app_get_collection1_address(unsigned char * p_buffer)
{		
	memcpy(p_buffer, collection_address1.addr, BD_ADDR_LEN);	
}




ke_task_id_t test_if_is_host2(uint8_t host_act_id)
{
		if(host_act_id == adv_device1_env.adv_actv_idx)
			return TASK_ADV_DEVICE1;
		else
			return APP_MAIN_TASK;
}


/*
 建立一个 TASK, 来广播信号 

*/
void app_host2_init(void)
{
		memset(&adv_device1_env, 0, sizeof(adv_device1_env));

		adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_IDLE;
	// Create Device1 task
    ke_task_create(TASK_ADV_DEVICE1, &TASK_DESC_DEVICE1);
	
	
		ke_state_set(TASK_ADV_DEVICE1, APP_DEVICE1_INIT);
	
	 // Get default Device Name (No name if not enough space)
    memcpy(adv_device1_env.dev_name, DEVICE_NAME, DEVICE_NAME_SIZE);
    adv_device1_env.dev_name_len = DEVICE_NAME_SIZE;
	
		app_host2_uart_init();
}


/*
  APP TASK 让第二个广播开始工作
*/
void app_host2_begin_work(void)
{
		struct gapm_gen_rand_nb_cmd *cmd = KE_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
                                                                TASK_GAPM, TASK_ADV_DEVICE1,
                                                                gapm_gen_rand_nb_cmd);
		cmd->operation   = GAPM_GEN_RAND_NB;
		adv_device1_env.rand_cnt = 1;
		ke_msg_send(cmd);
}



//************************************** 处理广播消息 ************************
static void app_host2_create_advertising(void)
{
		if (adv_device1_env.adv_state == APP_DEVICE1_ADV_STATE_IDLE)
    {
        // Prepare the GAPM_ACTIVITY_CREATE_CMD message
        struct gapm_activity_create_adv_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                                                                  TASK_GAPM, TASK_ADV_DEVICE1,
                                                                  gapm_activity_create_adv_cmd);

        // Set operation code
        p_cmd->operation = GAPM_CREATE_ADV_ACTIVITY;
			
//传统广播
				p_cmd->own_addr_type = GAPM_STATIC_ADDR;
			  p_cmd->adv_param.type = GAPM_ADV_TYPE_LEGACY;
        p_cmd->adv_param.prop = GAPM_ADV_PROP_UNDIR_CONN_MASK;
        p_cmd->adv_param.filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY;
        p_cmd->adv_param.prim_cfg.chnl_map = APP_ADV_CHMAP;
        p_cmd->adv_param.prim_cfg.phy = GAP_PHY_LE_1MBPS; 

				p_cmd->adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC;
				
#if(DIANBIAO_SLAVE_PARAM_SET_ENABE == 1)
       unsigned short temp_period = app_slave_device_get_connection_interval();
       if(temp_period != 0)
			 {
				  p_cmd->adv_param.prim_cfg.adv_intv_min = temp_period/1.25;
          p_cmd->adv_param.prim_cfg.adv_intv_max = temp_period/1.25;
			 }
			 else
			 {
				  p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
          p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;
			 }
#else				
        p_cmd->adv_param.prim_cfg.adv_intv_min = APP_ADV_INT_MIN;
        p_cmd->adv_param.prim_cfg.adv_intv_max = APP_ADV_INT_MAX;
#endif				
				// Send the message
        ke_msg_send(p_cmd);

        // Keep the current operation
        adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_CREATING;
        // And the next expected operation code for the command completed event
        adv_device1_env.adv_op = GAPM_CREATE_ADV_ACTIVITY;
				
	  }
}



//设置广播数据
static void app_host2_set_adv_data(void)
{
		// Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_GAPM, TASK_ADV_DEVICE1,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = adv_device1_env.adv_actv_idx;     //这个 adv_act_idx 来自GAPM 的分配
	
		p_cmd->length = 0;
    // GAP will use 3 bytes for the AD Type
    appm_build_adv_data(ADV_DATA_LEN - 3, &p_cmd->length, &p_cmd->data[0]);
		
		// Send the message
    ke_msg_send(p_cmd);

    // Update advertising state
    adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_SETTING_ADV_DATA;
    // And the next expected operation code for the command completed event
    adv_device1_env.adv_op = GAPM_SET_ADV_DATA;
		
}

static void app_host2_set_scan_rsp_data(void)
{
		// Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_GAPM, TASK_ADV_DEVICE1,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_SCAN_RSP_DATA;
    p_cmd->actv_idx = adv_device1_env.adv_actv_idx;
	
		p_cmd->length = APP_SCNRSP_DATA_LEN;
    memcpy(&p_cmd->data[0], APP_SCNRSP_DATA, APP_SCNRSP_DATA_LEN);
		
		// Send the message
    ke_msg_send(p_cmd);

    // Update advertising state
    adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_SETTING_SCAN_RSP_DATA;
    // And the next expected operation code for the command completed event
    adv_device1_env.adv_op = GAPM_SET_SCAN_RSP_DATA;
		
}


static void app_host2_start_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
                                                         TASK_GAPM, TASK_ADV_DEVICE1,
                                                         gapm_activity_start_cmd);
    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = adv_device1_env.adv_actv_idx;
		
	  printf("device activ =%d \n", p_cmd->actv_idx);
		
		p_cmd->u_param.adv_add_param.duration = 0;
		p_cmd->u_param.adv_add_param.max_adv_evt = 0;

    // Send the message
    ke_msg_send(p_cmd);

    // Keep the current operation
    adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_STARTING;   //正在广播
    // And the next expected operation code for the command completed event
    adv_device1_env.adv_op = GAPM_START_ACTIVITY;
		
}

void app_device1_stop_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_GAPM, TASK_ADV_DEVICE1,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = adv_device1_env.adv_actv_idx;

    // Send the message
    ke_msg_send(cmd);

    // Update advertising state
    adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_STOPPING;
    // And the next expected operation code for the command completed event
    adv_device1_env.adv_op = GAPM_STOP_ACTIVITY;
}


void app_device1_delete_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_delete_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
                                                              TASK_GAPM, TASK_ADV_DEVICE1,
                                                              gapm_activity_delete_cmd);

    // Set operation code
    p_cmd->operation = GAPM_DELETE_ALL_ACTIVITIES;

    // Send the message
    ke_msg_send(p_cmd);

    // Keep the current operation
    // And the next expected operation code for the command completed event
    adv_device1_env.adv_op = GAPM_DELETE_ALL_ACTIVITIES;
}

//作为电表, 需要有两个广播在进行
void app_device1_adv_fsm_next(void)
{
		switch (adv_device1_env.adv_state)
    {
				case (APP_DEVICE1_ADV_STATE_IDLE):              /////// step 1
        {
            // Create advertising
            app_host2_create_advertising();
        } 
				break;
				
				case (APP_DEVICE1_ADV_STATE_CREATING):         /////// step 2
        {
            // Set advertising data
            app_host2_set_adv_data();
        } 
				break;
				case (APP_DEVICE1_ADV_STATE_SETTING_ADV_DATA):    ///// step 3
        {
            // Set scan response data
            app_host2_set_scan_rsp_data();
        } break;
				
				case (APP_DEVICE1_ADV_STATE_CREATED):                   //// 设备停止连接, 再一次广播
        case (APP_DEVICE1_ADV_STATE_SETTING_SCAN_RSP_DATA):     ///step 4
        {
#if(DIANBIAO_ADV_ONLY_ONE_ENABLE == 1)						
					  if(app_env.adv_ok == 0)
#endif							
						{
              // Start advertising activity
              app_host2_start_advertising();                 ///
						}
        } break;
				
				case (APP_DEVICE1_ADV_STATE_STARTING):                ///step 5
        {
            // Go to started state
            adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_STARTED;   //不再发消息给 GAPM TASK
					  adv_device1_env.adv_ok = 1;
            printf("APP_DEVICE1_ADV_STATE_STARTING ................. \n");
        } break;
				
				case (APP_DEVICE1_ADV_STATE_STARTED):
        {
            // Stop advertising activity
            app_device1_stop_advertising();
        } break;

				
				case (APP_DEVICE1_ADV_STATE_STOPPING):
        {
 
            // Go created state
            adv_device1_env.adv_state = APP_DEVICE1_ADV_STATE_CREATED;
						//app_device1_start_advertising();    //liujian add 2019/11/21
        } break;
				
				default:
					break;
				
		}
	
	
}

void host2_appm_update_param(struct gapc_conn_param *conn_param, ke_task_id_t app_task)
{

    struct gapc_param_update_cmd *cmd = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, adv_device1_env.conidx), app_task,
                                                     gapc_param_update_cmd);
    cmd->operation  = GAPC_UPDATE_PARAMS;
    cmd->intv_min   = conn_param->intv_min;
    cmd->intv_max   = conn_param->intv_max;
    cmd->latency    = conn_param->latency;
    cmd->time_out   = conn_param->time_out;
		
	
    // not used by a slave device
    cmd->ce_len_min = 0xFFFF;
    cmd->ce_len_max = 0xFFFF;

	
    // Send the message；
    ke_msg_send(cmd);

	
}


void app_collection1_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, adv_device1_env.conidx), TASK_ADV_DEVICE1,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}



#endif


