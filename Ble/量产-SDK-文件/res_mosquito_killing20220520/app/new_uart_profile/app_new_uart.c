/**@file            app_new_uart.c
* @brief            
* @details          
* @author      			
* @date             2018-04-03
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/04/03  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/



#include "rwip_config.h" // SW configuration

#if (BLE_APP_UART == 1)

#include "app_new_uart.h"
#include "app.h"      // Application Definitions
#include "app_task.h" // application task definitions
#include "uart_new_task.h"
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h" // Platform Definitions
#include "prf.h"

#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "cobra_uart.h"
#include "rdw_gpio.h"
#include <string.h>

#if (UART_MODEL_TIMEOUT_ENABLE == 1)
#include "app_uart_module.h"
#endif


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#if (COBRA_POWER_DOWN_ENABLE)

extern void uart_profile_print(void);
extern uint8_t disallowsleep;
struct app_uart_env_tag app_uart_env __attribute__((section("retention_area"),zero_init)) ; 
#else
struct app_uart_env_tag app_uart_env;
#endif

typedef void (*profile_callback)(void *, uint8_t);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void app_uart_temp_send(void)
{
#if 1

    int32_t value = 0x11;

    struct uart_data_send_req *req = KE_MSG_ALLOC(UART_TEMP_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_UART),
                                                  TASK_APP,
                                                  uart_data_send_req);

    req->length = 10;
    memset(req->tx_data, 0x11, req->length);

    ke_msg_send(req);
#endif
}

#if (UART_MODEL_TIMEOUT_ENABLE == 0)

static void send_to_phone(struct app_uart_env_tag *env, uint8_t status)
{
    // Allocate the message
    struct uart_data_send_req *req = KE_MSG_ALLOC(UART_TEMP_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_UART),
                                                  TASK_APP,
                                                  uart_data_send_req);

    memcpy(req->tx_data, env->rx_data, MAX_UART_SIZE);

    req->length = MAX_UART_SIZE;

    // Send the message
    ke_msg_send(req);

    profile_uart_read(app_uart_env.rx_data, MAX_UART_SIZE, (profile_callback)&send_to_phone, &app_uart_env);
}

#endif

#if (UART_MODEL_TIMEOUT_ENABLE == 1)
void uart_send_to_phone(uint8_t *buffer, uint16_t length)
{
    // Allocate the message
    struct uart_data_send_req *req = KE_MSG_ALLOC(UART_TEMP_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_UART),
                                                  TASK_APP,
                                                  uart_data_send_req);

    memcpy(req->tx_data, buffer, length);

    req->length = length;

    // Send the message
    ke_msg_send(req);

}


#endif

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_uart_init(void)
{
    // Reset the environment
    memset(&app_uart_env, 0, sizeof(app_uart_env));

    // Initial measurement interval
    app_uart_env.uart_meas_intv = 3;
	
#if (UART_MODEL_TIMEOUT_ENABLE == 1)	
	  app_uart_module_timeout_init();  
#endif
	
}

void app_uart_stop_timer(void)
{
    // Stop the timer used for the measurement interval if enabled
    if (app_uart_env.timer_enable)
    {
        app_uart_env.timer_enable = false;
    }
		
		#if (UART_MODEL_TIMEOUT_ENABLE == 0)
    profile_uart_clear();
		#else
		app_uart_module_process_uart_rx_enable(0);
		#endif
}

void app_uart_add_uarts(void)
{
    struct uart_db_cfg *db_cfg;
    // Allocate the HTPT_CREATE_DB_REQ
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM, TASK_APP,
                                                             gapm_profile_task_add_cmd, sizeof(struct uart_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH) | PERM(SVC_UUID_LEN, UUID_128); //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_UART;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct uart_db_cfg *)req->param;
    // All features are supported
    db_cfg->features = UART_ALL_FEAT_SUP;

    db_cfg->meas_intv = app_uart_env.uart_meas_intv;

    // Send the message
    ke_msg_send(req);
}

void app_uart_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct uart_enable_req *req = KE_MSG_ALLOC(UART_ENABLE_REQ,
                                               prf_get_task_from_id(TASK_ID_UART),
                                               TASK_APP,
                                               uart_enable_req);

    // Fill in the parameter structure
    req->conidx = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg = PRF_CLI_STOP_NTFIND;

    // Send the message
    ke_msg_send(req);
}

/****************************************************************************************
 * MESSAGE HANDLERS
 ****************************************************************************************/

static int uart_temp_send_rsp_handler(ke_msg_id_t const msgid,
                                      struct uart_send_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

static int uart_cfg_indntf_ind_handler(ke_msg_id_t const msgid,
                                       struct uart_cfg_indntf_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{

    if (param->ntf_ind_cfg)
    {
			  #if (UART_MODEL_TIMEOUT_ENABLE == 0)
        profile_uart_read(app_uart_env.rx_data, 16, (profile_callback)&send_to_phone, &app_uart_env);
			  #else
			  app_uart_module_process_uart_rx_init();
			  app_uart_module_process_uart_rx_enable(1);
			  #endif
    }
    else
    {
			  #if (UART_MODEL_TIMEOUT_ENABLE == 0)
        profile_uart_read(app_uart_env.rx_data, 16, NULL, &app_uart_env);
			  #else
			  app_uart_module_process_uart_rx_enable(0);
			  #endif
    }
    return (KE_MSG_CONSUMED);
}



static int app_uart_msg_handler(ke_msg_id_t const msgid,
                                void const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

#if COBRA_POWER_DOWN_ENABLE
void print_callback(void *parm,uint8_t parm1)
{
	rwip_prevent_sleep_clear(RW_PLF_DEEP_SLEEP_DISABLED);
}
#endif
#if (UART_MSG_HANDLE_DWT)	
static void respond_to_client(uint8_t *param,uint8_t length)
{
    // Allocate the message
    struct uart_data_send_req *req = KE_MSG_ALLOC(UART_TEMP_SEND_REQ,
                                                  prf_get_task_from_id(TASK_ID_UART),
                                                  TASK_APP,
                                                  uart_data_send_req);

    memcpy(req->tx_data, param, length);//MAX_UART_SIZE

    req->length = length;//MAX_UART_SIZE;

    // Send the message
    ke_msg_send(req);
}
void uart_msg_handle_for_DWT(struct uart_rx_data const *param)
{
	char prefix_command1[]="DWT:GET_STATUS";
	char prefix_command2[]="DWT:GET_ADC";
	char prefix_error2[]="DWT:ERROR!";
	char prefix_respond1[8]="DWT:OK!";
	char prefix_respond2[17]="DWT:RET_ADC=";
	if(strcmp(prefix_command2,param->rx_data)==0)
	{
		#if(APP_ADC_SAMPLE_COMMON_ENABLE)
		prefix_respond2[12]= get_adc_value()/1000+0x30;
		prefix_respond2[13]= get_adc_value()/100%10+0x30;
		prefix_respond2[14]= get_adc_value()/10%10+0x30;
		prefix_respond2[15]= get_adc_value()%10+0x30;
		prefix_respond2[16]= '\0';
		respond_to_client(prefix_respond2,17);
		#else
		prefix_respond2[10]= '\0';
		respond_to_client(prefix_error2,11);
		#endif
	}
	else if(strcmp(prefix_command1,param->rx_data)==0)
	{
		prefix_respond2[7]= '\0';
		respond_to_client(prefix_respond1,8);
	}
}
#endif
static int app_uart_rx_handler(ke_msg_id_t const msgid,
                               struct uart_rx_data const *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id)
{
#if (UART_MODEL_TIMEOUT_ENABLE == 0)	
    // Do nothing
    memcpy(app_uart_env.receive_data, param->rx_data, param->length);	
    app_uart_env.receive_length = param->length;
#endif
	
#if COBRA_POWER_DOWN_ENABLE	
    rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
    profile_uart_write(app_uart_env.receive_data, app_uart_env.receive_length, print_callback, NULL);
#else
	  #if (UART_MODEL_TIMEOUT_ENABLE == 0)
    profile_uart_write(app_uart_env.receive_data, app_uart_env.receive_length, NULL, NULL);
	  #else
	  for(int i = 0; i < param->length; i++)
    {
        cobra_uart_write(M3_UART0, param->rx_data[i]);
    }			
	  #endif
#endif
	
#if (UART_MSG_HANDLE_DWT)	
    uart_msg_handle_for_DWT(param);
			
#endif

    return (KE_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_uart_msg_handler_list[] =
    {
        // Note: first message is latest message checked by kernel so default is put on top.
        {KE_MSG_DEFAULT_HANDLER, (ke_msg_func_t)app_uart_msg_handler},

        {UART_TEMP_SEND_RSP, (ke_msg_func_t)uart_temp_send_rsp_handler},

        {UART_CFG_INDNTF_IND, (ke_msg_func_t)uart_cfg_indntf_ind_handler},

        //    {APP_UART_MEAS_INTV_TIMER,        (ke_msg_func_t)app_uart_meas_intv_timer_handler},

        {UART_RX_DATA_IND, (ke_msg_func_t)app_uart_rx_handler},
};

const struct app_subtask_handlers app_uart_handlers = APP_HANDLERS(app_uart);
#endif
