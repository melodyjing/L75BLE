/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_calculate_meter.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/9

	Description :   
	  电表端, 用于校表		

	Changed Log	:

		Liujian  2020/11/9			- Creation


*****************************************************************************/
#include "app_calculate_meter.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "cobra_timer.h"
#include "cobra_ceva_baseband.h"
#include "rdw_mcu.h"
#include <string.h>
#include "cobra_aon_dcdc.h"
#include "device1_scan.h"
#include "cobra_uart.h"
#include "app_iris_console.h"

#if(BLE_DIANBIAO_CALCUALTE_ENABLE  && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)

#define METER_SEND_HEAD  0x0F

#define METER_DEBUG  0
typedef enum {
   METER_SEND_DATA,
   METER_RECEIVE_ACK,
   METER_DELAY_STATE,
   METER_RECEIVE_DATA,
   METER_IDLE_STATE,	
}METER_WORK_STATE;

static unsigned int        meter_send_channel;
static  APP_CSEM_PROTOCOL  csem_meter_protocol;
static METER_WORK_STATE    meter_state;
static unsigned char       calculate_plus_type;
static APP_CAL_FREQUENCY   meter_calculate_param;    //目前校准的参数
static unsigned char       address_is_odd_even;
static unsigned char       ack_data_request;
static unsigned char       send_pluse_data_ok;    //判断脉冲数据发送是否正确收到

static unsigned char       mete_ack_data[256];

static ble_meter_struct    backup_24g_data;     //保存 2.4G 模式下台体发送过来的数据  

static void app_iris_meter_calculate_timer_delay_callback(void);
void app_meter_send_data_to_manager_ic(void);
void meter_send_ack_data(void);

//判断 通讯 地址的 1 的 个数 是齐还是偶数
unsigned char app_meter_address_is_odd_even(unsigned int address)
{
	   unsigned int i;
	   unsigned char even_number = 0;
	   for(i = 0; i < 32; i++)
	   {
		    if(address & (1<<i))
				{
					 even_number++;
				}
	   }
		 if(even_number&0x01)  return 1;
		 return 0;
	
}

/*
   发射功率加大5DB
*/
static void app_meter_add5db(void)
{
	  unsigned char temp1;
	  unsigned char temp2;
	  unsigned short temp;
    cobra_aon_dcdc_ctrl_buck(0x0f);	
	
	  temp1 = cobra_ceva_baseband_csem_register_read(CS565_REG_PMU_TRIM);
	  temp2 = cobra_ceva_baseband_csem_register_read(CS565_REG_PMU_TRIM+1);
	  temp = temp2;
	  temp <<= 8;
	  temp += temp1;
	  temp |= (0x1f << 5);
	  temp1 = temp&0xff;
	  temp2 = temp>>=8;
	  cobra_ceva_baseband_csem_register_write(CS565_REG_PMU_TRIM,   temp1);
	  cobra_ceva_baseband_csem_register_write(CS565_REG_PMU_TRIM+1, temp2);
}

//秒脉冲
static void app_key_pin8_function(unsigned char  param)
{
	  volatile unsigned char level;
	  level = app_m3_gpio_read_pin(GPIO_PIN_8);
	  level = app_m3_gpio_read_pin(GPIO_PIN_8);
	  if(level)
		{
			  app_m3_gpio_enable_interrupt(GPIO_PIN_8, GPIO_LEVEL_LOW, app_key_pin8_function);			
			  app_meter_send_event(calculate_plus_type, 1);
		}
		else
		{
			  app_m3_gpio_enable_interrupt(GPIO_PIN_8, GPIO_LEVEL_HIGH, app_key_pin8_function);
			  app_meter_send_event(calculate_plus_type, 0);
		}	
#if(METER_DEBUG == 1)		
	       app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_HIGH);	
#endif		
		meter_state = METER_SEND_DATA;

		app_meter_send_data();
		
		send_pluse_data_ok = 0;
	
    meter_send_channel++;

		app_cobra_timer1_enable(1);
		
	   printf("B8\n");
}


//有功脉冲
static void app_key_pin3_function(unsigned char  param)
{
	    volatile unsigned char level;
	    level = app_m3_gpio_read_pin(GPIO_PIN_3);
	    level = app_m3_gpio_read_pin(GPIO_PIN_3);
	    if(level)
		{
				app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_LOW, app_key_pin3_function);
			  app_meter_send_event(calculate_plus_type, 1);
		}
		else
		{
				app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_HIGH, app_key_pin3_function);
			  app_meter_send_event(calculate_plus_type, 0);
		}	
		
//		printf("B3\n");

		meter_state = METER_SEND_DATA;
		app_meter_send_data();
		
		send_pluse_data_ok = 0;
		
    meter_send_channel++;
		
		app_cobra_timer1_enable(1);
		
}

void app_meter_false_send_data(void)
{
	   app_meter_send_event(calculate_plus_type, 0);
	
	   meter_state = METER_SEND_DATA;
		
		 app_meter_send_data();
		
		 send_pluse_data_ok = 0;
		
     meter_send_channel++;
		
		 app_cobra_timer1_enable(1);
		
}


//电表发送数据后的处理
void app_meter_tx_over_process(void)
{
	  app_csem_set_receive_mode();
}

static unsigned char meter_communicate_data[250];
static unsigned char meter_communicate_index = 0x00;

//电表接收数据的处理
void app_meter_rx_over_process(void)
{
	     unsigned char  i;
	     unsigned char  head;
	     unsigned char  status;
	     unsigned char  length;
	     unsigned char  control_word = 0x00;
	     unsigned char  good_control_word = 0x00;
	
	     if(address_is_odd_even)
		   {
			     good_control_word |= 0x80;
		   }
		 
	     status = cobra_ceva_baseband_csem_register_read(CS565_REG_DESER_STATUS);
			 
		   printf("status = 0x%2x \n", status);
	   
       if((status&0x43) == 0x41)
       {
	         head = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
	         length = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
				 
            if(head == 0x0B)  //ACK DATA    0xD0
            {
							   // printf("0x%02x \n", cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO));
              //    meter_send_channel = 0xFF;    //设置为最大,表示不再重复发送脉冲数据了
							
							    send_pluse_data_ok = 1;
            }
            else if(head == 0x0F)   //通信数据包   0xF0
            {
							  // printf("good .... \n");
                 if(length >= 1)
                 {
									     good_control_word |= 0x30;
                       control_word = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
									 
									   //  printf("control_word = 0x%02x  0x%02x \n", control_word, good_control_word);
									     
                       for(i = 0; i < (length - 1); i++)
                       {
                             meter_communicate_data[meter_communicate_index++] = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
                       }		 
                       if(control_word == good_control_word)
                       {
						                   app_meter_send_data_to_manager_ic();   //把 F20B 扩展规约发送到管理芯
                               meter_communicate_index = 0x00;
                       }
                 }
           }
			
      }		 
	    app_csem_set_receive_mode();		
}

//2ms 中断处理
//时序处理
static void app_iris_meter_calculate_timer_callback(void)
{

	  if((meter_send_channel < 5)&& (meter_state == METER_SEND_DATA)&& (send_pluse_data_ok == 0))
		{
			   app_csem_idle_mode();
			   app_iris_set_frequency(meter_calculate_param.frequecny[0]);
			   app_iris_set_whitelist_value(meter_calculate_param.frequecny[0]);
			   app_meter_send_data();
			   meter_send_channel++;	
			 //  printf("lose packet \n");			
         return;			
		}
		
	  if(((meter_send_channel >= 5) && (meter_state == METER_SEND_DATA)) || (send_pluse_data_ok))
		{
#if(METER_DEBUG == 1)		
	       app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_LOW);
#endif			
		//	printf("2222\n");	
          send_pluse_data_ok = 0;
			    meter_send_channel = 0;
			    meter_state = METER_DELAY_STATE;
			    app_csem_idle_mode();
			    app_iris_set_frequency(meter_calculate_param.frequecny[0]);
			    app_iris_set_whitelist_value(meter_calculate_param.frequecny[0]);
			    app_csem_set_receive_mode();
		}
		else if(meter_state == METER_DELAY_STATE)
		{
			   meter_state = METER_RECEIVE_DATA;
			  // printf("3333\n");	
		   //如果在通信窗口内收到脉冲信号， 转为发送脉冲信号模式

		}
		else if(meter_state == METER_RECEIVE_DATA)
		{
			  app_cobra_timer1_enable(0);

			  cobra_timer_set_frequency(APB2_TIMER1, SEND_DATA_HZ);
			  meter_state = METER_IDLE_STATE;
			//  printf("4444\n");	
		}
		
}

/*
在通讯的空闲阶段，发现电表有数据需要发送到台体的时候，可以在这里发送数据
给台体
*/
void app_metetr_process_commucate_data(void)
{
		if(meter_state == METER_IDLE_STATE)
		{
			  if(ack_data_request)
				{
					 meter_send_ack_data();
					 ack_data_request = 0;
				}
		}
}


/*
pluse_type 是测量的脉冲类型
0x00-有功，
0x01-无功，
0x02-秒脉冲，
0x03-正向谐波，
0x04-反向谐波，
0x05-需量，  
0x06 -时段投切
*/

void app_iris_calculate_meter_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type)
{
	   if(p_cal_param->frequency_number <= 0) return;
	
	   printf("meter work as calcualte mode number = %d\n", p_cal_param->frequency_number);
	
	   meter_send_channel = 0;
	
	   ack_data_request = 0;
	
	   address_is_odd_even = app_meter_address_is_odd_even(address);
	
	   memcpy((unsigned char *)&meter_calculate_param, (unsigned char *)p_cal_param, sizeof(APP_CAL_FREQUENCY));
	
	   app_iris_calculate_init(p_cal_param->frequecny[0]);
	
	   printf("frequency = %d \n", p_cal_param->frequecny[0]);

	   app_csem_idle_mode();
	   app_csem_pluse_calculate_init();
	
	   app_iris_set_frequency(p_cal_param->frequecny[0]);
		 app_iris_set_access_address(address);
		 app_iris_set_whitelist_value(p_cal_param->frequecny[0]);
		 
	 //  app_meter_add5db();    //发射功率加大5DB
	   switch(p_cal_param->power_level)
		 {
			 case LEVEL_4DB:    //4db
			 {
				 app_meter_add5db();
				 break;  
			 }
			 case LEVEL_0DB:   //0db
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
				  break;
			 }
			 case LEVEL_NEG_4DB:  //-4db
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x09);
				  break;
			 }
			 case LEVEL_NEG_8DB:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x06);
				  break;
			 }
			 case LEVEL_NEG_20DB:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x00);				 
				  break;
			 }
			 default:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
				  break;
			 }
		 }
		 
		// cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
		 calculate_plus_type = pluse_type;
	   

     if(pluse_type == PLUSE_ACTIVE)   //有功脉冲   输入为 GPIO8 
		 {			 
		    cobra_pin_pullup_enable(PIN8_INDEX, 1);
		    cobra_pin_input_enable(PIN8_INDEX, 1);
		    app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
		    app_m3_gpio_enable_interrupt(GPIO_PIN_8, GPIO_LEVEL_LOW, app_key_pin8_function);		
     }
		 
		 else //if(pluse_type == 0x00)  //如果是秒脉冲,   输入为 GPIO3
		 {
			  cobra_pin_pullup_enable(PIN3_INDEX, 1);
		    cobra_pin_input_enable(PIN3_INDEX, 1);
		    app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_INPUT);
		    app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_LOW, app_key_pin3_function);	
		 }


#if(METER_DEBUG == 1) 
	   app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_OUTPUT);
		 app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_LOW);
#endif		 
		 meter_state = METER_IDLE_STATE;
				 
		 app_cobra_timer1_enable(0);
		 app_cobra_timer1_set_callback(app_iris_meter_calculate_timer_callback);
	   app_cobra_init_timer1(SEND_DATA_HZ);   //2ms
		 
		 app_cobra_timer0_enable(0);
		 app_cobra_timer0_set_callback(app_iris_meter_calculate_timer_delay_callback);
	   app_cobra_init_timer0(0.2);   //3s
		 
		 
		 
		 app_csem_irq_enable(CSEM_IRQ_RECEIVED_FLAG);
		 app_csem_set_receive_mode();
		 
		 app_csem_irq_enable(CSEM_IRQ_TX_FLAG);
		 
	   app_csem_set_tx_callback(app_meter_tx_over_process);
		 app_csem_set_rx_callback(app_meter_rx_over_process);
		 
		 NVIC_EnableIRQ(CSEM_IRQ_TX);
		 NVIC_ClearPendingIRQ(PAD_TRIGGER_SINGLE_IRQ);
		 NVIC_EnableIRQ(PAD_TRIGGER_SINGLE_IRQ);
		 NVIC_SetPriority(PAD_TRIGGER_SINGLE_IRQ, 0);
		 NVIC_EnableIRQ(CSEM_IRQ_RECEIVED);	
	   NVIC_SetPriority(CSEM_IRQ_RECEIVED, 0);
}

/*
   电能表上报给蓝牙/脉冲转换器的脉冲EVENT数据包格式
   长度部分定义（1字节）：     0x01
   载荷部分定义（1字节）： 
   bit4~bit7      bit3~bit1                             Bit0
   预留          脉冲类型，                             脉冲电平
                 0x00-有功，                            0-低电平， 
                 0x01-无功，                            1-高电平  
                 0x02-秒脉冲，
                 0x03-正向谐波，
                 0x04-反向谐波，
                 0x05 -需量，
                 0x06 -时段投切   
*/
void app_meter_send_event(PLUSE_TYPE pluse_type, unsigned char level)
{
	      volatile unsigned char message = 0x00;
	
	      switch(pluse_type)
				{
					case PLUSE_PER_SECOND:
						message |= (0x00 << 1);
						break;
					case PLUSE_DEMAND:
						message |= (0x01 << 1);
						break;
					case PLUSE_SWITCH_TIME:
						message |= (0x02 << 1);
						break;
					case PLUSE_FORWARD_HARMONICS:
						message |= (0x03 << 1);
						break;
					case PLUSE_INVERSE_HARMONICS:
						message |= (0x04 << 1);
						break;
					case PLUSE_REACTIVE:
						message |= (0x05 << 1);
						break;
					case PLUSE_ACTIVE:
						message |= (0x06 << 1);
						break;
					default:
						break;
				}
				
//	      switch(pluse_type)
//				{
//					case PLUSE_ACTIVE:
//						message |= (0x00 << 1);
//						break;
//					case PLUSE_REACTIVE:
//						message |= (0x01 << 1);
//						break;
//					case PLUSE_PER_SECOND:
//						message |= (0x02 << 1);
//						break;
//					case PLUSE_FORWARD_HARMONICS:
//						message |= (0x03 << 1);
//						break;
//					case PLUSE_INVERSE_HARMONICS:
//						message |= (0x04 << 1);
//						break;
//					case PLUSE_DEMAND:
//						message |= (0x05 << 1);
//						break;
//					case PLUSE_SWITCH_TIME:
//						message |= (0x06 << 1);
//						break;
//					default:
//						break;
//				}
				if(level)
				{
					 message |= 0x01;
				}
				else
				{
					 message &= ~0x01;
				}
				
				if(address_is_odd_even)
				{
					  message |= 0x80;
				}
				
	      csem_meter_protocol.head = 0x0F;
				csem_meter_protocol.length = 0x1;
				csem_meter_protocol.payload[0] = message; //message;
				
				printf("t = %d \n", pluse_type);
}

//void app_meter_param_process(ble_meter_struct * p_meter_param)
//{
//	 if(p_meter_param->pluse_type == PLUSE_ERROR)
//	 {
//		 platform_watchdog_reset(0);
//	 }
//	 meter_calculate_param.power_level = p_meter_param->power_level;
//	 meter_calculate_param.frequency_number = p_meter_param->frequency_number;
//	 memcpy(meter_calculate_param.frequecny, p_meter_param->frequency, p_meter_param->frequency_number);
//	 
//	 switch(meter_calculate_param.power_level)
//	  {
//			 case LEVEL_4DB:    //4db
//			 {
//				 app_meter_add5db();
//				 break;  
//			 }
//			 case LEVEL_0DB:   //0db
//			 {
//				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
//				  break;
//			 }
//			 case LEVEL_NEG_4DB:  //-4db
//			 {
//				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x09);
//				  break;
//			 }
//			 case LEVEL_NEG_8DB:
//			 {
//				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x06);
//				  break;
//			 }
//			 case LEVEL_NEG_20DB:
//			 {
//				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x00);				 
//				  break;
//			 }
//			 default:
//			 {
//				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
//				  break;
//			 }
//	 }
//		
//	 calculate_plus_type = p_meter_param->pluse_type;
//	 
//   if(p_meter_param->pluse_type == PLUSE_ACTIVE)   //有功脉冲   输入为 GPIO8 
//	 {			 
//		    cobra_pin_pullup_enable(PIN8_INDEX, 1);
//		    cobra_pin_input_enable(PIN8_INDEX, 1);
//		    app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
//		    app_m3_gpio_enable_interrupt(GPIO_PIN_8, GPIO_LEVEL_LOW, app_key_pin8_function);	
//            app_m3_gpio_disable_interrupt(GPIO_PIN_3);		 
//     }		 
//	 else //if(pluse_type == 0x00)  //如果是秒脉冲,   输入为 GPIO3
//	 {
//			  cobra_pin_pullup_enable(PIN3_INDEX, 1);
//		    cobra_pin_input_enable(PIN3_INDEX, 1);
//		    app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_INPUT);
//		    app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_LOW, app_key_pin3_function);	
//		    app_m3_gpio_disable_interrupt(GPIO_PIN_8);	
//	  }

//}


void app_meter_param_process_uart_command(ble_meter_struct * p_meter_param)
{
	 unsigned int i;
	 unsigned char real_frequency_number;
	
	 if(p_meter_param->frequency_number <= 0) return;
	
	 if(p_meter_param->pluse_type == PLUSE_ERROR)
	 {
		 platform_watchdog_reset(0);
	 }
	 
	 real_frequency_number = 0;
	 
   for(i = 0; i < p_meter_param->frequency_number; i++)
	 {
			if(p_meter_param->frequency[i] != 0xFFFF)
			{
					real_frequency_number++;
			}
	 }
	
   
	 meter_calculate_param.power_level = p_meter_param->power_level;
	 meter_calculate_param.frequency_number = real_frequency_number;
	 memcpy(meter_calculate_param.frequecny, p_meter_param->frequency, p_meter_param->frequency_number);
	 
	 
	 printf("number = %d , f = %d \n", meter_calculate_param.frequency_number, meter_calculate_param.frequecny[0]);

	 switch(meter_calculate_param.power_level)
	  {
			 case LEVEL_4DB:    //4db
			 {
				 app_meter_add5db();
				 break;  
			 }
			 case LEVEL_0DB:   //0db
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
				  break;
			 }
			 case LEVEL_NEG_4DB:  //-4db
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x09);
				  break;
			 }
			 case LEVEL_NEG_8DB:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x06);
				  break;
			 }
			 case LEVEL_NEG_20DB:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x00);				 
				  break;
			 }
			 default:
			 {
				  cobra_ceva_baseband_csem_register_write(CS565_REG_PA_PWR, 0x0c);
				  break;
			 }
	 }
		
	 printf("level = %d, pluse_type = %d \n", meter_calculate_param.power_level, p_meter_param->pluse_type);
	 
	 calculate_plus_type = p_meter_param->pluse_type;
	 
   if(p_meter_param->pluse_type == PLUSE_ACTIVE)   //有功脉冲   输入为 GPIO8 
	 {			 
		    cobra_pin_pullup_enable(PIN8_INDEX, 1);
		    cobra_pin_input_enable(PIN8_INDEX, 1);
		    app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
		    app_m3_gpio_enable_interrupt(GPIO_PIN_8, GPIO_LEVEL_LOW, app_key_pin8_function);	
        app_m3_gpio_disable_interrupt(GPIO_PIN_3);
      
		    
   }		 
	 else //if(pluse_type == 0x00)  //如果是秒脉冲,   输入为 GPIO3
	 {
			  cobra_pin_pullup_enable(PIN3_INDEX, 1);
		    cobra_pin_input_enable(PIN3_INDEX, 1);
		    app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_INPUT);
		    app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_LOW, app_key_pin3_function);	
		    app_m3_gpio_disable_interrupt(GPIO_PIN_8);
		  
	  }
//2021/5/21 liujian delete	 
//		app_key_pin3_function(0x00);
		
      app_meter_false_send_data();
}

void meter_send_ack_data(void)
{
	  unsigned int i;
	
	  app_csem_idle_mode();
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, mete_ack_data[0] );
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, mete_ack_data[1]);
	
		for(i = 0; i < mete_ack_data[1]; i++)
		{
				cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, mete_ack_data[2 + i]);
		}
		
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
}


/*
 这是电表回复台体的命令， 应答信号,  先把要回复的命令保存到

buffer   中, 然后在合适的时候发送过去  通讯的空闲时段
*/
void app_meter_ack_data_to_taiti(unsigned char * mac_address, unsigned char length, unsigned char *p_buffer)
{
	   int i;
	   mete_ack_data[0] = 0x0F;
	   mete_ack_data[1] = length + 1;
     mete_ack_data[2] = 0x20;
     if(address_is_odd_even)
     {
          mete_ack_data[2] |= 0x80;
     }	
     memcpy(&mete_ack_data[3], p_buffer, length);
		 
		 ack_data_request = 1;
}

void app_meter_send_data(void)
{
	  unsigned int i;
	
	  app_csem_idle_mode();
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, csem_meter_protocol.head );
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, csem_meter_protocol.length);
	
		for(i = 0; i < csem_meter_protocol.length; i++)
		{
				cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, csem_meter_protocol.payload[i]);
		}
		
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
}

void app_meter_send_data_to_manager_ic(void)
{	
	        unsigned int i;
			unsigned short toatl_length;
			unsigned short checksum = 0;
	
            unsigned char *p_addr = iris_dianbiao_get_taiti_address();
			toatl_length = meter_communicate_index;
	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, toatl_length&0xFF);
			cobra_uart_write(M3_UART0, (toatl_length>>8)&0xFF);
			checksum += toatl_length&0xFF;
			checksum += ((toatl_length>>8)&0xFF);
	
			//连接的台体的MAC  地址

			cobra_uart_write(M3_UART0, p_addr[0]);
			cobra_uart_write(M3_UART0, p_addr[1]);
			cobra_uart_write(M3_UART0, p_addr[2]);
			cobra_uart_write(M3_UART0, p_addr[3]);
			cobra_uart_write(M3_UART0, p_addr[4]);
			cobra_uart_write(M3_UART0, p_addr[5]);
			
			checksum += p_addr[0];
			checksum += p_addr[1];
			checksum += p_addr[2];
			checksum += p_addr[3];
			checksum += p_addr[4];
			checksum += p_addr[5];

			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;
			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x80);   //ACK OK
			
			checksum += 0x68;
			checksum += 0x80;
			
			for(i = 0; i < meter_communicate_index; i++)
			{
				cobra_uart_write(M3_UART0, meter_communicate_data[i]);
				checksum += meter_communicate_data[i];
			}
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);		
}



/*
延迟 5 秒处理电表发送过来的数据
*/
void process_command_in_24g(ble_meter_struct *p_data)
{
	  memcpy(&backup_24g_data, p_data, sizeof(ble_meter_struct));
	  cobra_timer_set_frequency(APB2_TIMER0, 0.2);
	  app_cobra_timer0_enable(1);
}

static void app_iris_meter_calculate_timer_delay_callback(void)
{
	  app_meter_param_process_uart_command(&backup_24g_data);
	  app_cobra_timer0_enable(0);
}

#endif


