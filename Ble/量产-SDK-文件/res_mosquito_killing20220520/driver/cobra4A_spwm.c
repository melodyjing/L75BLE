/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_spwm.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/7/17

	Description :   


	Changed Log	:

		Liujian  2020/7/17 		- Creation

*****************************************************************************/
#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "cobra4A_spwm.h"

#if COBRA_SPWM_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_SPWM_T * cobra_spwm		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_SPWM_T * cobra_spwm;
#endif

void cobra_spwm_init(void)
{
	  cobra_spwm = (COBRA_SPWM_T * )COBRA_SPWM_BASE;
}

/*
pwm_en

0: pwm disable
1: pwm enable
*/
void cobra_spwm_pwm_en(unsigned int en)
{
		if(en)
		{
			  cobra_spwm->spwm_enable_reg |=  0x01;
		}
		else
		{
			 cobra_spwm->spwm_enable_reg &=  ~0x01;
		}
}

/*
pwm_A_enable

0: pwm port A(pwm_out) disable
1: pwm port A(pwm_out) enable
*/
void cobra_spwm_pwm_a_enable(unsigned int en)
{
		if(en)
		{
			  cobra_spwm->spwm_enable_reg |=  (1 << 1);
		}
		else
		{
			 cobra_spwm->spwm_enable_reg &=  ~(1 << 1);
		}
	
}

/*
pwm_B_enable
0: pwm port B(pwm_out) disable
1: pwm port B(pwm_out) enable

*/
void cobra_spwm_pwm_b_enable(unsigned int en)
{
		if(en)
		{
			  cobra_spwm->spwm_enable_reg |=  (1 << 2);
		}
		else
		{
			 cobra_spwm->spwm_enable_reg &=  ~(1 << 2);
		}
}

/*
pola_A
Selects polarity of the port A
0: Passive state is LOW, active state is HIGH.
1: Passive state is HIGH, active state is LOW.

*/
void cobra_spwm_pola_a_set(POLA_TYPE type)
{
		if(type == PASSIVE_LOW_ACTIVE_HIGH)
		{
			  cobra_spwm->spwm_enable_reg &=  ~(1 << 3);
		}
		else if(type == PASSIVE_HIGH_ACTIVE_LOW)
		{
			 cobra_spwm->spwm_enable_reg |=  (1 << 3);
		}
}

/*

pola_B

Selects polarity of the port B
0: Passive state is HIGH, active state is LOW.
1: Passive state is LOW, active state is HIGH.

*/
void cobra_spwm_pola_b_set(POLA_TYPE type)
{
		if(type == PASSIVE_LOW_ACTIVE_HIGH)
		{
			  cobra_spwm->spwm_enable_reg &=  ~(1 << 4);
		}
		else if(type == PASSIVE_HIGH_ACTIVE_LOW)
		{
			 cobra_spwm->spwm_enable_reg |=  (1 << 4);
		}
}

/*
pwm_deadband_en

0: dead band disable
1: dead band enable


*/
void cobra_spwm_pwm_deadband_en(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_enable_reg |= (1 << 5);
		}
		else
		{
			  cobra_spwm->spwm_enable_reg &= ~(1 << 5);
		}
}

/*
mci_edge_select

0: count advance with pwm clk
1: count advance with mci0/1/2 rise/fall edge

*/
void cobra_spwm_mci_edge_select(MCI_EDGE_SELECT_TYPE type)
{
	 if(type == COUNT_PWM_CLK)
	 {
      cobra_spwm->spwm_enable_reg &= ~(1 << 6);
	 }
   else if(type == COUNT_MCI_EDGE)
   {
      cobra_spwm->spwm_enable_reg |= (1 << 6);
   }		 
}
///////////////////////
/*
pwm_period_width
The cycle count of pwm clk for one pwm output period.
period cycle count = pwm_period_width + 1

*/
void cobra_spwm_pwm_period_width(unsigned int period)
{
	  cobra_spwm->spwm_period_width_reg = period;	
}

/////////////////////////
/*
pwm_pulse_width
The cycle count of pwm clk for pwm output high level in one period.
High level cycle count = pwm_pulse_width

*/
void cobra_spwm_pwm_pulse_width(unsigned int pulse)
{
	  cobra_spwm->spwm_pulse_width_reg = pulse;	
}

//////////////////////
/*
pwm_deadband_width
The cycle count of pwm clk for deadband.
deadband cycle count = pwm_deadband_width

*/
void cobra_spwm_pwm_deadband_width(unsigned int width)
{
	  cobra_spwm->spwm_deadband_width_reg = width;	
}

/*
1.  period_width_int_enable
    0: when count match pwm_period_width will not generate interrupt
    1: when count match pwm_period_width will generate interrupt
    Write 1 to generate correspond interrupt.
    Write 0 to clear correspond interrupt.
2.  pulse_width_int_enable
    0: when count match pulse_period_width will not generate interrupt
    1: when count match pulse_period_width will generate interrupt
    Write 1 to generate correspond interrupt.
    Write 0 to clear correspond interrupt.
3.  mci0_rise_int_enable	
    0: when mci0 rise edge will not generate interrupt
    1: when mci0 rise edge will generate interrupt
4.  mci0_fall_int_enable	
    0: when mci0 fall edge will not generate interrupt
    1: when mci0 fall edge will generate interrupt
5.  mci1_rise_int_enable	
    0: when mci1 rise edge will not generate interrupt
    1: when mci1 rise edge will generate interrupt
6.  mci1_fall_int_enable	
   0: when mci1 fall edge will not generate interrupt
   1: when mci1 fall edge will generate interrupt
7. mci2_rise_int_enable	
   0: when mci2 rise edge will not generate interrupt
   1: when mci2 rise edge will generate interrupt
8  mci2_fall_int_enable	
   0: when mci2 fall edge will not generate interrupt
   1: when mci2 fall edge will generate interrupt
*/
void cobra_spwm_pwm_mask_interrupt(PWM_INTERRUPT_TYPE type, unsigned int interrupt_enable)
{
	  switch(type)
		{
			case PERIOD_WIDTH_INT:
			{
				if(interrupt_enable)
				{
					 cobra_spwm->spwm_interrupt_mask_reg |= (0 << 1);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(0 << 1);
				}
				break;
			}
			case PULSE_WIDTH_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 1);
				}
				else
				{
					  cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 1);
				}
				break;
			}
			case MCI0_RISE_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 2);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 2);
				}
				break;
			}
			case MCI0_FALL_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 3);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 3);
				}
				break;
			}
			case MCI1_RISE_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 4);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 4);
				}
				break;
			}
			case MCI1_FALL_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 5);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 5);
				}
				break;
			}
			case MCI2_RISE_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 6);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 6);
				}
				break;
			}
			case MCI2_FALL_INT:
			{
				if(interrupt_enable)
				{
					  cobra_spwm->spwm_interrupt_mask_reg |= (1 << 7);
				}
				else
				{
					 cobra_spwm->spwm_interrupt_mask_reg &= ~(1 << 7);
				}
				break;
			}
			
			default:
				break;
		}
}
/*
PWM Interrupt Register
1. period_width_int	
   0: count not match pwm_period_width
   1: count match pwm_period_width
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
2. pulse_width_int	
   0: count not match pulse_period_width
   1: count match pulse_period_width
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
3. mci0_rise_int	
   0: mci0 rise edge not happen
   1: mci0 rise edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
4. mci0_fall_int	
   0: mci0 fall edge not happen
   1: mci0 fall edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
5. mci1_rise_int	
   0: mci1 rise edge not happen
   1: mci1 rise edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
6. mci1_fall_int	
   0: mci1 fall edge not happen
   1: mci1 fall edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
7. mci2_rise_int	
   0: mci2 rise edge not happen
   1: mci2 rise edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
8. mci2_fall_int	
   0: mci2 fall edge not happen
   1: mci2 fall edge happen
   Write 1 to generate correspond interrupt.
   Write 0 to clear correspond interrupt.
*/
unsigned int cobra_spwm_pwm_interrupt_status(PWM_INTERRUPT_TYPE type)
{
	 unsigned int status = 0;
	 switch(type)
	 {
		 case PERIOD_WIDTH_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 0))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case PULSE_WIDTH_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 1))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI0_RISE_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 2))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI0_FALL_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 3))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI1_RISE_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 4))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI1_FALL_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 5))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI2_RISE_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & ( 1 << 6))
			 {
					 status = 1;
			 }
			 break;
		 }
		 case MCI2_FALL_INT:
		 {
			 if(cobra_spwm->spwm_interrupt_status_reg & (1 << 7))
			 {
					 status = 1;
			 }
			 break;
		 }
		 
		 default:
			 break;
	 }
	 return status;
}

/*
mci0_rise_edge_enable	
0: count do not advances when mci0 rise edge
1: count advances when mci0 rise edge
*/
void cobra_spwm_pwm_mci0_rise_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 0);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 0);
		}
}

/*
mci0_fall_edge_enable	
0: count don抰 advances when mci0 fall edge
1: count advances when mci0 fall edge

*/
void cobra_spwm_pwm_mci0_fall_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 1);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 1);
		}
}

/*
mci1_rise_edge_enable	
0: count don抰 advances when mci1 rise edge
1: count advances when mci1 rise edge

*/
void cobra_spwm_pwm_mci1_rise_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 2);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 2);
		}
}

/*
mci1_fall_edge_enable	
0: count don抰 advances when mci1 fall edge
1: count advances when mci1 fall edge

*/
void cobra_spwm_pwm_mci1_fall_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 3);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 3);
		}
}

/*
mci2_rise_edge_enable	
0: count don抰 advances when mci2 rise edge
1: count advances when mci2 rise edge

*/
void cobra_spwm_pwm_mci2_rise_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 4);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 4);
		}
}

/*
mci2_fall_edge_enable	
0: count don抰 advances when mci2 fall edge
1: count advances when mci2 fall edge

*/
void cobra_spwm_pwm_mci2_fall_edge_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 5);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 5);
		}
}

/*
mci0_rise_clr_enable	
0: count don抰 clear to 0 when mci0 rise edge
1: count clear to 0 when mci0 rise edge

*/
void cobra_spwm_pwm_mci0_rise_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 6);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 6);
		}
}

/*
mci0_fall_clr_enable	
0: count don抰 clear to 0 when mci0 fall edge
1: count clear to 0 when mci0 fall edge

*/
void cobra_spwm_pwm_mci0_fall_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 7);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 7);
		}
}

/*
mci1_rise_clr_enable	
0: count don抰 clear to 0 when mci1 rise edge
1: count clear to 0 when mci1 rise edge

*/
void cobra_spwm_pwm_mci1_rise_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 8);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 8);
		}
}

/*
mci1_fall_clr_enable	
0: count don抰 clear to 0 when mci1 fall edge
1: count clear to 0 when mci1 fall edge
*/
void cobra_spwm_pwm_mci1_fall_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 9);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 9);
		}
}

/*
mci2_rise_clr_enable	
0: count don抰 clear to 0 when mci2 rise edge
1: count clear to 0 when mci2 rise edge

*/
void cobra_spwm_pwm_mci2_rise_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 10);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 10);
		}
}

/*
mci2_fall_clr_enable	
0: count don抰 clear to 0 when mci2 fall edge
1: count clear to 0 when mci2 fall edge

*/
void cobra_spwm_pwm_mci2_fall_clr_enable(unsigned int en)
{
	  if(en)
		{
			  cobra_spwm->spwm_mci_edge_config_reg |= (1 << 11);
		}
		else
		{
			 cobra_spwm->spwm_mci_edge_config_reg &= ~(1 << 11);
		}
}

/*
clock_cross_hold_count	
When signal cross from pwm_clk to HCLK,how many cycles of pwm clk the signal will hold
*/
void cobra_spwm_pwm_clock_cross_hold_count(unsigned int cross_hold_count)
{
	 cobra_spwm->spwm_clock_cross_hold_count_reg = cross_hold_count&0x1f;
}


/*
count	The value of pwm count
*/
unsigned int cobra_spwm_pwm_current_count(void)
{
	 return cobra_spwm->spwm_count_reg;
}


/*
 duty 是占空比      0 - 99
 frequency  是 SPWM 的频率, 单位HZ
*/
void app_cobra_spwm_set(unsigned int frequency, unsigned int duty)
{
	  unsigned int apb_frequency = cobra_system_get_m3_apb1_clk();
	
	  unsigned int count = apb_frequency/frequency;
	  
    duty = duty%100;
	
		cobra_spwm_pwm_period_width(count-1);
	  
	  cobra_spwm_pwm_pulse_width(count*duty/100);
	   
}

#if 0
APP_TIMER_DEF(spwm_test_timer_id);

static  int test_duty;
static  int pluse = 1;
void app_spwm_led_callback(void * temp)
{
	 test_duty += pluse;
	 if(test_duty >= 99)
	 {
			pluse = -1;
	 }
	 
	 if(test_duty <= 1)
	 {
			pluse = 1;
	 } 
	 app_cobra_spwm_set(1000, test_duty);
}

void app_cobra_spwm_test(void)
{
			cobra_map_pinmodeset(PIN28_INDEX, PIN28_SPWM_OUT_N);
	    cobra_map_pinmodeset(PIN27_INDEX, PIN27_SPWM_OUT);
	
	    cobra_spwm_pwm_b_enable(1);
	    cobra_spwm_pwm_a_enable(1);
	
			cobra_spwm_pola_a_set(PASSIVE_HIGH_ACTIVE_LOW);
			cobra_spwm_pola_b_set(PASSIVE_LOW_ACTIVE_HIGH);
	
		  app_cobra_spwm_set(1000, 2);
	    cobra_spwm_pwm_en(1);
//	    app_timer_create(&spwm_test_timer_id,  APP_TIMER_MODE_REPEATED, app_spwm_led_callback);
//	    app_timer_start(spwm_test_timer_id, APP_TIMER_TICKS(100), (void *)NULL);

}

#endif


#endif