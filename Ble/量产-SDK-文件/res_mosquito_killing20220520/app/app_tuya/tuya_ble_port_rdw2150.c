/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	tuya_ble_port_rdw2150.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/4/30

	Description :   ²Î¿¼nordic µÄ52832, ÒÆÖ² Í¿Ñ» SDK 
	Changed Log	:

		Liujian  2021/4/30		- Creation
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tuya_aes.h"
#include "md5.h"
#include "hmac.h"
#include "tuya_ble_port.h"
#include "tuya_ble_type.h"
#include "tuya_app_ble_services.h"
#include "co_bt.h"
#include "nvds.h"
#include "cobra_soft_timer.h"
#include "cobra_timer.h"
//#include "cobra_platform_util.h"
#include "cobra4C_eflash_ctrl.h"


tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const* p_ad_data, uint8_t ad_len)
{
    update_adv_data(p_ad_data,ad_len);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const *p_sr_data, uint8_t sr_len)
{
    update_scan_rsp_data(p_sr_data,sr_len);
    return TUYA_BLE_SUCCESS;
}



tuya_ble_status_t tuya_ble_gap_addr_get(tuya_ble_gap_addr_t *p_addr)
{
    uint32_t       err_code;
    struct bd_addr  temp_addr;
	  uint8_t         temp_length;
	
	  temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_addr.addr);
	
    if(temp_addr.addr[5] & 0xC0)
    {
        p_addr->addr_type = TUYA_BLE_ADDRESS_TYPE_RANDOM;
    }
    else
    {
        p_addr->addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;
    }
    
    memcpy(p_addr->addr,temp_addr.addr,6);
    
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t *p_addr)
{
    uint32_t       err_code;
    struct bd_addr bt_addr;
	
    memcpy(bt_addr.addr,p_addr->addr,6);
	
	  nvds_put(PARAM_ID_BD_ADDRESS, sizeof(struct bd_addr), (uint8_t *)&bt_addr);
	
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t *p_data,uint16_t len)
{
    uint8_t data_len = len;
	
    if(data_len > TUYA_BLE_DATA_MTU_MAX)
    {
        data_len = TUYA_BLE_DATA_MTU_MAX;
    }
		
    app_tuya_ble_send_mtu(p_data, data_len);
		
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_rand_generator(uint8_t* p_buf, uint8_t len)
{
    uint32_t cnt=len/4;
    uint8_t  remain = len%4;
    int32_t temp;
    for(uint32_t i=0; i<cnt; i++)
    {
        temp = rand();
        memcpy(p_buf,(uint8_t *)&temp,4);
        p_buf += 4;
    }
    temp = rand();
    memcpy(p_buf,(uint8_t *)&temp,remain);

    return TUYA_BLE_SUCCESS;
}

#define TIMER_MAX_NUM               4

#define ERROR_TIMER_ID              TIMER_MAX_NUM + 1 
typedef struct
{
    uint8_t is_avail;
	
    uint32_t data;
	
    uint32_t timeout_value_ms;
	
	  uint32_t  mode;
	
} tuya_ble_nrf_timer_item_t;

static tuya_ble_nrf_timer_item_t m_timer_pool[TIMER_MAX_NUM] = {
    [0] = { .is_avail = 1},
    [1] = { .is_avail = 1},
    [2] = { .is_avail = 1},
    [3] = { .is_avail = 1}
};

static int32_t * acquire_timer(uint32_t timeout_value_ms)
{
    uint8_t i;
    for (i = 0; i < TIMER_MAX_NUM; i++) 
    {
        if (m_timer_pool[i].is_avail) 
        {
            m_timer_pool[i].is_avail = 0;
            m_timer_pool[i].timeout_value_ms = timeout_value_ms;
            return &m_timer_pool[i].data ;
        }
    }
    return NULL;
}

static int32_t release_timer(void* timer_id)
{
    for (uint8_t i = 0; i < TIMER_MAX_NUM; i++) 
	  {
        if (timer_id == &m_timer_pool[i].data) 
				{
            m_timer_pool[i].is_avail = 1;
            return i;
        }
    }
    return -1;
}

static int32_t find_timer_index_value(void* timer_id)
{
	  for (uint8_t i = 0; i < TIMER_MAX_NUM; i++) 
	  {
        if (timer_id == &m_timer_pool[i].data) 
				{
            return i;
        }
    }
    return -1;
	
}

static int32_t find_timer_timeout_value(void* timer_id,uint32_t *value)
{
    for (uint8_t i = 0; i < TIMER_MAX_NUM; i++) 
	  {
        if (timer_id == &m_timer_pool[i].data) 
				{
            *value = m_timer_pool[i].timeout_value_ms;
            return i;
        }
    }
    return -1;
}


tuya_ble_status_t tuya_ble_timer_create(void** p_timer_id,uint32_t timeout_value_ms, tuya_ble_timer_mode mode,tuya_ble_timer_handler_t timeout_handler)
{
    tuya_ble_status_t ret = TUYA_BLE_SUCCESS;

  	int32_t index;

    int32_t *id  = acquire_timer(timeout_value_ms);
	
    if (id == NULL)
    {
        return TUYA_BLE_ERR_NOT_FOUND;
    }

    uint32_t m = mode == TUYA_BLE_TIMER_SINGLE_SHOT ? 0 : 1;
    
    timer_handler_t handler = (timer_handler_t)timeout_handler;
    
    *id = cobra_soft_timer_create(handler);
    
    *p_timer_id = id;
			
		if(*id >= 0)
		{
			 index  =  find_timer_index_value(id);
			 
			 m_timer_pool[index].mode = m;
			
			 return TUYA_BLE_SUCCESS;
    }
		
    return TUYA_BLE_ERR_NO_MEM;

}


tuya_ble_status_t tuya_ble_timer_delete(void* timer_id)
{
    uint32_t errno;
    int id = release_timer(timer_id);
    if (id == -1)
    {
        return TUYA_BLE_ERR_INVALID_PARAM;
    }

    cobra_soft_timer_stop(*(uint32_t *)timer_id);
		cobra_soft_timer_destroy(*(uint32_t *)timer_id);
		
    return TUYA_BLE_SUCCESS;

}

tuya_ble_status_t tuya_ble_timer_start(void* timer_id)
{
    uint32_t errno;
    uint32_t timeout_value_ms;
	  int32_t  index;
    
	  index =  find_timer_timeout_value(timer_id,&timeout_value_ms);
	
    if(index >= 0 )
    {
        cobra_soft_timer_start(*(uint32_t *)timer_id,  (COBRA_SOFT_TIMER_TICK) * (timeout_value_ms / 1000), m_timer_pool[index].mode);
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}

tuya_ble_status_t tuya_ble_timer_restart(void* timer_id,uint32_t timeout_value_ms)
{
    uint32_t errno;
    uint32_t temp;
	
    int32_t  index;
	
	  index =  find_timer_timeout_value(timer_id,&timeout_value_ms);
	
    if(index >= 0)
    {
         cobra_soft_timer_stop(*(uint32_t *)timer_id);

			   cobra_soft_timer_start(*(uint32_t *)timer_id,  (COBRA_SOFT_TIMER_TICK) * (timeout_value_ms / 1000), m_timer_pool[index].mode);
        
			   return TUYA_BLE_SUCCESS;
        
    }
    else
    {
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}


tuya_ble_status_t tuya_ble_timer_stop(void* timer_id)
{
     cobra_soft_timer_stop(*(uint32_t *)timer_id);
	
	   return TUYA_BLE_SUCCESS;
	
}


void tuya_ble_device_delay_ms(uint32_t ms)
{
	    cobra_soft_delay_ms(ms);
}

 
static uint32_t  volatile  cobra_irq_rest = 0;    

void tuya_ble_device_enter_critical(void)
{
    cobra_irq_rest = __get_PRIMASK();  	
		__set_PRIMASK(1); 
}

void tuya_ble_device_exit_critical(void)
{
    if(cobra_irq_rest == 0)    										
		{																						
				__set_PRIMASK(0);												
		}																						
		else																				
		{																						
				__set_PRIMASK(1);												
		}												
}


tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t *timestamp,int32_t *timezone)
{
    *timestamp = 0;
    *timezone = 0;
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp,int32_t timezone)
{

    return TUYA_BLE_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////

tuya_ble_status_t tuya_ble_nv_init(void)
{    
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr,uint32_t size)
{    
    tuya_ble_status_t result = TUYA_BLE_SUCCESS;

    uint32_t erase_pages;
    
    /* make sure the start address is a multiple of FLASH_ERASE_MIN_SIZE */
    if(addr % TUYA_NV_ERASE_MIN_SIZE != 0)
    {
        log_d("the start address is a not multiple of TUYA_NV_ERASE_MIN_SIZE");
        return TUYA_BLE_ERR_INVALID_ADDR;
    }
		
		cobra_flash_erase(addr, size);
    
    return result;
}

tuya_ble_status_t tuya_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size)
{  
	  cobra_tuya_write_nv_data(addr, (unsigned char *)p_data, size);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size)
{
   
	  memcpy(p_data, (uint32_t*)addr, size);
	
    return TUYA_BLE_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////
bool tuya_ble_aes128_ecb_encrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_ecb_decrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_cbc_encrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    
    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_ENCRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_cbc_decrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    
    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_DECRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}


bool tuya_ble_md5_crypt(uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, input, input_len);
    mbedtls_md5_finish(&md5_ctx, output);
    mbedtls_md5_free(&md5_ctx);    
    
    return true;
}

bool tuya_ble_hmac_sha1_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input, uint32_t input_len, uint8_t *output)
{    
    //hmac_sha1_crypt(key, key_len, input, input_len, output);
	return true;
}

bool tuya_ble_hmac_sha256_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input, uint32_t input_len, uint8_t *output)
{
   // hmac_sha256_crypt(key, key_len, input, input_len, output);
	return true;
}


