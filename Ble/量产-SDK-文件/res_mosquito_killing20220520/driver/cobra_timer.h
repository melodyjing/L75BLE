/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_timer.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/9

	Description :   

	Changed Log	:

		Liujian  2019/4/9		- Creation



*****************************************************************************/

#ifndef __COBRA_TIMER_H__
#define __COBRA_TIMER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_TIMER_EN

typedef struct cobra_timer_type
{
		volatile __attribute__ ((aligned (4)))   uint32_t  CTRL;          /*!< Offset: 0x000 Control Register (R/W) */
		volatile __attribute__ ((aligned (4)))   uint32_t  VALUE;         /*!< Offset: 0x004 Current Value Register (R/W) */
		volatile __attribute__ ((aligned (4)))   uint32_t  RELOAD;        /*!< Offset: 0x008 Reload Value Register  (R/W) */
  
		union INT_UINON_S{
				volatile __attribute__ ((aligned (4)))    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
				volatile __attribute__ ((aligned (4)))    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    }INT_UINON;
		
}COBRA_TIMER_TypeDef;



typedef enum
{
	APB2_TIMER0,
	APB2_TIMER1,
	APB0_TIMER1,
	APB10_TIMER10  //M0 
}time_id;

typedef void (*timer_callback_t)(void);

#define COBRA_APB2_TIMER0_BASE					(0x400C7000UL)
#define COBRA_APB2_TIMER1_BASE					(0x400C8000UL)

#define COBRA_APB0_TIMER1_BASE					(0x40086000UL)
#define COBRA_APB10_TIMER10_BASE				(0x40123000UL)


void rdw_timer_set_value(volatile COBRA_TIMER_TypeDef *rdw_timer, uint32_t value);

void cobra_timer_init(void);

void cobra_timer_set_frequency(time_id  timer_index, float hz);

void cobra_timer_enable_set(time_id timer_index, unsigned int en);

void cobra_timer_interrupt_set(time_id timer_index, unsigned int en);

void app_cobra_init_timer0_clock(void);

void cobra_soft_delay_ms(unsigned int count);
void cobra_soft_delay_us(unsigned int count);
void app_cobra_timer1_set_callback(timer_callback_t callback);

void app_cobra_init_timer1(int hz);

void app_cobra_timer1_enable(unsigned int en);

volatile COBRA_TIMER_TypeDef * cobra_timer_get_apb2_timer0(void);

#ifdef COBRA_TIMER_NEW

void cobra_timer_Init_IntClock(uint8_t timer_type, uint32_t reload,uint32_t irq_en);

void cobra_timer_irq_clear(uint8_t timer_type);

#endif

#if (DIANBIAO_MODULE_ENABLE | DWT_MIC_BREAKER_ENABLE)
void app_cobra_init_timer0(float hz);
void app_cobra_timer0_enable(unsigned int en);
void app_cobra_timer0_set_callback(timer_callback_t func);
#endif


#endif   //COBRA_TIMER_EN



#endif

