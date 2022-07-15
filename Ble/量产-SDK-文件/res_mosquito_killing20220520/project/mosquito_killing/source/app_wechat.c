/**@file            app_wechat.c
* @brief            
* @details          
* @author      			Liujian
* @date             2021-10-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/

#include "app_wechat.h"
#include "ble_wechat_util.h"
#include "mpbledemo2.h"
#include "cobra_system.h"
#include "cobra_button.h"
#if(APP_WECHAT_ENABLE == 1)

void app_wechat_button_button(void);

extern data_handler *m_mpbledemo2_handler;

void	data_handler_init(data_handler** p_data_handler, uint8_t product_type)
{
	if (*p_data_handler == NULL) 
		{
			*p_data_handler = get_handler_by_type(product_type);
		}
}

//function for register all products
 void register_all_products(void) {
		REGISTER(mpbledemo2);
}
 

void app_wechat_ble_init(void)
{
	 register_all_products();
	 data_handler_init(&m_mpbledemo2_handler,	PRODUCT_TYPE_MPBLEDEMO2);
	 m_mpbledemo2_handler->m_data_init_func();

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#else
	 app_wechat_button_button();
#endif	
}

unsigned char test_buffer[10];
void app_wechat_ble_loop(void)
{
	  m_mpbledemo2_handler->m_data_main_process_func(NULL);
}

void app_wechat_process_rx_data(unsigned char *buffer, unsigned int length)
{
	 m_mpbledemo2_handler->m_data_consume_func(buffer, length);
}

void get_mac_addr(uint8_t *p_mac_addr)
{
		uint32_t error_code;
    uint8_t temp_length;
	  uint8_t i;
		struct bd_addr  temp_bdaddr;
    temp_length = BD_ADDR_LEN;
    nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
	
		for( i = 0; i < 6; i++)
		{	
			p_mac_addr[5-i]= temp_bdaddr.addr[i];
		}

}


#define BUTTON_KEY_NUMBER			GPIO_PIN_29
#define BUTTON_PIN_INDEX			PIN29_INDEX

static struct Button cobra_key_SW9;

unsigned char app_wechat_read_cobra_key_SW9() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(BUTTON_KEY_NUMBER);
	//printf("key_value = 0x%x \n", key_value);
	return key_value;
}


void app_wechat_button_init(void)
{
	   app_m3_gpio_set_direction(BUTTON_KEY_NUMBER, GPIO_INPUT);
	
		 cobra_pin_pullup_enable(BUTTON_PIN_INDEX, 1);
	
		 button_init(&cobra_key_SW9, app_wechat_read_cobra_key_SW9, GPIO_LOW);
	
}


static void app_wechat_button_key_down_press(void * param)
{
    printf("1. Key press down message .... \n");
	  uint32_t err_code;
	  err_code = m_mpbledemo2_handler->m_data_button_handler_func(NULL , 0);
	
}

void app_wechat_button_button(void)
{
		 app_wechat_button_init();
	
		 button_attach(&cobra_key_SW9, PRESS_DOWN, app_wechat_button_key_down_press);
//		button_attach(&cobra_key_SW9, PRESS_UP, app_key_sw9_key_up_press);
//	  button_attach(&cobra_key_SW9, LONG_PRESS_HOLD, app_key_sw9_key_hold_press);
	
  	 button_start(&cobra_key_SW9);
}


#endif

