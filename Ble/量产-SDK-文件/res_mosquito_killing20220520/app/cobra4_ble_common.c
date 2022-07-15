/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_ble_common.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/7/9
	Description :
          把一部分 cobra4_common  关于蓝牙的函数移到这个文件中
	Changed Log	:
		Liujian  2021/7/9			- Creation

*****************************************************************************/
#include "cobra4_ble_common.h"
#include "cobra_system.h"
#include "prf.h"
#include "app.h"

#if (BLE_APP_OTA)
#include "rdw_ota.h"
#endif

#if (BLE_WECHAT_ENABLE)
#include "ble_wechat_service.h"
#endif


#if (COBRA_POWER_DOWN_ENABLE==1)
static unsigned char global_uuid_data[32] __attribute__((section("retention_area"),zero_init));
#else
static unsigned char global_uuid_data[32];
#endif

static unsigned int cobra4_common_uuid_data(void);

void convert_address_to_ascii(uint64_t address, unsigned char *p_buffer)
{
    uint32_t temp;

    if(p_buffer == NULL)  return ;

    p_buffer[5] = 0x30 + address%10;

    address /= 10;

    p_buffer[4] = 0x30 + address%10;

    address /= 10;

    p_buffer[3] = 0x30 + address%10;

    address /= 10;

    p_buffer[2] = 0x30 + address%10;

    address /= 10;

    p_buffer[1] = 0x30 + address%10;

    address /= 10;

    p_buffer[0] = 0x30 + address%10;

    p_buffer[6] = '\0';

}

//建立广播数据
void appm_build_adv_data(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf)
{
    unsigned char str_buf[32];

    uint8_t rem_len = max_length;
    uint8_t temp_length;

    uint32_t  mac_address;
    struct bd_addr  temp_bdaddr;

#if (BLE_APP_HID_ENABLE == 1)

    unsigned int length;
    memcpy(p_buf, APP_HID_ADV_DATA_APPEARANCE, APP_ADV_DATA_APPEARANCE_LEN);
    *p_length += APP_ADV_DATA_APPEARANCE_LEN;
    p_buf += APP_ADV_DATA_APPEARANCE_LEN;

    length = cobra4_common_uuid_data();
    if(length > 2)
    {
        memcpy(p_buf, global_uuid_data, length);
        *p_length += length;
        p_buf += length;
    }

#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    unsigned int length;

    length = cobra4_common_uuid_data();
    if(length > 2)
    {
        memcpy(p_buf, global_uuid_data, length);
        *p_length += length;
        p_buf += length;
    }

    p_buf[0] = sizeof("RW") + 1;
    p_buf[1] = 0x09;
    strcpy(&p_buf[2], "RW");
    *p_length += p_buf[0] + 1;
    p_buf += p_buf[0] + 1;
    return;


#endif

#if BLE_APP_RDW_VOICE
    unsigned int length;

    length = cobra4_common_uuid_data();
    if(length > 2)
    {
        memcpy(p_buf, global_uuid_data, length);
        *p_length += length;
        p_buf += length;
    }

    p_buf[0] = sizeof("WIRELESS") + 1;
    p_buf[1] = 0x09;
    strcpy(&p_buf[2], "WIRELESS");
    *p_length += p_buf[0] + 1;
    p_buf += p_buf[0] + 1;
    return;
#endif

#if (BLE_APP_UART == 1)

    const unsigned char app_uart_uuid[16] = {
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E
    };
    p_buf[0] = 17;
    p_buf[1] = 0x06; //128bit UUID
    p_buf += 2;
    *p_length += 2;
    memcpy(p_buf, app_uart_uuid, 16);
    *p_length += 16;
    p_buf += 16;

#if(APP_RANDOM_ADDRESS_ENABLE == 0)
    p_buf[0] = sizeof("UART") + 1;
    p_buf[1] = 0x09;
    strcpy(&p_buf[2], "UART");
    *p_length += p_buf[0] + 1;
    p_buf += p_buf[0] + 1;
#endif

#elif (BLE_APP_LED_BUTTON)
    {
        const unsigned char app_uart_uuid[16] =
        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x15, 0x23, 0x00, 0x00};

        p_buf[0] = 17;
        p_buf[1] = 0x06; //128bit UUID
        p_buf += 2;
        *p_length += 2;
        memcpy(p_buf, app_uart_uuid, 16);
        *p_length += 16;
        p_buf += 16;

        p_buf[0] = sizeof("SDK") + 1;
        p_buf[1] = 0x09;
        strcpy(&p_buf[2], "SDK");
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;

    }
#elif(DIANBIAO_MODULE_ENABLE == 1)
//2021/8/23 改动
    {

        if(app_get_ble_adv_length() > 3)
        {
            unsigned char *p_content = app_get_ble_adv_content();

            // 协议会自动添加这个AD 号,必须去掉这个 AD 号
            if((p_content[0] == 0x02) && (p_content[1] == 0x01) && (p_content[2] == 0x06) )
            {
                memcpy(p_buf, p_content + 3, app_get_ble_adv_length() - 3);


                *p_length += (app_get_ble_adv_length() - 3);
                p_buf += (app_get_ble_adv_length() - 3);

            }
            else
            {
                memcpy(p_buf, p_content + 3, app_get_ble_adv_length());
                *p_length += (app_get_ble_adv_length());
                p_buf += (app_get_ble_adv_length());
            }


        }
        else
        {

            int i ;

            unsigned char *p_pin = app_get_meter_master_pincode();

            p_buf[0] = 0x16;
            p_buf[1] = 0xff;

            p_buf[2] = 0xC1;    //设备类别码 0xC1 (电能表, 固定值)

            p_buf[3] = 0xFF;    //厂商代码    0xFF 0xFF ,厂家在国网 ID 号
            p_buf[4] = 0xFF;

            p_buf[5] = 0x00;
            p_buf[6] = 0x00;    //特征码 / 自动配对校验码

            if((p_pin[0] == 0xFF) && (p_pin[1] == 0xFF) && (p_pin[2] == 0xFF) && \
                    (p_pin[3] == 0xFF) && (p_pin[4] == 0xFF) && (p_pin[5] == 0xFF))
            {
                p_buf[5] = 0xFF;
                p_buf[6] = 0xFF;    //特征码 / 自动配对校验码
                for(i = 0; i < 16 ; i++)
                {
                    p_buf[7 + i] = 0xFF;
                }
            }
            else
            {
                p_buf[5] = 0x00;
                p_buf[6] = 0x01;
                for(i = 0; i < 6; i++)
                {
                    p_buf[7 + i] = p_pin[i];
                }
                for(; i < 16; i++)
                {
                    p_buf[7 + i] = 0xFF;
                }
            }

            *p_length += p_buf[0] + 1;
            p_buf += p_buf[0] + 1;
        }



    }
//		memcpy(p_buf, APP_ADV2, APP_ADV2_LEN);
//    *p_length += APP_ADV2_LEN;
//    p_buf += APP_ADV2_LEN;
//
//		memcpy(p_buf, APP_ADV3, APP_ADV3_LEN);
//    *p_length += APP_ADV3_LEN;
//    p_buf += APP_ADV3_LEN;
//

//		p_buf[0] = 0x09;
//		p_buf[1] = 0xff;
//		p_buf[2] = 0x01;
//		p_buf[3] = 0x18;
//
//		temp_length = BD_ADDR_LEN;
//    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
//
//    p_buf[4] =  temp_bdaddr.addr[0];
//		p_buf[5] =  temp_bdaddr.addr[1];
//		p_buf[6] =  temp_bdaddr.addr[2];
//		p_buf[7] =  temp_bdaddr.addr[3];
//		p_buf[8] =  temp_bdaddr.addr[4];
//		p_buf[9] =  temp_bdaddr.addr[5];
//
//		*p_length += 10;
//		p_buf += 10;
//

//		mac_address = 0;
//		mac_address <<= 8;
//		mac_address += temp_bdaddr.addr[4];

//	  mac_address <<= 8;
//		mac_address += temp_bdaddr.addr[3];
//
//		mac_address <<= 8;
//		mac_address += temp_bdaddr.addr[2];
//
//		mac_address <<= 8;
//		mac_address += temp_bdaddr.addr[1];
//
//		mac_address <<= 8;
//		mac_address += temp_bdaddr.addr[0];
//
//		p_buf[0] = 0x04;
//		p_buf[1] = 0x09;
//
//		convert_address_to_ascii(mac_address, str_buf);
//
//		p_buf[2] = str_buf[3];
//		p_buf[3] = str_buf[4];
//		p_buf[4] = str_buf[5];

//		p_buf += 5;
//		*p_length += 5;
#elif(APP_METER_AT_COMMAND == 1)
    {
        char *p_buffer =  app_meter_get_adv_name();
        uint8_t buffer_len = strlen(p_buffer);
        if(buffer_len == 0)
        {
#if(APP_METER_AS_SLAVE_ENABLE == 1)
            p_buf[0] = sizeof("METE") - 1 + 1;
            p_buf[1] = 0x09;
            strncpy((char *)&p_buf[2], "METE", sizeof("METE")-1);
#else
            p_buf[0] = sizeof("HOST") - 1 + 1;
            p_buf[1] = 0x09;
            strncpy((char *)&p_buf[2], "HOST", sizeof("HOST")-1);
#endif
        }
        else
        {
            p_buf[0] = 1 + buffer_len;
            p_buf[1] = 0x09;
            strncpy((char *)&p_buf[2], p_buffer, buffer_len);
        }
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;
#if(APP_METER_AS_SLAVE_ENABLE == 1)
        const unsigned char app_uart_uuid[16] =
        {0x79, 0x41, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};

        p_buf[0] = 17;
        p_buf[1] = 0x06; //128bit UUID
        p_buf += 2;
        *p_length += 2;
        memcpy(p_buf, app_uart_uuid, 16);
        *p_length += 16;
        p_buf += 16;

#endif
    }

#elif(BLE_APP_MESH == 1)
    {
        p_buf[0] = sizeof("MESH_OTA") + 1;
        p_buf[1] = 0x09;
        strcpy((char *)&p_buf[2], "MESH_OTA");
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;

    }
#elif(APP_WECHAT_ENABLE == 1)		
  {

		
		  p_buf[0] = 0x09;
		  p_buf[1] = 0xFF;
		  p_buf[2] = 0x56;   
		  p_buf[3] = 0x00;
		
		uint8_t temp_length;
		struct bd_addr  temp_bdaddr;
    temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
		
    p_buf[4] =  temp_bdaddr.addr[5];
		p_buf[5] =  temp_bdaddr.addr[4];
		p_buf[6] =  temp_bdaddr.addr[3];
		p_buf[7] =  temp_bdaddr.addr[2];
		p_buf[8] =  temp_bdaddr.addr[1];
		p_buf[9] =  temp_bdaddr.addr[0];	
		
		*p_length += p_buf[0] + 1;
		 p_buf += p_buf[0] + 1;
		 
		p_buf[0] = 0x03;
		p_buf[1] = 0x03;
		p_buf[2] = 0xE7;
		p_buf[3] = 0xFE;
		
		*p_length += p_buf[0] + 1;
		 p_buf += p_buf[0] + 1;
		 

#if 0		 
		p_buf[0] = sizeof("wechat") + 1;
		p_buf[1] = 0x09;
		
		strcpy(&p_buf[2], "wechat");
#endif
//		p_buf[0] = sizeof("LANHE-T1") + 1;
//		p_buf[1] = 0x09;
//		
//		strcpy(&p_buf[2], "LANHE-T1");		
		p_buf[0] = sizeof("YXYX") + 1;
		p_buf[1] = 0x09;
		
		strcpy(&p_buf[2], "YXYX");	
		
		*p_length += p_buf[0] + 1;
		 p_buf += p_buf[0] + 1;
		 
	}
#else
    if(cobra_nvds_param_ok())
    {
        const char *p_temp = (const char *)(M4_EFLASH_PARAM_ADDRESS + 16 + 6 + 1);
        strcpy(str_buf, p_temp);
        p_buf[0] = strlen(str_buf) + 1;
        p_buf[1] = 0x09;
        strcpy(&p_buf[2], str_buf);
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;
    }
    else
    {
        p_buf[0] = sizeof("SDK") + 1;
        p_buf[1] = 0x09;
        strcpy(&p_buf[2], "SDK");
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;
    }

#endif
}



static unsigned int cobra4_common_uuid_data(void)
{
    unsigned int length = 2;
    unsigned int index =  2;

    global_uuid_data[1] = 0x03;

#if BLE_APP_RDW_VOICE
    global_uuid_data[index++] = 0xF0;
    global_uuid_data[index++] = 0xFF;
    length += 2;
#endif

#if (BLE_APP_BLOOD)
    global_uuid_data[index++] = 0x10;
    global_uuid_data[index++] = 0x18;
    length += 2;
#endif

#if (BLE_APP_HT)
    global_uuid_data[index++] = 0x09;
    global_uuid_data[index++] = 0x18;
    length += 2;
#endif

#if (BLE_APP_HEART_RATE)
    global_uuid_data[index++] = 0x0D;
    global_uuid_data[index++] = 0x18;
    length += 2;
#endif

#if (BLE_APP_BATT_NO_HID)
//	 global_uuid_data[index++] = 0x0F;
//	 global_uuid_data[index++] = 0x18;
//	 length += 2;
#endif

#if (BLE_APP_HID_ENABLE)
    global_uuid_data[index++] = 0x12;
    global_uuid_data[index++] = 0x18;
    length += 2;
#endif

#if (CFG_APP_DIS)
//	 global_uuid_data[index++] = 0x0A;
//	 global_uuid_data[index++] = 0x18;
//	 length += 2;
#endif

#if(APP_WECHAT_ENABLE == 1)
    global_uuid_data[index++] = 0xE7;
    global_uuid_data[index++] = 0xFE;
    length += 2;
#endif

#if (BLE_APP_HID_ENABLE)
    global_uuid_data[index++] = 0x12;
    global_uuid_data[index++] = 0x18;
    length += 2;
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    global_uuid_data[index++] = 0x01;
    global_uuid_data[index++] = 0xA2;
    length += 2;
#endif

#if (BLE_APP_OTA)
    global_uuid_data[index++] = 0xF5;
    global_uuid_data[index++] = 0xFE;
    length += 2;
#endif

#if(APP_WECHAT_ENABLE == 1)
	 global_uuid_data[index++] = 0xE7;
	 global_uuid_data[index++] = 0xFE;
	 length += 2;
#endif
	 
    global_uuid_data[0] = length - 1;

    return length;

}


void mac_address_convert_to_str(unsigned char * mac_address_name)
{
    uint32_t  mac_address;

    struct bd_addr  temp_bdaddr;

    uint8_t temp_length;

    temp_length = BD_ADDR_LEN;

    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);

    mac_address = 0;
    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[4];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[3];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[2];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[1];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[0];

    convert_address_to_ascii(mac_address, mac_address_name);

}


void appm_build_scan_rsp_data(uint16_t *p_length, uint8_t *p_buf)
{

    unsigned int length;
#if(DIANBIAO_MODULE_ENABLE == 1)
    uint8_t temp_length;
    struct bd_addr  temp_bdaddr;

    memcpy(p_buf, APP_SCNRSP_DATA, APP_SCNRSP_DATA_LEN);

    *p_length += APP_SCNRSP_DATA_LEN;
    p_buf += APP_SCNRSP_DATA_LEN;

    p_buf[0] = 0x09;
    p_buf[1] = 0xFF;

    p_buf[2] = 0xFF;
    p_buf[3] = 0xFF;

    temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);

    p_buf[4] = temp_bdaddr.addr[0];
    p_buf[5] = temp_bdaddr.addr[1];
    p_buf[6] = temp_bdaddr.addr[2];
    p_buf[7] = temp_bdaddr.addr[3];
    p_buf[8] = temp_bdaddr.addr[4];
    p_buf[9] = temp_bdaddr.addr[5];

    return;
#endif
    length = cobra4_common_uuid_data();
    if(length > 2)
    {
        memcpy(p_buf, global_uuid_data, length);
        *p_length += length;
        p_buf += length;
    }
//名字不同
#if(APP_RANDOM_ADDRESS_ENABLE)
    {
        unsigned char uart_name[9];
        mac_address_convert_to_str(&uart_name[2]);
        uart_name[0]= 'R';
        uart_name[1]= 'W';

        p_buf[0] = strlen(uart_name) + 1;
        p_buf[1] = 0x09;
        strcpy(&p_buf[2], uart_name);
        *p_length += p_buf[0] + 1;
        p_buf += p_buf[0] + 1;
    }
#endif

}

uint8_t appm_get_dev_name(uint8_t* name)
{
    uint8_t temp_length;
    unsigned char str_buf[7];
    uint64_t  mac_address;
    struct bd_addr  temp_bdaddr;

#if(DIANBIAO_MODULE_ENABLE == 1)
    temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);

    mac_address = 0;
    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[4];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[3];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[2];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[1];

    mac_address <<= 8;
    mac_address += temp_bdaddr.addr[0];

    convert_address_to_ascii(mac_address, str_buf);

    memcpy(name, str_buf+3, 3);
    return 3;
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    strcpy(name, "RW");
    return sizeof("RW");
#elif(BLE_APP_MESH == 1)
    strcpy((char *)name, "MESH_UART");
    return sizeof("MESH_UART");
#endif

    if(cobra_nvds_param_ok())
    {
        const char *p_temp = (const char *)(M4_EFLASH_PARAM_ADDRESS + 16 + 6 + 1);
        strcpy((char *)name, p_temp);
        return (strlen(p_temp));
    }
    else
    {
#if(APP_METER_AT_COMMAND == 1)
        return 0;
#else

#if(APP_RANDOM_ADDRESS_ENABLE)
        unsigned char uart_name[9];
        mac_address_convert_to_str(&uart_name[2]);
        uart_name[0]= 'R';
        uart_name[1]= 'W';
        strcpy(name, uart_name);
        return strlen(uart_name);
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
        strcpy((char *)name, "YXYX");
        return sizeof("YXYX");
#else		

        strcpy((char *)name, "UART");
        return sizeof("UART");
#endif

#endif
    }
}

#if (BLE_APP_MESH == 1)

void app_ble_mesh_update_param(void)
{
    struct gapc_conn_param conn_param;
    conn_param.intv_min =  8;
    conn_param.intv_max =  8;
    conn_param.latency =   3;
    conn_param.time_out = 500;
    host_appm_update_param(&conn_param, TASK_APP);
}
#endif


#if (BLE_APP_MESH == 0)

static uint8_t dummy_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void *params)
{
    return GAP_ERR_NO_ERROR;
}

static void dummy_destroy(struct prf_task_env* env)
{

}

static void dummy_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Nothing to do */
}

static void dummy_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{

}

const struct prf_task_cbs dummy_itf =
{
    (prf_init_fnct) dummy_init,
    dummy_destroy,
    dummy_create,
    dummy_cleanup,
};

const struct prf_task_cbs* mal_prf_itf_get(void)
{
    return &dummy_itf;
}

#endif

/*
  这个函数主要用于profile 内部调用, 找到 自定义的 pforile.
  自己写的Profile 请添加到这里
*/
const struct prf_task_cbs* rdw_custom_define_profile(uint32_t task_id)
{
    const struct prf_task_cbs* prf_cbs = NULL;

#if APP_DWT_MASTER
    if (task_id == TASK_ID_MASTER)
    {
        prf_cbs = master_prf_itf_get();
    }
#endif

#if BLE_APP_RDW_VOICE                //无线语音发送端
    if(task_id == TASK_ID_VOICE)
    {
        prf_cbs = voice_prf_itf_get();
    }
#endif

#if BLE_APP_OTA                     // OTA 下载, 通过串口 发送数据到设备, 串口协议为 z-mode
    if(task_id == TASK_ID_OTA)
    {
        prf_cbs = ota_prf_itf_get();
    }
#endif

#if APP_SOUND_RECEIVER_ENABLE             //无线语音 接收端
    if (task_id == TASK_ID_VOICE_MASTER)
    {
        prf_cbs = voice_master_prf_itf_get();
    }
#endif

#if BLE_APP_UART
    if(task_id == TASK_ID_UART)
    {
        prf_cbs = uart_prf_itf_get();
    }
#endif

#if BLE_DEVICE1_UART_ENABLE
    if(task_id == TASK_ID_DEVICE1_UART)
    {
        prf_cbs = device1_uart_prf_itf_get();
    }
#endif

#if (APP_SCAN_DEVICE_ENABLE	)
#if 	(BLE_DEVICE1_UART_CLIENT_ENABLE)
    if(task_id == TASK_ID_DEVICE1_COLLECTOR_UART)
    {
        prf_cbs = device1_uart_collector_prf_itf_get();
    }
#endif

#if (BLE_DEVICE2_UART_CLIENT_ENABLE)
    if(task_id == TASK_ID_DEVICE2_COLLECTOR_UART)
    {
        //	prf_cbs = device2_uart_collector_prf_itf_get();
    }
#endif

#if (BLE_DEVICE3_UART_CLIENT_ENABLE)
    if(task_id == TASK_ID_DEVICE3_COLLECTOR_UART)
    {
        //		prf_cbs = device3_uart_collector_prf_itf_get();
    }
#endif

#endif

#if (BLE_APP_BLOOD)
    if( task_id == TASK_ID_BLPS)
    {
        prf_cbs = blps_prf_itf_get();
    }
#endif

#if (BLE_APP_HEART_RATE)
    if( task_id == TASK_ID_HRPS)
    {
        prf_cbs = hrps_prf_itf_get();
    }
#endif

#if BLE_APP_BATT_NO_HID
    if(task_id == TASK_ID_BASS)
    {
        prf_cbs = bass_prf_itf_get();
    }
#endif

#if (BLE_MESH)
    if(task_id ==  TASK_ID_MESH)
    {
        prf_cbs = mal_prf_itf_get();
    }
#endif //(BLE_MESH)	

#if (BLE_HID_DEVICE)
    if(task_id == TASK_ID_HOGPD)
    {
        prf_cbs = hogpd_prf_itf_get();
    }
#endif // (BLE_HID_DEVICE)

#if (APP_OTA_MATSER_ENABLE == 1)
    if( task_id == TASK_ID_OTA_MASTER)
    {
        prf_cbs = ota_master_prf_itf_get();
    }
#endif

#if (BLE_APP_LED_BUTTON)
    if( task_id == TASK_ID_LED_BUTTON)
    {
        prf_cbs = led_button_prf_itf_get();
    }
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))
    if(task_id == TASK_ID_AT_CMD_SLAVE)
    {
        prf_cbs = slave_uart_prf_itf_get();
    }
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
    if(task_id == TASK_ID_DEVICE1_COLLECTOR_UART)
    {
        prf_cbs = master_uart_collector_prf_itf_get();
    }
#endif

#if(TUYA_BLE_SDK_ENABLE == 1)
    if(task_id == TASK_ID_TUYA_BLE)
    {
        prf_cbs = tuya_ble_prf_itf_get();
    }
#endif

#if (BLE_WECHAT_ENABLE)
     if(task_id == TASK_ID_WECHAT) 
		 {
         prf_cbs = wechat_prf_itf_get();
     }
#endif
		 
    return prf_cbs;

}



void ble_update_adv_data(uint8_t const* p_ad_data, uint8_t ad_len, enum KE_TASK_TYPE task_number)
{

    struct gapm_set_adv_data_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                          TASK_GAPM, task_number,
                                          gapm_set_adv_data_cmd,
                                          ADV_DATA_LEN);

    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = app_env.adv_actv_idx;

//   memcpy(tuya_ble_adv_data, p_ad_data, ad_len);
    //	tuya_ble_adv_length = ad_len;

    p_cmd->length = ad_len;

    memcpy(&p_cmd->data[0], p_ad_data, p_cmd->length);

    ke_msg_send(p_cmd);

//		TUYA_APP_LOG_INFO("update_adv_data.....\n");

}


