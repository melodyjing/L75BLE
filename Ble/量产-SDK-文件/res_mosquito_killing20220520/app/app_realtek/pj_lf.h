
#ifndef __PJ_LH_H__
#define __PJ_LH_H__
/****************Realtek �ͻ�������� begin**************/
#if DIANBIAO_PROTOCOL_REALTEK_ENABLE
#undef DIANBIAO_SLAVE_PARAM_SET_ENABE
#undef DIANBIAO_AUTO_ENABLE
#define DIANBIAO_AUTO_ENABLE             1

#define ADV_INTER_SELF_DEFINE             1     // �Զ���㲥����¼�

#define ADV_DATA_SELF_DEFINE             1     //  �Զ�Ӧ�㲥����

#define ADV_RESQ_SELF_DEFINE             1     //  �Զ�Ӧ�㲥��Ӧ����

#define SCAN_ENABLE_FUCTION              1     //   ���ɨ��ʹ�ܿ��ƹ���

#define BLE_DATA_RECEIVE_MODE            1      //  ���û��ȡ������·���ݽ���ģʽ

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

#define  DIANBIAO_ADV_ONLY_ONE_ENABLE           1    //ÿ��ֻ�㲥һ���㲥, ������֮��, ����һ���㲥�ſ�ʼ�㲥
                                                //���Ϊ 0, ��ô���й㲥����ͬ���ĵ�ַ�Ĺ㲥�ź�

#endif  //DIANBIAO_PROTOCOL_REALTEK_ENABLE
/****************Realtek �ͻ�������� end**************/

#endif



