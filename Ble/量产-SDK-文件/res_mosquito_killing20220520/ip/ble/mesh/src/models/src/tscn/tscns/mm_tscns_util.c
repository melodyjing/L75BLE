/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_util.c
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/2

	Description :   


	Changed Log	:

		Liujian  2021/2/2		- Creation

*****************************************************************************/

#include "mm_tscns_util.h"


#define DAYS_400YEAR            ((uint32_t)(400*365 + 400/4 - 4 + 1))   // = 146103
#define DAYS_100YEAR(leap)      ((uint32_t)(100*365 + 100/4 - (leap ? 0 : 1)))       // = 36524 for no leap400
#define DAYS_4YEAR(leap)        ((uint32_t)(4*365 + (leap ? 1 : 0)))                 // = 1461
#define DAYS_1YEAR(leap)        ((uint32_t)(1*365 + (leap ? 1 : 0)))


const uint8_t days_by_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static inline uint8_t is_leap_year(const uint32_t year)
{

    return ((year%4==0 && year%100!=0) || year%400==0);

}


static inline uint8_t get_days_by_month(uint32_t leap_year, uint8_t month)
{
    if(leap_year && (2==month))
		{
        return 29;
    }
		else
		{
        return days_by_month[(month-1)];
    }
}

uint8_t get_days_one_month(uint32_t year, uint8_t month)
{
    return get_days_by_month(is_leap_year(year), month);
}


int8_t is_valid_utc(mesh_utc_t *p_utc)
{
    uint8_t days = get_days_by_month(is_leap_year(p_utc->year), p_utc->month);
    
    if((p_utc->year < YEAR_BASE || p_utc->year >= YEAR_MAX )
      ||(!p_utc->month || (p_utc->month > 12))
      ||(!p_utc->day || (p_utc->day > days))
      ||(p_utc->hour >= 24)
      ||(p_utc->minute >= 60)
      ||(p_utc->second >= 60)){
        return 0;
    }

    return 1;
}


int is_valid_tai_second(int32_t second)
{
    return (second && second < TAI_SECOND_MAX);
}


static inline uint8_t get_week(mesh_utc_t *p_utc)
{
    uint32_t y = p_utc->year;
    uint32_t m = p_utc->month;
    uint32_t d = p_utc->day;
    if((1 == m) || (2 == m)){
        y = y - 1;
        m = m + 12;
    }
    //monday is 0
    uint8_t week = ((d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)) % 7;
    return week;
}

int get_utc(const int32_t second, mesh_utc_t *p_utc)
{
    uint32_t count_days = (uint32_t)(second / (3600*24));
    uint32_t y = YEAR_BASE, m = 1;
    uint32_t w = ((count_days + WEEK_BASE) % 7);


    int leap_100_flag = 1;  // leap year for 2000
    if(count_days >= DAYS_400YEAR)
		{
        y+= (count_days / DAYS_400YEAR) * 400;
        count_days %= DAYS_400YEAR;
    }

    if(count_days >= DAYS_100YEAR(1))
		{
        uint32_t temp = (count_days - 1);
        y += (temp / DAYS_100YEAR(0)) * 100;
        count_days = temp % DAYS_100YEAR(0);
        leap_100_flag = 0;
    }
    
    int flag_0_3year = 1;
    if(count_days >=  DAYS_4YEAR(leap_100_flag))
		{
        uint32_t temp = (count_days + (leap_100_flag ? 0 : 1));
        y += (temp / DAYS_4YEAR(1)) * 4;
        count_days = (temp) % DAYS_4YEAR(1);
        flag_0_3year = 0;
    }

    int leap_4 = (leap_100_flag || (!flag_0_3year));
    if(count_days >= DAYS_1YEAR(leap_4))
		{
        if(leap_4)
				{
            y++;
            count_days -= DAYS_1YEAR(1);
        }
        if(count_days >= DAYS_1YEAR(0))
				{
            y += (count_days / DAYS_1YEAR(0)) * 1;
            count_days = count_days % DAYS_1YEAR(0);
        }
    }
    
    while(count_days)
		{
        uint32_t days_one_month = get_days_by_month(is_leap_year(y), m);
        if(count_days >= days_one_month)
				{
            count_days -= days_one_month;
            m++;
        }
				else
				{
            break;
        }
    }

    p_utc->year = (uint16_t)y;
    p_utc->month = (uint8_t)(m);
    p_utc->day = count_days + 1;
    p_utc->hour = (second / 3600 % 24);
    p_utc->minute = (second / 60 % 60);
    p_utc->second = (second % 60);
    p_utc->week = w;
		
    return 0;

}


uint32_t get_tai_sec( mesh_utc_t *p_utc )
{
    uint16_t  curyear;                    // current year
    uint32_t  cday = 0;                   // total days from base year
    uint32_t  curmonthday = 0;            // current month days

    curyear= p_utc->year;
    if(!is_valid_utc(p_utc)){
        return 0;
    }
    
    uint32_t cyear= curyear - YEAR_BASE;

    uint32_t temp = 0;
    int leap_100_flag = 1;      // leap year for 2000
    if(cyear >= 400)
		{
        temp = (cyear / 400);
        cyear -= temp * 400;
        cday += temp * DAYS_400YEAR;
    }

    if(cyear >= 100)
		{
        temp = (cyear / 100);
        cyear -= temp * 100;
        cday += temp * DAYS_100YEAR(0) + 1;
        leap_100_flag = 0;
    }

    int flag_0_3year = 1;
    if(cyear >= 4)
		{
        temp = (cyear / 4);
        cyear -= temp * 4;
        cday += temp * DAYS_4YEAR(1) - (leap_100_flag ? 0 : 1);
        flag_0_3year = 0;
    }
    
    if(cyear > 0)
		{
        cday += (cyear)*DAYS_1YEAR(0) + 1 - ((!leap_100_flag && flag_0_3year) ? 1 : 0);
        cyear = 0;
    }


    // no days for current month
    switch(p_utc->month ){
        case  2:  curmonthday = 31;  break;
        case  3:  curmonthday = 59;  break;
        case  4:  curmonthday = 90;  break;
        case  5:  curmonthday = 120; break;
        case  6:  curmonthday = 151; break;
        case  7:  curmonthday = 181; break;
        case  8:  curmonthday = 212; break;
        case  9:  curmonthday = 243; break;
        case 10:  curmonthday = 273; break;
        case 11:  curmonthday = 304; break;
        case 12:  curmonthday = 334; break;
        default:  curmonthday = 0;   break;
    }
    if(is_leap_year(curyear) && (p_utc->month >= 3)){
        curmonthday+= 1;
    }
    cday += curmonthday; 
    cday += (p_utc->day-1);	//no days for current day

    return (((cday*24+p_utc->hour)*60+p_utc->minute)*60+p_utc->second);  // TAI second
		
}

