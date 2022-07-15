/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_util.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/9/3

	Description :   
    添加一些辅助功能,用于SDK 的开发应用
1. 添加蓝牙白名单的处理

	Changed Log	:

		Liujian  2020/9/3 		- Creation

*****************************************************************************/

#include "cobra_util.h"
#include <string.h>
#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "co_bt.h"                   // Common BT Definition
#include "co_math.h"                 // Common Maths Definition
#include <cobra_config.h>

#if COBRA4_BLE_WHITELIST_EN
void cobra_white_list_add(struct gap_bdaddr *p_bd_addr)
{
	  // Reset the stack
    struct gapm_list_set_wl_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_LIST_SET_CMD,
                                                TASK_GAPM, TASK_APP,
                                                gapm_list_set_wl_cmd, sizeof(struct gap_bdaddr));

    p_cmd->operation = GAPM_SET_WL;
		
		p_cmd->size = 1;
		
		memcpy(p_cmd->wl_info, p_bd_addr, sizeof(struct gap_bdaddr));

    ke_msg_send(p_cmd);
	
}


void cobra_white_list_get_size(void)
{
	   struct gapm_get_dev_info_cmd* p_cmd = KE_MSG_ALLOC(GAPM_GET_DEV_INFO_CMD, 
			                                               TASK_GAPM, TASK_APP,
		                                                 gapm_get_dev_info_cmd);
		
     p_cmd->operation = GAPM_GET_WLIST_SIZE;
     
		 ke_msg_send(p_cmd);
	
}

uint32_t cobra_app_whitelist_add(void)
{
	  struct gap_bdaddr addr;
	  int8_t addr_len = NVDS_LEN_PEER_BD_ADDRESS;
	  // 添加白名单
    if (app_sec_get_bond_status() == true)
    {
			   
        if (nvds_get(NVDS_TAG_PEER_BD_ADDRESS, &addr_len, (uint8_t *)&addr) == NVDS_OK)
        {
              cobra_white_list_add(&addr);
					    return 1;
        }
		}			
    return 0;						
}

#endif



