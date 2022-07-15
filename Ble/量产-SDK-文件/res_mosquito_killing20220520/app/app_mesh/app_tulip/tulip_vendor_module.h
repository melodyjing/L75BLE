/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	tulip_vendor_module.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/3/25

	Description :  
	
*******************************************************************************/
#ifndef  __TULIPEI_VENDOR_MODULE_H__
#define  __TULIPEI_VENDOR_MODULE_H__
#if (BLE_APP_MESH == 1)

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions

#define TULIPEI_VENDOR_SERVER_ID		0x02110211
#if (COBRA_SCENE_VERSION2_ENABLE==0)
#define TULIPEI_VENDOR_MESSAGE_ATTR_GET         0x000211C0        //vendor Message Attr Get
#define TULIPEI_VENDOR_MESSAGE_ATTR_SET         0x000211C1        //vendor Message Attr Set
#define TULIPEI_VENDOR_MESSAGE_ATTR_SET_UNACK   0x000211C2        //vendor Message Attr Set Unacknowledged
#define TULIPEI_VENDOR_MESSAGE_ATTR_STATUS      0x000211C3        //vendor Message Attr Status
#else
#define TULIPEI_VENDOR_MESSAGE_ATTR_GET         0x000211E0        //vendor Message Attr Get
#define TULIPEI_VENDOR_MESSAGE_ATTR_SET         0x000211E1        //vendor Message Attr Set
#define TULIPEI_VENDOR_MESSAGE_ATTR_SET_UNACK   0x000211E2        //vendor Message Attr Set Unacknowledged
#define TULIPEI_VENDOR_MESSAGE_ATTR_STATUS      0x000211E3        //vendor Message Attr Status
#endif
uint16_t m_tulipi_vendor_register_server(uint8_t elmt_idx, bool main);

enum tulipi_vendor_set_command
{
	 TULIPEI_OTA_SET_CMD,
	
	 TULIPEI_SCENE_COLOR_VALUE,
	
};


/// State update indication structure
typedef struct mm_api_vendor_msg_upd_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_SRV_STATE_UPD_IND: Inform application about state update
    uint32_t ind_code;

    /// Transition time in milliseconds
    uint32_t trans_time_ms;
    /// State identifier (@see mm_state_idx))
    uint16_t state_id;
    /// Element index
    uint8_t elmt_idx;
		uint8_t msg_len;
    /// New state value or targeted state value depending on transition time
    uint8_t msg[__ARRAY_EMPTY];
} mm_api_vendor_msg_upd_ind_t;



#endif
#endif