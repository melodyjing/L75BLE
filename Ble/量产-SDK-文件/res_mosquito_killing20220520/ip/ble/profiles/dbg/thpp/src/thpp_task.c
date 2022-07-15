/**
 ****************************************************************************************
 *
 * @file thpp_task.c
 *
 * @brief Throughput tester profile Task for debug purpose implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup THPPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_DBG_THPP)
#include "co_utils.h"
#include "gattc_task.h"
#include "attm.h"
#include "prf_utils.h"
#include "thpp_task.h"
#include "ke_mem.h"
#include "ke_timer.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of task instances
#define THPP_IDX_MAX  BLE_CONNECTION_MAX


// Throughput notification characteristic/ service  UUID
#define THPP_NTF_SVC_UUID  { 0x00, 0xAA, 0xFF, 0xF1, 0xBB, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define THPP_NTF_CHAR_UUID { 0x01, 0xAA, 0xFF, 0xF1, 0xBB, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

// default read perm
#define RD_P   (PERM(RD, ENABLE)         | PERM(RP, NO_AUTH))
// default write perm
#define WR_P   (PERM(WRITE_REQ, ENABLE)  | PERM(WP, NO_AUTH))
// default notify perm
#define NTF_P  (PERM(NTF, ENABLE)        | PERM(NP, NO_AUTH))

#define EXT_PERM PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_128)


/// retrieve attribute index from handle
#define THPP_HANDLE_TO_IDX(handle) (handle - thpp_env->shdl)
/// retrieve attribute handle from index
#define THPP_IDX_TO_HANDLE(handle) (thpp_env->shdl + handle)

#define THPP_NOT_STARTED     0xFFFF

/// 1 second timer
#define THPP_STAT_TIMEOUT   (100)

/// Throughput DB Handles offsets
enum thppp_idx_att
{
    /// service attribute
    THPP_IDX_SVC,

    /// Notify Characteristic
    THPP_IDX_NTF_CHAR,
    THPP_IDX_NTF_VAL,
    THPP_IDX_NTF_CCC,

    THPP_IDX_NB
};

/*
 * PROFILE ATTRIBUTES
 ****************************************************************************************
 */
/// Throughput tester profile Database Description - Used to add attributes into the database
const struct attm_desc_128 thpp_att_db[THPP_IDX_NB] =
{
/// ATT Index                           | ATT UUID                    | Permission        | EXT PERM | MAX ATT SIZE
    /// Service Attribute
    [THPP_IDX_SVC]                      = {ATT_128_PRIMARY_SERVICE,    PERM(RD, ENABLE),    0,        0},
    /// Notify Characteristic
    [THPP_IDX_NTF_CHAR]                 = {ATT_128_CHARACTERISTIC,     PERM(RD, ENABLE),    0,        0},
    [THPP_IDX_NTF_VAL]                  = {THPP_NTF_CHAR_UUID,         NTF_P | RD_P,        EXT_PERM, 2048},
    [THPP_IDX_NTF_CCC]                  = {ATT_128_CLIENT_CHAR_CFG,    RD_P | WR_P,         0,        0},
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Test info structure
struct thpp_test_info
{
    /// Handle of the TX UUID
    uint16_t ntf_handle;

    /// Transmit notification size
    uint16_t tx_size;
    /// Number of packet to transmit
    uint16_t nb_tx_pkt;
    /// Counter of transmitted packet
    uint16_t tx_counter;

    /// RX expected size
    uint16_t rx_size;
    /// Number of packet to receive
    uint16_t nb_rx_pkt;
    /// Counter of received packet
    uint16_t rx_counter;
    /// On-going operation code
    uint8_t  op_code;
};

///environment variable
struct thpp_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Database start handle
    uint16_t shdl;

    /// Test info
    struct thpp_test_info test_info[THPP_IDX_MAX];

    /// State of the task
    uint8_t  state[THPP_IDX_MAX];
};


/*
 * LOCAL FUNCTIONS
 ****************************************************************************************
 */


// send statistics
void thpp_stat_send(uint8_t conidx)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

    struct thpp_stat_ind* stat  = KE_MSG_ALLOC(THPP_STAT_IND, prf_dst_task_get(&(thpp_env->prf_env), conidx),
                                                 prf_src_task_get(&(thpp_env->prf_env), conidx), thpp_stat_ind);
    stat->nb_rx      = (test->rx_counter != THPP_NOT_STARTED) ? test->rx_counter : 0;
    stat->nb_byte_rx = stat->nb_rx * (test->rx_size + 7); // size of L2CAP header + Notification data
    stat->nb_tx      = (test->rx_counter != THPP_NOT_STARTED) ? test->tx_counter : 0;
    stat->nb_byte_tx = stat->nb_tx * (test->tx_size + 7); // size of L2CAP header + Notification data
    ke_msg_send(stat);
}


/// send the operation complete event
static void thpp_cmp_evt_send(uint8_t conidx, uint8_t op,  uint8_t status)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    struct thpp_cmp_evt* cmp_evt  = KE_MSG_ALLOC(THPP_CMP_EVT, prf_dst_task_get(&(thpp_env->prf_env), conidx),
                                                 prf_src_task_get(&(thpp_env->prf_env), conidx), thpp_cmp_evt);
    cmp_evt->op     = op;
    cmp_evt->status = status;
    ke_msg_send(cmp_evt);
}


/// Check that memory follow a specific pattern
static bool thpp_mem_chk(uint16_t data_len, uint8_t* data, uint8_t val)
{
    int32_t ref = val;
    bool res = true;
    while (data_len > 0) {
        int32_t cursor = *((uint8_t*) data++);
        int32_t check = cursor - ref;
        if (check)
        {
            res = false;
            break;
        }
        data_len--;
    }

    return res;
}

/// Check if throughput test is over
static void thpp_check_end(uint8_t conidx, uint8_t status)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

    if((status != GAP_ERR_NO_ERROR) || ((test->tx_counter == test->nb_tx_pkt) && (test->rx_counter == test->nb_rx_pkt)))
    {
        thpp_stat_send(conidx);

        test->op_code    = THPP_OP_NONE;
        test->rx_counter = THPP_NOT_STARTED;
        test->tx_counter = THPP_NOT_STARTED;

        thpp_cmp_evt_send(conidx, THPP_OP_START, status);
    }
}

/**
 ****************************************************************************************
 * @brief Loop used to manage reception of data packet
 *
 * @param[in] conidx   Link connection index
 * @param[in] data_len Length of received data
 * @param[in] data     data content
 ****************************************************************************************
 */
static void thpp_rx_loop(uint8_t conidx, uint16_t data_len, uint8_t* data)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

    // check that packet to receive expected
    if((test->op_code == THPP_OP_START) && (test->rx_counter != THPP_NOT_STARTED))
    {
        bool rx_succeed = false;
        // check te
        if((test->rx_counter < test->nb_rx_pkt) && (test->rx_size == data_len))
        {
            // check memory info
            if(   (data[0] == (test->rx_counter & 0xFF))
               && (data[1] == (test->rx_counter >> 8))
               && thpp_mem_chk(data_len-2 , data+2, (test->rx_counter & 0xFF)))
            {
                // increment the counter to check next packet
                test->rx_counter++;
                rx_succeed = true;
            }
        }

        // check if througput test is over
        if((!rx_succeed) || (test->rx_counter == test->nb_rx_pkt))
        {
            thpp_check_end(conidx, rx_succeed ?  GAP_ERR_NO_ERROR : GAP_ERR_PROTOCOL_PROBLEM);
        }
    }
}

/**
 ****************************************************************************************
 * @brief Loop used to manage transmission of data packet
 *
 * @param[in] conidx  Link connection index
 * @param[in] status  Status of previous transmission
 ****************************************************************************************
 */
static void thpp_tx_loop(uint8_t conidx, uint8_t status)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

    if(test->op_code == THPP_OP_START)
    {
        if((test->tx_counter < test->nb_tx_pkt) && (status == GAP_ERR_NO_ERROR))
        {
            struct gattc_send_evt_cmd* notify = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                                 KE_BUILD_ID(TASK_GATTC, conidx),
                                                                 prf_src_task_get(&(thpp_env->prf_env), conidx),
                                                                 gattc_send_evt_cmd, test->tx_size);

            notify->operation   = GATTC_NOTIFY;
            notify->handle      = THPP_IDX_TO_HANDLE(THPP_IDX_NTF_VAL);
            notify->length      = test->tx_size;
            notify->seq_num     = test->tx_counter;
            notify->value[0]    = (test->tx_counter & 0xFF);
            notify->value[1]    = (test->tx_counter >> 8);
            memset(notify->value + 2, (test->tx_counter & 0xFF), test->tx_size-2);

            ke_msg_send(notify);

            test->tx_counter++;
        }
        else
        {
            thpp_check_end(conidx, status);
        }
    }
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/// @brief Handles reception of throughput test configuration
static int thpp_configure_cmd_handler(ke_msg_id_t const msgid, struct thpp_configure_cmd *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = GAP_ERR_NO_ERROR;

    if(param->op != THPP_OP_CONFIGURE)
    {
        status = GAP_ERR_INVALID_PARAM;
    }
    else if(thpp_env->test_info[conidx].op_code != THPP_OP_NONE)
    {
        status = GAP_ERR_COMMAND_DISALLOWED;
    }
    else
    {
        struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

        test->op_code                         = THPP_OP_CONFIGURE;
        test->nb_tx_pkt                       = param->nb_pkt_tx;
        test->tx_size                         = param->tx_size;
        test->tx_counter                      = THPP_NOT_STARTED;
        test->nb_rx_pkt                       = param->nb_pkt_rx;
        test->rx_size                         = param->rx_size;
        test->rx_counter                      = THPP_NOT_STARTED;

        // peer device handle to be searched
        if(test->ntf_handle == ATT_INVALID_HANDLE)
        {
            const uint8_t thpp_char_uuid[] = THPP_NTF_CHAR_UUID;

            /* allocate read name req message */
            struct gattc_read_cmd *read_cmd = KE_MSG_ALLOC_DYN(GATTC_READ_CMD,
                                                               KE_BUILD_ID(TASK_GATTC,conidx), dest_id, gattc_read_cmd,
                                                               ATT_UUID_128_LEN);

            // Send read by UUID to get handle of the throughput profile onto peer device
            read_cmd->operation                   = GATTC_READ_BY_UUID;
            read_cmd->nb                          = 1;
            read_cmd->req.by_uuid.start_hdl       = ATT_1ST_REQ_START_HDL;
            read_cmd->req.by_uuid.end_hdl         = ATT_1ST_REQ_END_HDL;
            read_cmd->req.by_uuid.uuid_len        = ATT_UUID_128_LEN;

            memcpy(read_cmd->req.by_uuid.uuid, thpp_char_uuid, ATT_UUID_128_LEN);

            ke_msg_send(read_cmd);
        }
        else
        {
            test->op_code = THPP_OP_NONE;
            thpp_cmp_evt_send(conidx, param->op, GAP_ERR_NO_ERROR);
        }
    }

    // An error occurs, reject the request
    if(status != GAP_ERR_NO_ERROR)
    {
        thpp_cmp_evt_send(conidx, param->op, status);
    }

    return (KE_MSG_CONSUMED);
}

/// @brief Handles reception of throughput test start
static int thpp_start_cmd_handler(ke_msg_id_t const msgid, struct thpp_start_cmd *param,
                                  ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = GAP_ERR_NO_ERROR;

    if(param->op != THPP_OP_START)
    {
        status = GAP_ERR_INVALID_PARAM;
    }
    else if(thpp_env->test_info[conidx].op_code != THPP_OP_NONE)
    {
        status = GAP_ERR_COMMAND_DISALLOWED;
    }
    else if(thpp_env->test_info[conidx].ntf_handle == ATT_INVALID_HDL)
    {
        status = GAP_ERR_REJECTED;
    }
    else
    {
        struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

        test->op_code                         = THPP_OP_START;
        test->rx_counter                      = THPP_NOT_STARTED;

        // activate notifications
        prf_gatt_write_ntf_ind(&(thpp_env->prf_env), conidx, test->ntf_handle + 1, PRF_CLI_START_NTF);
    }

    // An error occurs, reject the request
    if(status != GAP_ERR_NO_ERROR)
    {
        thpp_cmp_evt_send(conidx, param->op, status);
    }

    return (KE_MSG_CONSUMED);
}

/// @brief Handles reception of throughput test start
static int thpp_stat_timeout_ind_handler(ke_msg_id_t const msgid, void *param,
                                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx = KE_IDX_GET(dest_id);
    struct thpp_test_info * test  = &(thpp_env->test_info[conidx]);

    if(test->op_code == THPP_OP_START)
    {
        // send statistics
        thpp_stat_send(conidx);

        ke_timer_set(THPP_STAT_TIMEOUT_IND, dest_id, THPP_STAT_TIMEOUT);
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid, struct gattc_att_info_req_ind *param,
                                          ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t  att_idx   = THPP_HANDLE_TO_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        /// Challenge Response Client configuration
        case THPP_IDX_NTF_CHAR:
        {
            // CCC attribute length = 2
            cfm->length = 2;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        default:
        {
            cfm->length = 0;
            cfm->status = ATT_ERR_WRITE_NOT_PERMITTED;
        } break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of write request message.
 * The handler will analyse what has been set and decide alert level
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind *param,
                                       ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx  = KE_IDX_GET(src_id);
    /// retrieve attribute index
    uint8_t att_idx = THPP_HANDLE_TO_IDX(param->handle);

    switch(att_idx)
    {
        /// Notification client configuration
        case THPP_IDX_NTF_CCC:
        {
            uint16_t value = co_read16p(param->value);
            uint8_t  status = ATT_ERR_APP_ERROR;
            // return notification configuration
            struct gattc_write_cfm* cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
            cfm->handle   = param->handle;

            if((param->length == sizeof(uint16_t)) && (value <= PRF_CLI_START_NTF))
            {
                status = ((value == PRF_CLI_START_NTF) && (thpp_env->test_info[conidx].tx_counter == THPP_NOT_STARTED))
                       ? ATT_ERR_NO_ERROR
                       : ATT_ERR_APP_ERROR;
            }

            cfm->status   = status;
            // Send value to peer device.
            ke_msg_send(cfm);


            // check that we are ready to start transmission
            if(status == ATT_ERR_NO_ERROR)
            {
                thpp_env->test_info[conidx].tx_counter = 0;
                thpp_tx_loop(conidx, CO_ERROR_NO_ERROR);
            }

        }break;

        default:
        {
            ASSERT_INFO(0, param->handle, src_id);
        } break;
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the read request from peer device
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_read_req_ind *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    // retrieve attribute index
    uint8_t att_idx = THPP_HANDLE_TO_IDX(param->handle);

    switch(att_idx)
    {
        // Notification Characteristic
        case THPP_IDX_NTF_VAL:
        {
            struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, 0);
            cfm->length   = 0;
            cfm->handle   = param->handle;
            cfm->status   = GAP_ERR_NO_ERROR;
            // Send value to peer device.
            ke_msg_send(cfm);
        }break;

        // Notification client configuration
        case THPP_IDX_NTF_CCC:
        {
            struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
            cfm->length   = sizeof(uint16_t);
            cfm->handle   = param->handle;
            cfm->status   = GAP_ERR_NO_ERROR;
            co_write16p(cfm->value, PRF_CLI_STOP_NTFIND);
            // Send value to peer device.
            ke_msg_send(cfm);
        }break;

        default:
        {
            ASSERT_INFO(0, param->handle, src_id);
        } break;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief handles GATT Read value reception
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_ind_handler(ke_msg_id_t const msgid, struct gattc_read_ind *param,
                                  ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx  = KE_IDX_GET(src_id);

    // Retrieve handle that manage notifications
    if(thpp_env->test_info[conidx].op_code == THPP_OP_CONFIGURE)
    {
        thpp_env->test_info[conidx].ntf_handle = param->handle;
    }

    return KE_MSG_CONSUMED;
}


/**
 ****************************************************************************************
 * @brief handles GATT Notification reception
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_event_ind_handler(ke_msg_id_t const msgid, struct gattc_event_ind *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx  = KE_IDX_GET(src_id);

    // execute next element to notify - or an error occurs
    thpp_rx_loop(conidx, param->length, param->value);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief handles GATT request completion
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);
    uint8_t conidx  = KE_IDX_GET(src_id);

    if((param->operation == GATTC_NOTIFY) && (thpp_env->test_info[conidx].op_code == THPP_OP_START))
    {
        // execute next element to notify - or an error occurs
        thpp_tx_loop(conidx, param->status);
    }
    else if((param->operation == GATTC_WRITE) && (thpp_env->test_info[conidx].op_code == THPP_OP_START))
    {
        // inform that reception can be started - or an error occurs
        if(param->status != CO_ERROR_NO_ERROR)
        {
            thpp_env->test_info[conidx].op_code = THPP_OP_NONE;
            thpp_cmp_evt_send(conidx, THPP_OP_CONFIGURE, param->status);
        }
        else
        {
            // mark reception ready to be started
            thpp_env->test_info[conidx].rx_counter = 0;

            // send statistics to inform that test starts
            thpp_stat_send(conidx);

            // start statistics timer
            ke_timer_set(THPP_STAT_TIMEOUT_IND, dest_id, THPP_STAT_TIMEOUT);

            // check if TX must be started
            if((thpp_env->test_info[conidx].tx_counter != THPP_NOT_STARTED) && (thpp_env->test_info[conidx].tx_counter == 0))
            {
                thpp_tx_loop(conidx, GAP_ERR_NO_ERROR);
            }
        }
    }
    else if((param->operation == GATTC_READ_BY_UUID) && (thpp_env->test_info[conidx].op_code == THPP_OP_CONFIGURE))
    {
        if(thpp_env->test_info[conidx].ntf_handle != ATT_INVALID_HDL)
        {
            //register profile task in gatt for indication/notifications
            struct gattc_reg_to_peer_evt_cmd * reg = KE_MSG_ALLOC(GATTC_REG_TO_PEER_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(&(thpp_env->prf_env), conidx),
                    gattc_reg_to_peer_evt_cmd);

            reg->operation =  GATTC_REGISTER;
            reg->start_hdl = thpp_env->test_info[conidx].ntf_handle;
            reg->end_hdl   = thpp_env->test_info[conidx].ntf_handle+1;

            ke_msg_send(reg);
        }
        else
        {
            // Inform that configuration operation is over
            thpp_env->test_info[conidx].op_code = THPP_OP_NONE;
            thpp_cmp_evt_send(conidx, THPP_OP_CONFIGURE, GAP_ERR_NOT_SUPPORTED);
        }
    }
    else if((param->operation == GATTC_REGISTER) && (thpp_env->test_info[conidx].op_code == THPP_OP_CONFIGURE))
    {
        // Inform that configuration operation is over
        thpp_env->test_info[conidx].op_code = THPP_OP_NONE;
        thpp_cmp_evt_send(conidx, THPP_OP_CONFIGURE, param->status);
    }

    return KE_MSG_CONSUMED;
}

/*
 * TASK MESSAGE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
KE_MSG_HANDLER_TAB(thpp)
{
    { THPP_CONFIGURE_CMD,                  (ke_msg_func_t) thpp_configure_cmd_handler     },
    { THPP_START_CMD,                      (ke_msg_func_t) thpp_start_cmd_handler         },
    { THPP_STAT_TIMEOUT_IND,               (ke_msg_func_t) thpp_stat_timeout_ind_handler  },

    { GATTC_ATT_INFO_REQ_IND,              (ke_msg_func_t) gattc_att_info_req_ind_handler },
    { GATTC_WRITE_REQ_IND,                 (ke_msg_func_t) gattc_write_req_ind_handler    },
    { GATTC_READ_REQ_IND,                  (ke_msg_func_t) gattc_read_req_ind_handler     },

    { GATTC_READ_IND,                      (ke_msg_func_t) gattc_read_ind_handler         },
    { GATTC_EVENT_IND,                     (ke_msg_func_t) gattc_event_ind_handler        },

    { GATTC_CMP_EVT,                       (ke_msg_func_t) gattc_cmp_evt_handler          },
};


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the THPP module.
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
static uint8_t thpp_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct thpp_db_cfg* params)
{
    //------------------ create the attribute database for the profile -------------------

    // Service content flag
    uint32_t cfg_flag             = 0xF;
    const uint8_t thpp_svc_uuid[] = THPP_NTF_SVC_UUID;

    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;


    status = attm_svc_create_db_128(start_hdl, thpp_svc_uuid, (uint8_t*)&cfg_flag,
                                    THPP_IDX_NB, NULL, env->task, &thpp_att_db[0],
                                    (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_EKS))
                                    | PERM(SVC_UUID_LEN, UUID_128) | PERM(SVC_MI, ENABLE) );

    //-------------------- allocate memory required for the profile  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {
        struct thpp_env_tag* thpp_env =
                (struct thpp_env_tag* ) ke_malloc(sizeof(struct thpp_env_tag), KE_MEM_ATT_DB);

        // allocate THPP required environment variable
        env->env = (prf_env_t*) thpp_env;
        thpp_env->shdl              = *start_hdl;
        thpp_env->prf_env.app_task  = app_task | (PERM_GET(sec_lvl, SVC_MI)
                                    ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        thpp_env->prf_env.prf_task  = env->task | PERM(PRF_MI, ENABLE);

        // initialize environment variable
        env->id                     = TASK_ID_THPP;
        env->desc.msg_handler_tab   = thpp_msg_handler_tab;
        env->desc.msg_cnt           = ARRAY_LEN(thpp_msg_handler_tab);
        env->desc.state             = thpp_env->state;
        env->desc.idx_max           = THPP_IDX_MAX;
    }
    return (status);
}

/**
 ****************************************************************************************
 * @brief Destruction of the FINDT module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void thpp_destroy(struct prf_task_env* env)
{
    struct thpp_env_tag* thpp_env = (struct thpp_env_tag*) env->env;

    // free profile environment variables
    env->env = NULL;
    ke_free(thpp_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void thpp_create(struct prf_task_env* env, uint8_t conidx)
{
    struct thpp_env_tag *thpp_env = PRF_ENV_GET(THPP, thpp);

    // mark no operation on-going
    memset(&(thpp_env->test_info[conidx]), 0, sizeof(struct thpp_test_info));
    thpp_env->test_info[conidx].ntf_handle = ATT_INVALID_HDL;
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void thpp_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
   /* nothing to do */
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// FINDT Task interface required by profile manager
const struct prf_task_cbs thpp_itf =
{
    (prf_init_fnct) thpp_init,
    thpp_destroy,
    thpp_create,
    thpp_cleanup,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* thpp_prf_itf_get(void)
{
   return &thpp_itf;
}


#endif // BLE_DBG_THPP

/// @} FINDTTASK
