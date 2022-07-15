/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	File Name	:	  martis_client.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/7/13
	Description :   
	    微断网关
	Changed Log	:
	Liujian  2021/7/13		- Creation
*****************************************************************************/
#include "martis_client.h"

#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions
#include "mm_api_int.h"     // Mesh Model Application Programming Interface Internal Definitions
#include <stdio.h>

/// Structure for martis Client model environment
typedef struct mm_martis_client_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
} mm_martis_clinet_env_t;

/*
LENGTH + DATA (CAN DATA)
*/
/*
微断发送 过来的 数据
*/
__STATIC void mm_martis_client_cb_rx(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                               mm_route_buf_env_t *p_route_env)
{
    if (p_route_env->opcode == MARTIS_VENDOR_MESSAGE_STATE)
    {
        // Get pointer to data
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf);
			
			  uint8_t data_lenght = p_buf->data_len;
			
			  printf("rx: \n");
			
			  for(int i = 0; i < p_buf->data_len; i++)
			  {
					 printf("0x%x ", *(p_data + i));
				}
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_CLIENT_LIGHT == 1))				
				app_micro_gateway_process_rx_data(p_data, data_lenght);
#endif
				
        // Current state
//        uint8_t cur_state = *(p_data + MM_GEN_OO_STATUS_OO_POS);
//        // Target state
//        uint8_t tgt_state = 0;
//        // Remaining time
//        uint8_t rem_time = 0;

//        // Read optional parameters if presents
//        if (p_buf->data_len == MM_GEN_OO_STATUS_LEN)
//        {
//            tgt_state = *(p_data + MM_GEN_OO_STATUS_TGT_OO_POS);
//            rem_time = *(p_data + MM_GEN_OO_STATUS_REM_TIME_POS);
//        }

//        // Inform the application about the received Generic OnOff state value
//        mm_api_send_cli_state_ind(p_route_env->u_addr.src, MM_STATE_MARTIS_VENDOR_RX_DATA, cur_state,
//                              tgt_state, mm_tb_get_trans_time_ms(rem_time));
    }
}

__STATIC uint16_t mm_martis_client_cb_opcode_check(mm_tb_state_mdl_env_t *p_env, uint32_t opcode)
{
    uint16_t status;
	
	//  printf("opcode = 0x%x \n", opcode);

    if (opcode == MARTIS_VENDOR_MESSAGE_STATE)
    {
        status = MESH_ERR_NO_ERROR;
    }
    else
    {
        status = MESH_ERR_MDL_INVALID_OPCODE;
    }

    return (status);
}

/*
  用于向断路器单点发送数据， 请求得到对方的响应
*/
__STATIC uint16_t mm_martis_client_cb_get(mm_tb_state_mdl_env_t *p_env, m_lid_t app_key_lid, uint16_t dst,
                                    uint16_t get_info)
{
    // Pointer to the buffer that will contain the message
    mesh_tb_buf_t *p_buf_get;
    // Allocate a new buffer for the message
    uint16_t status = mm_route_buf_alloc(&p_buf_get, 0);

    if (status == MESH_ERR_NO_ERROR)
    {
        // Get pointer to environment
        mm_route_buf_env_t *p_buf_env = (mm_route_buf_env_t *)&p_buf_get->env;

        // Prepare environment
        p_buf_env->app_key_lid = app_key_lid;
        p_buf_env->u_addr.dst = dst;
        p_buf_env->info = 0;
        p_buf_env->mdl_lid = p_env->mdl_lid;
        p_buf_env->opcode = MARTIS_VENDOR_MESSAGE_GET;

        // Send the message
        mm_route_send(p_buf_get);
    }

    return (status);
}

__STATIC uint16_t mm_martis_client_cb_transition(mm_tb_state_mdl_env_t *p_env, m_lid_t app_key_lid, uint16_t dst,
                                           uint32_t state_1, uint32_t state_2,
                                           uint32_t trans_time_ms, uint16_t delay_ms,
                                           uint16_t trans_info)
{
    // Get message length
    uint8_t length = MARTIS_VENDOR_SEND_LENGTH;  //(GETB(trans_info, MM_TRANS_INFO_LONG) || trans_time_ms || delay_ms)
                                                 //   ? MM_GEN_OO_SET_LEN : MM_GEN_OO_SET_MIN_LEN;
    // Pointer to the buffer that will contain the message
    mesh_tb_buf_t *p_buf_set;
    // Allocate a new buffer
    uint8_t status = mm_route_buf_alloc(&p_buf_set, length);

    if (status == MESH_ERR_NO_ERROR)
    {
        // Get pointer to data
        uint8_t *p_data = MESH_TB_BUF_DATA(p_buf_set);
        // Get pointer to environment
        mm_route_buf_env_t *p_buf_env = (mm_route_buf_env_t *)&p_buf_set->env;
        // Generic OnOff state value
        uint8_t onoff = state_1;

        // Prepare environment
        p_buf_env->app_key_lid = app_key_lid;
        p_buf_env->u_addr.dst = dst;
        p_buf_env->info = 0;
        p_buf_env->mdl_lid = p_env->mdl_lid;
        p_buf_env->opcode = MARTIS_VENDOR_MESSAGE_SET;
			
        // Fill the message
        *(p_data + 0) = length - 1;
			  
			  for(int i = 0; i < length - 1; i++)
			  {
            *(p_data + 1 + i) = 0x55;
				}

        // Send the message
        mm_route_send(p_buf_set);
    }

    return (status);
}


//////////////////////////////////////////////////////////////
uint16_t mm_martis_client_register(void)
{
    // Model local index
    m_lid_t mdl_lid;
    // Register the model
    uint16_t status = m_api_register_model(MARTIS_VENDOR_CLIENT_ID, 0, M_MDL_CONFIG_PUBLI_AUTH_BIT,
                                           &mm_route_cb, &mdl_lid);

    if (status == MESH_ERR_NO_ERROR)
    {
        // Inform the Model State Manager about registered model
        status = mm_tb_state_register(0, MARTIS_VENDOR_CLIENT_ID, mdl_lid,
                                      MM_TB_STATE_ROLE_CLI | MM_TB_STATE_CFG_CB_BIT,
                                      sizeof(mm_martis_clinet_env_t));

        if (status == MESH_ERR_NO_ERROR)
        {
            // Get allocated environment
            mm_martis_clinet_env_t *p_env_martis_client = (mm_martis_clinet_env_t *)mm_tb_state_get_env(mdl_lid);
            // Get client-specific callback functions
            mm_cli_cb_t *p_cb_cli = p_env_martis_client->env.cb.u.p_cb_cli;

            // Set internal callback functions
            p_env_martis_client->env.cb.cb_rx = mm_martis_client_cb_rx;
            p_env_martis_client->env.cb.cb_opcode_check = mm_martis_client_cb_opcode_check;
            p_cb_cli->cb_get = mm_martis_client_cb_get;
            p_cb_cli->cb_transition = mm_martis_client_cb_transition;

            // Inform application about registered model
            mm_api_send_register_ind(MARTIS_VENDOR_CLIENT_ID, 0, mdl_lid);
        }
    }

    return (status);
}

uint16_t mm_api_martis_client_register(void)
{
	 uint16_t status = MESH_ERR_NO_ERROR;
	 if (mm_tb_state_get_lid(0, MARTIS_VENDOR_CLIENT_ID) == MESH_INVALID_LID)
   {
       // Register the Martis Client model
        status = mm_martis_client_register();
   }
	
}

