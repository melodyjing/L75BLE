/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_dualtimer_aon.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/7/14

	Description :   
	  Always ÉÏÃæµÄ dualtime
	Changed Log	:

		Liujian  2021/7/14		- Creation



*****************************************************************************/

#ifndef __COBRA_DUALTIMER_AON_H__
#define __COBRA_DUALTIMER_AON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if (COBRA_DUALTIMER_AON_EN == 1)

typedef struct cobra_aon_dualtimer_both_type
{
  __attribute__ ((aligned (4))) uint32_t Timer1Load;   // <h> Timer 1 Load </h>
  __attribute__ ((aligned (4))) uint32_t Timer1Value;  // <h> Timer 1 Counter Current Value <r></h>
  __attribute__ ((aligned (4))) uint32_t Timer1Control;// <h> Timer 1 Control
                              //   <o.7> TimerEn: Timer Enable
                              //   <o.6> TimerMode: Timer Mode
                              //     <0=> Freerunning-mode
                              //     <1=> Periodic mode
                              //   <o.5> IntEnable: Interrupt Enable
                              //   <o.2..3> TimerPre: Timer Prescale
                              //     <0=> / 1
                              //     <1=> / 16

                              //     <2=> / 256
                              //     <3=> Undefined!
                              //   <o.1> TimerSize: Timer Size
                              //     <0=> 16-bit counter
                              //     <1=> 32-bit counter
                              //   <o.0> OneShot: One-shoot mode
                              //     <0=> Wrapping mode
                              //     <1=> One-shot mode
                              // </h>
  __attribute__ ((aligned (4)))  uint32_t Timer1IntClr; // <h> Timer 1 Interrupt Clear <w></h>
  __attribute__ ((aligned (4)))  uint32_t Timer1RIS;    // <h> Timer 1 Raw Interrupt Status <r></h>
  __attribute__ ((aligned (4)))  uint32_t Timer1MIS;    // <h> Timer 1 Masked Interrupt Status <r></h>
  __attribute__ ((aligned (4)))  uint32_t Timer1BGLoad; // <h> Background Load Register </h>
  __attribute__ ((aligned (4)))  uint32_t RESERVED0;
  __attribute__ ((aligned (4)))  uint32_t Timer2Load;   // <h> Timer 2 Load </h>
  __attribute__ ((aligned (4)))  uint32_t Timer2Value;  // <h> Timer 2 Counter Current Value <r></h>
  __attribute__ ((aligned (4)))  uint32_t Timer2Control;// <h> Timer 2 Control
                              //   <o.7> TimerEn: Timer Enable
                              //   <o.6> TimerMode: Timer Mode
                              //     <0=> Freerunning-mode
                              //     <1=> Periodic mode
                              //   <o.5> IntEnable: Interrupt Enable
                              //   <o.2..3> TimerPre: Timer Prescale
                              //     <0=> / 1
                              //     <1=> / 16
                              //     <2=> / 256
                              //     <3=> Undefined!
                              //   <o.1> TimerSize: Timer Size
                              //     <0=> 16-bit counter
                              //     <1=> 32-bit counter
                              //   <o.0> OneShot: One-shoot mode
                              //     <0=> Wrapping mode
                              //     <1=> One-shot mode
                              // </h>
  __attribute__ ((aligned (4)))  uint32_t Timer2IntClr; // <h> Timer 2 Interrupt Clear <w></h>
  __attribute__ ((aligned (4)))  uint32_t Timer2RIS;    // <h> Timer 2 Raw Interrupt Status <r></h>
  __attribute__ ((aligned (4)))  uint32_t Timer2MIS;    // <h> Timer 2 Masked Interrupt Status <r></h>
  __attribute__ ((aligned (4)))  uint32_t Timer2BGLoad; // <h> Background Load Register </h>
  __attribute__ ((aligned (4)))  uint32_t RESERVED1[945];
  __attribute__ ((aligned (4)))  uint32_t ITCR;         // <h> Integration Test Control Register </h>
  __attribute__ ((aligned (4)))  uint32_t ITOP;         // <h> Integration Test Output Set Register </h>

  __attribute__ ((aligned (4))) uint32_t RESERVED2[63];
	
} COBRA_AON_DUALTIMER_BOTH_TypeDef;

#define CMSDK_DUALTIMER1_LOAD_Pos            0                                               /*!< CMSDK_DUALTIMER1 LOAD: LOAD Position */
#define CMSDK_DUALTIMER1_LOAD_Msk            (0xFFFFFFFFul << CMSDK_DUALTIMER1_LOAD_Pos)     /*!< CMSDK_DUALTIMER1 LOAD: LOAD Mask */

#define CMSDK_DUALTIMER1_VALUE_Pos           0                                               /*!< CMSDK_DUALTIMER1 VALUE: VALUE Position */
#define CMSDK_DUALTIMER1_VALUE_Msk           (0xFFFFFFFFul << CMSDK_DUALTIMER1_VALUE_Pos)    /*!< CMSDK_DUALTIMER1 VALUE: VALUE Mask */

#define CMSDK_DUALTIMER1_CTRL_EN_Pos         7                                               /*!< CMSDK_DUALTIMER1 CTRL_EN: CTRL Enable Position */
#define CMSDK_DUALTIMER1_CTRL_EN_Msk         (0x1ul << CMSDK_DUALTIMER1_CTRL_EN_Pos)         /*!< CMSDK_DUALTIMER1 CTRL_EN: CTRL Enable Mask */

#define CMSDK_DUALTIMER1_CTRL_MODE_Pos       6                                               /*!< CMSDK_DUALTIMER1 CTRL_MODE: CTRL MODE Position */
#define CMSDK_DUALTIMER1_CTRL_MODE_Msk       (0x1ul << CMSDK_DUALTIMER1_CTRL_MODE_Pos)       /*!< CMSDK_DUALTIMER1 CTRL_MODE: CTRL MODE Mask */

#define CMSDK_DUALTIMER1_CTRL_INTEN_Pos      5                                               /*!< CMSDK_DUALTIMER1 CTRL_INTEN: CTRL Int Enable Position */
#define CMSDK_DUALTIMER1_CTRL_INTEN_Msk      (0x1ul << CMSDK_DUALTIMER1_CTRL_INTEN_Pos)      /*!< CMSDK_DUALTIMER1 CTRL_INTEN: CTRL Int Enable Mask */

#define CMSDK_DUALTIMER1_CTRL_PRESCALE_Pos   2                                               /*!< CMSDK_DUALTIMER1 CTRL_PRESCALE: CTRL PRESCALE Position */
#define CMSDK_DUALTIMER1_CTRL_PRESCALE_Msk   (0x3ul << CMSDK_DUALTIMER1_CTRL_PRESCALE_Pos)   /*!< CMSDK_DUALTIMER1 CTRL_PRESCALE: CTRL PRESCALE Mask */

#define CMSDK_DUALTIMER1_CTRL_SIZE_Pos       1                                               /*!< CMSDK_DUALTIMER1 CTRL_SIZE: CTRL SIZE Position */
#define CMSDK_DUALTIMER1_CTRL_SIZE_Msk       (0x1ul << CMSDK_DUALTIMER1_CTRL_SIZE_Pos)       /*!< CMSDK_DUALTIMER1 CTRL_SIZE: CTRL SIZE Mask */

#define CMSDK_DUALTIMER1_CTRL_ONESHOOT_Pos   0                                               /*!< CMSDK_DUALTIMER1 CTRL_ONESHOOT: CTRL ONESHOOT Position */
#define CMSDK_DUALTIMER1_CTRL_ONESHOOT_Msk   (0x1ul << CMSDK_DUALTIMER1_CTRL_ONESHOOT_Pos)   /*!< CMSDK_DUALTIMER1 CTRL_ONESHOOT: CTRL ONESHOOT Mask */

#define CMSDK_DUALTIMER1_INTCLR_Pos          0                                               /*!< CMSDK_DUALTIMER1 INTCLR: INT Clear Position */
#define CMSDK_DUALTIMER1_INTCLR_Msk          (0x1ul << CMSDK_DUALTIMER1_INTCLR_Pos)          /*!< CMSDK_DUALTIMER1 INTCLR: INT Clear  Mask */

#define CMSDK_DUALTIMER1_RAWINTSTAT_Pos      0                                               /*!< CMSDK_DUALTIMER1 RAWINTSTAT: Raw Int Status Position */
#define CMSDK_DUALTIMER1_RAWINTSTAT_Msk      (0x1ul << CMSDK_DUALTIMER1_RAWINTSTAT_Pos)      /*!< CMSDK_DUALTIMER1 RAWINTSTAT: Raw Int Status Mask */

#define CMSDK_DUALTIMER1_MASKINTSTAT_Pos     0                                               /*!< CMSDK_DUALTIMER1 MASKINTSTAT: Mask Int Status Position */
#define CMSDK_DUALTIMER1_MASKINTSTAT_Msk     (0x1ul << CMSDK_DUALTIMER1_MASKINTSTAT_Pos)     /*!< CMSDK_DUALTIMER1 MASKINTSTAT: Mask Int Status Mask */

#define CMSDK_DUALTIMER1_BGLOAD_Pos          0                                               /*!< CMSDK_DUALTIMER1 BGLOAD: Background Load Position */
#define CMSDK_DUALTIMER1_BGLOAD_Msk          (0xFFFFFFFFul << CMSDK_DUALTIMER1_BGLOAD_Pos)   /*!< CMSDK_DUALTIMER1 BGLOAD: Background Load Mask */

#define CMSDK_DUALTIMER2_LOAD_Pos            0                                               /*!< CMSDK_DUALTIMER2 LOAD: LOAD Position */
#define CMSDK_DUALTIMER2_LOAD_Msk            (0xFFFFFFFFul << CMSDK_DUALTIMER2_LOAD_Pos)     /*!< CMSDK_DUALTIMER2 LOAD: LOAD Mask */

#define CMSDK_DUALTIMER2_VALUE_Pos           0                                               /*!< CMSDK_DUALTIMER2 VALUE: VALUE Position */
#define CMSDK_DUALTIMER2_VALUE_Msk           (0xFFFFFFFFul << CMSDK_DUALTIMER2_VALUE_Pos)    /*!< CMSDK_DUALTIMER2 VALUE: VALUE Mask */

#define CMSDK_DUALTIMER2_CTRL_EN_Pos         7                                               /*!< CMSDK_DUALTIMER2 CTRL_EN: CTRL Enable Position */
#define CMSDK_DUALTIMER2_CTRL_EN_Msk         (0x1ul << CMSDK_DUALTIMER2_CTRL_EN_Pos)         /*!< CMSDK_DUALTIMER2 CTRL_EN: CTRL Enable Mask */

#define CMSDK_DUALTIMER2_CTRL_MODE_Pos       6                                               /*!< CMSDK_DUALTIMER2 CTRL_MODE: CTRL MODE Position */
#define CMSDK_DUALTIMER2_CTRL_MODE_Msk       (0x1ul << CMSDK_DUALTIMER2_CTRL_MODE_Pos)       /*!< CMSDK_DUALTIMER2 CTRL_MODE: CTRL MODE Mask */

#define CMSDK_DUALTIMER2_CTRL_INTEN_Pos      5                                               /*!< CMSDK_DUALTIMER2 CTRL_INTEN: CTRL Int Enable Position */
#define CMSDK_DUALTIMER2_CTRL_INTEN_Msk      (0x1ul << CMSDK_DUALTIMER2_CTRL_INTEN_Pos)      /*!< CMSDK_DUALTIMER2 CTRL_INTEN: CTRL Int Enable Mask */

#define CMSDK_DUALTIMER2_CTRL_PRESCALE_Pos   2                                               /*!< CMSDK_DUALTIMER2 CTRL_PRESCALE: CTRL PRESCALE Position */
#define CMSDK_DUALTIMER2_CTRL_PRESCALE_Msk   (0x3ul << CMSDK_DUALTIMER2_CTRL_PRESCALE_Pos)   /*!< CMSDK_DUALTIMER2 CTRL_PRESCALE: CTRL PRESCALE Mask */

#define CMSDK_DUALTIMER2_CTRL_SIZE_Pos       1                                               /*!< CMSDK_DUALTIMER2 CTRL_SIZE: CTRL SIZE Position */
#define CMSDK_DUALTIMER2_CTRL_SIZE_Msk       (0x1ul << CMSDK_DUALTIMER2_CTRL_SIZE_Pos)       /*!< CMSDK_DUALTIMER2 CTRL_SIZE: CTRL SIZE Mask */

#define CMSDK_DUALTIMER2_CTRL_ONESHOOT_Pos   0                                               /*!< CMSDK_DUALTIMER2 CTRL_ONESHOOT: CTRL ONESHOOT Position */
#define CMSDK_DUALTIMER2_CTRL_ONESHOOT_Msk   (0x1ul << CMSDK_DUALTIMER2_CTRL_ONESHOOT_Pos)   /*!< CMSDK_DUALTIMER2 CTRL_ONESHOOT: CTRL ONESHOOT Mask */

#define CMSDK_DUALTIMER2_INTCLR_Pos          0                                               /*!< CMSDK_DUALTIMER2 INTCLR: INT Clear Position */
#define CMSDK_DUALTIMER2_INTCLR_Msk          (0x1ul << CMSDK_DUALTIMER2_INTCLR_Pos)          /*!< CMSDK_DUALTIMER2 INTCLR: INT Clear  Mask */

#define CMSDK_DUALTIMER2_RAWINTSTAT_Pos      0                                               /*!< CMSDK_DUALTIMER2 RAWINTSTAT: Raw Int Status Position */
#define CMSDK_DUALTIMER2_RAWINTSTAT_Msk      (0x1ul << CMSDK_DUALTIMER2_RAWINTSTAT_Pos)      /*!< CMSDK_DUALTIMER2 RAWINTSTAT: Raw Int Status Mask */

#define CMSDK_DUALTIMER2_MASKINTSTAT_Pos     0                                               /*!< CMSDK_DUALTIMER2 MASKINTSTAT: Mask Int Status Position */
#define CMSDK_DUALTIMER2_MASKINTSTAT_Msk     (0x1ul << CMSDK_DUALTIMER2_MASKINTSTAT_Pos)     /*!< CMSDK_DUALTIMER2 MASKINTSTAT: Mask Int Status Mask */

#define CMSDK_DUALTIMER2_BGLOAD_Pos          0                                               /*!< CMSDK_DUALTIMER2 BGLOAD: Background Load Position */
#define CMSDK_DUALTIMER2_BGLOAD_Msk          (0xFFFFFFFFul << CMSDK_DUALTIMER2_BGLOAD_Pos)   /*!< CMSDK_DUALTIMER2 BGLOAD: Background Load Mask */

typedef struct cobra_aon_dualtimer_single_type
{
  __attribute__ ((aligned (4))) uint32_t TimerLoad;    // <h> Timer Load </h>
  __attribute__ ((aligned (4))) uint32_t TimerValue;   // <h> Timer Counter Current Value <r></h>
  __attribute__ ((aligned (4))) uint32_t TimerControl; // <h> Timer Control
                              //   <o.7> TimerEn: Timer Enable
                              //   <o.6> TimerMode: Timer Mode
                              //     <0=> Freerunning-mode
                              //     <1=> Periodic mode
                              //   <o.5> IntEnable: Interrupt Enable
                              //   <o.2..3> TimerPre: Timer Prescale
                              //     <0=> / 1
                              //     <1=> / 16
                              //     <2=> / 256
                              //     <3=> Undefined!
                              //   <o.1> TimerSize: Timer Size
                              //     <0=> 16-bit counter
                              //     <1=> 32-bit counter
                              //   <o.0> OneShot: One-shoot mode
                              //     <0=> Wrapping mode
                              //     <1=> One-shot mode
                              // </h>
  __attribute__ ((aligned (4))) uint32_t TimerIntClr;  // <h> Timer Interrupt Clear <w></h>
  __attribute__ ((aligned (4))) uint32_t TimerRIS;     // <h> Timer Raw Interrupt Status <r></h>
  __attribute__ ((aligned (4))) uint32_t TimerMIS;     // <h> Timer Masked Interrupt Status <r></h>
  __attribute__ ((aligned (4))) uint32_t TimerBGLoad;  // <h> Background Load Register </h>


}COBRA_AON_DUALTIMER_SINGLE_TypeDef;

#define CMSDK_DUALTIMER_LOAD_Pos             0                                               /*!< CMSDK_DUALTIMER LOAD: LOAD Position */
#define CMSDK_DUALTIMER_LOAD_Msk             (0xFFFFFFFFul << CMSDK_DUALTIMER_LOAD_Pos)      /*!< CMSDK_DUALTIMER LOAD: LOAD Mask */

#define CMSDK_DUALTIMER_VALUE_Pos            0                                               /*!< CMSDK_DUALTIMER VALUE: VALUE Position */
#define CMSDK_DUALTIMER_VALUE_Msk            (0xFFFFFFFFul << CMSDK_DUALTIMER_VALUE_Pos)     /*!< CMSDK_DUALTIMER VALUE: VALUE Mask */

#define CMSDK_DUALTIMER_CTRL_EN_Pos          7                                               /*!< CMSDK_DUALTIMER CTRL_EN: CTRL Enable Position */
#define CMSDK_DUALTIMER_CTRL_EN_Msk          (0x1ul << CMSDK_DUALTIMER_CTRL_EN_Pos)          /*!< CMSDK_DUALTIMER CTRL_EN: CTRL Enable Mask */

#define CMSDK_DUALTIMER_CTRL_MODE_Pos        6                                               /*!< CMSDK_DUALTIMER CTRL_MODE: CTRL MODE Position */
#define CMSDK_DUALTIMER_CTRL_MODE_Msk        (0x1ul << CMSDK_DUALTIMER_CTRL_MODE_Pos)        /*!< CMSDK_DUALTIMER CTRL_MODE: CTRL MODE Mask */

#define CMSDK_DUALTIMER_CTRL_INTEN_Pos       5                                               /*!< CMSDK_DUALTIMER CTRL_INTEN: CTRL Int Enable Position */
#define CMSDK_DUALTIMER_CTRL_INTEN_Msk       (0x1ul << CMSDK_DUALTIMER_CTRL_INTEN_Pos)       /*!< CMSDK_DUALTIMER CTRL_INTEN: CTRL Int Enable Mask */

#define CMSDK_DUALTIMER_CTRL_PRESCALE_Pos    2                                               /*!< CMSDK_DUALTIMER CTRL_PRESCALE: CTRL PRESCALE Position */
#define CMSDK_DUALTIMER_CTRL_PRESCALE_Msk    (0x3ul << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos)    /*!< CMSDK_DUALTIMER CTRL_PRESCALE: CTRL PRESCALE Mask */

#define CMSDK_DUALTIMER_CTRL_SIZE_Pos        1                                               /*!< CMSDK_DUALTIMER CTRL_SIZE: CTRL SIZE Position */
#define CMSDK_DUALTIMER_CTRL_SIZE_Msk        (0x1ul << CMSDK_DUALTIMER_CTRL_SIZE_Pos)        /*!< CMSDK_DUALTIMER CTRL_SIZE: CTRL SIZE Mask */

#define CMSDK_DUALTIMER_CTRL_ONESHOOT_Pos    0                                               /*!< CMSDK_DUALTIMER CTRL_ONESHOOT: CTRL ONESHOOT Position */
#define CMSDK_DUALTIMER_CTRL_ONESHOOT_Msk    (0x1ul << CMSDK_DUALTIMER_CTRL_ONESHOOT_Pos)    /*!< CMSDK_DUALTIMER CTRL_ONESHOOT: CTRL ONESHOOT Mask */

#define CMSDK_DUALTIMER_INTCLR_Pos           0                                               /*!< CMSDK_DUALTIMER INTCLR: INT Clear Position */
#define CMSDK_DUALTIMER_INTCLR_Msk           (0x1ul << CMSDK_DUALTIMER_INTCLR_Pos)           /*!< CMSDK_DUALTIMER INTCLR: INT Clear  Mask */

#define CMSDK_DUALTIMER_RAWINTSTAT_Pos       0                                               /*!< CMSDK_DUALTIMER RAWINTSTAT: Raw Int Status Position */
#define CMSDK_DUALTIMER_RAWINTSTAT_Msk       (0x1ul << CMSDK_DUALTIMER_RAWINTSTAT_Pos)       /*!< CMSDK_DUALTIMER RAWINTSTAT: Raw Int Status Mask */

#define CMSDK_DUALTIMER_MASKINTSTAT_Pos      0                                               /*!< CMSDK_DUALTIMER MASKINTSTAT: Mask Int Status Position */
#define CMSDK_DUALTIMER_MASKINTSTAT_Msk      (0x1ul << CMSDK_DUALTIMER_MASKINTSTAT_Pos)      /*!< CMSDK_DUALTIMER MASKINTSTAT: Mask Int Status Mask */

#define CMSDK_DUALTIMER_BGLOAD_Pos           0                                               /*!< CMSDK_DUALTIMER BGLOAD: Background Load Position */
#define CMSDK_DUALTIMER_BGLOAD_Msk           (0xFFFFFFFFul << CMSDK_DUALTIMER_BGLOAD_Pos)    /*!< CMSDK_DUALTIMER BGLOAD: Background Load Mask */



typedef enum {
	AON_DUALTIMER1,
	AON_DUALTIMER2
}AON_DUALTIME_INDEX;

typedef enum {
	AON_COUNT_16BIT,
	AON_COUNT_32BIT
}AON_DUALTIMER_COUNT_SIZE;

typedef enum {
	AON_DUALTIME_FREE_RUNNING,
	AON_DUALTIME_PERIODIC,
	AON_DUALTIME_ONE_SHOT
}AON_DUALTIME_MODE;


typedef void (*aon_dual_timer_callback_t)(void);

void cobra_aon_dualtimer_init(void);

void cobra_aon_dualtimer_set(AON_DUALTIME_INDEX time_index, AON_DUALTIMER_COUNT_SIZE count_size, AON_DUALTIME_MODE mode,  unsigned int cycle, unsigned int prescale, unsigned int interrupt);

void cobra_aon_dualtimer_set_frequency(AON_DUALTIME_INDEX timer_index, float hz);

void cobra_aon_dualtimer_interrupt(void);



void app_cobra_init_aon_dualtimer1(int hz);


void app_cobra_init_aon_dualtimer2(int hz);


void app_cobra_aon_dualtimer1_set_callback(aon_dual_timer_callback_t func);


void app_cobra_aon_dualtimer2_set_callback(aon_dual_timer_callback_t func);


#define COBRA_AON_DUALTIMER_BASE				(0x40086000UL)

#define COBRA_AON_DUALTIMER_1_BASE  (COBRA_AON_DUALTIMER_BASE)
#define COBRA_AON_DUALTIMER_2_BASE  (COBRA_AON_DUALTIMER_BASE + 0x20UL)


#endif   //COBRA_DUALTIMER_EN



#endif

