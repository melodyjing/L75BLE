/*
Liujian
��ͨ����Ӧ�õ������ļ�

*/
#ifndef __NORMAL_CONFIG_H__
#define __NORMAL_CONFIG_H__

///////////////////////////////��ͨ����profile ///////////////////////////////////////

#define   BLE_APP_UART				    1        //�Զ��崮�� Profile ʹ��
#define   BLE_APP_BLOOD           0        // Ѫѹ�� Profile ʹ��
#define   BLE_APP_HEART_RATE		  0        // ���ʼ�ʹ��  profile

#define   BLE_APP_HID_ENABLE      0        //���߼���    ʹ�ܺ�,  ��ò�Ҫ��ʹ������Ӧ��

#define   BLE_APP_LED_BUTTON      0        //��ʾ �ֻ�����LED ���� ��������Ϣ���ֻ�

#define   BLE_APP_BATT_NO_HID     0        //���ʹ��  profile
#define   BLE_APP_DIS             0        //�豸��Ϣ
#define   APP_OTA_MATSER_ENABLE   0        // ʹ�� OTA master ����, ͨ�����ڿ������س���
                                           // �������Ĺ��ܶ���Ҫ��
#define   APP_RDW_SHELL_ENABLE    0

#define   APP_RF_NORMAL_PROTOCOL_ENABLE    0    //�������ܲ���Ҫ, ʹ�� RF ģ����Ϊ��Ƶģ��

#define   APP_MOUSE_ENABLE         0


//****************** ��������ң�����Ŀ��� *************************/
#define   APP_SOUND_RECEIVER_ENABLE			0            //������������һ��
#define   APP_SOUND_SEND_ENABLE         0            //ң������һ��
/////////////////////////////////////////////////////////////////////////////
#if(APP_SOUND_SEND_ENABLE == 1)      
//������������͵ķ��Ͷ�
#define BLE_APP_RDW_VOICE				1
#define RDW_VOICE_PROFILE				1
#define COBRA4_AUDIO_EN         1
#define COBRA4_PLL_EN       1
//#define COBRA_PDM_EN				  1
#define COBRA_I2S_EN					  1
#elif(APP_SOUND_RECEIVER_ENABLE == 1)
//������������ն�
#define BLE_APP_RECEIVE_VOICE		1
#define BLE_VOICE_RECOGNIZE_IS_ENABLE		1     //���Ҫʹ��

#define COBRA4_PLL_EN               1
#define COBRA_I2S_EN					      1
#define COBRA_I2C_EN					      1
#define COBRA_WM8978_WIRELESS_EN    1
#define COBRA_RINGBUFFER_EN			    1  

#define TASK_SOUND_RECIVER  TASK_RESERVE_00
#define TASK_VOICE_DEVICE   TASK_RESERVE_01
#undef  APP_OTA_MATSER_ENABLE

#endif


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

#if ((APP_SOUND_RECEIVER_ENABLE == 1) || (APP_SOUND_SEND_ENABLE == 1) || (BLE_APP_MESH == 1))
#undef CFG_APP_HT	                    //���¼�Profile ʹ��
#undef BLE_APP_UART				
#undef CFG_PRF_UART
#undef BLE_APP_BLOOD    
#undef CFG_PRF_BLPS
#undef CFG_PRF_HRPS					
#undef BLE_APP_HEART_RATE		
#undef CFG_PRF_BASS					      
#undef BLE_APP_BATT_NO_HID
#undef CFG_PRF_LED_BUTTON						
#undef BLE_APP_LED_BUTTON	
#undef APP_OTA_MATSER_ENABLE
#undef CFG_PRF_DISS    
#undef CFG_APP_DIS 
#undef BLE_APP_HID_ENABLE
#endif


#if(BLE_APP_HID_ENABLE == 1)
#define CFG_PRF_HOGPD
#define COBRA_BUTTON_EN          1
#define COBRA4_BLE_WHITELIST_EN  1
#undef  BLE_APP_UART
#undef  BLE_APP_BLOOD
#undef  BLE_APP_HEART_RATE
#define COBRA_LINK_LIBRARY_ENABLE           1
#define COBRA_LINKEN_SOFT_TIMER_ENABLE      1
#endif

#if(BLE_APP_LED_BUTTON == 1)
#define CFG_PRF_LED_BUTTON							// LED �� BUTTON Profile ʹ��
#define COBRA_BUTTON_EN          1
#undef  BLE_APP_UART
#endif

//--------------------------------------------------------------------------------------
#if(BLE_APP_UART==1)

#define  UART_MODEL_TIMEOUT_ENABLE       1         //ʹ�õ�ģ��ʹ��

////////////////////////////////
#if (UART_MODEL_TIMEOUT_ENABLE == 0)
#undef   CFG_APP_CONSOLE_CMD
#undef   APP_RDW_SHELL_ENABLE
#else
#define  CFG_APP_CONSOLE_CMD     1
#endif
////////////////////////////////
#define  APP_RANDOM_ADDRESS_ENABLE  1      //ÿ��ģ��� MAC ��ַ��ͬ �� ���ֲ�ͬ
#define  APP_SLEEP_MODE_ENABLE      0         //���Խ���͹���ģʽ
#endif

//----------------------------------------------------------------------------------------

#if(APP_OTA_MATSER_ENABLE == 1)
#undef  APP_RDW_SHELL_ENABLE
#define APP_RDW_SHELL_ENABLE   1
#undef  CFG_APP_CONSOLE_CMD
#define COBRA_LINK_LIBRARY_ENABLE           1
#define COBRA_LINKEN_SOFT_TIMER_ENABLE      1
#endif


#if(APP_RF_NORMAL_PROTOCOL_ENABLE == 1)

#define  APP_RF_MODULE_SEND_ENABLE       1
#define  APP_RF_MODULE_RECEIVE_ENABLE    0

#endif

#if (APP_MOUSE_ENABLE == 1)

#define COBRA_APP_ADC_DATA_EN       1
#define APP_MOUSE_TEST_EMI_ENABLE  0

#endif




#endif
