/**
 ****************************************************************************************
 *
 * @file cgmc.c
 *
 * @brief Continuous Glucose Monitoring Profile Collector implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup CGMC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_CGM_CLIENT)
#include "prf.h"
#include "cgmp_common.h"

#include "cgmc_task.h"
#include "cgmc.h"
#include "gap.h"

#include "ke_mem.h"
#include "co_utils.h"

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
 * @brief Initialization of the CGMC module.
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
static uint8_t cgmc_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
    uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct cgmc_env_tag* cgmc_env =
            (struct cgmc_env_tag* ) ke_malloc(sizeof(struct cgmc_env_tag), KE_MEM_ATT_DB);

    // allocate CGMC required environment variable
    env->env = (prf_env_t*) cgmc_env;

    cgmc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    cgmc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_CGMC;
    cgmc_task_init(&(env->desc));

    for(idx = 0; idx < CGMC_IDX_MAX ; idx++)
    {
        cgmc_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), CGMC_FREE);
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
static void cgmc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct cgmc_env_tag* cgmc_env = (struct cgmc_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(cgmc_env->env[conidx] != NULL)
    {
        ke_free(cgmc_env->env[conidx]);
        cgmc_env->env[conidx] = NULL;
    }

    /* Put WSCP Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), CGMC_FREE);
}

/**
 ****************************************************************************************
 * @brief Destruction of the CGMC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void cgmc_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct cgmc_env_tag* cgmc_env = (struct cgmc_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < CGMC_IDX_MAX ; idx++)
    {
        cgmc_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(cgmc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void cgmc_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put WSC Client in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), CGMC_IDLE);
}

/// CGMC Task interface required by profile manager
const struct prf_task_cbs cgmc_itf =
{
        cgmc_init,
        cgmc_destroy,
        cgmc_create,
        cgmc_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* cgmc_prf_itf_get(void)
{
   return &cgmc_itf;
}

/**
 ****************************************************************************************
 * @brief Sends a CGMC_ENABLE_RSP messge.
 *
 * @param[in] cgmc_env -- the profile environment for the connection
 * @param[in] conidx   -- Connection Identifier
 * @param[in] status   -- indicates the outcome of the operation
 *
 * @return none.
 ****************************************************************************************
 */
void cgmc_enable_rsp_send(struct cgmc_env_tag *cgmc_env, uint8_t conidx, uint8_t status)
{
    // Send to APP the details of the discovered attributes on WSCP
    struct cgmc_enable_rsp * rsp = KE_MSG_ALLOC(
            CGMC_ENABLE_RSP,
            prf_dst_task_get(&(cgmc_env->prf_env), conidx),
            prf_src_task_get(&(cgmc_env->prf_env), conidx),
            cgmc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        /// Provide all Discovered Characteristics and Descritors
        rsp->cgmc = cgmc_env->env[conidx]->cgmc;

        // Register CGMC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&(cgmc_env->prf_env), conidx, &(cgmc_env->env[conidx]->cgmc.svc));
        // Go to connected state
        ke_state_set(prf_src_task_get(&(cgmc_env->prf_env), conidx), CGMC_IDLE);
    }
    else
    {
        // clean-up environment variable allocated for task instance
        if(cgmc_env->env[conidx] != NULL)
        {
            ke_free(cgmc_env->env[conidx]);
            cgmc_env->env[conidx] = NULL;
        }
    }

    ke_msg_send(rsp);
}

/**
 ****************************************************************************************
 * @brief Sends a Complete_Event to the App
 *
 * @param[in] cgmc_env -- the profile environment
 * @param[in] conidx    -- Connection Identifier
 * @param[in] operation -- Indicates the operation for which the cmp_evt is being sent.
 * @param[in] status -- indicates the outcome of the operation
 *
 * @return Error_Code.
 ****************************************************************************************
 */

void cgmc_send_cmp_evt(struct cgmc_env_tag *cgmc_env, uint8_t conidx, uint8_t operation, uint8_t status)
{

    // Go back to the CONNECTED state if the state is busy
    if (ke_state_get(prf_src_task_get(&(cgmc_env->prf_env), conidx)) == CGMC_BUSY)
    {
        ke_state_set(prf_src_task_get(&(cgmc_env->prf_env), conidx), CGMC_IDLE);
    }

    // Send the message
    struct cgmc_cmp_evt *evt = KE_MSG_ALLOC(CGMC_CMP_EVT,
                                             prf_dst_task_get(&(cgmc_env->prf_env), conidx),
                                             prf_src_task_get(&(cgmc_env->prf_env), conidx),
                                             cgmc_cmp_evt);

    evt->operation  = operation;
    evt->status     = status;

    ke_msg_send(evt);
}

#endif //(BLE_CGM_CLIENT)

/// @} WSC_CLIENT
