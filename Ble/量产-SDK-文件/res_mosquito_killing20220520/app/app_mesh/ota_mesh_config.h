/*
Liujian

MESH 应用的配置文件

*/

#ifndef __OTA_MESH_CONFIG_H__
#define __OTA_MESH_CONFIG_H__
/****************************/
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// MESH 功能模块 /////////////////////////////////////////////////////
//Mesh 关于 Mesh 的配置在这里
#define CFG_BLE_MESH							1     //Mesh 功能打开
#define CFG_BLE_MESH_MSG_API  		1
#define CFG_BLE_MESH_GATT_PROXY		1
#define CFG_BLE_MESH_GATT_PROV		1
#define CFG_BLE_MESH_RELAY        1

#define CFG_BLE_MESH_MDL_CLIENT		1
#define CFG_BLE_MESH_MDL_SERVER		1

#define CFG_BLE_MESH_MDL_GENS			1              //通用开关服务
#define CFG_BLE_MESH_MDL_GENC			1							 //通用开关客户

#define CFG_BLE_MESH_MDL_LIGHTS		1
#define CFG_BLE_MESH_MDL_LIGHTC		1

#define CFG_BLE_MESH_MDL_TSCNS    1            

#define BLE_APP_MESH_TIANMAO_LIGHT		0   
#define BLE_APP_MESH_CLIENT_LIGHT     0    
#define BLE_APP_MESH_SERVER_LIGHT			0  
#define BLE_APP_MESH_TULIPE_LIGHT  1   

#define APP_RDW_SHELL_ENABLE          0

#if(BLE_APP_MESH_CLIENT_LIGHT == 1)
#define COBRA_BUTTON_EN             1
#define COBRA_MULTI_TIMER_ENABLE    1
#define COBRA_APP_BUTTON_EN		      1  
#endif

#if(BLE_APP_MESH_TIANMAO_LIGHT == 1)
#define COBRA4_MMU_ENABLE          1
#define COBRA_LINK_LIBRARY_ENABLE  1
#define COBRA_LINKEN_SOFT_TIMER_ENABLE  1
#endif

#if(BLE_APP_MESH_TULIPE_LIGHT == 1)

#define COBRA_MPWM_EN            1
#define COBRA_SPWM_EN            1
#define COBRA_MPWM_NEW           1
#define APP_COBRA_RTC_EN			   0
#define BM_DETECT                1
#define BLE_APP_MESH_IOS_ENABLE  1
#define SOFT_PWM                 0
#define COBRA_SCENE_EDIT_ENABLE  1
#define COBRA_POWER_DOWN_SAVE_ENABLE  1
#define COBRA_SCENE_VERSION2_ENABLE   1
#define COBRA_EMU_BOARD				0

#define APP_FLASH_LIGHT_NUMBERS    1    
#define APP_LIGHT_BREATH_FUNCTION  1

#define COBRA_FLASH_POWERON_CRASE   1

#define COBRA_APP_ADC_DATA_EN      0
#define APP_TULIP_BATTER_DETECT_VERSION2  0

#endif

#endif
