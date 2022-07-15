/**
 ****************************************************************************************
 *
 * @file ble_util_buf.c
 *
 * @brief BLE EM buffers
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup BLE_UTIL_BUF
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include "arch.h"
#include "co_bt.h"
#include "co_utils.h"        // common utility definition
#include "co_math.h"
#include "ble_util_buf.h"
#include "em_map.h"
#include "ke_mem.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * STRUCTURE DEFINITION
 ****************************************************************************************
 */

/// BLE EM buffer management environment structure
struct ble_util_buf_env_tag
{
    /// List of free LLCP RX buffers
    struct co_list                llcp_tx_free;
    /// List of free RX buffers
    struct co_list                rx_free;
    /// List of free ACL TX buffers
    struct co_list                acl_tx_free;
    /// List of free advertising data TX buffers
    struct co_list                adv_tx_free;

    #if (BLE_ISO_PRESENT)
    /// List of free isochronous data TX/RX buffers
    struct co_list                iso_free;
    #endif // (BLE_ISO_PRESENT)

    #if (BLE_CIS || BLE_BIS)
    /// List of free hoping sequence
    struct co_list                hop_seq_free;
    #endif // (BLE_CIS || BLE_BIS)

    /// Pool of LLCP TX buffers (common for all links)
    struct ble_em_llcp_buf_elt    llcp_tx_pool[EM_BLE_LLCPTXBUF_NB];

    /// Pool of RX buffers (common for all activities) (use the format of ACL packets)
    struct ble_em_acl_buf_elt     rx_pool[EM_BLE_DATARXBUF_NB];

    /// Pool of ACL TX buffers (common for all links)
    struct ble_em_acl_buf_elt     acl_tx_pool[EM_BLE_ACLTXBUF_NB];

    /// Pool of advertising data TX buffers (common for all advertising sets)
    struct ble_em_adv_buf_elt     adv_tx_pool[EM_BLE_ADVDATATXBUF_NB];

    #if (BLE_ISO_PRESENT)
    /// Pool of isochronous data TX/RX buffers (common for all isochronous channels)
    struct ble_em_iso_buf_elt     iso_pool[EM_BLE_ISO_BUFFER_NB];
    #endif // (BLE_ISO_PRESENT)

    #if (BLE_CIS | BLE_BIS)
    /// Pool of hop sequence exchange memory area (common for all isochronous channels)
    struct ble_em_hop_seq_elt     hop_seq_pool[EM_BLE_ISO_HOP_SEQ_NB];
    #endif // (BLE_CIS | BLE_BIS)
};


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// BLE EM buffer management environment
#if COBRA_POWER_DOWN_ENABLE
static struct ble_util_buf_env_tag ble_util_buf_env 	__attribute__((section("retention_area"),zero_init));
#else
static struct ble_util_buf_env_tag ble_util_buf_env;
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static volatile unsigned int acl_buffer_number;
void ble_util_buf_init(uint8_t init_type)
{
    #if defined EM_SIZE_LIMIT
    ASSERT_INFO(EM_BLE_END <= EM_SIZE_LIMIT, EM_BLE_END/1024, EM_SIZE_LIMIT/1024);
    #endif // defined EM_SIZE_LIMIT

    switch (init_type)
    {
        case RWIP_INIT:
        {
            int16_t i;
            // Clear environment memory
            memset(&ble_util_buf_env, 0, sizeof(ble_util_buf_env));

            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_LLCPTXBUF_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.llcp_tx_pool[i].buf_ptr = EM_BLE_LLCPTXBUF_OFFSET + i * EM_BLE_LLCPTXBUF_SIZE;
            }

            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_DATARXBUF_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.rx_pool[i].buf_ptr = EM_BLE_DATARXBUF_OFFSET + i * EM_BLE_DATARXBUF_SIZE + BLE_ACL_RX_BUF_HEADER_SPACE;
            }

            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_ACLTXBUF_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.acl_tx_pool[i].buf_ptr = EM_BLE_ACLTXBUF_OFFSET + i * EM_BLE_ACLTXBUF_SIZE;
            }

            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_ADVDATATXBUF_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.adv_tx_pool[i].buf_ptr = EM_BLE_ADVDATATXBUF_OFFSET + i * EM_BLE_ADVDATATXBUF_SIZE;
            }

            #if (BLE_ISO_PRESENT)
            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_ISO_BUFFER_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.iso_pool[i].buf_ptr = EM_BLE_ISO_BUFFER_OFFSET + i * EM_BLE_ISO_BUFFER_SIZE;
                ble_util_buf_env.iso_pool[i].buf_idx = i;
            }
            #endif // (BLE_ISO_PRESENT)

            #if (BLE_CIS | BLE_BIS)
            // Point each pool element to its associated buffer in EM
            for(i = (EM_BLE_ISO_HOP_SEQ_NB-1) ; i >= 0 ; i--)
            {
                ble_util_buf_env.hop_seq_pool[i].em_ptr = EM_BLE_ISO_HOP_SEQ_OFFSET + (i * EM_BLE_ISO_HOP_SEQ_SIZE);
            }
            #endif // (BLE_CIS | BLE_BIS)
        }
        break;

        case RWIP_RST:
        {
            // Do nothing
        }
        // No break

        case RWIP_1ST_RST:
        {
            // Initialize the list of free LLCP TX buffers
            co_list_pool_init(&ble_util_buf_env.llcp_tx_free,
                              &ble_util_buf_env.llcp_tx_pool[0],
                              sizeof(struct ble_em_llcp_buf_elt),
                              EM_BLE_LLCPTXBUF_NB);

            // Initialize the list of free ACL RX buffers
            co_list_pool_init(&ble_util_buf_env.rx_free,
                              &ble_util_buf_env.rx_pool[0],
                              sizeof(struct ble_em_acl_buf_elt),
                              EM_BLE_DATARXBUF_NB);

            // Initialize the list of free ACL TX buffers
            co_list_pool_init(&ble_util_buf_env.acl_tx_free,
                              &ble_util_buf_env.acl_tx_pool[0],
                              sizeof(struct ble_em_acl_buf_elt),
                              EM_BLE_ACLTXBUF_NB);
            acl_buffer_number = EM_BLE_ACLTXBUF_NB;
            // Initialize the list of free advertising data TX buffers
            co_list_pool_init(&ble_util_buf_env.adv_tx_free,
                              &ble_util_buf_env.adv_tx_pool[0],
                              sizeof(struct ble_em_adv_buf_elt),
                              EM_BLE_ADVDATATXBUF_NB);

            #if (BLE_ISO_PRESENT)
            // Initialize the list of free isochronous data TX buffers
            co_list_pool_init(&ble_util_buf_env.iso_free,
                              &ble_util_buf_env.iso_pool[0],
                              sizeof(struct ble_em_iso_buf_elt),
                              EM_BLE_ISO_BUFFER_NB);
            #endif // (BLE_ISO_PRESENT)

            #if (BLE_CIS | BLE_BIS)
            // Initialize the list of free Hopping sequence
            co_list_pool_init(&ble_util_buf_env.hop_seq_free,
                              &ble_util_buf_env.hop_seq_pool[0],
                              sizeof(struct ble_em_hop_seq_elt),
                              EM_BLE_ISO_HOP_SEQ_NB);
            #endif // (BLE_CIS | BLE_BIS)
        }
        break;

        default:
        {
            // Do nothing
        }
        break;
    }
}

struct ble_em_llcp_buf_elt* ble_util_buf_llcp_tx_alloc(void)
{
    // Get free element from free list
	  GLOBAL_INT_DISABLE();
    struct ble_em_llcp_buf_elt* elt = (struct ble_em_llcp_buf_elt*) co_list_pop_front(&ble_util_buf_env.llcp_tx_free);
    GLOBAL_INT_RESTORE();
    return elt;
}

void ble_util_buf_llcp_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_LLCPTXBUF_OFFSET) / EM_BLE_LLCPTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_LLCPTXBUF_NB, index, buf);

    // Push to free list
	  GLOBAL_INT_DISABLE();
    co_list_push_back(&ble_util_buf_env.llcp_tx_free, &ble_util_buf_env.llcp_tx_pool[index].hdr);
	   GLOBAL_INT_RESTORE();
}

uint16_t ble_util_buf_rx_alloc(void)
{
    uint16_t buf_ptr = 0;
    struct ble_em_acl_buf_elt* elt;

    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    elt = (struct ble_em_acl_buf_elt*) co_list_pop_front(&ble_util_buf_env.rx_free);
    GLOBAL_INT_RESTORE();

    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }


    return buf_ptr;
}

void ble_util_buf_rx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - BLE_ACL_RX_BUF_HEADER_SPACE - EM_BLE_DATARXBUF_OFFSET) / EM_BLE_DATARXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_DATARXBUF_NB, index, buf);

    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&ble_util_buf_env.rx_free, &ble_util_buf_env.rx_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}

uint16_t ble_util_buf_acl_tx_alloc(void)
{
    uint16_t buf_ptr = 0;
    // Get free element from free list
	  GLOBAL_INT_DISABLE();
    struct ble_em_acl_buf_elt* elt = (struct ble_em_acl_buf_elt*) co_list_pop_front(&ble_util_buf_env.acl_tx_free);
	  acl_buffer_number -= 1;
	 // printf("A= %d \n", acl_buffer_number);
	  GLOBAL_INT_RESTORE();
    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }

    return buf_ptr;
}

struct ble_em_acl_buf_elt* ble_util_buf_acl_tx_elt_get(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ACLTXBUF_OFFSET) / EM_BLE_ACLTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ACLTXBUF_NB, index, buf);

    return &(ble_util_buf_env.acl_tx_pool[index]);
}


void ble_util_buf_acl_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ACLTXBUF_OFFSET) / EM_BLE_ACLTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ACLTXBUF_NB, index, buf);

    // Push to free list
	  GLOBAL_INT_DISABLE();
    co_list_push_back(&ble_util_buf_env.acl_tx_free, &ble_util_buf_env.acl_tx_pool[index].hdr);
	  acl_buffer_number += 1;
	 //printf("F= %d \n", acl_buffer_number);
	  GLOBAL_INT_RESTORE();
}

uint16_t ble_util_buf_adv_tx_alloc(void)
{
    uint16_t buf_ptr = 0;
    // Get free element from free list
	  GLOBAL_INT_DISABLE();
    struct ble_em_adv_buf_elt* elt = (struct ble_em_adv_buf_elt*) co_list_pop_front(&ble_util_buf_env.adv_tx_free);
	   GLOBAL_INT_RESTORE();
    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }

    return buf_ptr;
}

void ble_util_buf_adv_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ADVDATATXBUF_OFFSET) / EM_BLE_ADVDATATXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ADVDATATXBUF_NB, index, buf);

    // Push to free list
	  GLOBAL_INT_DISABLE();
    co_list_push_back(&ble_util_buf_env.adv_tx_free, &ble_util_buf_env.adv_tx_pool[index].hdr);
	  GLOBAL_INT_RESTORE();
}

#if (BLE_ISO_PRESENT)
uint16_t ble_util_buf_iso_alloc(uint8_t* index)
{
    uint16_t em_ptr = 0;
    // Get free element from free list
    struct ble_em_iso_buf_elt* elt = (struct ble_em_iso_buf_elt*) co_list_pop_front(&ble_util_buf_env.iso_free);

    if(elt != NULL)
    {
        em_ptr = elt->buf_ptr;
        *index = elt->buf_idx;
    }

    return em_ptr;
}

void ble_util_buf_iso_free(uint8_t index)
{
    ASSERT_INFO(index < EM_BLE_ISO_BUFFER_NB, index, 0);
    // Push to free list
    co_list_push_back(&ble_util_buf_env.iso_free, &ble_util_buf_env.iso_pool[index].hdr);
}
#endif // (BLE_ISO_PRESENT)

#if (BLE_CIS | BLE_BIS)
uint16_t ble_util_hop_seq_alloc(void)
{
    uint16_t em_ptr = 0;
    // Get free element from free list
    struct ble_em_hop_seq_elt* elt = (struct ble_em_hop_seq_elt*) co_list_pop_front(&ble_util_buf_env.hop_seq_free);

    if(elt != NULL)
    {
        em_ptr = elt->em_ptr;
    }
    else
    {
        ASSERT_ERR(0);
    }

    return em_ptr;
}

void ble_util_hop_seq_free(uint16_t em_ptr)
{
    uint8_t index;
    ASSERT_INFO((em_ptr >= EM_BLE_ISO_HOP_SEQ_OFFSET) && (em_ptr < EM_BLE_ISO_HOP_SEQ_END), em_ptr, 0);

    index = ((em_ptr - EM_BLE_ISO_HOP_SEQ_OFFSET) / EM_BLE_ISO_HOP_SEQ_SIZE);

    // Push to free list
    co_list_push_back(&ble_util_buf_env.hop_seq_free, &ble_util_buf_env.hop_seq_pool[index].hdr);
}
#endif // (BLE_CIS | BLE_BIS)


/// @} BLE_UTIL_BUF
