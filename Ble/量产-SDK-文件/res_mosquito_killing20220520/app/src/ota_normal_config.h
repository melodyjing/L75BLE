/*
Liujian
普通蓝牙应用的配置文件

*/
#ifndef __NORMAL_CONFIG_H__
#define __NORMAL_CONFIG_H__

///////////////////////////////普通蓝牙profile ///////////////////////////////////////
#define   BLE_APP_UART				    1        //自定义串口 Profile 使能
#define   BLE_APP_BLOOD           1        // 血压计 Profile 使能
#define   BLE_APP_HEART_RATE		  1        // 心率计使能  profile
#define   BLE_APP_BATT_NO_HID     0        //电池使能  profile
#define   BLE_APP_DIS             0        //设备信息

#if(BLE_APP_UART == 1)
#define   CFG_PRF_UART
#else
#undef    CFG_PRF_UART
#endif

#if(BLE_APP_BLOOD == 1)
#define   CFG_PRF_BLPS
#else
#undef    CFG_PRF_BLPS
#endif

#if(BLE_APP_HEART_RATE == 1)
#define   CFG_PRF_HRPS					          
#else
#undef    CFG_PRF_HRPS
#endif

#if(BLE_APP_BATT_NO_HID == 1)
#define   CFG_PRF_BASS	
#else
#undef   CFG_PRF_BASS	
#endif

#if(BLE_APP_DIS == 1)
#define   CFG_PRF_DISS
#else
#undef    CFG_PRF_DISS
#endif


#if(BLE_APP_UART==1)
#undef   CFG_APP_CONSOLE_CMD
#endif

#endif
