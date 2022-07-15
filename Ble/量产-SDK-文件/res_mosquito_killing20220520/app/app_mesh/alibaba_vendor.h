/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	alibaba_vendor.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/8/3

	Description :   和天猫精灵 对接的时候, 需要一个Vendor Model
	
*******************************************************************************/
#ifndef  __ALIBABA_VENDOR_H__
#define  __ALIBABA_VENDOR_H__
#if (BLE_APP_MESH == 1)

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions

#define ALIBABA_VENDOR_SERVER_ID		0x01A80000
#define ALIBABA_VENDOR_CLIENT_ID	  0x01A80001


#define ALIBABA_VENDOR_MESSAGE_ATTR_SET   0x0001A8D1

uint16_t m_alibaba_vendor_register_server(uint8_t elmt_idx, bool main);

enum alibaba_vendor_set_pos
{
	 ALIBABA_SET_TID_POS = 0,
	 
	 ALIBABA_SET_ATTR_NAME_POS = 1,
	
	 ALIBABA_SET_PARAMETER_POS = 3,
	
};






#endif
#endif