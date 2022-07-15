/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_calculate_box.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/9

	Description :   
	    ̨��ˣ����ڽ��������źţ��ָ��ź�

	Changed Log	:

		Liujian  2020/11/9			- Creation


*****************************************************************************/
#include "app_calculate_box.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "cobra_timer.h"
#include "cobra_ceva_baseband.h"
#include "rdw_mcu.h"
#include <string.h>

#define BOX_DEBUG  0

typedef enum {
	   BOX_PLUSE_STATE,
	   BOX_CHANGE_STATE,
	   BOX_DELAY_STATE,
	   BOX_DATA_STATE,
	   BOX_IDLE_STATE,
}BOX_WORK_STATE;

static unsigned int box_receive_channel;

static BOX_WORK_STATE   box_state;
static APP_CAL_FREQUENCY   box_calculate_param;    //Ŀǰ ����̨�� У׼�Ĳ���
static void app_iris_box_calculate_timer_callback(void);
static void app_box_rx_over_process(void);
static void app_box_tx_over_process(void);

static  APP_CSEM_PROTOCOL  csem_box_protocol;

/*
type �ǲ�������������
0x00-�й���
0x01-�޹���
0x02-�����壬
0x03-����г����
0x04-����г����
0x05-������  
0x06 -ʱ��Ͷ��
*/
void app_iris_calculate_box_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type)
{
	
		 if(p_cal_param->frequency_number <= 0) return;
	   
	   box_receive_channel = 0;
	
   	 memcpy((unsigned char *)&box_calculate_param, (unsigned char *)p_cal_param, sizeof(APP_CAL_FREQUENCY));
	
	   app_iris_calculate_init(p_cal_param->frequecny[0]);

	   app_csem_idle_mode();
	   app_csem_pluse_calculate_init();
	
	   app_iris_set_frequency(p_cal_param->frequecny[0]);
		 app_iris_set_access_address(address);
		 app_iris_set_whitelist_value(p_cal_param->frequecny[0]);
		 

     app_csem_receive_go();

		 app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_OUTPUT);
		 app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_HIGH);
	
		 NVIC_EnableIRQ(CSEM_IRQ_RECEIVED);	
	   NVIC_SetPriority(CSEM_IRQ_RECEIVED, 0);

	   app_csem_set_receive_mode();
		
		 app_csem_irq_enable(CSEM_IRQ_TX_FLAG);
		 NVIC_EnableIRQ(CSEM_IRQ_TX);
		 
		 app_csem_set_tx_callback(app_box_tx_over_process);
		 app_csem_set_rx_callback(app_box_rx_over_process);
		 
		 app_cobra_timer1_enable(0);
		 app_cobra_timer1_set_callback(app_iris_box_calculate_timer_callback);
	   app_cobra_init_timer1(SEND_DATA_HZ);   //2ms
		 
		 box_state = BOX_IDLE_STATE;
		 
#if(BOX_DEBUG == 1)
    app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_OUTPUT);
		app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_LOW);
#endif
}

void app_box_receive_data(void)
{
	  unsigned int i;
	  csem_box_protocol.head = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
	  csem_box_protocol.length = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
	  for(i = 0; i < csem_box_protocol.length; i++)
    {
         csem_box_protocol.payload[i] = cobra_ceva_baseband_csem_register_read(CS565_REG_RXFIFO);
    }	
}

/*
�յ������źŵ� ACK  ����
*/
void app_box_ack_meter_data(void)
{
	
	  app_csem_idle_mode();
#if 0	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0xf1 );
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0x00);
#else
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0xD0 );    // 1101  0000  0x01  DATA 
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0x01);
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0x10);
#endif	
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
}


static void app_box_rx_over_process(void)
{
	   unsigned char  status;
	
	   status = cobra_ceva_baseband_csem_register_read(CS565_REG_DESER_STATUS);

	   
		 if(status == 0x41)
		 {
			        app_box_receive_data();
			        //printf("level = %d \n", csem_box_protocol.payload[0]);
			        if(csem_box_protocol.head == (unsigned char )(0xF0))
		          {
			            if(csem_box_protocol.payload[0] & 0x01)
				          {
					              app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_LOW);
										 //   printf("0");
				          }
				          else
				          {
					              app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_HIGH);
										//    printf("1");
				          }
		          }
							
							app_box_ack_meter_data();

		          if(box_state == BOX_IDLE_STATE)
		          {
		              app_cobra_timer1_enable(1);
#if(BOX_DEBUG == 1)
		              app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_HIGH);
#endif
		          }
		          box_state = BOX_PLUSE_STATE;

             box_receive_channel++;	
             return;
							
		 }
		 else
		 {
			    app_csem_idle_mode();
			    app_iris_set_frequency(box_calculate_param.frequecny[0]);
			    app_iris_set_whitelist_value(box_calculate_param.frequecny[0]);	
		      app_csem_set_receive_mode();	
		 }
//		 app_box_ack_meter_data();

//		 if(box_state == BOX_IDLE_STATE)
//		 {
//		    app_cobra_timer1_enable(1);
//#if(BOX_DEBUG == 1)
//		 app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_HIGH);
//#endif
//		 }
//		 box_state = BOX_PLUSE_STATE;
//	   
//		 box_receive_channel++;
}


static void app_box_tx_over_process(void)
{
#if 0
//	  if((box_receive_channel < box_calculate_param.frequency_number) && (box_state == BOX_PLUSE_STATE))
//		{
//			   app_csem_idle_mode();
//			   app_iris_set_frequency(box_calculate_param.frequecny[box_receive_channel]);
//			   app_iris_set_whitelist_value(box_calculate_param.frequecny[box_receive_channel]);	
//		}
//		else if(box_state == BOX_PLUSE_STATE)
//		{
//			  box_state = BOX_CHANGE_STATE;
//		}
//		app_csem_set_receive_mode();
#endif
	   app_csem_set_receive_mode();
}

//�춨�����У�ͨ�����ݰ���ʽ��
void app_box_send_communicate_data(unsigned char length, unsigned char *p_buffer)
{
	  unsigned char i;
	  app_csem_idle_mode();
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0xf0 );
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, length + 1);
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0x30);
	
	  for(i = 0; i < length; i++)
	  {
	     cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, *p_buffer++);
		}
	
		cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
}

//����/����ת����������ܱ��л��춨ģʽ�����ݰ���ʽ
//�������ͣ�0x00-�й���0x01-�޹���0x02-�����壬0x03-����г����0x04-����г����0x05 -���� 0x06 -ʱ��Ͷ�� �� 0xFF-�ر�����
//CMD ����
void app_box_send_command_data(unsigned char cmd)
{
	  app_csem_idle_mode();
	  
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO, 0xf1);   //Header 
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO,  0x01);   //Length
	
	 //�������ͣ�0x00-�й���0x01-�޹���0x02-�����壬0x03-����г����0x04-����г����0x05 -���� 0x06 -ʱ��Ͷ�� �� 0xFF-�ر�����
	  cobra_ceva_baseband_csem_register_write(CS565_REG_TXFIFO,  0x10 + cmd&0x0f);
	
	  cobra_ceva_baseband_csem_register_write(CS565_REG_FSM_MODE, 0x07);
}

static unsigned char test_communicate[100];
//2ms �жϴ���
static void app_iris_box_calculate_timer_callback(void)
{
#if 1
	 //    printf("C=%d\n", box_receive_channel);
        box_receive_channel++;
        if((box_receive_channel < box_calculate_param.frequency_number) && (box_state == BOX_PLUSE_STATE))
		    {
			     app_csem_idle_mode();
			     app_iris_set_frequency(box_calculate_param.frequecny[box_receive_channel]);
			     app_iris_set_whitelist_value(box_calculate_param.frequecny[box_receive_channel]);	
					 app_csem_set_receive_mode();	
		    }
		    else if(box_state == BOX_PLUSE_STATE)
		    {
			      box_state = BOX_CHANGE_STATE;
		    }
		    
#endif	
	     if((box_receive_channel >= box_calculate_param.frequency_number) && (box_state == BOX_CHANGE_STATE))
		   {
			    box_state = BOX_DELAY_STATE;
			    box_receive_channel = 0;
			    app_csem_idle_mode();
			    app_iris_set_frequency(box_calculate_param.frequecny[box_receive_channel]);
			    app_iris_set_whitelist_value(box_calculate_param.frequecny[box_receive_channel]);
			    app_csem_set_receive_mode();
		//	  printf("1\n");
		  }
		  else if(box_state == BOX_DELAY_STATE)
		  {
#if(BOX_DEBUG == 1)
		     app_m3_gpio_write_pin(GPIO_PIN_28, GPIO_LOW);
#endif
			   box_state = BOX_DATA_STATE;
			   app_box_send_communicate_data(100, test_communicate);
		//	  printf("2\n");
		  }
		  else if(box_state == BOX_DATA_STATE)
		  {
			   app_cobra_timer1_enable(0);
			   cobra_timer_set_frequency(APB2_TIMER1, SEND_DATA_HZ);
			   box_state = BOX_IDLE_STATE;
		//	  printf("3\n");
		  }
}


