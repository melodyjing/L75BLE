/**@file            app_ble_wechat.c
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/

#include "rwip_config.h" // SW configuration

#if(APP_WECHAT_ENABLE == 1)

#include "app.h"      // Application Definitions
#include "app_task.h" // application task definitions
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h" // Platform Definitions
#include "prf.h"
#include "co_utils.h"
#include "ble_wechat_service.h"
#include "ble_wechat_task.h"
#include "app_ble_wechat.h"
#include <string.h>

struct app_wechat_env_tag app_wechat_env;

void app_wechat_init(void)
{
    // Reset the environment
    memset(&app_wechat_env, 0, sizeof(app_wechat_env));

}

void app_wechat_disconnection(void)
{
    // Reset the environment
    memset(&app_wechat_env, 0, sizeof(app_wechat_env));
	  mpbledemo2_reset();
}


void app_wechat_add_service(void)
{
    struct wechat_db_cfg *db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM, TASK_APP,
                                                             gapm_profile_task_add_cmd, sizeof(struct wechat_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH); //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_WECHAT;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct wechat_db_cfg *)req->param;
    // All features are supported
    db_cfg->features = WECHAT_ALL_FEAT_SUP;

//    db_cfg->meas_intv = app_uart_env.uart_meas_intv;

    // Send the message
    ke_msg_send(req);
}


void app_wechat_send_data(unsigned char *p_buffer, unsigned char length)
{
	  if(app_wechat_env.ind_enable)
		{
       // Allocate the message
       struct wechat_data_send_req *req = KE_MSG_ALLOC(WECHAT_DATA_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_WECHAT),
                                                  TASK_APP,
                                                  wechat_data_send_req);

//	   printf("mtu = %d \n", gattc_get_mtu(app_env.conidx));
			 
       memcpy(req->tx_data, p_buffer, ((length > BLE_WECHAT_MAX_DATA_LEN)?BLE_WECHAT_MAX_DATA_LEN:length));

       req->length = ((length > BLE_WECHAT_MAX_DATA_LEN)?BLE_WECHAT_MAX_DATA_LEN:length);

       // Send the message
       ke_msg_send(req);
	}
}



static int app_wechat_msg_handler(ke_msg_id_t const msgid,
                                void const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}


static int app_wechat_rx_handler(ke_msg_id_t const msgid,
                               struct wechat_rx_data const *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id)
{
    // Do nothing
//    memcpy(app_uart_env.receive_data, param->rx_data, param->length);
//    app_uart_env.receive_length = param->length;
//    profile_uart_write(app_uart_env.receive_data, app_uart_env.receive_length, NULL, NULL);
    //  app_m3_gpio_toggle_pin(GPIO_PIN_11);

#if 0
	  unsigned int i;
	  printf("len[%d]\n", param->length);
	  for(i = 0; i < param->length; i++)
	  {
			  printf("[0x%02x] ", param->rx_data[i]);
			  if((i+1)%8 == 0)
			  	printf("\n");
		}
	  printf("\n");
#endif		

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
//	app_mosquito_killing_bt_data_receive(param->rx_data, param->length);
	app_mosquito_killing_bt_data_assemble(param->rx_data, param->length);

#else
	app_wechat_process_rx_data(param->rx_data, param->length);
#endif		

    return (KE_MSG_CONSUMED);
}

static int app_wechat_ind_handler(ke_msg_id_t const msgid,
                                       struct wechat_cfg_indntf_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{

    if (param->ntf_ind_cfg)
    {
			  printf("listen begin ...\n");
			  app_wechat_env.ind_enable = 1;
			  
			  app_wechat_enable_ind();
        //profile_uart_read(app_uart_env.rx_data, 16, (profile_callback)&send_to_phone, &app_uart_env);
    }
    else
    {
			 printf("listen end ...\n");
			 app_wechat_env.ind_enable = 0;
			
			 app_wechat_disbale_ind();
			
      //  profile_uart_read(app_uart_env.rx_data, 16, NULL, &app_uart_env);
    }
    return (KE_MSG_CONSUMED);
}


static int app_wechat_send_rsp_handler(ke_msg_id_t const msgid,
                                      struct wechat_data_send_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}



/// Default State handlers definition
const struct ke_msg_handler app_wechat_msg_handler_list[] =
{
        // Note: first message is latest message checked by kernel so default is put on top.
         {KE_MSG_DEFAULT_HANDLER, (ke_msg_func_t)app_wechat_msg_handler},

         {WECHAT_DATA_SEND_RSP, (ke_msg_func_t)app_wechat_send_rsp_handler},

         {WECHAT_CFG_INDNTF_IND, (ke_msg_func_t)app_wechat_ind_handler},

         {WECHAT_RX_DATA_IND, (ke_msg_func_t)app_wechat_rx_handler},
};

const struct app_subtask_handlers app_wechat_handlers = APP_HANDLERS(app_wechat);

#endif
