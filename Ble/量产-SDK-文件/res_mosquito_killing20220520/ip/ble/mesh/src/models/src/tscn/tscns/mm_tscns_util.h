/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_util.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/2

	Description :   


	Changed Log	:

		Liujian  2021/2/2		- Creation

*****************************************************************************/

#ifndef __MM_TSCNS_UTIL_H__
#define __MM_TSCNS_UTIL_H__
#include "mm_tscns.h"


// internal -----------
#define YEAR_BASE       (2000)
#define WEEK_BASE       (6-1)  // 0 means monday.  2000/01/02: Saturday

#define YEAR_MAX        (YEAR_BASE + 100)   // (2000 -- 2099)
#define TAI_SECOND_MAX  (0xbc191380)        // (2100/01/01 00:00:00)
#define SECOND_ONE_DAY  (60*60*24)

#define OFFSET_1970_2000        (946684800)
//#define OFFSET_1970_2000_EAST8  (946684800 - 8*3600)
#define TIME_ZONE_0     (0x40)


typedef struct{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t week;    // 0 means monday
}mesh_utc_t;


static inline int16_t get_time_zone_offset_min(uint8_t zone_offset)
{
    return (int16_t)(((int32_t)zone_offset - 64)*15);
}


static inline uint8_t get_time_zone_offset(int32_t zone_min) // Positive numbers are eastwards
{
    return (uint8_t)((zone_min/15) + 64);
}


int get_utc(const int32_t second, mesh_utc_t *p_utc);

uint32_t get_tai_sec( mesh_utc_t *p_utc);

int is_valid_tai_second(int32_t second);

//u32 get_local_TAI();
uint8_t get_days_one_month(uint32_t year, uint8_t month);

uint8_t get_days_current_month();

#define BIT(n)                  		(1<<(n) )

#endif


