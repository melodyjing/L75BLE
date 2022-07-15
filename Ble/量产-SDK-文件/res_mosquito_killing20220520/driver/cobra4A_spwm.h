/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_spwm.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/7/17

	Description :   


	Changed Log	:

		Liujian  2020/7/17 		- Creation

*****************************************************************************/

#ifndef __COBRA4A_SPWM_H__
#define __COBRA4A_SPWM_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_SPWM_EN

typedef struct cobra_spwm_t
{
	   __attribute__((aligned(4))) volatile unsigned int  spwm_enable_reg;   //PWM enable Register 

		 __attribute__((aligned(4))) volatile unsigned int  spwm_period_width_reg;   //pwm_period_width Register
	
	   __attribute__((aligned(4))) volatile unsigned int  spwm_pulse_width_reg;   //pwm_pulse_width Register
	
	   __attribute__((aligned(4))) volatile unsigned int  spwm_deadband_width_reg;   //pwm_deadband_width Register 
	
	   __attribute__((aligned(4))) volatile unsigned int  spwm_interrupt_mask_reg;  //PWM Interrupt Mask Register
	
	   __attribute__((aligned(4))) volatile unsigned int  spwm_interrupt_status_reg;  //PWM Interrupt Register 
	
	   __attribute__((aligned(4))) volatile unsigned int  spwm_mci_edge_config_reg; //MCI edge config Register
	
	  
	   __attribute__((aligned(4))) volatile unsigned int  spwm_clock_cross_hold_count_reg;  //clock_cross_hold_count Register
	
	   
	   __attribute__((aligned(4))) volatile unsigned int  spwm_count_reg;  //count Register
	
	
}COBRA_SPWM_T;


typedef enum {
	  PASSIVE_LOW_ACTIVE_HIGH,
		PASSIVE_HIGH_ACTIVE_LOW
}POLA_TYPE;

typedef enum {
		COUNT_PWM_CLK,
	  COUNT_MCI_EDGE,
	
}MCI_EDGE_SELECT_TYPE;

typedef enum {
	 PERIOD_WIDTH_INT,
	 PULSE_WIDTH_INT,
	 MCI0_RISE_INT,
	 MCI0_FALL_INT,
	 MCI1_RISE_INT,
	 MCI1_FALL_INT,
	 MCI2_RISE_INT,
	 MCI2_FALL_INT,	
}PWM_INTERRUPT_TYPE;

void cobra_spwm_init(void);

void cobra_spwm_pwm_en(unsigned int en);
void cobra_spwm_pwm_a_enable(unsigned int en);
void cobra_spwm_pwm_b_enable(unsigned int en);
void cobra_spwm_pola_a_set(POLA_TYPE type);
void cobra_spwm_pola_b_set(POLA_TYPE type);
void cobra_spwm_pwm_period_width(unsigned int period);
void cobra_spwm_pwm_pulse_width(unsigned int pulse);
void cobra_spwm_pwm_deadband_width(unsigned int width);
void app_cobra_spwm_set(unsigned int frequency, unsigned int duty);
void cobra_spwm_pwm_deadband_en(unsigned int en);

#define COBRA_SPWM_BASE  (0x400A0000UL + 0x3000UL)


#endif

#endif
