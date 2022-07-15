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

   
#define   APP_WECHAT_ENABLE  1
#define   BLE_WECHAT_ENABLE  1

#define   COBRA_MULTI_TIMER_ENABLE  1
#define   COBRA_APP_BUTTON_EN   1
#define   COBRA_BUTTON_EN       1

//#define   CATCH_LOG   1

#define  APP_MOSQUITO_KILLING_ENABLE  1
#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#define COBRA_APP_ADC_DATA_EN  1
#define COBRA_ADC_EN_VERSION2   1

#define COBRA_MPWM_EN            1
#define COBRA_MPWM_NEW           1


#define APP_MOSQUITO_KILLING_WATCHDOG_ENABLE  1

#define APP_MOSQUITO_KILLING_TIMING_ENABLE  1

#define APP_MOSQUITO_KILLING_NEW_VER_HARDWARE  1

#define APP_MOSQUITO_KILLING_HD_I2C_ENABLE  0
#if (APP_MOSQUITO_KILLING_HD_I2C_ENABLE == 1)
#undef  COBRA_I2C_EN
#define COBRA_I2C_EN             1
#define COBRA_I2C_NEW            1
#endif

#define APP_MOSQUITO_KILLING_POWER_ON_PWM_ENABLE  0
#define APP_MOSQUITO_POWER_ORDER_ENABLE  1

#define APP_MOSQUITO_KILLING_DETECT_ADC_5MS_PERIOD_ENABLE  1

#define APP_COBRA_RTC_EN			   1
#define APP_MOSQUITO_RTC_ACCURACY_TEST_ENABLE  0

#define APP_MOSQUITO_UUID_DEBUG_211201_ENABLE  1

#define APP_MOSQUITO_ADD_TEST_CODE_ENABLE  1

#define APP_MOSQUITO_BT_CONNECT_DEBUG_211217_ENABLE  1

#define APP_MOSQUITO_KILLING_OTA_ENABLE  1

#define APP_MOSQUITO_KILLING_DETECT_DIFF_ENABLE  0

#define APP_MOSQUITO_KILLING_DETECT_AVERAGE_DIFF_ENABLE 1

#define  APP_MOSQUITO_KILLING_DETECT_LIMIT_ENABLE  0

#define  APP_MOSQUITO_KILLING_DETECT_LINEAR_ENABLE  1

#define  APP_MOSQUITO_KILLING_BAUDRATE_FIX   0

#define  APP_MOSQUITO_KILLING_DISCHARGE_TIME_DEBUG_220119   0
#endif

/*************************************************************************************/
/*print*/

extern unsigned char TRACE_LEVEL;
//#define  TRACE_LEVEL  1

#define CATFILENAME(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x

#define  __direct_printf(...)  if(TRACE_LEVEL > 0) \
						{     \
							printf(__VA_ARGS__);  \
						}


#define  __here  if(TRACE_LEVEL > 0) \
				{     \
					printf("(%d):%s()-%s \n", __LINE__, __FUNCTION__, CATFILENAME(__FILE__));  \
				}

#define  __printf(...)  if(TRACE_LEVEL > 0) \
						{     \
							printf("(%d)-%s:%s ", __LINE__,CATFILENAME(__FILE__),__FUNCTION__);   \
							printf(__VA_ARGS__);  \
						}

#define  __direct_printf_flash(...)  if(TRACE_LEVEL > 1) \
						{     \
							printf(__VA_ARGS__);  \
						}

#define  __direct_printf_statistics(...)  if(TRACE_LEVEL > 1) \
						{     \
							printf(__VA_ARGS__);  \
						}

/*************************************************************************************/

#endif





