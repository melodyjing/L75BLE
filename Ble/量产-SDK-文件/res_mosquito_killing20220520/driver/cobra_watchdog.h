/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_watchdog.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/20

	Description :   
	
	Changed Log	:

		Liujian  2019/5/20		- Creation



*****************************************************************************/


#ifndef __COBRA_WATCHDOG_H__
#define __COBRA_WATCHDOG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_WATCHDOG_EN
typedef struct cobra_watchdog_type
{
		__attribute__ ((aligned (4)))    uint32_t  watchdog_LOAD;          // <h> Watchdog Load Register </h>
		__attribute__ ((aligned (4)))    uint32_t  watchdog_VALUE;         // <h> Watchdog Value Register </h>
		__attribute__ ((aligned (4)))    uint32_t  watchdog_CTRL;          // <h> Watchdog Control Register
																															 //   <o.1>    RESEN: Reset enable
																															 //   <o.0>    INTEN: Interrupt enable
																															 // </h>
		__attribute__ ((aligned (4)))    uint32_t  INTCLR;        // <h> Watchdog Clear Interrupt Register </h>
		__attribute__ ((aligned (4)))    uint32_t  RAWINTSTAT;    // <h> Watchdog Raw Interrupt Status Register </h>
		__attribute__ ((aligned (4)))    uint32_t  MASKINTSTAT;   // <h> Watchdog Interrupt Status Register </h>
		__attribute__ ((aligned (4)))    uint32_t  RESERVED0[762];
		__attribute__ ((aligned (4)))    uint32_t  LOCK;          // <h> Watchdog Lock Register </h>
		__attribute__ ((aligned (4)))    uint32_t  RESERVED1[191];
		__attribute__ ((aligned (4)))    uint32_t  ITCR;          // <h> Watchdog Integration Test Control Register </h>
		__attribute__ ((aligned (4)))    uint32_t  ITOP;          // <h> Watchdog Integration Test Output Set Register </h>
		

} COBRA_WATCHDOG_TypeDef; 

typedef enum{
		WATCHDOG_NO_ACTION,
		WATCHDOG_NMI,
		WATCHDOG_RESET,	
}WATCHDOG_MODE;


#define  COBRA_WATCHDOG_BASE		(0x40088000UL)


void cobra_watchdog_init(void);

void app_cobra_watchdog_init(unsigned int ms, WATCHDOG_MODE mode);

void cobra_watchdog_irq_clear(void);

void cobra_watchdog_set(unsigned int ms);

void cobra_watchdog_disable(void);

void cobra_watchdog_unlock(void);

void cobra_watchdog_lock(void);
#endif   //COBRA_WATCHDOG_EN


#endif

