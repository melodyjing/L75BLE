/*	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Lufeng
	
	Cobra Drivers

	File Name	:	app_realtek_console.c
	Version		:	0.1
	Created	By	:	Lufeng
	Date		:	2021/1/7

	Description :   
	  串口命令, 用于实现 68 协议		

	Changed Log	:

		Lufeng  2021/1/7			- Creation

*****************************************************************************/
#if (CFG_APP_CONSOLE_CMD && DIANBIAO_PROTOCOL_REALTEK_ENABLE)
#include "app_realtek_console.h"
#include "app_realtek_device.h"
#include "app_realtek_uart_cmd_handler_export.h"
#if BLE_DATA_RECEIVE_MODE

extern struct app_host_uart_env_tag app_host_env_1;
extern struct app_host_uart_env_tag app_host_env_2;

extern struct connection_device_env_tag  connection_devive1_env;
extern struct connection_device_env_tag  connection_devive2_env;
extern struct connection_device_env_tag  connection_devive3_env;

struct sch_realtek_rece_env_tag  dianbiao_recelist;
void realtek_client_handle_data(struct sch_realtek_rece_elt_tag * elt);
void realtek_handle_host_data(struct sch_realtek_rece_elt_tag * elt);
//struct sch_realtek_rece_elt_tag elt_realtek[5];
#define REALTEK_ALLOC_EVT(data_struct)           (struct data_struct *) ke_malloc(sizeof(struct data_struct), KE_MEM_ENV);

bool realtek_malloc_elt(void)
{
	if(dianbiao_recelist.cnt<7)
	{
		dianbiao_recelist.elt_prog = REALTEK_ALLOC_EVT(sch_realtek_rece_elt_tag);
		dianbiao_recelist.cnt++;
		return true;
	}
	else 
	{
		printf("WARM TOO MANY!");
		return false;
	}
}
void realtek_client_rx_data_handle(UINT8 conidx,struct uart_client_rx_data_ind  CONST *param)
{
	if(realtek_malloc_elt())
	{
	dianbiao_recelist.elt_prog->conidx = conidx;
	dianbiao_recelist.elt_prog->length = param->length;
	memcpy(dianbiao_recelist.elt_prog->rx_data, param->value, param->length);
	co_list_push_back(&dianbiao_recelist.elt_wait, &dianbiao_recelist.elt_prog->hdr);
	}
}
void realtek_slave_rx_data_handle(UINT8 conidx,struct device1_uart_rx_data  CONST *param)
{
	if(realtek_malloc_elt())
	{	
	dianbiao_recelist.elt_prog->conidx = conidx;
	dianbiao_recelist.elt_prog->length = param->length;
	memcpy(dianbiao_recelist.elt_prog->rx_data, param->rx_data, param->length);
	co_list_push_back(&dianbiao_recelist.elt_wait, &dianbiao_recelist.elt_prog->hdr);
	}
}
void realtek_pop_data_handle(void)
{
	if(dianbiao_recelist.cnt > 0)
	{
	struct sch_realtek_rece_elt_tag *first_elt = (struct sch_realtek_rece_elt_tag *)co_list_pop_front(&dianbiao_recelist.elt_wait);
	
	if(first_elt->conidx == app_host_env_1.connection_index)
	{
		realtek_handle_host_data(first_elt);
	}
	else if(first_elt->conidx == app_host_env_2.connection_index)
	{
		realtek_handle_host_data(first_elt);
	}
	else if(first_elt->conidx == connection_devive1_env.conidx)
	{
		realtek_client_handle_data(first_elt);
	}
	else if(first_elt->conidx == connection_devive2_env.conidx)
	{
		realtek_client_handle_data(first_elt);
	}
	else if(first_elt->conidx == connection_devive3_env.conidx)
	{
		realtek_client_handle_data(first_elt);
	}	
	ke_free(first_elt);
	dianbiao_recelist.cnt--;
}
}
#endif
unsigned char app_get_ble_adv_length(void)
{
//	 return global_adv_length;
	return 0;
}
unsigned char * app_get_ble_adv_content(void)
{
	return NULL;
//	  return global_adv_content;
}
//建立广播数据
void appm_build_adv_data1(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf)
{
    unsigned char str_buf[32];
	
    uint8_t rem_len = max_length;
		uint8_t temp_length;
	
		uint32_t  mac_address;
		struct bd_addr  temp_bdaddr;

		memcpy(p_buf, APP_ADV2, APP_ADV2_LEN);	
    *p_length += APP_ADV2_LEN;
    p_buf += APP_ADV2_LEN;
	
		memcpy(p_buf, APP_ADV3, APP_ADV3_LEN);
    *p_length += APP_ADV3_LEN;
    p_buf += APP_ADV3_LEN;
	

		p_buf[0] = 0x09;
		p_buf[1] = 0xff;
		p_buf[2] = 0x01;
		p_buf[3] = 0x18;
		
		temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
		
    p_buf[4] =  temp_bdaddr.addr[0];
		p_buf[5] =  temp_bdaddr.addr[1];
		p_buf[6] =  temp_bdaddr.addr[2];
		p_buf[7] =  temp_bdaddr.addr[3];
		p_buf[8] =  temp_bdaddr.addr[4];
		p_buf[9] =  temp_bdaddr.addr[5];
		
		*p_length += 10;
		p_buf += 10;
		

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
		
		#if CMD_DEV_NAME_FUNCTION
		p_buf[0] = app_env.dev_name_len + 1;
		p_buf[1] = 0x09;
		
		memcpy(&p_buf[2],app_env.dev_name,app_env.dev_name_len);
		
		p_buf += app_env.dev_name_len + 2;
		*p_length += app_env.dev_name_len + 2;
		#else
	  p_buf[0] = 0x04;
		p_buf[1] = 0x09;
		
		convert_address_to_ascii(mac_address, str_buf);
		
		p_buf[2] = str_buf[3];
		p_buf[3] = str_buf[4];
		p_buf[4] = str_buf[5];
    
		p_buf += 5;
		*p_length += 5;
    #endif		
}
#endif