

#include "app_host2_uart.h"
#include "app.h"

#if (BLE_HOST2_UART)
static struct app_host2_uart_env_tag app_host_env;
	
extern struct adv_env_tag  adv_device1_env;	
void app_host2_uart_init(void)
{
    // Reset the environment
    memset(&app_host_env, 0, sizeof(app_host_env));

    // Initial measurement interval 
    app_host_env.uart_meas_intv   = 3;	
}

void app_host2_uart_add_uarts(void)
{
    struct device2_uart_db_cfg* db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_ADV_DEVICE1,
                                                  gapm_profile_task_add_cmd, sizeof(struct device2_uart_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_128);  //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_DEVICE2_UART;
    req->app_task = TASK_ADV_DEVICE1;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct device2_uart_db_cfg* ) req->param;
    // All features are supported
    db_cfg->features = DEVICE2_UART_ALL_FEAT_SUP;

    db_cfg->meas_intv = app_host_env.uart_meas_intv;
    // Send the message
    ke_msg_send(req);

}

void app_host2_uart_enable_prf_app(uint8_t conidx)
{
    // Allocate the message
    struct device2_uart_enable_req * req = KE_MSG_ALLOC(DEVICE2_UART_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_DEVICE2_UART),
                                                TASK_ADV_DEVICE1,
                                               device2_uart_enable_req);
		printf("host2 uart conidx = %d \n", conidx);
    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;
    // Send the message
    ke_msg_send(req);		
}

static int app_host2_uart_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing

    return (KE_MSG_CONSUMED);
}

static int app_host2_uart_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                        struct device2_uart_cfg_indntf_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Do nothing
    printf("host2 cfg changed %d \n", param->ntf_ind_cfg);
	  if(param->ntf_ind_cfg)
		{
		}
		else
		{
		}
    return (KE_MSG_CONSUMED);
}


void app_meter_send_data_to_collection1(unsigned char length, unsigned char *p_buffer)
{
	

    struct device2_uart_data_send_req * req = KE_MSG_ALLOC(DEVICE2_UART_TEMP_SEND_REQ,
                                                    prf_get_task_from_id(TASK_ID_DEVICE2_UART),
                                                    TASK_ADV_DEVICE1,
                                                    device2_uart_data_send_req);

		req->length = length;
		memcpy(req->tx_data, p_buffer, req->length);
		
    ke_msg_send(req);
	
}


extern void app_iris_up_collection_data(unsigned char connection_index, unsigned char length, unsigned char *p_buffer);


static int app_host2_uart_rx_handler(ke_msg_id_t const msgid,
                                        struct device2_uart_rx_data const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
		unsigned int i;
    // Do nothing
		memcpy(app_host_env.receive_data, param->rx_data, param->length);
		app_host_env.receive_length = param->length;
	
//		app_iris_up_collection_data(adv_device1_env.conidx, param->length, param->rx_data);
	
		printf("host2222 ...... \n");
		for(i = 0; i < param->length; i++)
		{
				printf("0x%02x, ", param->rx_data[i]);
		}	
		printf("\n");
	   
	//	app_test_led_change();
    return (KE_MSG_CONSUMED);
}


/// Default State handlers definition
const struct ke_msg_handler app_host2_uart_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_host2_uart_msg_handler},

//    {DEVICE1_UART_TEMP_SEND_RSP,            (ke_msg_func_t)uart_temp_send_rsp_handler},
		
    {DEVICE2_UART_CFG_INDNTF_IND,           (ke_msg_func_t)app_host2_uart_cfg_indntf_ind_handler},

//    {APP_DEVICE1_UART_MEAS_INTV_TIMER,        (ke_msg_func_t)app_uart_meas_intv_timer_handler},
		
		{DEVICE2_UART_RX_DATA_IND,              (ke_msg_func_t)app_host2_uart_rx_handler},  
};

const struct app_subtask_handlers app_host2_uart_handlers = APP_HANDLERS(app_host2_uart);


#endif
