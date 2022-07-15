/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_rtc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/5

	Description :   


	Changed Log	:

		Liujian  2019/5/5			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra4_aon_top.h"
#include "cobra_system.h"
//#include "cobra_aon_pmu.h"
#include "cobra4_aon_wk.h"
#include "cobra_rtc.h"

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include "app_mesh_tscns.h"
#endif


#if COBRA_RTC_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_RTC_TypeDef *cobra_rtc __attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_RTC_TypeDef * cobra_rtc; 
#endif

void cobra_rtc_init(void)
{
		cobra_rtc = (COBRA_RTC_TypeDef * )COBRA_RTC_BASE;
}

/*
 *  Freezes time and date registers, the time must
	be unfrozen before date and time registers are
	updated.
	The read value indicates when a valid Time value
	is available to read

Write:
	0 – Free running time
	1 – Freeze time
Read:
	1 – All date and time values are valid
	0 – Data and time values not guaranteed

 */
void cobra_rtc_freeze_mode_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL |= 0x01;
	}
	else
	{
		cobra_rtc->CONTROL &= (~0x01 & 0xFFFF);
	}
}

/*
 * Time write enable
 * 0 – Write enable
   1 – Write lock
 */
void cobra_rtc_timer_write_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL &= (~0x02 & 0xFFFF);
	}
	else
	{
		cobra_rtc->CONTROL |= 0x02;
	}
}

/*
 * Alarm 1 write enable
 * 0 – Write enable
   1 – Write lock
 */
void cobra_rtc_alarm1_write_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL &= (~0x04 & 0xFFFF);
	}
	else
	{
		cobra_rtc->CONTROL |= 0x04;
	}
}
/*
 * Alarm 2 write enable
 * 0 – Write enable
   1 – Write lock
 */
void cobra_rtc_alarm2_write_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL &= (~0x08 & 0xFFFF);
	}
	else
	{
		cobra_rtc->CONTROL |= 0x08;
	}
}

/*
 * Enables the RTC clock gate
 * 0 – RTC Disable
   1 – RTC Enable
 */
void cobra_rtc_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL |= 0x10;
	}
	else
	{
		cobra_rtc->CONTROL &= (~0x10 & 0xFFFF);
	}
}

/*
 * Enables the alarm1 hardware output port whenever alarm1 triggers
 * 0 – Alarm1 Disabled
   1 – Alarm1 Enabled

 */
void cobra_rtc_alarm1_hardware_output_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL |= 0x20;
	}
	else
	{
		cobra_rtc->CONTROL &= (~0x20 & 0xFFFF);
	}
}

/*
 * Enables the alarm2 hardware output port whenever alarm2 triggers
 * 0 – Alarm2 Disabled
   1 – Alarm2 Enabled
 */
void cobra_rtc_alarm2_hardware_output_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->CONTROL |= 0x40;
	}
	else
	{
		cobra_rtc->CONTROL &= (~0x40 & 0xFFFF);
	}
}

/*
 * Enable fast mode for test. 32,768 times faster.
 * 0 – User Mode
   1 – Test Mode
 */
void cobra_rtc_work_mode(RTC_WORK_MODE work_mode)
{
	if(work_mode == USER_MODE)
	{
		cobra_rtc->CONTROL &= (~0x80 & 0xFFFF);
	}
	else if(work_mode == TEST_MODE)
	{
		cobra_rtc->CONTROL |= 0x80;
	}
}


/*
 * Second interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_second_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x01;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x01 & 0xFFFF);
	}
}

/*
 * Minute interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_minute_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x02;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x02 & 0xFFFF);
	}
}

/*
 * Hours interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_hours_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x04;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x04 & 0xFFFF);
	}
}

/*
 * Days interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_days_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x08;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x08 & 0xFFFF);
	}
}

/*
 * Months interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_months_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x10;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x10&0xFFFF);
	}
}

/*
 * Years interrupt enable
 * 0 – Disabled
   1 – Enabled
 */
void cobra_rtc_years_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x20;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x20 & 0xFFFF);
	}
}

/*
 * Alarm 1 interrupt enable
 *  0 – Disabled
    1 – Enabled
 */
void cobra_rtc_alarm1_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x40;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x40 & 0xFFFF);
	}
}

/*
 * Alarm 2 interrupt enable
 *  0 – Disabled
    1 – Enabled
 */
void cobra_rtc_alarm2_interrupt_en(unsigned int en)
{
	if(en)
	{
		cobra_rtc->INT_ENABLE |= 0x80;
	}
	else
	{
		cobra_rtc->INT_ENABLE &= (~0x80 & 0xFFFF);
	}
}

/*
 * @return int - 1 if second interrupt flag is set
 */
unsigned int cobra_rtc_second_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x01) != 0;
}

/*
 * Clear 'second' interrupt
 */
void  cobra_rtc_clear_second_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x01 & 0xFFFF);
}


/*
 * @return int - 1 if minuter interrupt flag is set
 */
unsigned int cobra_rtc_minuter_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x02) != 0;
}

/*
 * Clear 'Minuter' interrupt
 */
void  cobra_rtc_clear_minuter_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x02 & 0xFFFF);
}

/*
 * @return int - 1 if Hours interrupt flag is set
 */
unsigned int cobra_rtc_hour_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x04) != 0;
}

/*
 * Clear 'Hours' interrupt
 */
void  cobra_rtc_clear_hour_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x04 & 0xFFFF);
}


/*
 * @return int - 1 if Days interrupt flag is set
 */
unsigned int cobra_rtc_day_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x08) != 0;
}

/*
 * Clear 'Days' interrupt
 */
void  cobra_rtc_clear_day_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x08 & 0xFFFF);
}


/*
 * @return int - 1 if Months interrupt flag is set
 */
unsigned int cobra_rtc_month_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x10) != 0;
}

/*
 * Clear 'Months' interrupt
 */
void  cobra_rtc_clear_month_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x10 & 0xFFFF);
}


/*
 * @return int - 1 if Years interrupt flag is set
 */
unsigned int cobra_rtc_year_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x20) != 0;
}

/*
 * Clear 'Years' interrupt
 */
void  cobra_rtc_clear_year_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x20 & 0xFFFF);
}


/*
 * @return int - 1 if alarm1 interrupt flag is set
 */
unsigned int cobra_rtc_alarm1_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x40) != 0;
}

/*
 * Clear 'alarm1' interrupt
 */
void  cobra_rtc_clear_alarm1_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x40 & 0xFFFF);
}

/*
 * @return int - 1 if alarm2 interrupt flag is set
 */
unsigned int cobra_rtc_alarm2_interrupt_status(void)
{
	return (cobra_rtc->INT_STATUS & 0x80) != 0;
}

/*
 * Clear 'alarm2' interrupt
 */
void  cobra_rtc_clear_alarm2_interrupt_status(void)
{
	cobra_rtc->INT_STATUS &= (0x80 & 0xFFFF);
}


/** Set prescaler divider value. A value of 0 means no division
 * @param divide - division ratio - 1
 */

void cobra_rtc_set_prescaler (unsigned int divide)
{
	cobra_rtc->PRESCALER = divide&0xFFFF;
}

/** Set clock trim value. A value of 0 means no trim
 * @param trim -
 */

void cobra_rtc_set_clock_trim (unsigned int trim)
{
	cobra_rtc->TRIM = trim&0xFFFF;
}


/** Get current date and time
 * @param td  - Pointer to returned time+date structure
 */
void cobra_rtc_get_current_time_date (volatile COBRA_RTC_TIME_DATE_T *current_time)
{
	unsigned int short temp;
	
	cobra_rtc_freeze_mode_en(1);
		
	while(!(cobra_rtc->CONTROL & 0x01));
	
	current_time->SUBSEC = (cobra_rtc->TIME_SSEC & 0x7FFFF);

	temp = cobra_rtc->TIME_SEC_MIN;
	current_time->SEC = (temp & 0x3F);
	current_time->MIN = (temp & 0x3F00) >> 8;

	current_time->HOUR = (cobra_rtc->TIME_HOUR & 0x1F);

	temp = cobra_rtc->DATE_DAY_MONTH;
	current_time->DAY = (temp & 0x1F);
	current_time->MONTH = (temp & 0xF00) >> 8;

	current_time->YEAR  = (cobra_rtc->DATE_YEAR & 0x7F);
	
	cobra_rtc_freeze_mode_en(0);
}

/** Set current date and time
 * @param td  - Pointer to time+date structure
 */
void cobra_rtc_set_current_time_date (volatile COBRA_RTC_TIME_DATE_T *current_time)
{
	cobra_rtc->TIME_SSEC      =  current_time->SUBSEC;
	cobra_rtc->TIME_SEC_MIN   = (current_time->MIN   << 8) | current_time->SEC;
	cobra_rtc->TIME_HOUR      =  current_time->HOUR;
	cobra_rtc->DATE_DAY_MONTH = (current_time->MONTH << 8) | current_time->DAY;
	cobra_rtc->DATE_YEAR      =  current_time->YEAR;
}

/** Set alarm1 date and time
 * @param alrm  - pointer to time+date structure
 */
void cobra_rtc_set_alarm1 (volatile COBRA_RTC_TIME_DATE_T *alarm1_time)
{
	cobra_rtc->ALARM1_SEC_MIN   = (alarm1_time->MIN   << 8) | alarm1_time->SEC;
	cobra_rtc->ALARM1_HOUR      =  alarm1_time->HOUR;
	cobra_rtc->ALARM1_DAY_MONTH = (alarm1_time->MONTH << 8) | alarm1_time->DAY;
	cobra_rtc->ALARM1_YEAR      =  alarm1_time->YEAR;
}

/** Get alarm1 date and time
 * @param alrm - Pointer to returned time+date structure
 */
void cobra_rtc_get_alarm1 (volatile COBRA_RTC_TIME_DATE_T *alarm1_time)
{
  unsigned int short temp;

  temp = cobra_rtc->ALARM1_SEC_MIN;
  alarm1_time->SEC   = (temp & 0x3F);
  alarm1_time->MIN   = (temp & 0x3F00) >> 8;

  alarm1_time->HOUR  = (cobra_rtc->ALARM1_HOUR & 0x1F);

  temp = cobra_rtc->ALARM1_DAY_MONTH;
  alarm1_time->DAY   = (temp & 0x1F);
  alarm1_time->MONTH = (temp & 0xF00) >> 8;
  alarm1_time->YEAR  = (cobra_rtc->ALARM1_YEAR & 0x7F);
}


/** Set alarm2 date and time
 * @param alrm  - pointer to time+date structure
 */
void coneyrtc_set_alarm2 (volatile COBRA_RTC_TIME_DATE_T *alarm2_time)
{
	cobra_rtc->ALARM2_SEC_MIN   = (alarm2_time->MIN   << 8) | alarm2_time->SEC;
	cobra_rtc->ALARM2_HOUR      =  alarm2_time->HOUR;
	cobra_rtc->ALARM2_DAY_MONTH = (alarm2_time->MONTH << 8) | alarm2_time->DAY;
	cobra_rtc->ALARM2_YEAR      =  alarm2_time->YEAR;
}

/** Get alarm2 date and time
 * @param alrm - Pointer to returned time+date structure
 */
void cobra_rtc_get_alarm2 (volatile COBRA_RTC_TIME_DATE_T *alarm2_time)
{
  unsigned int short temp;

  temp = cobra_rtc->ALARM2_SEC_MIN;
  alarm2_time->SEC   = (temp & 0x3F);
  alarm2_time->MIN   = (temp & 0x3F00) >> 8;

  alarm2_time->HOUR  = (cobra_rtc->ALARM2_HOUR & 0x1F);

  temp = cobra_rtc->ALARM2_DAY_MONTH;
  alarm2_time->DAY   = (temp & 0x1F);
  alarm2_time->MONTH = (temp & 0xF00) >> 8;
  alarm2_time->YEAR  = (cobra_rtc->ALARM2_YEAR & 0x7F);
}


void app_cobra_rtc_wait_date_ready(void)
{
		while(!(cobra_rtc->CONTROL & 0x01));	
}

#if APP_COBRA_RTC_EN
void cobra_rtc_interrupt(void)
{
    
		/* Clear whichever interrupt was requested */
		if (cobra_rtc_second_interrupt_status())
		{
			//printf("RTC second time \n");
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
      app_mesh_tscns_inc_tai_seconds();
#endif	

			cobra_rtc_clear_second_interrupt_status();
			
		}

		if (cobra_rtc_minuter_interrupt_status())
		{

			cobra_rtc_clear_minuter_interrupt_status();
		}

		if (cobra_rtc_hour_interrupt_status())
		{
	
			cobra_rtc_clear_hour_interrupt_status();
		}

		if (cobra_rtc_day_interrupt_status())
		{
	
			cobra_rtc_clear_day_interrupt_status();
		}

		if (cobra_rtc_month_interrupt_status())
		{

			cobra_rtc_clear_month_interrupt_status();
		}

		if (cobra_rtc_year_interrupt_status())
		{

			cobra_rtc_clear_year_interrupt_status();
		}

		if (cobra_rtc_alarm1_interrupt_status())
		{

			cobra_rtc_clear_alarm1_interrupt_status();
		}

		if (cobra_rtc_alarm2_interrupt_status())
		{
		
			cobra_rtc_clear_alarm2_interrupt_status();
		}
		
}

void app_cobra_rtc_init(void)
{
		COBRA_RTC_TIME_DATE_T alarm;
		COBRA_RTC_TIME_DATE_T time;
	

		cobra4_aon_wk_protection_en(1);

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
		cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_XTAL32K);  
#else
#if (APP_SLEEP_MODE_ENABLE == 1)			
			cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_XTAL32K);
#else
      cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_RCOSC32K);  
#endif

#endif

		cobra4_aon_wk_protection_en(0);
	
		cobra4_aon_pd_top_protection_en(1);
	
		cobra4_aon_pd_top_aon2mcu_source3_selection(AON_RTC_INT0);
		cobra4_aon_pd_top_protection_en(0);
	
		cobra_rtc_work_mode(USER_MODE);
		cobra_rtc_timer_write_en(1);
		cobra_rtc_alarm1_write_en(1);
		//cobra_rtc_freeze_mode_en(1);

		__here;
#if (APP_MOSQUITO_KILLING_ENABLE == 1)
		/* Setup current date/time params */
		time.SUBSEC = 0;
		time.SEC	= 0;
		time.MIN	= 0;
		time.HOUR	= 12;
		time.DAY	= 1;
		time.MONTH	= 1;
		time.YEAR	= 21;
#else
		/* Setup current date/time params */
		time.SUBSEC = 0;
		time.SEC    = 0;
		time.MIN    = 5;
		time.HOUR   = 14;
		time.DAY    = 30;
		time.MONTH  = 1;
		time.YEAR   = 19;
#endif		
		cobra_rtc_set_current_time_date(&time);
		

		
		/* Enable 'second' interrupts */
	//	cobra_rtc_minute_interrupt_en(1);
		cobra_rtc_second_interrupt_en(1);

		/* Setup alarm1 params */
		alarm.SUBSEC = 0;
		alarm.SEC    = 0;
		alarm.MIN    = 6;
		alarm.HOUR   = 14;
		alarm.DAY    = 30;
		alarm.MONTH  = 1;
		alarm.YEAR   = 19;
		cobra_rtc_set_alarm1(&alarm);

		cobra_rtc_timer_write_en(0);
		cobra_rtc_alarm1_write_en(0);
		
		/* Enable RTC counters */
		cobra_rtc_en(1);
	

		NVIC_ClearPendingIRQ(AON_EVENT3_IRQn);
		NVIC_EnableIRQ(AON_EVENT3_IRQn);
		NVIC_SetPriority(AON_EVENT3_IRQn, 1);
			

		
		cobra_rtc_get_current_time_date(&time);
				
}

void app_cobra_rtc_uninit(void)
{
			NVIC_DisableIRQ(AON_EVENT3_IRQn);
			cobra_rtc_en(0);
			cobra_rtc_second_interrupt_en(0);
	
}

#endif

#endif














