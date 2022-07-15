/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	led_button.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   Led and button service implementation
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/

#include "rwip_config.h"
#include "led_button.h"
#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "co_utils.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "co_utils.h"
#include "prf.h"


#if defined(CFG_PRF_LED_BUTTON)

#define LED_BUTTON_UUID_BASE					{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x15, 0x23, 0x00, 0x00}
#define LED_BUTTON_UUID_BUTTON_CHAR		{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x15, 0x24, 0x00, 0x00}
#define LED_BUTTON_UUID_LED_CHAR			{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x15, 0x25, 0x00, 0x00}

const struct attm_desc_128 led_button_att_db[LED_BUTTON_IDX_NB] =
{
     [LED_BUTTON_IDX_SVC] = {{0x00, 0x28}, PERM(RD, ENABLE), 0, 0},		

		   //0x1524
     [LED_BUTTON_IDX_BUTTON_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE), 0, 0},
     //按键Notify
		 [LED_BUTTON_IDX_BUTTON_VAL] = {LED_BUTTON_UUID_BUTTON_CHAR, PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0}, 

     [LED_BUTTON_IDX_BUTTON_IND_CFG] = {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},
		 
			   //0x1525
     [LED_BUTTON_IDX_LED_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE), 0, 0},
      //Led 可以写控制
		 [LED_BUTTON_IDX_LED_VAL] = {LED_BUTTON_UUID_LED_CHAR, PERM(WRITE_REQ, ENABLE)|PERM(RD, ENABLE),PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), 1*sizeof(uint8_t)},            //Led  写只能接收一个字节
		
//		 [LED_BUTTON_IDX_LED_IND_CFG] = {{0x02, 0x29}, 
//                                     PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 
//                                     0, 0},
		
      
};

		
static uint16_t led_button_compute_att_table(uint16_t features);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t led_button_init(struct prf_task_env *env,
                             uint16_t *start_hdl,
                             uint16_t app_task,
                             uint8_t sec_lvl,
                             struct led_button_db_cfg *params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    cfg_flag = led_button_compute_att_table(params->features);


    uint8_t led_button_uuid[16] = LED_BUTTON_UUID_BASE;
	
    status = attm_svc_create_db_128(start_hdl, led_button_uuid, (uint8_t *)&cfg_flag,
                                    LED_BUTTON_IDX_NB, NULL, env->task,
                                    &led_button_att_db[0], sec_lvl);


    if (status == ATT_ERR_NO_ERROR)
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct led_button_env_tag *led_button_env = (struct led_button_env_tag *)ke_malloc(sizeof(struct led_button_env_tag), KE_MEM_ATT_DB);

        // allocate PROXR required environment variable
        env->env = (prf_env_t *)led_button_env;

        led_button_env->shdl = *start_hdl;
        led_button_env->prf_env.app_task = app_task | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        led_button_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id = TASK_ID_LED_BUTTON;
        led_button_task_init(&(env->desc));

        //Save features on the environment
        led_button_env->features = params->features;
        led_button_env->operation = NULL;
        memset(led_button_env->ntf_ind_cfg, 0, sizeof(led_button_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, LED_BUTTON_IDLE);
    }
    return (status);
}


static void led_button_destroy(struct prf_task_env *env)
{
    struct led_button_env_tag *led_button_env = (struct led_button_env_tag *)env->env;

    // free profile environment variables
    if (led_button_env->operation != NULL)
    {
        ke_free(led_button_env->operation);
    }

    env->env = NULL;
    ke_free(led_button_env);
}

static void led_button_create(struct prf_task_env *env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct led_button_env_tag *led_button_env = (struct led_button_env_tag *)env->env;
    led_button_env->ntf_ind_cfg[conidx] = 0;
}

static void led_button_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct led_button_env_tag *led_button_env = (struct led_button_env_tag *)env->env;
    led_button_env->ntf_ind_cfg[conidx] = 0;
}



static uint16_t led_button_compute_att_table(uint16_t features)
{
    //LED Button Measurement Characteristic is mandatory
    uint16_t att_table = 0x7f;
    return att_table;
}

/// LED BUTTON Task interface required by profile manager
const struct prf_task_cbs led_button_itf =
    {
        (prf_init_fnct)led_button_init,
        led_button_destroy,
        led_button_create,
        led_button_cleanup,
};
		
const struct prf_task_cbs* led_button_prf_itf_get(void)
{
   return &led_button_itf;
}


//////////////////////////////////////////////////////

void led_button_exe_operation(void)
{
    struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);

    bool finished = true;

    while(led_button_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        if(((led_button_env->ntf_ind_cfg[led_button_env->operation->cursor] & led_button_env->operation->op) != 0)
            // and event not filtered on current connection
            && (led_button_env->operation->conidx != led_button_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd * evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC , led_button_env->operation->cursor), prf_src_task_get(&led_button_env->prf_env, 0),
                    gattc_send_evt_cmd, led_button_env->operation->length);

            evt->operation = (led_button_env->operation->op != LED_BUTTON_CFG_BUTTON_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
            evt->length    = led_button_env->operation->length;
            evt->handle    = led_button_env->operation->handle;
            memcpy(evt->value, led_button_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            led_button_env->operation->cursor++;
            break;
        }
        led_button_env->operation->cursor++;
    }


    // check if operation is finished
    if(finished)
    {
        // free operation
        ke_free(led_button_env->operation);
        led_button_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(led_button_env->prf_env), 0), LED_BUTTON_IDLE);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t led_button_att_hdl_get(struct led_button_env_tag *led_button_env, uint8_t att_idx)
{
    uint16_t handle = led_button_env->shdl;

    do
    {
        if (att_idx >= LED_BUTTON_IDX_NB)
            return ATT_INVALID_HDL;

        handle += att_idx;
    } while (0);

    // check if handle found
    return handle;
}


uint8_t led_button_att_idx_get(struct led_button_env_tag *led_button_env, uint16_t handle)
{
    uint16_t handle_ref = led_button_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if (handle < handle_ref)
        {
            break;
        }

        att_idx = (handle - handle_ref) + LED_BUTTON_IDX_SVC;

        if (att_idx >= LED_BUTTON_IDX_NB)
            return ATT_INVALID_IDX;

        return att_idx;

    } while (0);

    return att_idx;
}


uint8_t led_button_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);
    uint8_t status = GAP_ERR_NO_ERROR;

    if((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;
    }
	
    if (value == valid_val)
    {
        led_button_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        led_button_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        // inform application that notification/indication configuration has changed
        struct led_button_cfg_indntf_ind * ind = KE_MSG_ALLOC(LED_BUTTON_CFG_INDNTF_IND,
                prf_dst_task_get(&led_button_env->prf_env, conidx), prf_src_task_get(&led_button_env->prf_env, conidx),
                led_button_cfg_indntf_ind);
        ind->conidx      = conidx;
        ind->ntf_ind_cfg = led_button_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
    }

    return (status);
}


#endif
