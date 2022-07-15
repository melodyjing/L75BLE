/******************************************************************************
	File Name	:	device2_client_uart.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/28

	Description :   

	Changed Log	:
		Liujian  2019/11/28			- Creation

*****************************************************************************/
#if (BLE_DEVICE2_UART_CLIENT_ENABLE && APP_SCAN_DEVICE_ENABLE)

#include "rwip_config.h"
#include "device2_client_uart.h"
#include "device2_client_uart_task.h"
#include "gap.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "rdw_mcu.h"

static uint8_t  device2_uart_client_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
		uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct device2_uart_client_env_tag* device2_uart_env =
            (struct device2_uart_client_env_tag* ) ke_malloc(sizeof(struct device2_uart_client_env_tag), KE_MEM_ATT_DB);

    // allocate BLPC required environment variable
    env->env = (prf_env_t*) device2_uart_env;

    device2_uart_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    device2_uart_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_DEVICE2_COLLECTOR_UART;
    device2_connector_uart_task_init(&(env->desc));

    for(idx = 0; idx < UART_COUNT_IDX_MAX ; idx++)
    {
        device2_uart_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), DEVICE2_UART_FREE);
    }

    return GAP_ERR_NO_ERROR;
	
}

static void device2_uart_client_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct device2_uart_client_env_tag* device2_uart_env = (struct device2_uart_client_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(device2_uart_env->env[conidx] != NULL)
    {
        ke_free(device2_uart_env->env[conidx]);
        device2_uart_env->env[conidx] = NULL;
    }

		
		printf("device2_uart_client_cleanup conidx = %d \n", conidx);
    /* Put UART Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), DEVICE2_UART_FREE);
}


static void device2_uart_client_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct device2_uart_client_env_tag* device2_uart_env = (struct device2_uart_client_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < UART_COUNT_IDX_MAX ; idx++)
    {
        device2_uart_client_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(device2_uart_env);
}

	
static void device2_uart_client_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put BLP Client in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), DEVICE2_UART_FREE);
}


const struct prf_task_cbs device2_uart_collector_itf =
{
        device2_uart_client_init,
        device2_uart_client_destroy,
        device2_uart_client_create,
        device2_uart_client_cleanup,
};



const struct prf_task_cbs* device2_uart_collector_prf_itf_get(void)
{
   return &device2_uart_collector_itf;
}




void device2_uart_client_enable_rsp_send(struct device2_uart_client_env_tag *device_uart_env, uint8_t conidx, uint8_t status)
{
    // Send to APP the details of the discovered attributes on UART server
    struct device2_client_uart_enable_rsp * rsp = KE_MSG_ALLOC(
            DEVICE2_CLIENT_ENABLE_RSP,
            prf_dst_task_get(&(device_uart_env->prf_env), conidx),
            prf_src_task_get(&(device_uart_env->prf_env), conidx),
            device2_client_uart_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bps = device_uart_env->env[conidx]->bps;

        prf_register_atthdl2gatt(&(device_uart_env->prf_env), conidx, &(device_uart_env->env[conidx]->bps.svc));

        // Go to connected state
        ke_state_set(prf_src_task_get(&(device_uart_env->prf_env), conidx), DEVICE2_UART_IDLE);
    }

    ke_msg_send(rsp);
}






#endif

