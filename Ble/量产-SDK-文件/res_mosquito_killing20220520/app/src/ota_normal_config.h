/*
Liujian
��ͨ����Ӧ�õ������ļ�

*/
#ifndef __NORMAL_CONFIG_H__
#define __NORMAL_CONFIG_H__

///////////////////////////////��ͨ����profile ///////////////////////////////////////
#define   BLE_APP_UART				    1        //�Զ��崮�� Profile ʹ��
#define   BLE_APP_BLOOD           1        // Ѫѹ�� Profile ʹ��
#define   BLE_APP_HEART_RATE		  1        // ���ʼ�ʹ��  profile
#define   BLE_APP_BATT_NO_HID     0        //���ʹ��  profile
#define   BLE_APP_DIS             0        //�豸��Ϣ

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
