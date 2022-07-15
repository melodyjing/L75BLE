
#ifndef __PJ_LH_H__
#define __PJ_LH_H__
/****************Realtek 客户电表蓝牙 begin**************/
#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
#undef DIANBIAO_SLAVE_PARAM_SET_ENABE
#undef DIANBIAO_AUTO_ENABLE
#define DIANBIAO_AUTO_ENABLE             1

#define ADV_INTER_SELF_DEFINE             1     // 自定义广播间隔事件

#define ADV_DATA_SELF_DEFINE             1     //  自定应广播数据

#define ADV_RESQ_SELF_DEFINE             1     //  自定应广播响应数据

#define SCAN_ENABLE_FUCTION              1     //   添加扫描使能控制功能

#define BLE_DATA_RECEIVE_MODE            1      //  设置或读取蓝牙链路数据接收模式

#define CMD_PD_UPDATE                     1

#define CMD_DEV_NAME_FUNCTION            1      //

#define CMD_TX_POWER_FUNCTION            1      //

#define CMD_DEV_ADDR_FUNCTION            1      //

#define CMD_ADV_INTERVAL_FUNCTION            1      //

#define CMD_DEV_PAIR_MODE_FUNCTION            1      //

#define CMD_DEV_PAIR_VALUE_FUNCTION            1      //

#define CMD_LINK_CMD_FUNCTION            1      //

#define CMD_ADV_FUNCTION            1      //

#define CMD_FIRMWARE_BUILD_TIME_FUNCTION            1      //

#define CMD_LINK_RSSI_FUNCTION            1      //

#define CMD_UART_PARAM_FUNCTION            1      //

#define  DIANBIAO_ADV_ONLY_ONE_ENABLE           1    //每次只广播一个广播, 当连接之后, 另外一个广播才开始广播
                                                //如果为 0, 那么空中广播两个同样的地址的广播信号

#endif  //DIANBIAO_PROTOCOL_REALTEK_ENABLE
/****************Realtek 客户电表蓝牙 end**************/

#endif



