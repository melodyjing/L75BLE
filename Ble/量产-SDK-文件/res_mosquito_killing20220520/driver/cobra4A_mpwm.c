/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_mpwm.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/5

	Description :   


	Changed Log	:

		Liujian  2020/8/5 		- Creation

*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "cobra4A_mpwm.h"
#if COBRA_MPWM_EN

#if (COBRA_POWER_DOWN_ENABLE)

static volatile COBRA_MPWM_T * cobra_mpwm	__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_MPWM_T * cobra_mpwm;
#endif

void cobra_mpwm_init(void)
{
	  cobra_mpwm = (COBRA_MPWM_T * )COBRA_MPWM_BASE;
}

#ifdef COBRA_MPWM_NEW
/*CON*/
#define DCMODE                                                                0x80000000UL
bool read_dcmode(void)
{
    return (cobra_mpwm->mpwm_con & DCMODE) == DCMODE;
}
#define ACMODE                                                                0x40000000UL
bool read_acmode(void)
{
    return (cobra_mpwm->mpwm_con & ACMODE) == ACMODE;
}
#define INVBDC                                                                0x20000000UL
bool read_invbdc(void)
{
    return (cobra_mpwm->mpwm_con & INVBDC) == INVBDC;
}
#define DISUP2                                                                0x00100000UL
bool read_disup2(void)
{
    return (cobra_mpwm->mpwm_con & DISUP2) == DISUP2;
}
#define DTE2                                                                  0x00080000UL
bool read_dte2(void)
{
    return (cobra_mpwm->mpwm_con & DTE2) == DTE2;
}
#define POLA2                                                                 0x00040000UL
bool read_pola2(void)
{
    return (cobra_mpwm->mpwm_con & POLA2) == POLA2;
}
#define CENTER2                                                               0x00020000UL
bool read_center2(void)
{
    return (cobra_mpwm->mpwm_con & CENTER2) == CENTER2;
}
#define RUN2                                                                  0x00010000UL
bool read_run2(void)
{
    return (cobra_mpwm->mpwm_con & RUN2) == RUN2;
}
#define DISUP1                                                                0x00001000UL
bool read_disup1(void)
{
    return (cobra_mpwm->mpwm_con & DISUP1) == DISUP1;
}
#define DTE1                                                                  0x00000800UL
bool read_dte1(void)
{
    return (cobra_mpwm->mpwm_con & DTE1) == DTE1;
}
#define POLA1                                                                 0x00000400UL
bool read_pola1(void)
{
    return (cobra_mpwm->mpwm_con & POLA1) == POLA1;
}
#define CENTER1                                                               0x00000200UL
bool read_center1(void)
{
    return (cobra_mpwm->mpwm_con & CENTER1) == CENTER1;
}
#define RUN1                                                                  0x00000100UL
bool read_run1(void)
{
    return (cobra_mpwm->mpwm_con & RUN1) == RUN1;
}
#define DISUP0                                                                0x00000010UL
bool read_disup0(void)
{
    return (cobra_mpwm->mpwm_con & DISUP0) == DISUP0;
}
#define DTE0                                                                  0x00000008UL
bool read_dte0(void)
{
    return (cobra_mpwm->mpwm_con & DTE0) == DTE0;
}
#define POLA0                                                                 0x00000004UL
bool read_pola0(void)
{
    return (cobra_mpwm->mpwm_con & POLA0) == POLA0;
}
#define CENTER0                                                               0x00000002UL
bool read_center0(void)
{
    return (cobra_mpwm->mpwm_con & CENTER0) == CENTER0;
}
#define RUN0                                                                  0x00000001UL
bool read_run0(void)
{
    return (cobra_mpwm->mpwm_con & RUN0) == RUN0;
}

/*CON_SET*/
#define DCMODE_SET                                                            0x80000000UL
void write_dcmode_set(void)
{
    cobra_mpwm->mpwm_con_set |= DCMODE_SET;
}
#define ACMODE_SET                                                            0x40000000UL
void write_acmode_set(void)
{
    cobra_mpwm->mpwm_con_set |= ACMODE_SET;
}
#define INVBDC_SET                                                            0x20000000UL
void write_invbdc_set(void)
{
    cobra_mpwm->mpwm_con_set |= INVBDC_SET;
}
#define DISUP2_SET                                                            0x00100000UL
void write_disup2_set(void)
{
    cobra_mpwm->mpwm_con_set |= DISUP2_SET;
}
#define DTE2_SET                                                              0x00080000UL
void write_dte2_set(void)
{
    cobra_mpwm->mpwm_con_set |= DTE2_SET;
}
#define POLA2_SET                                                             0x00040000UL
void write_pola2_set(void)
{
    cobra_mpwm->mpwm_con_set |= POLA2_SET;
}
#define CENTER2_SET                                                           0x00020000UL
void write_center2_set(void)
{
    cobra_mpwm->mpwm_con_set |= CENTER2_SET;
}
#define RUN2_SET                                                              0x00010000UL
void write_run2_set(void)
{
    cobra_mpwm->mpwm_con_set |= RUN2_SET;
}
#define DISUP1_SET                                                            0x00001000UL
void write_disup1_set(void)
{
    cobra_mpwm->mpwm_con_set |= DISUP1_SET;
}
#define DTE1_SET                                                              0x00000800UL
void write_dte1_set(void)
{
    cobra_mpwm->mpwm_con_set |= DTE1_SET;
}
#define POLA1_SET                                                             0x00000400UL
void write_pola1_set(void)
{
    cobra_mpwm->mpwm_con_set |= POLA1_SET;
}
#define CENTER1_SET                                                           0x00000200UL
void write_center1_set(void)
{
    cobra_mpwm->mpwm_con_set |= CENTER1_SET;
}
#define RUN1_SET                                                              0x00000100UL
void write_run1_set(void)
{
    cobra_mpwm->mpwm_con_set |= RUN1_SET;
}
#define DISUP0_SET                                                            0x00000010UL
void write_disup0_set(void)
{
    cobra_mpwm->mpwm_con_set |= DISUP0_SET;
}
#define DTE0_SET                                                              0x00000008UL
void write_dte0_set(void)
{
    cobra_mpwm->mpwm_con_set |= DTE0_SET;
}
#define POLA0_SET                                                             0x00000004UL
void write_pola0_set(void)
{
    cobra_mpwm->mpwm_con_set |= POLA0_SET;
}
#define CENTER0_SET                                                           0x00000002UL
void write_center0_set(void)
{
    cobra_mpwm->mpwm_con_set |= CENTER0_SET;
}
#define RUN0_SET                                                              0x00000001UL
void write_run0_set(void)
{
    cobra_mpwm->mpwm_con_set |= RUN0_SET;
}

/*CON_CLR*/
#define DCMODE_CLR                                                            0x80000000UL
void write_dcmode_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DCMODE_CLR;
}
#define ACMODE_CLR                                                            0x40000000UL
void write_acmode_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= ACMODE_CLR;
}
#define INVBDC_CLR                                                            0x20000000UL
void write_invbdc_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= INVBDC_CLR;
}
#define DISUP2_CLR                                                            0x00100000UL
void write_disup2_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DISUP2_CLR;
}
#define DTE2_CLR                                                              0x00080000UL
void write_dte2_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DTE2_CLR;
}
#define POLA2_CLR                                                             0x00040000UL
void write_pola2_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= POLA2_CLR;
}
#define CENTER2_CLR                                                           0x00020000UL
void write_center2_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= CENTER2_CLR;
}
#define RUN2_CLR                                                              0x00010000UL
void write_run2_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= RUN2_CLR;
}
#define DISUP1_CLR                                                            0x00001000UL
void write_disup1_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DISUP1_CLR;
}
#define DTE1_CLR                                                              0x00000800UL
void write_dte1_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DTE1_CLR;
}
#define POLA1_CLR                                                             0x00000400UL
void write_pola1_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= POLA1_CLR;
}
#define CENTER1_CLR                                                           0x00000200UL
void write_center1_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= CENTER1_CLR;
}
#define RUN1_CLR                                                              0x00000100UL
void write_run1_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= RUN1_CLR;
}
#define DISUP0_CLR                                                            0x00000010UL
void write_disup0_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DISUP0_CLR;
}
#define DTE0_CLR                                                              0x00000008UL
void write_dte0_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= DTE0_CLR;
}
#define POLA0_CLR                                                             0x00000004UL
void write_pola0_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= POLA0_CLR;
}
#define CENTER0_CLR                                                           0x00000002UL
void write_center0_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= CENTER0_CLR;
}
#define RUN0_CLR                                                              0x00000001UL
void write_run0_clr(void)
{
    cobra_mpwm->mpwm_con_clr |= RUN0_CLR;
}

/*CAPCON*/
#define RT2                                                                   0x00100000UL
bool read_rt2(void)
{
    return (cobra_mpwm->mpwm_capcon & RT2) == RT2;
}
#define RT1                                                                   0x00080000UL
bool read_rt1(void)
{
    return (cobra_mpwm->mpwm_capcon & RT1) == RT1;
}
#define RT0                                                                   0x00040000UL
bool read_rt0(void)
{
    return (cobra_mpwm->mpwm_capcon & RT0) == RT0;
}
#define CAP2MCI2_FE                                                           0x00020000UL
bool read_cap2mci2_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI2_FE) == CAP2MCI2_FE;
}
#define CAP2MCI2_RE                                                           0x00010000UL
bool read_cap2mci2_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI2_RE) == CAP2MCI2_RE;
}
#define CAP2MCI1_FE                                                           0x00008000UL
bool read_cap2mci1_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI1_FE) == CAP2MCI1_FE;
}
#define CAP2MCI1_RE                                                           0x00004000UL
bool read_cap2mci1_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI1_RE) == CAP2MCI1_RE;
}
#define CAP2MCI0_FE                                                           0x00002000UL
bool read_cap2mci0_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI0_FE) == CAP2MCI0_FE;
}
#define CAP2MCI0_RE                                                           0x00001000UL
bool read_cap2mci0_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP2MCI0_RE) == CAP2MCI0_RE;
}
#define CAP1MCI2_FE                                                           0x00000800UL
bool read_cap1mci2_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI2_FE) == CAP1MCI2_FE;
}
#define CAP1MCI2_RE                                                           0x00000400UL
bool read_cap1mci2_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI2_RE) == CAP1MCI2_RE;
}
#define CAP1MCI1_FE                                                           0x00000200UL
bool read_cap1mci1_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI1_FE) == CAP1MCI1_FE;
}
#define CAP1MCI1_RE                                                           0x00000100UL
bool read_cap1mci1_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI1_RE) == CAP1MCI1_RE;
}
#define CAP1MCI0_FE                                                           0x00000080UL
bool read_cap1mci0_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI0_FE) == CAP1MCI0_FE;
}
#define CAP1MCI0_RE                                                           0x00000040UL
bool read_cap1mci0_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP1MCI0_RE) == CAP1MCI0_RE;
}
#define CAP0MCI2_FE                                                           0x00000020UL
bool read_cap0mci2_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI2_FE) == CAP0MCI2_FE;
}
#define CAP0MCI2_RE                                                           0x00000010UL
bool read_cap0mci2_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI2_RE) == CAP0MCI2_RE;
}
#define CAP0MCI1_FE                                                           0x00000008UL
bool read_cap0mci1_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI1_FE) == CAP0MCI1_FE;
}
#define CAP0MCI1_RE                                                           0x00000004UL
bool read_cap0mci1_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI1_RE) == CAP0MCI1_RE;
}
#define CAP0MCI0_FE                                                           0x00000002UL
bool read_cap0mci0_fe(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI0_FE) == CAP0MCI0_FE;
}
#define CAP0MCI0_RE                                                           0x00000001UL
bool read_cap0mci0_re(void)
{
    return (cobra_mpwm->mpwm_capcon & CAP0MCI0_RE) == CAP0MCI0_RE;
}

/*CAPCON_SET*/
#define RT2_SET                                                               0x00100000UL
void write_rt2_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= RT2_SET;
}
#define RT1_SET                                                               0x00080000UL
void write_rt1_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= RT1_SET;
}
#define RT0_SET                                                               0x00040000UL
void write_rt0_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= RT0_SET;
}
#define CAP2MCI2_FE_SET                                                       0x00020000UL
void write_cap2mci2_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI2_FE_SET;
}
#define CAP2MCI2_RE_SET                                                       0x00010000UL
void write_cap2mci2_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI2_RE_SET;
}
#define CAP2MCI1_FE_SET                                                       0x00008000UL
void write_cap2mci1_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI1_FE_SET;
}
#define CAP2MCI1_RE_SET                                                       0x00004000UL
void write_cap2mci1_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI1_RE_SET;
}
#define CAP2MCI0_FE_SET                                                       0x00002000UL
void write_cap2mci0_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI0_FE_SET;
}
#define CAP2MCI0_RE_SET                                                       0x00001000UL
void write_cap2mci0_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP2MCI0_RE_SET;
}
#define CAP1MCI2_FE_SET                                                       0x00000800UL
void write_cap1mci2_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI2_FE_SET;
}
#define CAP1MCI2_RE_SET                                                       0x00000400UL
void write_cap1mci2_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI2_RE_SET;
}
#define CAP1MCI1_FE_SET                                                       0x00000200UL
void write_cap1mci1_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI1_FE_SET;
}
#define CAP1MCI1_RE_SET                                                       0x00000100UL
void write_cap1mci1_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI1_RE_SET;
}
#define CAP1MCI0_FE_SET                                                       0x00000080UL
void write_cap1mci0_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI0_FE_SET;
}
#define CAP1MCI0_RE_SET                                                       0x00000040UL
void write_cap1mci0_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP1MCI0_RE_SET;
}
#define CAP0MCI2_FE_SET                                                       0x00000020UL
void write_cap0mci2_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI2_FE_SET;
}
#define CAP0MCI2_RE_SET                                                       0x00000010UL
void write_cap0mci2_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI2_RE_SET;
}
#define CAP0MCI1_FE_SET                                                       0x00000008UL
void write_cap0mci1_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI1_FE_SET;
}
#define CAP0MCI1_RE_SET                                                       0x00000004UL
void write_cap0mci1_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI1_RE_SET;
}
#define CAP0MCI0_FE_SET                                                       0x00000002UL
void write_cap0mci0_fe_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI0_FE_SET;
}
#define CAP0MCI0_RE_SET                                                       0x00000001UL
void write_cap0mci0_re_set(void)
{
    cobra_mpwm->mpwm_capcon_set |= CAP0MCI0_RE_SET;
}

/*CAPCON_CLR*/
#define RT2_CLR                                                               0x00100000UL
void write_rt2_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= RT2_CLR;
}
#define RT1_CLR                                                               0x00080000UL
void write_rt1_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= RT1_CLR;
}
#define RT0_CLR                                                               0x00040000UL
void write_rt0_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= RT0_CLR;
}
#define CAP2MCI2_FE_CLR                                                       0x00020000UL
void write_cap2mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI2_FE_CLR;
}
#define CAP2MCI2_RE_CLR                                                       0x00010000UL
void write_cap2mci2_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI2_RE_CLR;
}
#define CAP2MCI1_FE_CLR                                                       0x00008000UL
void write_cap2mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI1_FE_CLR;
}
#define CAP2MCI1_RE_CLR                                                       0x00004000UL
void write_cap2mci1_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI1_RE_CLR;
}
#define CAP2MCI0_FE_CLR                                                       0x00002000UL
void write_cap2mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI0_FE_CLR;
}
#define CAP2MCI0_RE_CLR                                                       0x00001000UL
void write_cap2mci0_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP2MCI0_RE_CLR;
}
#define CAP1MCI2_FE_CLR                                                       0x00000800UL
void write_cap1mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI2_FE_CLR;
}
#define CAP1MCI2_RE_CLR                                                       0x00000400UL
void write_cap1mci2_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI2_RE_CLR;
}
#define CAP1MCI1_FE_CLR                                                       0x00000200UL
void write_cap1mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI1_FE_CLR;
}
#define CAP1MCI1_RE_CLR                                                       0x00000100UL
void write_cap1mci1_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI1_RE_CLR;
}
#define CAP1MCI0_FE_CLR                                                       0x00000080UL
void write_cap1mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI0_FE_CLR;
}
#define CAP1MCI0_RE_CLR                                                       0x00000040UL
void write_cap1mci0_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP1MCI0_RE_CLR;
}
#define CAP0MCI2_FE_CLR                                                       0x00000020UL
void write_cap0mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI2_FE_CLR;
}
#define CAP0MCI2_RE_CLR                                                       0x00000010UL
void write_cap0mci2_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI2_RE_CLR;
}
#define CAP0MCI1_FE_CLR                                                       0x00000008UL
void write_cap0mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI1_FE_CLR;
}
#define CAP0MCI1_RE_CLR                                                       0x00000004UL
void write_cap0mci1_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI1_RE_CLR;
}
#define CAP0MCI0_FE_CLR                                                       0x00000002UL
void write_cap0mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI0_FE_CLR;
}
#define CAP0MCI0_RE_CLR                                                       0x00000001UL
void write_cap0mci0_re_clr(void)
{
    cobra_mpwm->mpwm_capcon_clr |= CAP0MCI0_RE_CLR;
}

/*TC0*/
void set_tc0(uint32_t tc0)
{
    cobra_mpwm->mpwm_tc[0] = tc0;
}
uint32_t read_tc0(void)
{
    return (cobra_mpwm->mpwm_tc[0]);
}

/*TC1*/
void set_tc1(uint32_t tc1)
{
    cobra_mpwm->mpwm_tc[1] = tc1;
}
uint32_t read_tc1(void)
{
    return (cobra_mpwm->mpwm_tc[1]);
}

/*TC2*/
void set_tc2(uint32_t tc2)
{
   cobra_mpwm->mpwm_tc[2] = tc2;
}
uint32_t read_tc2(void)
{
    return (cobra_mpwm->mpwm_tc[2]);
}

/*LIM0*/
void set_lim0(uint32_t lim0)
{
    cobra_mpwm->mpwm_lim[0] = lim0;
}
uint32_t read_lim0(void)
{
    return (cobra_mpwm->mpwm_lim[0]);
}

/*LIM1*/
void set_lim1(uint32_t lim1)
{
    cobra_mpwm->mpwm_lim[1] = lim1;
}
uint32_t read_lim1(void)
{
    return (cobra_mpwm->mpwm_lim[1]);
}

/*LIM2*/
void set_lim2(uint32_t lim2)
{
    cobra_mpwm->mpwm_lim[2] = lim2;
}
uint32_t read_lim2(void)
{
    return (cobra_mpwm->mpwm_lim[2]);
}

/*MAT0*/
void set_mat0(uint32_t mat0)
{
    cobra_mpwm->mpwm_mat[0] = mat0;
}
uint32_t read_mat0(void)
{
    return (cobra_mpwm->mpwm_mat[0]);
}

/*MAT1*/
void set_mat1(uint32_t mat1)
{
    cobra_mpwm->mpwm_mat[1] = mat1;
}
uint32_t read_mat1(void)
{
    return (cobra_mpwm->mpwm_mat[1]);
}

/*MAT2*/
void set_mat2(uint32_t mat2)
{
    cobra_mpwm->mpwm_mat[2] = mat2;
}
uint32_t read_mat2(void)
{
    return (cobra_mpwm->mpwm_mat[2]);
}

/*DT*/
#define DT0_POW                                                                        0UL
#define DT0_MASK                                                              0x000003ffUL
void set_dt0(uint32_t dt0)
{
    cobra_mpwm->mpwm_dt &= ~DT0_MASK;
    cobra_mpwm->mpwm_dt |= dt0<<DT0_POW;
}
uint32_t read_dt0(void)
{
    return (cobra_mpwm->mpwm_dt & DT0_MASK) >> DT0_POW;
}
#define DT1_POW                                                                       10UL
#define DT1_MASK                                                              0x000ffc00UL
void set_dt1(uint32_t dt1)
{
    cobra_mpwm->mpwm_dt &= ~DT1_MASK;
    cobra_mpwm->mpwm_dt |= dt1<<DT1_POW;
}
uint32_t read_dt1(void)
{
    return (cobra_mpwm->mpwm_dt & DT1_MASK) >> DT1_POW;
}
#define DT2_POW                                                                       20UL
#define DT2_MASK                                                              0x3ff00000UL
void set_dt2(uint32_t dt2)
{
    cobra_mpwm->mpwm_dt &= ~DT2_MASK;
    cobra_mpwm->mpwm_dt |= dt2<<DT2_POW;
}
uint32_t read_dt2(void)
{
    return (cobra_mpwm->mpwm_dt & DT2_MASK) >> DT2_POW;
}

/*MCCP*/
#define CCPB2                                                                 0x00000020UL
void set_ccpb2(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPB2;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPB2;
    }
}
bool read_ccpb2(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPB2) == CCPB2;
}
#define CCPA2                                                                 0x00000010UL
void set_ccpa2(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPA2;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPA2;
    }
}
bool read_ccpa2(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPA2) == CCPA2;
}
#define CCPB1                                                                 0x00000008UL
void set_ccpb1(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPB1;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPB1;
    }
}
bool read_ccpb1(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPB1) == CCPB1;
}
#define CCPA1                                                                 0x00000004UL
void set_ccpa1(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPA1;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPA1;
    }
}
bool read_ccpa1(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPA1) == CCPA1;
}
#define CCPB0                                                                 0x00000002UL
void set_ccpb0(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPB0;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPB0;
    }
}
bool read_ccpb0(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPB0) == CCPB0;
}
#define CCPA0                                                                 0x00000001UL
void set_ccpa0(bool en)
{
    if(en)
    {
        cobra_mpwm->mpwm_ccp |= CCPA0;
    }else
    {
        cobra_mpwm->mpwm_ccp &= ~CCPA0;
    }
}
bool read_ccpa0(void)
{
    return (cobra_mpwm->mpwm_ccp & CCPA0) == CCPA0;
}

/*CAP0*/
uint32_t read_cap0(void)
{
    return (cobra_mpwm->mpwm_cap[0]);
}

/*CAP1*/
uint32_t read_cap1(void)
{
    return (cobra_mpwm->mpwm_cap[1]);
}

/*CAP2*/
uint32_t read_cap2(void)
{
    return (cobra_mpwm->mpwm_cap[2]);
}

/*INTEN*/
#define ABORT_INT                                                             0x00008000UL
bool read_abort_int(void)
{
    return (cobra_mpwm->mpwm_inten & ABORT_INT) == ABORT_INT;
}
#define ICAP2_INT                                                             0x00000400UL
bool read_icap2_int(void)
{
    return (cobra_mpwm->mpwm_inten & ICAP2_INT) == ICAP2_INT;
}
#define IMAT2_INT                                                             0x00000200UL
bool read_imat2_int(void)
{
    return (cobra_mpwm->mpwm_inten & IMAT2_INT) == IMAT2_INT;
}
#define ILIM2_INT                                                             0x00000100UL
bool read_ilim2_int(void)
{
    return (cobra_mpwm->mpwm_inten & ILIM2_INT) == ILIM2_INT;
}
#define ICAP1_INT                                                             0x00000040UL
bool read_icap1_int(void)
{
    return (cobra_mpwm->mpwm_inten & ICAP1_INT) == ICAP1_INT;
}
#define IMAT1_INT                                                             0x00000020UL
bool read_imat1_int(void)
{
    return (cobra_mpwm->mpwm_inten & IMAT1_INT) == IMAT1_INT;
}
#define ILIM1_INT                                                             0x00000010UL
bool read_ilim1_int(void)
{
    return (cobra_mpwm->mpwm_inten & ILIM1_INT) == ILIM1_INT;
}
#define ICAP0_INT                                                             0x00000004UL
bool read_icap0_int(void)
{
    return (cobra_mpwm->mpwm_inten & ICAP0_INT) == ICAP0_INT;
}
#define IMAT0_INT                                                             0x00000002UL
bool read_imat0_int(void)
{
    return (cobra_mpwm->mpwm_inten & IMAT0_INT) == IMAT0_INT;
}
#define ILIM0_INT                                                             0x00000001UL
bool read_ilim0_int(void)
{
    return (cobra_mpwm->mpwm_inten & ILIM0_INT) == ILIM0_INT;
}

/*INTEN_SET*/
#define ABORT_INT_SET                                                         0x00008000UL
void write_abort_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ABORT_INT_SET;
}
#define ICAP2_INT_SET                                                         0x00000400UL
void write_icap2_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ICAP2_INT_SET;
}
#define IMAT2_INT_SET                                                         0x00000200UL
void write_imat2_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= IMAT2_INT_SET;
}
#define ILIM2_INT_SET                                                         0x00000100UL
void write_ilim2_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ILIM2_INT_SET;
}
#define ICAP1_INT_SET                                                         0x00000040UL
void write_icap1_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ICAP1_INT_SET;
}
#define IMAT1_INT_SET                                                         0x00000020UL
void write_imat1_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= IMAT1_INT_SET;
}
#define ILIM1_INT_SET                                                         0x00000010UL
void write_ilim1_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ILIM1_INT_SET;
}
#define ICAP0_INT_SET                                                         0x00000004UL
void write_icap0_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ICAP0_INT_SET;
}
#define IMAT0_INT_SET                                                         0x00000002UL
void write_imat0_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= IMAT0_INT_SET;
}
#define ILIM0_INT_SET                                                         0x00000001UL
void write_ilim0_int_set(void)
{
    cobra_mpwm->mpwm_inten_set |= ILIM0_INT_SET;
}

/*INTEN_CLR*/
#define ABORT_INT_CLR                                                         0x00008000UL
void write_abort_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ABORT_INT_CLR;
}
#define ICAP2_INT_CLR                                                         0x00000400UL
void write_icap2_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ICAP2_INT_CLR;
}
#define IMAT2_INT_CLR                                                         0x00000200UL
void write_imat2_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= IMAT2_INT_CLR;
}
#define ILIM2_INT_CLR                                                         0x00000100UL
void write_ilim2_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ILIM2_INT_CLR;
}
#define ICAP1_INT_CLR                                                         0x00000040UL
void write_icap1_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ICAP1_INT_CLR;
}
#define IMAT1_INT_CLR                                                         0x00000020UL
void write_imat1_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= IMAT1_INT_CLR;
}
#define ILIM1_INT_CLR                                                         0x00000010UL
void write_ilim1_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ILIM1_INT_CLR;
}
#define ICAP0_INT_CLR                                                         0x00000004UL
void write_icap0_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ICAP0_INT_CLR;
}
#define IMAT0_INT_CLR                                                         0x00000002UL
void write_imat0_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= IMAT0_INT_CLR;
}
#define ILIM0_INT_CLR                                                         0x00000001UL
void write_ilim0_int_clr(void)
{
    cobra_mpwm->mpwm_inten_clr |= ILIM0_INT_CLR;
}

/*CNTCON*/
#define CNTR2                                                                 0x80000000UL
bool read_cntr2(void)
{
    return (cobra_mpwm->mpwm_cntcon & CNTR2) == CNTR2;
}
#define CNTR1                                                                 0x40000000UL
bool read_cntr1(void)
{
    return (cobra_mpwm->mpwm_cntcon & CNTR1) == CNTR1;
}
#define CNTR0                                                                 0x20000000UL
bool read_cntr0(void)
{
    return (cobra_mpwm->mpwm_cntcon & CNTR0) == CNTR0;
}
#define TC2MCI2_FE                                                            0x00020000UL
bool read_tc2mci2_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI2_FE) == TC2MCI2_FE;
}
#define TC2MCI2_RE                                                            0x00010000UL
bool read_tc2mci2_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI2_RE) == TC2MCI2_RE;
}
#define TC2MCI1_FE                                                            0x00008000UL
bool read_tc2mci1_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI1_FE) == TC2MCI1_FE;
}
#define TC2MCI1_RE                                                            0x00004000UL
bool read_tc2mci1_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI1_RE) == TC2MCI1_RE;
}
#define TC2MCI0_FE                                                            0x00002000UL
bool read_tc2mci0_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI0_FE) == TC2MCI0_FE;
}
#define TC2MCI0_RE                                                            0x00001000UL
bool read_tc2mci0_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC2MCI0_RE) == TC2MCI0_RE;
}
#define TC1MCI2_FE                                                            0x00000800UL
bool read_tc1mci2_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI2_FE) == TC1MCI2_FE;
}
#define TC1MCI2_RE                                                            0x00000400UL
bool read_tc1mci2_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI2_RE) == TC1MCI2_RE;
}
#define TC1MCI1_FE                                                            0x00000200UL
bool read_tc1mci1_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI1_FE) == TC1MCI1_FE;
}
#define TC1MCI1_RE                                                            0x00000100UL
bool read_tc1mci1_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI1_RE) == TC1MCI1_RE;
}
#define TC1MCI0_FE                                                            0x00000080UL
bool read_tc1mci0_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI0_FE) == TC1MCI0_FE;
}
#define TC1MCI0_RE                                                            0x00000040UL
bool read_tc1mci0_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC1MCI0_RE) == TC1MCI0_RE;
}
#define TC0MCI2_FE                                                            0x00000020UL
bool read_tc0mci2_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI2_FE) == TC0MCI2_FE;
}
#define TC0MCI2_RE                                                            0x00000010UL
bool read_tc0mci2_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI2_RE) == TC0MCI2_RE;
}
#define TC0MCI1_FE                                                            0x00000008UL
bool read_tc0mci1_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI1_FE) == TC0MCI1_FE;
}
#define TC0MCI1_RE                                                            0x00000004UL
bool read_tc0mci1_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI1_RE) == TC0MCI1_RE;
}
#define TC0MCI0_FE                                                            0x00000002UL
bool read_tc0mci0_fe(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI0_FE) == TC0MCI0_FE;
}
#define TC0MCI0_RE                                                            0x00000001UL
bool read_tc0mci0_re(void)
{
    return (cobra_mpwm->mpwm_cntcon & TC0MCI0_RE) == TC0MCI0_RE;
}

/*CNTCON_SET*/
#define CNTR2_SET                                                             0x80000000UL
void write_cntr2_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= CNTR2_SET;
}
#define CNTR1_SET                                                             0x40000000UL
void write_cntr1_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= CNTR1_SET;
}
#define CNTR0_SET                                                             0x20000000UL
void write_cntr0_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= CNTR0_SET;
}
#define TC2MCI2_FE_SET                                                        0x00020000UL
void write_tc2mci2_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI2_FE_SET;
}
#define TC2MCI2_RE_SET                                                        0x00010000UL
void write_tc2mci2_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI2_RE_SET;
}
#define TC2MCI1_FE_SET                                                        0x00008000UL
void write_tc2mci1_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI1_FE_SET;
}
#define TC2MCI1_RE_SET                                                        0x00004000UL
void write_tc2mci1_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI1_RE_SET;
}
#define TC2MCI0_FE_SET                                                        0x00002000UL
void write_tc2mci0_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI0_FE_SET;
}
#define TC2MCI0_RE_SET                                                        0x00001000UL
void write_tc2mci0_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC2MCI0_RE_SET;
}
#define TC1MCI2_FE_SET                                                        0x00000800UL
void write_tc1mci2_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI2_FE_SET;
}
#define TC1MCI2_RE_SET                                                        0x00000400UL
void write_tc1mci2_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI2_RE_SET;
}
#define TC1MCI1_FE_SET                                                        0x00000200UL
void write_tc1mci1_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI1_FE_SET;
}
#define TC1MCI1_RE_SET                                                        0x00000100UL
void write_tc1mci1_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI1_RE_SET;
}
#define TC1MCI0_FE_SET                                                        0x00000080UL
void write_tc1mci0_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI0_FE_SET;
}
#define TC1MCI0_RE_SET                                                        0x00000040UL
void write_tc1mci0_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC1MCI0_RE_SET;
}
#define TC0MCI2_FE_SET                                                        0x00000020UL
void write_tc0mci2_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI2_FE_SET;
}
#define TC0MCI2_RE_SET                                                        0x00000010UL
void write_tc0mci2_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI2_RE_SET;
}
#define TC0MCI1_FE_SET                                                        0x00000008UL
void write_tc0mci1_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI1_FE_SET;
}
#define TC0MCI1_RE_SET                                                        0x00000004UL
void write_tc0mci1_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI1_RE_SET;
}
#define TC0MCI0_FE_SET                                                        0x00000002UL
void write_tc0mci0_fe_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI0_FE_SET;
}
#define TC0MCI0_RE_SET                                                        0x00000001UL
void write_tc0mci0_re_set(void)
{
    cobra_mpwm->mpwm_cntcon_set |= TC0MCI0_RE_SET;
}

/*CNTCON_CLR*/
#define CNTR2_CLR                                                             0x80000000UL
void write_cntr2_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= CNTR2_CLR;
}
#define CNTR1_CLR                                                             0x40000000UL
void write_cntr1_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= CNTR1_CLR;
}
#define CNTR0_CLR                                                             0x20000000UL
void write_cntr0_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= CNTR0_CLR;
}
#define TC2MCI2_FE_CLR                                                        0x00020000UL
void write_tc2mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI2_FE_CLR;
}
#define TC2MCI2_RE_CLR                                                        0x00010000UL
void write_tc2mci2_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI2_RE_CLR;
}
#define TC2MCI1_FE_CLR                                                        0x00008000UL
void write_tc2mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI1_FE_CLR;
}
#define TC2MCI1_RE_CLR                                                        0x00004000UL
void write_tc2mci1_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI1_RE_CLR;
}
#define TC2MCI0_FE_CLR                                                        0x00002000UL
void write_tc2mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI0_FE_CLR;
}
#define TC2MCI0_RE_CLR                                                        0x00001000UL
void write_tc2mci0_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC2MCI0_RE_CLR;
}
#define TC1MCI2_FE_CLR                                                        0x00000800UL
void write_tc1mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI2_FE_CLR;
}
#define TC1MCI2_RE_CLR                                                        0x00000400UL
void write_tc1mci2_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI2_RE_CLR;
}
#define TC1MCI1_FE_CLR                                                        0x00000200UL
void write_tc1mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI1_FE_CLR;
}
#define TC1MCI1_RE_CLR                                                        0x00000100UL
void write_tc1mci1_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI1_RE_CLR;
}
#define TC1MCI0_FE_CLR                                                        0x00000080UL
void write_tc1mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI0_FE_CLR;
}
#define TC1MCI0_RE_CLR                                                        0x00000040UL
void write_tc1mci0_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC1MCI0_RE_CLR;
}
#define TC0MCI2_FE_CLR                                                        0x00000020UL
void write_tc0mci2_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI2_FE_CLR;
}
#define TC0MCI2_RE_CLR                                                        0x00000010UL
void write_tc0mci2_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI2_RE_CLR;
}
#define TC0MCI1_FE_CLR                                                        0x00000008UL
void write_tc0mci1_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI1_FE_CLR;
}
#define TC0MCI1_RE_CLR                                                        0x00000004UL
void write_tc0mci1_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI1_RE_CLR;
}
#define TC0MCI0_FE_CLR                                                        0x00000002UL
void write_tc0mci0_fe_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI0_FE_CLR;
}
#define TC0MCI0_RE_CLR                                                        0x00000001UL
void write_tc0mci0_re_clr(void)
{
    cobra_mpwm->mpwm_cntcon_clr |= TC0MCI0_RE_CLR;
}

/*INTF*/
#define ABORT_INTF                                                            0x00008000UL
bool read_abort_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ABORT_INTF) == ABORT_INTF;
}
#define ICAP2_INTF                                                            0x00000400UL
bool read_icap2_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ICAP2_INTF) == ICAP2_INTF;
}
#define IMAT2_INTF                                                            0x00000200UL
bool read_imat2_intf(void)
{
    return (cobra_mpwm->mpwm_intf & IMAT2_INTF) == IMAT2_INTF;
}
#define ILIM2_INTF                                                            0x00000100UL
bool read_ilim2_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ILIM2_INTF) == ILIM2_INTF;
}
#define ICAP1_INTF                                                            0x00000040UL
bool read_icap1_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ICAP1_INTF) == ICAP1_INTF;
}
#define IMAT1_INTF                                                            0x00000020UL
bool read_imat1_intf(void)
{
    return (cobra_mpwm->mpwm_intf & IMAT1_INTF) == IMAT1_INTF;
}
#define ILIM1_INTF                                                            0x00000010UL
bool read_ilim1_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ILIM1_INTF) == ILIM1_INTF;
}
#define ICAP0_INTF                                                            0x00000004UL
bool read_icap0_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ICAP0_INTF) == ICAP0_INTF;
}
#define IMAT0_INTF                                                            0x00000002UL
bool read_imat0_intf(void)
{
    return (cobra_mpwm->mpwm_intf & IMAT0_INTF) == IMAT0_INTF;
}
#define ILIM0_INTF                                                            0x00000001UL
bool read_ilim0_intf(void)
{
    return (cobra_mpwm->mpwm_intf & ILIM0_INTF) == ILIM0_INTF;
}

/*INTF_SET*/
#define ABORT_INTF_SET                                                        0x00008000UL
void write_abort_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ABORT_INTF_SET;
}
#define ICAP2_INTF_SET                                                        0x00000400UL
void write_icap2_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ICAP2_INTF_SET;
}
#define IMAT2_INTF_SET                                                        0x00000200UL
void write_imat2_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= IMAT2_INTF_SET;
}
#define ILIM2_INTF_SET                                                        0x00000100UL
void write_ilim2_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ILIM2_INTF_SET;
}
#define ICAP1_INTF_SET                                                        0x00000040UL
void write_icap1_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ICAP1_INTF_SET;
}
#define IMAT1_INTF_SET                                                        0x00000020UL
void write_imat1_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= IMAT1_INTF_SET;
}
#define ILIM1_INTF_SET                                                        0x00000010UL
void write_ilim1_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ILIM1_INTF_SET;
}
#define ICAP0_INTF_SET                                                        0x00000004UL
void write_icap0_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ICAP0_INTF_SET;
}
#define IMAT0_INTF_SET                                                        0x00000002UL
void write_imat0_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= IMAT0_INTF_SET;
}
#define ILIM0_INTF_SET                                                        0x00000001UL
void write_ilim0_intf_set(void)
{
    cobra_mpwm->mpwm_intf_set |= ILIM0_INTF_SET;
}

/*INTF_CLR*/
#define ABORT_INTF_CLR                                                        0x00008000UL
void write_abort_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ABORT_INTF_CLR;
}
#define ICAP2_INTF_CLR                                                        0x00000400UL
void write_icap2_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ICAP2_INTF_CLR;
}
#define IMAT2_INTF_CLR                                                        0x00000200UL
void write_imat2_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= IMAT2_INTF_CLR;
}
#define ILIM2_INTF_CLR                                                        0x00000100UL
void write_ilim2_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ILIM2_INTF_CLR;
}
#define ICAP1_INTF_CLR                                                        0x00000040UL
void write_icap1_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ICAP1_INTF_CLR;
}
#define IMAT1_INTF_CLR                                                        0x00000020UL
void write_imat1_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= IMAT1_INTF_CLR;
}
#define ILIM1_INTF_CLR                                                        0x00000010UL
void write_ilim1_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ILIM1_INTF_CLR;
}
#define ICAP0_INTF_CLR                                                        0x00000004UL
void write_icap0_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ICAP0_INTF_CLR;
}
#define IMAT0_INTF_CLR                                                        0x00000002UL
void write_imat0_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= IMAT0_INTF_CLR;
}
#define ILIM0_INTF_CLR                                                        0x00000001UL
void write_ilim0_intf_clr(void)
{
    cobra_mpwm->mpwm_intf_clr |= ILIM0_INTF_CLR;
}

/*CAP_CLR*/
#define CAP_CLR2                                                              0x00000004UL
void write_cap_clr2(void)
{
    cobra_mpwm->mpwm_cap_clr |= CAP_CLR2;
}
#define CAP_CLR1                                                              0x00000002UL
void write_cap_clr1(void)
{
    cobra_mpwm->mpwm_cap_clr |= CAP_CLR1;
}
#define CAP_CLR0                                                              0x00000001UL
void write_cap_clr0(void)
{
    cobra_mpwm->mpwm_cap_clr |= CAP_CLR0;
}

void mpwm_en(uint8_t channel_en)
{
    uint32_t en = cobra_mpwm->mpwm_con;
    if((channel_en & MPWM_CHANNEL0_EN) == MPWM_CHANNEL0_EN)
    {
        en |= RUN0_SET;
    }
    if((channel_en & MPWM_CHANNEL1_EN) == MPWM_CHANNEL1_EN)
    {
        en |= RUN1_SET;
    }
    if((channel_en & MPWM_CHANNEL2_EN) == MPWM_CHANNEL2_EN)
    {
        en |= RUN2_SET;
    }
 
    cobra_mpwm->mpwm_con_set = en;
}

void mpwm_close(uint8_t channel_en)
{
    uint32_t en = cobra_mpwm->mpwm_con;
    if((channel_en & MPWM_CHANNEL0_EN) == MPWM_CHANNEL0_EN)
    {
        en |= RUN0_CLR;
    }
    if((channel_en & MPWM_CHANNEL1_EN) == MPWM_CHANNEL1_EN)
    {
        en |= RUN1_CLR;
    }
    if((channel_en & MPWM_CHANNEL2_EN) == MPWM_CHANNEL2_EN)
    {
        en |= RUN2_CLR;
    }
 
    cobra_mpwm->mpwm_con_clr = en;
}

#endif

/*********************************************************************
 * @brief		  Initializes the MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_Init(void)
{
	
	 cobra_mpwm->mpwm_cap_clr = MCPWM_CAPCLR_CAP(0) | MCPWM_CAPCLR_CAP(1) | MCPWM_CAPCLR_CAP(2);

	 cobra_mpwm->mpwm_intf_clr = MCPWM_INT_ILIM(0) | MCPWM_INT_ILIM(1) | MCPWM_INT_ILIM(2) \
								| MCPWM_INT_IMAT(0) | MCPWM_INT_IMAT(1) | MCPWM_INT_IMAT(2) \
								| MCPWM_INT_ICAP(0) | MCPWM_INT_ICAP(1) | MCPWM_INT_ICAP(2);

	  cobra_mpwm->mpwm_inten_clr = MCPWM_INT_ILIM(0) | MCPWM_INT_ILIM(1) | MCPWM_INT_ILIM(2) \
								| MCPWM_INT_IMAT(0) | MCPWM_INT_IMAT(1) | MCPWM_INT_IMAT(2) \
								| MCPWM_INT_ICAP(0) | MCPWM_INT_ICAP(1) | MCPWM_INT_ICAP(2);
}

/********************************************************************* 
 * @brief		  Configures each channel in MCPWM peripheral according to the
 * 				    specified parameters in the MCPWM_CHANNEL_CFG_Type.
 * @param[in]	channel_number	Channel number, should be: 0..2.
 * @param[in]	channelSetup Pointer to a MCPWM_CHANNEL_CFG_Type structure
 * 				    that contains the configuration information for the specified MCPWM channel.
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_config_channel(uint32_t channel_number, MCPWM_CHANNEL_CFG_Type * channelSetup)
{
	if (channel_number <= 2)
	{
		if (channel_number == MCPWM_CHANNEL_0)
		{
			  cobra_mpwm->mpwm_tc[0] = channelSetup->channelTimercounterValue;
			  cobra_mpwm->mpwm_lim[0] = channelSetup->channelPeriodValue;
			  cobra_mpwm->mpwm_mat[0] = channelSetup->channelPulsewidthValue;
		}
		else if (channel_number == MCPWM_CHANNEL_1)
		{
			  cobra_mpwm->mpwm_tc[1] = channelSetup->channelTimercounterValue;
			  cobra_mpwm->mpwm_lim[1] = channelSetup->channelPeriodValue;
			  cobra_mpwm->mpwm_mat[1] = channelSetup->channelPulsewidthValue;
		}
		else if (channel_number == MCPWM_CHANNEL_2)
		{
			  cobra_mpwm->mpwm_tc[2] = channelSetup->channelTimercounterValue;
			  cobra_mpwm->mpwm_lim[2] = channelSetup->channelPeriodValue;
			  cobra_mpwm->mpwm_mat[2] = channelSetup->channelPulsewidthValue;
		}
		else
		{
			return;
		}

		if (channelSetup->channelType == MCPWM_CHANNEL_CENTER_MODE)
		{
			  cobra_mpwm->mpwm_con_set = MCPWM_CON_CENTER(channel_number);
		}
		else
		{
			  cobra_mpwm->mpwm_con_clr = MCPWM_CON_CENTER(channel_number);
		}

		if (channelSetup->channelPolarity == MCPWM_CHANNEL_PASSIVE_HI)
		{
			  cobra_mpwm->mpwm_con_set = MCPWM_CON_POLAR(channel_number);
		}
		else
		{
			 cobra_mpwm->mpwm_con_clr = MCPWM_CON_POLAR(channel_number);
		}

		if (channelSetup->channelDeadtimeEnable == ENABLE)
		{
			 cobra_mpwm->mpwm_con_set = MCPWM_CON_DTE(channel_number);
			 cobra_mpwm->mpwm_dt &= ~(MCPWM_DT(channel_number, 0x3FF));
			 cobra_mpwm->mpwm_dt |= MCPWM_DT(channel_number, channelSetup->channelDeadtimeValue);
		}
		else
		{
			 cobra_mpwm->mpwm_con_clr = MCPWM_CON_DTE(channel_number);
		}

		if (channelSetup->channelUpdateEnable == ENABLE)
		{
			 cobra_mpwm->mpwm_con_clr = MCPWM_CON_DISUP(channel_number);
		}
		else
		{
			 cobra_mpwm->mpwm_con_set = MCPWM_CON_DISUP(channel_number);
		}
	}
}

/*********************************************************************
 * @brief		Write to MCPWM shadow registers - Update the value for period
 * 				  and pulse width in MCPWM peripheral.
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channel_number	Channel Number, should be: 0..2.
 * @param[in]	channelSetup Pointer to a MCPWM_CHANNEL_CFG_Type structure
 * 				    that contains the configuration information for the specified
 * 				    MCPWM channel.
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_write_to_shadow(uint32_t channelNum, MCPWM_CHANNEL_CFG_Type *channelSetup)
{
	if (channelNum == MCPWM_CHANNEL_0)
	{
		cobra_mpwm->mpwm_lim[0] = channelSetup->channelPeriodValue;
		cobra_mpwm->mpwm_mat[0] = channelSetup->channelPulsewidthValue;
	}
	else if (channelNum == MCPWM_CHANNEL_1)
	{
		cobra_mpwm->mpwm_lim[1] = channelSetup->channelPeriodValue;
		cobra_mpwm->mpwm_mat[1] = channelSetup->channelPulsewidthValue;
	}
	else if (channelNum == MCPWM_CHANNEL_2)
	{
		cobra_mpwm->mpwm_lim[2] = channelSetup->channelPeriodValue;
		cobra_mpwm->mpwm_mat[2] = channelSetup->channelPulsewidthValue;
	}
}

/*********************************************************************//**
 * @brief		Configures capture function in MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channelNum	MCI (Motor Control Input pin) number, should be: 0..2
 * @param[in]	captureConfig	Pointer to a MCPWM_CAPTURE_CFG_Type structure
 * 				that contains the configuration information for the
 * 				specified MCPWM capture.
 * @return
 **********************************************************************/
void app_cobra_mcpwm_config_capture( uint32_t channelNum, MCPWM_CAPTURE_CFG_Type *captureConfig)
{
	if ((channelNum <= MCPWM_CHANNEL_2))
	{

		if (captureConfig->captureFalling == ENABLE)
		{
			cobra_mpwm->mpwm_capcon_set = MCPWM_CAPCON_CAPMCI_FE(captureConfig->captureChannel, channelNum);
		}
		else
		{
			cobra_mpwm->mpwm_capcon_clr = MCPWM_CAPCON_CAPMCI_FE(captureConfig->captureChannel, channelNum);
		}

		if (captureConfig->captureRising == ENABLE)
		{
			cobra_mpwm->mpwm_capcon_set = MCPWM_CAPCON_CAPMCI_RE(captureConfig->captureChannel, channelNum);
		}
		else
		{
			cobra_mpwm->mpwm_capcon_clr = MCPWM_CAPCON_CAPMCI_RE(captureConfig->captureChannel, channelNum);
		}

		if (captureConfig->timerReset == ENABLE)
		{
			cobra_mpwm->mpwm_capcon_set = MCPWM_CAPCON_RT(captureConfig->captureChannel);
		}
		else
		{
			cobra_mpwm->mpwm_capcon_clr = MCPWM_CAPCON_RT(captureConfig->captureChannel);
		}

		if (captureConfig->hnfEnable == ENABLE)
		{
			cobra_mpwm->mpwm_capcon_set = MCPWM_CAPCON_HNFCAP(channelNum);
		}
		else
		{
			cobra_mpwm->mpwm_capcon_clr = MCPWM_CAPCON_HNFCAP(channelNum);
		}
	}
}

/*********************************************************************
 * @brief		Clears current captured value in specified capture channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	captureChannel	Capture channel number, should be: 0..2
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_clear_capture(uint32_t captureChannel)
{
	  cobra_mpwm->mpwm_cap_clr = MCPWM_CAPCLR_CAP(captureChannel);
}

/*********************************************************************//**
 * @brief		Get current captured value in specified capture channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	captureChannel	Capture channel number, should be: 0..2
 * @return		None
 **********************************************************************/
uint32_t app_cobra_mcpwm_get_capture(uint32_t captureChannel)
{
	  if (captureChannel == MCPWM_CHANNEL_0)
	  {
		   return (cobra_mpwm->mpwm_cap[0]);
	  }
	  else if (captureChannel == MCPWM_CHANNEL_1)
	  {
		   return (cobra_mpwm->mpwm_cap[1]);
	  }
	  else if (captureChannel == MCPWM_CHANNEL_2)
	  {
		   return (cobra_mpwm->mpwm_cap[2]);
	  }
	  return (0);
}

/*********************************************************************
 * @brief		Configures Count control in MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channelNum	Channel number, should be: 0..2
 * @param[in]	countMode	Count mode, should be:
 * 					- ENABLE: Enables count mode.
 * 					- DISABLE: Disable count mode, the channel is in timer mode.
 * @param[in]	countConfig	Pointer to a MCPWM_COUNT_CFG_Type structure
 * 				that contains the configuration information for the
 * 				specified MCPWM count control.
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_count_config(uint32_t channelNum, uint32_t countMode, MCPWM_COUNT_CFG_Type *countConfig)
{
	if ((channelNum <= 2))
	{
		if (countMode == ENABLE)
		{
			  cobra_mpwm->mpwm_cntcon_set = MCPWM_CNTCON_CNTR(channelNum);

			if (countConfig->countFalling == ENABLE)
			{
				  cobra_mpwm->mpwm_cntcon_set = MCPWM_CNTCON_TCMCI_FE(countConfig->counterChannel,channelNum);
			}
			else
			{
				  cobra_mpwm->mpwm_cntcon_clr = MCPWM_CNTCON_TCMCI_FE(countConfig->counterChannel,channelNum);
			}

			if (countConfig->countRising == ENABLE)
			{
				  cobra_mpwm->mpwm_cntcon_set = MCPWM_CNTCON_TCMCI_RE(countConfig->counterChannel,channelNum);
			}
			else
			{
				  cobra_mpwm->mpwm_cntcon_clr = MCPWM_CNTCON_TCMCI_RE(countConfig->counterChannel,channelNum);
			}
		}
		else
		{
			   cobra_mpwm->mpwm_cntcon_clr = MCPWM_CNTCON_CNTR(channelNum);
		}
	}
}

/*********************************************************************
 * @brief		Start MCPWM activity for each MCPWM channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channel0 State of this command on channel 0:
 * 					- ENABLE: 'Start' command will effect on channel 0
 * 					- DISABLE: 'Start' command will not effect on channel 0
 * @param[in]	channel1 State of this command on channel 1:
 * 					- ENABLE: 'Start' command will effect on channel 1
 * 					- DISABLE: 'Start' command will not effect on channel 1
 * @param[in]	channel2 State of this command on channel 2:
 * 					- ENABLE: 'Start' command will effect on channel 2
 * 					- DISABLE: 'Start' command will not effect on channel 2
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_start(uint32_t channel0, uint32_t channel1, uint32_t channel2)
{
	  uint32_t regVal = 0;

	  regVal = (channel0 ? MCPWM_CON_RUN(0) : 0) | (channel1 ? MCPWM_CON_RUN(1) : 0) | (channel2 ? MCPWM_CON_RUN(2) : 0);

	  cobra_mpwm->mpwm_con_set = regVal;
}

/*********************************************************************
 * @brief		Stop MCPWM activity for each MCPWM channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channel0 State of this command on channel 0:
 * 					- ENABLE: 'Stop' command will effect on channel 0
 * 					- DISABLE: 'Stop' command will not effect on channel 0
 * @param[in]	channel1 State of this command on channel 1:
 * 					- ENABLE: 'Stop' command will effect on channel 1
 * 					- DISABLE: 'Stop' command will not effect on channel 1
 * @param[in]	channel2 State of this command on channel 2:
 * 					- ENABLE: 'Stop' command will effect on channel 2
 * 					- DISABLE: 'Stop' command will not effect on channel 2
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_stop(uint32_t channel0, uint32_t channel1, uint32_t channel2)
{
	  uint32_t regVal = 0;

	  regVal = (channel0 ? MCPWM_CON_RUN(0) : 0) | (channel1 ? MCPWM_CON_RUN(1) : 0) | (channel2 ? MCPWM_CON_RUN(2) : 0);

	  cobra_mpwm->mpwm_con_clr = regVal;
}

/*********************************************************************
 * @brief		Enables/Disables 3-phase AC motor mode on MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	acMode	State of this command, should be:
 * 					- ENABLE.
 * 					- DISABLE.
 * @return		None
 **********************************************************************/
void app_cobra_mcpwm_ac_mode(uint32_t acMode)
{
	  if (acMode)
	  {
		   cobra_mpwm->mpwm_con_set = MCPWM_CON_ACMODE;
	  }
	  else
	  {
		   cobra_mpwm->mpwm_con_clr = MCPWM_CON_ACMODE;
	  }
}

/*********************************************************************
 * @brief		Enables/Disables 3-phase DC motor mode on MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	dcMode	State of this command, should be:
 * 					- ENABLE.
 * 					- DISABLE.
 * @param[in]	outputInvered	Polarity of the MCOB outputs for all 3 channels,
 * 				should be:
 * 					- ENABLE	:The MCOB outputs have opposite polarity from the MCOA outputs.
 * 					- DISABLE	:The MCOB outputs have the same basic polarity as the MCOA outputs.
 * @param[in]	outputPattern	A value contains bits that enables/disables the specified
 * 				output pins route to the internal MCOA0 signal, should be:
 * 					- MCPWM_PATENT_A0	:MCOA0 tracks internal MCOA0
 * 					- MCPWM_PATENT_B0	:MCOB0 tracks internal MCOA0
 * 					- MCPWM_PATENT_A1	:MCOA1 tracks internal MCOA0
 * 					- MCPWM_PATENT_B1	:MCOB1 tracks internal MCOA0
 * 					- MCPWM_PATENT_A2	:MCOA2 tracks internal MCOA0
 * 					- MCPWM_PATENT_B2	:MCOB2 tracks internal MCOA0
 * @return		None
 *
 * Note: all these outputPatent values above can be ORed together for using as input parameter.
 **********************************************************************/
void app_cobra_mcpwm_dc_mode(uint32_t dcMode, uint32_t outputInvered, uint32_t outputPattern)
{
	   if (dcMode)
	   {
		     cobra_mpwm->mpwm_con_set = MCPWM_CON_DCMODE;
	   }
	   else
	   {
		     cobra_mpwm->mpwm_con_clr = MCPWM_CON_DCMODE;
	   }

	   if (outputInvered)
	   {
		     cobra_mpwm->mpwm_con_set = MCPWM_CON_INVBDC;
	   }
	   else
	   {
		     cobra_mpwm->mpwm_con_clr = MCPWM_CON_INVBDC;
	   }

	   cobra_mpwm->mpwm_ccp = outputPattern;
}

/*********************************************************************
 * @brief		Configures the specified interrupt in MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @param[in]	NewState	New State of this command, should be:
 * 					- ENABLE.
 * 					- DISABLE.
 * @return		None
 *
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void app_cobra_mcpwm_int_config(uint32_t ulIntType, FunctionalState NewState)
{
	if (NewState)
	{
		cobra_mpwm->mpwm_inten_set = ulIntType;
	}
	else
	{
		cobra_mpwm->mpwm_inten_clr = ulIntType;
	}
}

/*********************************************************************
 * @brief		Sets/Forces the specified interrupt for MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void app_cobra_mcpwm_int_set(uint32_t ulIntType)
{
	   cobra_mpwm->mpwm_intf_set = ulIntType;
}

/*********************************************************************
 * @brief		Clear the specified interrupt pending for MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void app_cobra_mcpwm_int_clear(uint32_t ulIntType)
{
	   cobra_mpwm->mpwm_intf_clr = ulIntType;
}


/*********************************************************************//**
 * @brief		Check whether if the specified interrupt in MCPWM is set or not
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 **********************************************************************/
FlagStatus app_cobra_mcpwm_get_int_status(uint32_t ulIntType)
{
	return ((cobra_mpwm->mpwm_intf & ulIntType) ? SET : RESET);
}

void app_cobra_mcpwm_set_pattern(unsigned int pattern)
{
		cobra_mpwm->mpwm_ccp = pattern;
}














#endif
