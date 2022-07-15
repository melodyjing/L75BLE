/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_rtc.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/5

	Description :   
	
	Changed Log	:

		Liujian  2019/5/5		- Creation



*****************************************************************************/

#ifndef __COBRA_RTC_H__
#define __COBRA_RTC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_RTC_EN
typedef struct cobra_rtc_type
{
			__attribute__ ((aligned (4))) volatile unsigned int CONTROL;                    /**< Control fields  */
			__attribute__ ((aligned (4))) volatile unsigned int INT_ENABLE;                 /**< Interrupt enable */
			__attribute__ ((aligned (4))) volatile unsigned int INT_STATUS;                 /**< Interrupt status */
			__attribute__ ((aligned (4))) volatile unsigned int PRESCALER;                  /**< RTC prescaler */
			__attribute__ ((aligned (4))) volatile unsigned int TRIM;                       /**< Clock trimming value */
			__attribute__ ((aligned (4))) volatile unsigned int TIME_SSEC;                  /**< Current time, sub-seconds */
			__attribute__ ((aligned (4))) volatile unsigned int TIME_SEC_MIN;               /**< Current time, seconds + mins */
			__attribute__ ((aligned (4))) volatile unsigned int TIME_HOUR;                  /**< Current time, hours */
			__attribute__ ((aligned (4))) volatile unsigned int DATE_DAY_MONTH;             /**< Current date, day + month */
			__attribute__ ((aligned (4))) volatile unsigned int DATE_YEAR;                  /**< Current date, years */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM1_SEC_MIN;             /**< Alarm1 time, seconds + mins */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM1_HOUR;                /**< Alarm1 time, hours */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM1_DAY_MONTH;           /**< Alarm1 date, day + month */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM1_YEAR;                /**< Alarm1 date, years */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM2_SEC_MIN;             /**< Alarm2 time, seconds + mins */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM2_HOUR;                /**< Alarm2 time, hours */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM2_DAY_MONTH;           /**< Alarm2 date, day + month */
			__attribute__ ((aligned (4))) volatile unsigned int ALARM2_YEAR;                /**< Alarm2 date, years */

} COBRA_RTC_TypeDef; 


/**
 *  Structure type to conveniently manipulate time+date information
 */
typedef struct cobra_rtc_time_date
{
  unsigned int SUBSEC : 15;      /**< sub-second field, from 0 to 32767  */
  unsigned int SEC    :  6;      /**< second field, from 0 to 59         */
  unsigned int MIN    :  6;      /**< minute field, from 0 to 59         */
  unsigned int HOUR   :  5;      /**< hour field, from 0 to 23           */
  unsigned int DAY    :  5;      /**< day field, from 1 to [28|29|30|31] */
  unsigned int MONTH  :  4;      /**< month field, from 1 to 12          */
  unsigned int YEAR   :  7;      /**< year field, from 00 to 99          */
}COBRA_RTC_TIME_DATE_T;


typedef enum rtc_work_mode {
	USER_MODE,
	TEST_MODE
}RTC_WORK_MODE;


#define COBRA_RTC_BASE				(0x40080000UL)



void cobra_rtc_init(void);

/** Set current date and time
 * @param td  - Pointer to time+date structure
 */
void cobra_rtc_set_current_time_date (volatile COBRA_RTC_TIME_DATE_T *td);
/** Get current date and time
 * @param td  - Pointer to returned time+date structure
 */
void cobra_rtc_get_current_time_date (volatile COBRA_RTC_TIME_DATE_T *current_time);
/** Get alarm2 date and time
 * @param alrm - Pointer to returned time+date structure
 */
void cobra_rtc_get_alarm2 (volatile COBRA_RTC_TIME_DATE_T *alarm2_time);
/** Set alarm2 date and time
 * @param alrm  - pointer to time+date structure
 */
void cobra_rtc_set_alarm2 (volatile COBRA_RTC_TIME_DATE_T *alarm2_time);
/** Get alarm1 date and time
 * @param alrm - Pointer to returned time+date structure
 */
void cobra_rtc_get_alarm1 (volatile COBRA_RTC_TIME_DATE_T *alarm1_time);
/** Set alarm1 date and time
 * @param alrm  - pointer to time+date structure
 */
void cobra_rtc_set_alarm1 (volatile COBRA_RTC_TIME_DATE_T *alarm1_time);


void cobra_rtc_interrupt_function(void);

void app_cobra_rtc_init(void);

void app_cobra_rtc_uninit(void);

void  cobra_rtc_interrupt(void);

void cobra_rtc_work_mode(RTC_WORK_MODE work_mode);

void cobra_rtc_freeze_mode_en(unsigned int en);

void cobra_rtc_timer_write_en(unsigned int en);

void cobra_rtc_alarm1_write_en(unsigned int en);

void cobra_rtc_second_interrupt_en(unsigned int en);

void cobra_rtc_minute_interrupt_en(unsigned int en);

void cobra_rtc_hours_interrupt_en(unsigned int en);

void cobra_rtc_days_interrupt_en(unsigned int en);

void cobra_rtc_months_interrupt_en(unsigned int en);

void cobra_rtc_years_interrupt_en(unsigned int en);

void cobra_rtc_alarm1_interrupt_en(unsigned int en);

void cobra_rtc_alarm2_interrupt_en(unsigned int en);

unsigned int cobra_rtc_second_interrupt_status(void);

void cobra_rtc_clear_second_interrupt_status(void);

unsigned int cobra_rtc_minuter_interrupt_status(void);

void cobra_rtc_clear_minuter_interrupt_status(void);

unsigned int cobra_rtc_hour_interrupt_status(void);

void cobra_rtc_clear_hour_interrupt_status(void);

unsigned int cobra_rtc_day_interrupt_status(void);

void cobra_rtc_clear_day_interrupt_status(void);

unsigned int cobra_rtc_month_interrupt_status(void);

void cobra_rtc_clear_month_interrupt_status(void);

unsigned int cobra_rtc_year_interrupt_status(void);

void cobra_rtc_clear_year_interrupt_status(void);

unsigned int cobra_rtc_alarm1_interrupt_status(void);

void cobra_rtc_clear_alarm1_interrupt_status(void);

unsigned int cobra_rtc_alarm2_interrupt_status(void);

void cobra_rtc_clear_alarm2_interrupt_status(void);

void cobra_rtc_en(unsigned int en);


#if APP_COBRA_RTC_EN
void  app_cobra_rtc_init(void);
#endif
#endif   //COBRA_RTC_EN



#endif

