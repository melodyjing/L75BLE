/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  tuya_node_init.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/28

	Description :   tuya mesh task


	Changed Log	:

		Liujian   2021/5/28			- Creation

*****************************************************************************/

#ifndef __TUYA_NODE_INIT_H__
#define __TUYA_NODE_INIT_H__
#include "rwip_config.h"  

#if ((BLE_APP_MESH == 1) && (BLE_TUYA_MESH_SDK_ENABLE == 1))

#include <stdint.h>

#ifndef _PACKED_
#define _PACKED_  __attribute__ ((packed))
#endif

typedef struct
{
    union {
        struct {
            uint16_t rfu: 8;
            uint16_t is_fmkey: 1; //0-uuid is mac+pid+category; 1-uuid is mac+firmware key+category
            uint16_t key_share: 1; //model key share & group key bind share
            uint16_t pub_addr_need: 1;
            uint16_t rfu2: 5;
        } pid;
        uint16_t pid_val;
    };
} _PACKED_ compo_data_pid_t;

typedef struct
{
    uint8_t ttl;
    uint32_t relay: 2; //!< relay
    uint32_t proxy: 2; //!< Mesh Proxy Service & Proxy feature, binding with node identity
    uint32_t fn: 2; //!< friend feature
    uint32_t lpn: 2; //!< low power feature
    uint8_t net_trans_count: 3; //!< :3 transmit (net_trans_count + net_trans_count_base + 1) times
    uint8_t net_trans_steps: 5; //!< :5 retransmission interval = (net_trans_steps + 1) * 10ms

    uint8_t uuid[16]; // mac + pid + category
    
    uint16_t company_id; // composition data head: cid
    compo_data_pid_t product_id; // composition data head: pid
    uint16_t version_id; // composition data head: vid
} _PACKED_ node_info_t;


void  tuya_mesh_para_init(void);

uint8_t * tuya_get_device_uuid(void);

unsigned int tuya_mesh_gap_addr_get(unsigned char *p_addr);

#endif

#endif
