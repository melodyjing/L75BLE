/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  tuya_node_init.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/28

	Description :   tuya mesh task


	Changed Log	:

		Liujian   2021/5/28			- Creation

*****************************************************************************/

#if ((BLE_APP_MESH == 1) && (BLE_TUYA_MESH_SDK_ENABLE == 1))

#include "tuya_node_init.h"
#include "nvds.h"
#include "co_bt.h"
#include <string.h>

static uint8_t tuya_pid[8] = {0x70, 0x61, 0x79, 0x65, 0x6C, 0x73, 0x76, 0x67};

static node_info_t node_info = {0};


/**
* @description: get mesh category of prov info: uuid + fast prov from json
* @param {none}
* @return: none
**/
uint16_t tuya_get_mesh_category(void)
{
    return 0x1015;  //ty_light_json_config_get_category();
}

uint8_t* tuya_get_pid(void)
{
    return tuya_pid;
}

uint8_t*  tuya_get_device_uuid(void)
{
	   return node_info.uuid;   	
}

/**
* @description: mesh information init
* @param {none}
* @return: none 
**/
void tuya_mesh_para_init(void)
{
    node_info.ttl = 8;
    node_info.relay = 1;
    node_info.proxy = 1;
    node_info.fn = 1;
    node_info.lpn = 0;
    node_info.net_trans_count = 2;
    node_info.net_trans_steps = 2;

//    node_info.company_id = VENDOR_ID;

//    node_info.product_id.pid.is_fmkey = tuya_get_is_key_or_pid()? 1 : 0;

//    node_info.product_id.pid.key_share = 1;

//    node_info.product_id.pid.pub_addr_need = app_tuya_get_if_need_pub_addr()? 1 : 0;

//    node_info.version_id = tuya_get_firmware_version();

//    uint8_t mac[12],tmp;
//	
//    uint8_t ret = NV_USER_ITEM_LOAD(NV_USER_ITEM_MAC, mac, 12);
//	
//    ty_string_op_hexstr2hex(mac, 12, node_info.uuid);
		
		uint8_t mac[6], tmp;
		
		uint8_t ret = tuya_mesh_gap_addr_get(mac);
		
		memcpy(node_info.uuid, mac, 6);
		
    for(int8_t i=0; i<3; i++)
		{
        tmp = node_info.uuid[i];
        node_info.uuid[i] = node_info.uuid[5-i];
        node_info.uuid[5-i] = tmp;
    }
		
    node_info.uuid[6] = tuya_get_mesh_category() >> 8;
    node_info.uuid[7] = tuya_get_mesh_category() & 0xFF;
		
    memcpy(&(node_info.uuid[8]), tuya_get_pid(), 8);

//    if(get_fast_prov_en_state())
//		{
//        node_info.uuid[6] = node_info.uuid[6] | 0x40;
//    }
//    else
//		{
//        node_info.uuid[6] = node_info.uuid[6] & (~0x40);
//    }

//    tuya_mesh_node_init(&node_info);

//    u8 tuya_country_code = 0;
//	  u8 tuya_nv_sts = 0;
//    tuya_nv_sts = NV_USER_ITEM_LOAD(NV_USER_ITEM_COUNTRY_CODE, &tuya_country_code, 1);
//	  
//		if(0 != tuya_nv_sts)
//	  {
//		    tuya_country_code = 0;
//	  }
//		
//    if(COUNTRY_CODE_EU == tuya_country_code)
//		{
//        tuya_mesh_rf_power_set(TUYA_RF_POWER_P6p14dBm);
//    }
//    else
//		{
//        tuya_mesh_rf_power_set(TUYA_RF_POWER_P10p46dBm);
//    }	
	
	
}


unsigned int tuya_mesh_gap_addr_get(unsigned char *p_addr)
{
    uint32_t       err_code;
    struct bd_addr  temp_addr;
	  uint8_t         temp_length;
	
	  temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_addr.addr);
	
    memcpy(p_addr,temp_addr.addr,6);
    
    return 1;
}





#endif
