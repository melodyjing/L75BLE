/*
 
 这个是整个应用的配置文件, 参数的改动需要谨慎 
 
*/
#ifndef __STACK_CONFIG_H__
#define __STACK_CONFIG_H__


#define CFG_BLE
#define CFG_EMB

#define CFG_HOST
#define CFG_APP
#define CFG_PRF
#define CFG_APP_SEC

#define CFG_ALLROLES
#define CFG_RF_ATLAS

#define CFG_RF_ATLAS_V24   


#define CFG_HCITL
//#define CFG_AHITL

#define CFG_NB_PRF	    8
#define CFG_CON			    7          //liujian add
#define	CFG_ACT			    8
#define CFG_SEC_CON
#define CFG_RAL			    3

#define RDW_CGF_NVDS_SRAM           1
#define CFG_NVDS
#define COBRA4_ASIC_EN				1

#define COBRA4C_INSIDE_SPI_FLASH_ENABLE    0

#define APP_ENABLE_WATCHDOAG			    0
#define APP_UART_TASK_ENABLE		      0    // 串口输出通过任务的方式输出
//#define CFG_APP_CONSOLE_CMD			    1    //与 APP_UART 不能同时使用

#define CFG_PRODUCT_HCI_MODE_ENABLE   0    //确定是否需要在产品阶段需要HCI 接综测仪

////////////////////////////////// 调试功能  //////////////////////////////////////////

//#define COBRA_SOFT_UART_ENABLE    1
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// Application 配置定义 ////////////////////////////////

#define  BLE_APP_NORMAL_ENABLE     1            //普通蓝牙应用使能
#define  DIANBIAO_MODULE_ENABLE    0           //电表功能的使能
#define  APP_METER_AT_COMMAND      0            //电表 AT command 模组  
#define  BLE_APP_MESH			         0            // 使用 MESH

        
/////////////////////////////////// 以下的配置使用者不要随便改动 ///////////////////////////
#if(BLE_APP_MESH == 1)
#undef DIANBIAO_MODULE_ENABLE
#undef BLE_APP_NORMAL_ENABLE
#endif

#if(DIANBIAO_MODULE_ENABLE == 1)
#undef BLE_APP_NORMAL_ENABLE
#endif

////////////////////////////////////////////////////////////////////////////////////////
#if (APP_METER_AT_COMMAND == 1)
#define  APP_AT_COMMAND_ENABLE       1
#define  COBRA_LOGGER_ENABLE         1
#undef   APP_RDW_SHELL_ENABLE

#define  APP_METER_AS_SLAVE_ENABLE   0

#endif

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////电表应用的配置   ////////////////////////////////////////////////
#if(DIANBIAO_MODULE_ENABLE == 1)
#define  DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE    0
/****************Realtek 客户电表蓝牙 begin**************/
#define  DIANBIAO_PROTOCOL_REALTEK_ENABLE     1    // 请打开 DIANBIAO_MODULE_ENABLE，关闭同类 DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE等

#if((DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE ==1) &&(DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1))
   #error error config ! NOT ALL ONES        
#elif((DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE ==0) &&(DIANBIAO_PROTOCOL_REALTEK_ENABLE == 0))
   #erro error configer ! NOT ALL ZER0ES
#endif

/////////////////////////////////// 电表模块的功能 ///////////////////////////////////
//代表支持两个Host 连接
#define BLE_HOST_UART                         1       // 支持一个主广播
#define BLE_HOST2_UART		                    1       // 支持第二个广播
#define APP_HOST2_ENABLE			                1
#define APP_SCAN_DEVICE_ENABLE		            1       //支持扫描
#define DIANBIAO_ENABLE_PRIV_EN					      1       //使用私有协议地址
#define APP_METER_DEBUG_ENABLE		            0       //为1 的时候,测试是否重新启动了

#define BLE_DEVICE1_UART_ENABLE                 1       //支持广播中支持的 profile

#define BLE_DEVICE3_UART_CLIENT_ENABLE	        1            //支持连接几个 连接
#define BLE_DEVICE2_UART_CLIENT_ENABLE	        1
#define BLE_DEVICE1_UART_CLIENT_ENABLE          1

#define DIANBIAO_AUTO_ENABLE				            0
#define DUANLUQI_ENABLE							            0

#define BLE_DIANBIAO_CALCUALTE_ENABLE           1
#define DIANBIAO_SPM_ENABLE                     1     //设置 SMP 等级命令使能

#define CFG_APP_CONSOLE_CMD			                1
#if(DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1)
#include "../app/app_iris/iris_config.h"
#endif

#if(DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1)
#include "../app/app_realtek/pj_lf.h"
#endif

#endif

////////////////// 普通应用的配置 //////////////////////////////////////
#if(BLE_APP_NORMAL_ENABLE == 1)
#include "../app/src/normal_config.h"
#endif


////////////////  MESH应用的配置  ////////////////////////////////////// 
#if(BLE_APP_MESH == 1)
#include "../app/app_mesh/app_mesh_config.h"
#endif

#endif





