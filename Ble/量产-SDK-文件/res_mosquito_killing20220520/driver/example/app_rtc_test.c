
#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
#include "cobra_system.h"
#if (COBRA_RTC_DRIVER_TEST)
void print_time_date(volatile COBRA_RTC_TIME_DATE_T *td, const char *prefix)
{
    printf("%s%02d:%02d:%02d on %02d-%02d-20%02d\n", prefix, td->HOUR, td->MIN, td->SEC, td->DAY, td->MONTH, td->YEAR);
}


bool second_int = 0;
bool day_int = 0;
bool year_int = 0;
bool alarm1_int = 0;
void rdw_rtc_test(void)
{
    unsigned int          ctr;

	COBRA_RTC_TIME_DATE_T alarm;
	COBRA_RTC_TIME_DATE_T time;
    
	cobra4_aon_pd_top_protection_en(1);

	cobra4_aon_pd_top_aon2mcu_source3_selection(AON_RTC_INT0);
    cobra4_aon_pd_top_rtc_gclk_gate_en(1);
    cobra4_aon_pd_top_rtc0_pclk_en(1); 
	cobra4_aon_pd_top_protection_en(0);
    
    for(int i = 24000;i>0;i--);
    NVIC_SetPriority((IRQn_Type)AON_EVENT3_IRQn, 1);
	NVIC_ClearPendingIRQ((IRQn_Type)AON_EVENT3_IRQn);
	NVIC_EnableIRQ((IRQn_Type)AON_EVENT3_IRQn);
    
//	cobra4_aon_wk_protection_en(1);
//	cobra4_aon_wk_aon_rdw_clk_selection(rdw_CLK_XTAL32K);
//	cobra4_aon_wk_protection_en(0);

    cobra_rtc_en(0);
	cobra_rtc_work_mode(USER_MODE);
	cobra_rtc_timer_write_en(1);
	cobra_rtc_alarm1_write_en(1);

	/* Setup current date/time params */
    time.SUBSEC = 0;
    time.SEC    = 25;
    time.MIN    = 59;
    time.HOUR   = 23;
    time.DAY    = 31;
    time.MONTH  = 12;
    time.YEAR   = 20;
	cobra_rtc_set_current_time_date(&time);

	/* Enable 'second' interrupts */
	//	cobra_rtc_minute_interrupt_en(1);
    for(int i = 24000;i>0;i--);
	cobra_rtc_second_interrupt_en(1);

	/* Setup alarm1 params */
    alarm.SEC   = 10;
    alarm.MIN   = 0;
    alarm.HOUR  = 0;
    alarm.DAY   = 1;
    alarm.MONTH = 1;
    alarm.YEAR  = 16;
	cobra_rtc_set_alarm1(&alarm);

	cobra_rtc_timer_write_en(0);
	cobra_rtc_alarm1_write_en(0);

	/* Enable RTC counters */
	cobra_rtc_en(1);
    printf("Loop for 10 seconds...\n");
    ctr = 0;
    while (ctr < 10)
    {
        while(second_int == 0);
        second_int = 0;
        cobra_rtc_get_current_time_date(&time);
        print_time_date(&time, "CURRENT_TIME: ");
        ctr++;
    }
    printf("\n");

    cobra_rtc_get_alarm1(&time);
    print_time_date(&time, "SET ALARM1: ");

    /* Disable 'second' interrupts */
    for(int i = 24000;i>0;i--);
    cobra_rtc_second_interrupt_en(0);
    
    /* Enable 'day' interrupts */
    for(int i = 24000;i>0;i--);
    cobra_rtc_years_interrupt_en(1);//rdw_rtc_day_int_enable(rdw_rtc);

    printf("\n");
    cobra_rtc_get_current_time_date(&time);
    print_time_date(&time, "CURRENT_TIME: ");
    printf("Wait for YEAR interrupt...\n");
    while(year_int == 0);
    year_int = 0;
    cobra_rtc_get_current_time_date(&time);
    print_time_date(&time, "CURRENT_TIME: ");

    /* Disable 'day' interrupts */
    for(int i = 24000;i>0;i--);
    cobra_rtc_years_interrupt_en(0);

    /* Enable 'alarm1' interrupts */
    for(int i = 24000;i>0;i--);
    cobra_rtc_alarm1_interrupt_en(1);
    printf("\n");
    printf("Wait for ALARM1 interrupt...\n");
    while(alarm1_int == 0);
    alarm1_int = 0;
    cobra_rtc_get_current_time_date(&time);
    print_time_date(&time, "CURRENT_TIME: ");
    printf("\nDONE\n");
}

void cobra_aon_event3_interrupt(void)
{
//    printf("AON_EVENT_3\n");
    if(cobra4_aon_pd_top_aon2mcu_source3_get() == AON_RTC_INT0)
    {	
        /* Clear whichever interrupt was requested */
        if (cobra_rtc_second_interrupt_status())
        {
            second_int = 1;
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
            day_int = 1;
            cobra_rtc_clear_day_interrupt_status();
        }
        if (cobra_rtc_month_interrupt_status())
        {
            cobra_rtc_clear_month_interrupt_status();
        }
        if (cobra_rtc_year_interrupt_status())
        {
            year_int = 1;
            cobra_rtc_clear_year_interrupt_status();
        }
        if (cobra_rtc_alarm1_interrupt_status())
        {
            alarm1_int = 1;
            cobra_rtc_clear_alarm1_interrupt_status();
        }
        if (cobra_rtc_alarm2_interrupt_status())
        {
            cobra_rtc_clear_alarm2_interrupt_status();
        }
    }
}
#endif