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

#define COBRA4C_INSIDE_SPI_FLASH_ENABLE    0      //使用内部 SPI flash 来保存数据

#define APP_ENABLE_WATCHDOAG			    0
#define APP_UART_TASK_ENABLE		      0    // 串口输出通过任务的方式输出
//#define CFG_APP_CONSOLE_CMD			    1    //与 APP_UART 不能同时使用

#define CFG_PRODUCT_HCI_MODE_ENABLE   0    //确定是否需要在产品阶段需要HCI 接综测仪

////////////////////////////////// 调试功能  //////////////////////////////////////////

//#define COBRA_SOFT_UART_ENABLE    1
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// Application 配置定义 ////////////////////////////////

#define   APP_OTA_MATSER_ENABLE   1        // Ê¹ÄÜ OTA master ¹¦ÄÜ, Í¨¹ý´®¿Ú¿ÉÒÔÏÂÔØ³ÌÐò
#define COBRA_LINK_LIBRARY_ENABLE           1
#define COBRA_LINKEN_SOFT_TIMER_ENABLE      1
#if(APP_OTA_MATSER_ENABLE == 1)
#define APP_RDW_SHELL_ENABLE   1
#undef  CFG_APP_CONSOLE_CMD
#endif

#endif





