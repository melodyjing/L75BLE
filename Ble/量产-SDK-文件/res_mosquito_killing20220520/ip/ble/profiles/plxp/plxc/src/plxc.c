/**
 ****************************************************************************************
 *
 * @file plxc.c
 *
 * @brief Pulse Oximeter Profile Collector implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup PLXC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_PLX_CLIENT)
#include "prf.h"
#include "plxp_common.h"

#include "plxc_task.h"
#include "plxc.h"
#include "gap.h"

#include "ke_mem.h"

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the PLXC module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t plxc_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
    uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct plxc_env_tag* plxc_env =
            (struct plxc_env_tag* ) ke_malloc(sizeof(struct plxc_env_tag), KE_MEM_ATT_DB);

    // allocate PLXC required environment variable
    env->env = (prf_env_t*) plxc_env;

    plxc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    plxc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_PLXC;
    plxc_task_init(&(env->desc));

    for(idx = 0; idx < PLXC_IDX_MAX ; idx++)
    {
        plxc_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), PLXC_FREE);
    }

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Clean-up connection dedicated environment parameters
 * This function performs cleanup of ongoing operations
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void plxc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct plxc_env_tag* plxc_env = (struct plxc_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(plxc_env->env[conidx] != NULL)
    {
//        if (plxc_env->env[conidx]->operation != NULL)
//        {
//            ke_free(ke_param2msg(plxc_env->env[conidx]->operation));
//        }
        ke_free(plxc_env->env[conidx]);
        plxc_env->env[conidx] = NULL;
        // No other profiles unregister
        //prf_unregister_atthdl2gatt(&(plxc_env->prf_env), conidx, &(plxc_env->env[conidx]->es.svc));
    }

    /* Put WSCP Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), PLXC_FREE);
}

/**
 ****************************************************************************************
 * @brief Destruction of the PLXC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void plxc_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct plxc_env_tag* plxc_env = (struct plxc_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < PLXC_IDX_MAX ; idx++)
    {
        plxc_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(plxc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void plxc_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put WSC Client in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), PLXC_IDLE);
}

/// PLXC Task interface required by profile manager
const struct prf_task_cbs plxc_itf =
{
        plxc_init,
        plxc_destroy,
        plxc_create,
        plxc_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* plxc_prf_itf_get(void)
{
   return &plxc_itf;
}

/**
 ****************************************************************************************
 * @brief Sends a PLXC_ENABLE_RSP messge.
 *
 * @param[in] plxc_env -- the profile environment for the connection
 * @param[in] conidx   -- Connection Identifier
 * @param[in] status   -- indicates the outcome of the operation
 *
 * @return none.
 ****************************************************************************************
 */
void plxc_enable_rsp_send(struct plxc_env_tag *plxc_env, uint8_t conidx, uint8_t status,    
    uint16_t supported_features, uint16_t measurement_status_supported, uint32_t device_status_supported)
{
    // Send to APP the details of the discovered attributes on WSCP
    struct plxc_enable_rsp * rsp = KE_MSG_ALLOC(
            PLXC_ENABLE_RSP,
            prf_dst_task_get(&(plxc_env->prf_env), conidx),
            prf_src_task_get(&(plxc_env->prf_env), conidx),
            plxc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        /// Provide all Discovered Characteristics and Descritors
        rsp->plxc = plxc_env->env[conidx]->plxc;
        /// Supported Features @see common supported_features
        rsp->supported_features = supported_features;
        /// If enabled in  Supported Features @see common measurement_status_supported
        rsp->measurement_status_supported = measurement_status_supported;
        /// If enabled in  Supported Features @see common device_status_supported
        rsp->device_status_supported = device_status_supported;

        // Register PLXC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&(plxc_env->prf_env), conidx, &(plxc_env->env[conidx]->plxc.svc));
        // Go to connected state
        ke_state_set(prf_src_task_get(&(plxc_env->prf_env), conidx), PLXC_IDLE);
    }
    else
    {
        memset(&rsp->plxc, 0 , sizeof(rsp->plxc));
        /// Provide all Discovered Characteristics and Descritors
        rsp->plxc = plxc_env->env[conidx]->plxc;
        // Enabled failed - so go back to the FREE state
        ke_state_set(prf_src_task_get(&(plxc_env->prf_env), conidx), PLXC_FREE);
    }

    ke_msg_send(rsp);
}

/**
 ****************************************************************************************
 * @brief Sends a Complete_Event to the App
 *
 * @param[in] plxc_env -- the profile environment
 * @param[in] conidx    -- Connection Identifier
 * @param[in] operation -- Indicates the operation for which the cmp_evt is being sent.
 * @param[in] status -- indicates the outcome of the operation
 *
 * @return Error_Code.
 ****************************************************************************************
 */

void plxc_send_cmp_evt(struct plxc_env_tag *plxc_env, uint8_t conidx, uint8_t operation, uint8_t status)
{
//    /// Free the stored operation if needed
//    if (plxc_env->env[conidx]->operation != NULL)
//    {
//        ke_msg_free(ke_param2msg(plxc_env->env[conidx]->operation));
//        plxc_env->env[conidx]->operation = NULL;
//        plxc_env->env[conidx]->op_pending = 0;
//    }

    // Go back to the CONNECTED state if the state is busy
    if (ke_state_get(prf_src_task_get(&(plxc_env->prf_env), conidx)) == PLXC_BUSY)
    {
        ke_state_set(prf_src_task_get(&(plxc_env->prf_env), conidx), PLXC_IDLE);
    }

    // Send the message
    struct plxc_cmp_evt *evt = KE_MSG_ALLOC(PLXC_CMP_EVT,
                                             prf_dst_task_get(&(plxc_env->prf_env), conidx),
                                             prf_src_task_get(&(plxc_env->prf_env), conidx),
                                             plxc_cmp_evt);

    evt->operation  = operation;
    evt->status     = status;

    ke_msg_send(evt);
}

#endif //(BLE_PLX_CLIENT)

/// @} WSC_CLIENT
