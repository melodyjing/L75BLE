/**
 ****************************************************************************************
 * @file m_tb_replay.h
 *
 * @brief Header file for Message Replay Protection Manager
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

#ifndef _M_TB_REPLAY_H_
#define _M_TB_REPLAY_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_REPLAY Message Replay Protection Manage
 * @ingroup MESH
 * @brief Message Replay Protection Manage
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "../../../inc/m_api.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Inform Replay Manager about a received message
 *
 * @param[in] src_addr      Source address
 * @param[in] iv            Received IV value
 * @param[in] seq           Received Sequence Number value
 *
 * @return True if message can be forwarded to upper layers, False if message has already
 * been received
 ****************************************************************************************
 */
bool m_tb_replay_rx_ind(uint16_t src_addr, uint32_t iv, uint32_t seq);

/// @} end of group

#endif //_M_TB_REPLAY_H_
