/*
Liujian

电表普通协议的配置文件

*/

#ifndef __IRIS_CONFIG_H___
#define __IRIS_CONFIG_H___

#define  DIANBIAO_MASTER_PARAM_SET_ENABE        0     // 设置作为主设备 时, 基本连接参数使能
#define  DIANBIAO_ADV_ONLY_ONE_ENABLE           0    //每次只广播一个广播, 当连接之后, 另外一个广播才开始广播
                                                //如果为 0, 那么空中广播两个同样的地址的广播信号
#define  DIANBIAO_ADV_CONTENT_SET_ENABLE     1    //广播内容设置使能

#define  DIANBIAO_BLE_WORK_MODE_SET_ENABLE   0    //蓝牙工作模式设置使能

#define  DIANBIAO_SLAVE_PARAM_SET_ENABE      0     //设置作为从设备时, 基本连接参数使能

																								
#endif
