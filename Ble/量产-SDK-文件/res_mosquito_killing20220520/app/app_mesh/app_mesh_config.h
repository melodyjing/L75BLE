/*
Liujian

MESH 应用的配置文件

*/

#ifndef __APP_MESH_CONFIG_H__
#define __APP_MESH_CONFIG_H__
/****************Realtek 客户电表蓝牙 end**************/
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

#define CFG_BLE_MESH_MDL_TSCNS    1             //Time 服务器打开

/////////////////////////////////////////////////////////////////////////////////////////
#define BLE_APP_MESH_TIANMAO_LIGHT		0    //和天猫精灵 连接
#define BLE_APP_MESH_CLIENT_LIGHT     0    //作为一个灯 的控制设备, 连接开关, 控制灯
#define BLE_APP_MESH_SERVER_LIGHT			1    //灯, 作为服务端, 连接灯
#define BLE_APP_MESH_TULIPE_LIGHT     0    //灯 Tulips

#define BLE_TUYA_MESH_SDK_ENABLE      0     // 涂鸦  MESH

///////////////////////////////////////////////////////////////////////////////////////////////////
#define APP_RDW_SHELL_ENABLE          1

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
#define APP_COBRA_RTC_EN			   1
#define BM_DETECT                1
#define BLE_APP_MESH_IOS_ENABLE  1
#define SOFT_PWM                 1
#define COBRA_SCENE_EDIT_ENABLE  1
#define COBRA_POWER_DOWN_SAVE_ENABLE  1
#define COBRA_SCENE_VERSION2_ENABLE   1
#define COBRA_EMU_BOARD				0

#endif

#if(BLE_TUYA_MESH_SDK_ENABLE == 1)
#define COBRA4_MMU_ENABLE          1
#define COBRA_LINK_LIBRARY_ENABLE  1
#define COBRA_LINKEN_SOFT_TIMER_ENABLE  1
#endif


#endif
