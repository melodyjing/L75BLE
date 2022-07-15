/**
 ****************************************************************************************
 * @file m_tb_iv.h
 *
 * @brief Header file for IV Index Manager
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

#ifndef _M_TB_IV_H_
#define _M_TB_IV_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_IV IV Index Manager
 * @ingroup MESH
 * @brief IV Index Manager
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
 * @brief Get IV value to be used for decryption of a received message
 *
 * @param[in] ivi       Received IVI value
 *
 * @return IV value to be used
 ****************************************************************************************
 */
uint32_t m_tb_iv_get_rx(uint8_t ivi);

/**
 ****************************************************************************************
 * @brief Get current IV value
 *
 * @return Current IV value
 ****************************************************************************************
 */
uint32_t m_tb_iv_get(void);

/**
 ****************************************************************************************
 * @brief Set current IV value
 *
 * @param[in] iv    Current IV value
 ****************************************************************************************
 */
void m_tb_iv_set(uint32_t iv);

/**
 ****************************************************************************************
 * @brief Set sequence number
 *
 * @param[in] seq   Sequence number
 ****************************************************************************************
 */
void m_tb_iv_set_seq(uint32_t seq);

/**
 ****************************************************************************************
 * @brief Get current sequence number
 *
 * @return Current sequence number
 ****************************************************************************************
 */
uint32_t m_tb_iv_get_seq(void);

/**
 ****************************************************************************************
 * @brief Get IV and sequence number value to be used for encryption of a message to transmit.
 * Note that sequence number value is incremented by 1 once value has been returned.
 *
 * @param[in] is_on_prim    True if node is part of primary subnet, else false
 * @param[in] p_iv          Pointer to variable in which IV value will be returned
 * @param[in] p_seq         Pointer to variable in which sequence number value will be returned.
 ****************************************************************************************
 */
void m_tb_iv_get_ivseq_tx(bool is_on_prim, uint32_t *p_iv, uint32_t *p_seq);

/**
 ****************************************************************************************
 * @brief Get current IV state (IV Update in Progress or IV Normal state)
 *
 * @return True if IV state is IV Update in Progress, else False
 ****************************************************************************************
 */
bool m_tb_iv_get_upd_status(void);

/**
 ****************************************************************************************
 * @brief Force switch to IV Update In Progress or IV Normal state
 *
 * @param[in] update        True if switch to IV Update In Progress state is required else False
 * @param[in] is_on_prim    True if node is part of primary subnet, else False
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t m_tb_iv_test_mode(bool update, bool is_on_prim);

/**
 ****************************************************************************************
 * @brief Inform IV Index Manager about a received Secure Network Beacon
 *
 * @param[in] iv_rx     Received IV value
 * @param[in] flags     Received flags value
 *
 * @return True if IV in the beacon is valid, else False
 ****************************************************************************************
 */
bool m_tb_iv_rx_bcn_ind(uint32_t iv_rx, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Inform IV Index Manager about a received sequence number value.
 *
 * @param[in] seq_rx    Received sequence number value
 ****************************************************************************************
 */
void m_tb_iv_rx_ind(uint32_t seq_rx);

/**
 ****************************************************************************************
 * @brief Inform IV Index Manager about a SAR state update
 *
 * @param[in] wait      True if switch to IV Normal state must be delayed, else False
 ****************************************************************************************
 */
void m_tb_iv_sar_upd_ind(bool wait);

/**
 ****************************************************************************************
 * @brief Get information to be stored
 *
 * @param[in] p_status      Pointer to the variable in which current IV Index status will
 * be returned
 * @param[in] p_time_ms     Pointer to the variable in which time of last event has occured
 * (milliseconds part)
 * @param[in] p_nb_wrap     Pointer to the variable in which time of last event has occured
 * (number of wraps part)
 ****************************************************************************************
 */
void m_tb_iv_get_store_info(uint8_t *p_status, uint32_t *p_time_ms, uint16_t *p_nb_wrap);

/**
 ****************************************************************************************
 * @brief Set stored information
 *
 * @param[in] status        Stored IV Index status
 * @param[in] time_ms       Time at which last event has occured (milliseconds part)
 * @param[in] nb_wrap       Time at which last event has occurent (number of wraps part)
 ****************************************************************************************
 */
void m_tb_iv_set_store_info(uint8_t status, uint32_t time_ms, uint16_t nb_wrap);

/// @} end of group

#endif //_M_TB_IV_H_
