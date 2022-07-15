/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_spwm.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/5

	Description :   


	Changed Log	:

		Liujian  2020/8/5 		- Creation

*****************************************************************************/
#ifndef __COBRA4A_MPWM_H__
#define __COBRA4A_MPWM_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>
#include "cobra4_common.h"

#if COBRA_MPWM_EN

typedef struct cobra_mpwm_t
{
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_con;   //PWM Control read address 

		 __attribute__((aligned(4))) volatile unsigned int  mpwm_con_set;   //PWM Control set address 
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_con_clr;   //PWM Control clear address
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_capcon;    // Capture Control read address
	
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_capcon_set;     // Capture Control set address 
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_capcon_clr;    // Event Control clear address 
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_tc[3];       // Timer Counter register 
	
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_lim[3];      // Limit register         
	  
	   __attribute__((aligned(4))) volatile unsigned int  mpwm_mat[3];      // Match register        
	
	   
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_dt;           // Dead time register    
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_ccp;          // Communication Pattern register
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_cap[3];       // Capture register      
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_inten;        // Interrupt Enable read address
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_inten_set;   // Interrupt Enable set address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_inten_clr;   // Interrupt Enable clear address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_cntcon;      // Count Control read address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_cntcon_set;   // Count Control set address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_cntcon_clr;   // Count Control clear address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_intf;        // Interrupt flags read address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_intf_set;   // Interrupt flags set address 
	 
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_intf_clr;   // Interrupt flags clear address 
	
	 __attribute__((aligned(4))) volatile unsigned int  mpwm_cap_clr;   // Capture clear address  
	 
}COBRA_MPWM_T;



/* Private Macros ------------------------------------------------------------- */
/** @defgroup MCPWM_Private_Macros MCPWM Private Macros
 * @{
 */
/** Edge aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_EDGE_MODE			((uint32_t)(0))
/** Center aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_CENTER_MODE		((uint32_t)(1))

/** Polarity of the MCOA and MCOB pins: Passive state is LOW, active state is HIGH */
#define MCPWM_CHANNEL_PASSIVE_LO		((uint32_t)(0))
/** Polarity of the MCOA and MCOB pins: Passive state is HIGH, active state is LOW */
#define MCPWM_CHANNEL_PASSIVE_HI		((uint32_t)(1))

/* Output Patent in 3-phase DC mode, the internal MCOA0 signal is routed to any or all of
 * the six output pins under the control of the bits in this register */
#define MCPWM_PATENT_A0		((uint32_t)(1<<0))	/**< MCOA0 tracks internal MCOA0 */
#define MCPWM_PATENT_B0		((uint32_t)(1<<1))	/**< MCOB0 tracks internal MCOA0 */
#define MCPWM_PATENT_A1		((uint32_t)(1<<2))	/**< MCOA1 tracks internal MCOA0 */
#define MCPWM_PATENT_B1		((uint32_t)(1<<3))	/**< MCOB1 tracks internal MCOA0 */
#define MCPWM_PATENT_A2		((uint32_t)(1<<4))	/**< MCOA2 tracks internal MCOA0 */
#define MCPWM_PATENT_B2		((uint32_t)(1<<5))	/**< MCOB2 tracks internal MCOA0 */

/* Interrupt type in MCPWM */
/** Limit interrupt for channel (0) */
#define MCPWM_INTFLAG_LIM0	MCPWM_INT_ILIM(0)
/** Match interrupt for channel (0) */
#define MCPWM_INTFLAG_MAT0	MCPWM_INT_IMAT(0)
/** Capture interrupt for channel (0) */
#define MCPWM_INTFLAG_CAP0	MCPWM_INT_ICAP(0)

/** Limit interrupt for channel (1) */
#define MCPWM_INTFLAG_LIM1	MCPWM_INT_ILIM(1)
/** Match interrupt for channel (1) */
#define MCPWM_INTFLAG_MAT1	MCPWM_INT_IMAT(1)
/** Capture interrupt for channel (1) */
#define MCPWM_INTFLAG_CAP1	MCPWM_INT_ICAP(1)

/** Limit interrupt for channel (2) */
#define MCPWM_INTFLAG_LIM2	MCPWM_INT_ILIM(2)
/** Match interrupt for channel (2) */
#define MCPWM_INTFLAG_MAT2	MCPWM_INT_IMAT(2)
/** Capture interrupt for channel (2) */
#define MCPWM_INTFLAG_CAP2	MCPWM_INT_ICAP(2)

/** Fast abort interrupt */
#define MCPWM_INTFLAG_ABORT	MCPWM_INT_ABORT



/*********************************************************************//**
 * Macro defines for MCPWM Control register
 **********************************************************************/
/* MCPWM Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Control read address
 * - MCPWM Control set address
 * - MCPWM Control clear address
 */
/**< Stops/starts timer channel n */
#define MCPWM_CON_RUN(n)		(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*8)+0))) : (0))
/**< Edge/center aligned operation for channel n */
#define MCPWM_CON_CENTER(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+1))) : (0))
/**< Select polarity of the MCOAn and MCOBn pin */
#define MCPWM_CON_POLAR(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+2))) : (0))
/**< Control the dead-time feature for channel n */
#define MCPWM_CON_DTE(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+3))) : (0))
/**< Enable/Disable update of functional register for channel n */
#define MCPWM_CON_DISUP(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+4))) : (0))
/**< Control the polarity for all 3 channels */
#define MCPWM_CON_INVBDC		((uint32_t)(1<<29))
/**< 3-phase AC mode select */
#define MCPWM_CON_ACMODE		((uint32_t)(1<<30))
/**< 3-phase DC mode select */
#define MCPWM_CON_DCMODE		(((uint32_t)1<<31))

/*********************************************************************//**
 * Macro defines for MCPWM Capture Control register
 **********************************************************************/
/* Capture Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Capture Control read address
 * - MCPWM Capture Control set address
 * - MCPWM Capture control clear address
 */
/** Enables/Disable channel (cap) capture event on a rising edge on MCI(mci) */
#define MCPWM_CAPCON_CAPMCI_RE(cap,mci)	(((cap<=2)&&(mci<=2)) ? ((uint32_t)(1<<((cap*6)+(mci*2)+0))) : (0))
/** Enables/Disable channel (cap) capture event on a falling edge on MCI(mci) */
#define MCPWM_CAPCON_CAPMCI_FE(cap,mci)	(((cap<=2)&&(mci<=2)) ? ((uint32_t)(1<<((cap*6)+(mci*2)+1))) : (0))
/** TC(n) is reset by channel (n) capture event */
#define MCPWM_CAPCON_RT(n)				(((n<=2)) ? ((uint32_t)(1<<(18+(n)))) : (0))
/** Hardware noise filter: channel (n) capture events are delayed */
#define MCPWM_CAPCON_HNFCAP(n)			(((n<=2)) ? ((uint32_t)(1<<(21+(n)))) : (0))

/*********************************************************************//**
 * Macro defines for MCPWM Interrupt register
 **********************************************************************/
/* Interrupt registers, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Interrupt Enable read address
 * - MCPWM Interrupt Enable set address
 * - MCPWM Interrupt Enable clear address
 * - MCPWM Interrupt Flags read address
 * - MCPWM Interrupt Flags set address
 * - MCPWM Interrupt Flags clear address
 */
/** Limit interrupt for channel (n) */
#define MCPWM_INT_ILIM(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+0))) : (0))
/** Match interrupt for channel (n) */
#define MCPWM_INT_IMAT(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+1))) : (0))
/** Capture interrupt for channel (n) */
#define MCPWM_INT_ICAP(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+2))) : (0))
/** Fast abort interrupt */
#define MCPWM_INT_ABORT		((uint32_t)(1<<15))

/*********************************************************************//**
 * Macro defines for MCPWM Count Control register
 **********************************************************************/
/* MCPWM Count Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Count Control read address
 * - MCPWM Count Control set address
 * - MCPWM Count Control clear address
 */
/** Counter(tc) advances on a rising edge on MCI(mci) pin */
#define MCPWM_CNTCON_TCMCI_RE(tc,mci)	(((tc<=2)&&(mci<=2)) ? ((uint32_t)(1<<((6*tc)+(2*mci)+0))) : (0))
/** Counter(cnt) advances on a falling edge on MCI(mci) pin */
#define MCPWM_CNTCON_TCMCI_FE(tc,mci)	(((tc<=2)&&(mci<=2)) ? ((uint32_t)(1<<((6*tc)+(2*mci)+1))) : (0))
/** Channel (n) is in counter mode */
#define MCPWM_CNTCON_CNTR(n)			(((n<=2)) ? ((uint32_t)(1<<(29+n))) : (0))


/*********************************************************************//**
 * Macro defines for MCPWM Capture clear address register
 **********************************************************************/
/** Clear the MCCAP (n) register */
#define MCPWM_CAPCLR_CAP(n)		(((n<=2)) ? ((uint32_t)(1<<n)) : (0))


/*********************************************************************//**
 * Macro defines for MCPWM Dead-time register
 **********************************************************************/
/** Dead time value x for channel n */
#define MCPWM_DT(n,x)		(((n<=2)) ? ((uint32_t)((x&0x3FF)<<(n*10))) : (0))



/*********************************************************************//**
 * @brief MCPWM enumeration
 **********************************************************************/
/**
 * @brief	MCPWM channel identifier definition
 */
typedef enum
{
	MCPWM_CHANNEL_0 = 0,		/**< MCPWM channel 0 */
	MCPWM_CHANNEL_1,			/**< MCPWM channel 1 */
	MCPWM_CHANNEL_2				/**< MCPWM channel 2 */
} en_MCPWM_Channel_Id;

/*********************************************************************//**
 * @brief MCPWM structure definitions
 **********************************************************************/
/**
 * @brief Motor Control PWM Channel Configuration structure type definition
 */
typedef struct {
	       uint32_t channelType;					  //  Edge/center aligned mode for this channel, should be:
												                  //       - MCPWM_CHANNEL_EDGE_MODE: Channel is in Edge mode
												                  //       - MCPWM_CHANNEL_CENTER_MODE: Channel is in Center mode 
	
	       uint32_t channelPolarity;			   //  Polarity of the MCOA and MCOB pins, should be:
												                   //  - MCPWM_CHANNEL_PASSIVE_LO: Passive state is LOW, active state is HIGH
												                    // - MCPWM_CHANNEL_PASSIVE_HI: Passive state is HIGH, active state is LOW 

	       uint32_t channelDeadtimeEnable;			// Enable/Disable DeadTime function for channel, should be:
												                      //  - ENABLE.
												                      //   - DISABLE. 
	
	        uint32_t channelDeadtimeValue;			// DeadTime value, should be less than 0x3FF 
	
	        uint32_t channelUpdateEnable;			   // Enable/Disable updates of functional registers, should be:
												                       //    - ENABLE.
												                       //    - DISABLE.  
												
	       uint32_t channelTimercounterValue;		 // MCPWM Timer Counter value 
	
	       uint32_t channelPeriodValue;			     // MCPWM Period value 
				 
	       uint32_t channelPulsewidthValue;		   // MCPWM Pulse Width value 
} MCPWM_CHANNEL_CFG_Type;


/* MCPWM Capture Configuration type definition */
typedef struct {
	   uint32_t captureChannel;		  //Capture Channel Number, should be in range from 0 to 2 
	
	   uint32_t captureRising;			// Enable/Disable Capture on Rising Edge event, should be:
										              //  - ENABLE.
										              //  - DISABLE.
										              
	
	   uint32_t captureFalling;		  // Enable/Disable Capture on Falling Edge event, should be:
										              //   - ENABLE.
										              //   - DISABLE.
										       
	    uint32_t timerReset;			  // Enable/Disable Timer reset function an capture, should be:
										              //  - ENABLE.
										              //  - DISABLE.
										            
	
	    uint32_t hnfEnable;				  //  Enable/Disable Hardware noise filter function, should be:
										              // - ENABLE.
										              // - DISABLE.
										             
} MCPWM_CAPTURE_CFG_Type;



/* MCPWM Count Control Configuration type definition */
typedef struct {
	    uint32_t counterChannel;		// Counter Channel Number, should be in range from 0 to 2 
	
	    uint32_t countRising;			// Enable/Disable Capture on Rising Edge event, should be:
										            //   - ENABLE.
										            //  - DISABLE.
										        
	    uint32_t countFalling;		// Enable/Disable Capture on Falling Edge event, should be:
										            // - ENABLE.
										            // - DISABLE.								
} MCPWM_COUNT_CFG_Type;




#define COBRA_MPWM_BASE  (0x400A0000UL + 0x2000UL)

void cobra_mpwm_init(void);

void app_cobra_mcpwm_Init(void);

void app_cobra_mcpwm_config_channel(uint32_t channel_number, MCPWM_CHANNEL_CFG_Type * channelSetup);

void app_cobra_mcpwm_write_to_shadow(uint32_t channelNum, MCPWM_CHANNEL_CFG_Type *channelSetup);

void app_cobra_mcpwm_config_capture( uint32_t channelNum, MCPWM_CAPTURE_CFG_Type *captureConfig);

void app_cobra_mcpwm_clear_capture(uint32_t captureChannel);

uint32_t app_cobra_mcpwm_get_capture(uint32_t captureChannel);

void app_cobra_mcpwm_count_config(uint32_t channelNum, uint32_t countMode, MCPWM_COUNT_CFG_Type *countConfig);

void app_cobra_mcpwm_start(uint32_t channel0, uint32_t channel1, uint32_t channel2);

void app_cobra_mcpwm_stop(uint32_t channel0, uint32_t channel1, uint32_t channel2);

void app_cobra_mcpwm_ac_mode(uint32_t acMode);

void app_cobra_mcpwm_dc_mode(uint32_t dcMode, uint32_t outputInvered, uint32_t outputPattern);

void app_cobra_mcpwm_int_config(uint32_t ulIntType, FunctionalState NewState);

void app_cobra_mcpwm_int_set(uint32_t ulIntType);

void app_cobra_mcpwm_int_clear(uint32_t ulIntType);

FlagStatus app_cobra_mcpwm_get_int_status(uint32_t ulIntType);

void app_cobra_mcpwm_set_pattern(unsigned int pattern);
#ifdef COBRA_MPWM_NEW
bool read_dcmode(void);
bool read_acmode(void);
bool read_invbdc(void);
bool read_disup2(void);
bool read_dte2(void);
bool read_pola2(void);
bool read_center2(void);
bool read_run2(void);
bool read_disup1(void);
bool read_dte1(void);
bool read_pola1(void);
bool read_center1(void);
bool read_run1(void);
bool read_disup0(void);
bool read_dte0(void);
bool read_pola0(void);
bool read_center0(void);
bool read_run0(void);
void write_dcmode_set(void);
void write_acmode_set(void);
void write_invbdc_set(void);
void write_disup2_set(void);
void write_dte2_set(void);
void write_pola2_set(void);
void write_center2_set(void);
void write_run2_set(void);
void write_disup1_set(void);
void write_dte1_set(void);
void write_pola1_set(void);
void write_center1_set(void);
void write_run1_set(void);
void write_disup0_set(void);
void write_dte0_set(void);
void write_pola0_set(void);
void write_center0_set(void);
void write_run0_set(void);
void write_dcmode_clr(void);
void write_acmode_clr(void);
void write_invbdc_clr(void);
void write_disup2_clr(void);
void write_dte2_clr(void);
void write_pola2_clr(void);
void write_center2_clr(void);
void write_run2_clr(void);
void write_disup1_clr(void);
void write_dte1_clr(void);
void write_pola1_clr(void);
void write_center1_clr(void);
void write_run1_clr(void);
void write_disup0_clr(void);
void write_dte0_clr(void);
void write_pola0_clr(void);
void write_center0_clr(void);
void write_run0_clr(void);
bool read_rt2(void);
bool read_rt1(void);
bool read_rt0(void);
bool read_cap2mci2_fe(void);
bool read_cap2mci2_re(void);
bool read_cap2mci1_fe(void);
bool read_cap2mci1_re(void);
bool read_cap2mci0_fe(void);
bool read_cap2mci0_re(void);
bool read_cap1mci2_fe(void);
bool read_cap1mci2_re(void);
bool read_cap1mci1_fe(void);
bool read_cap1mci1_re(void);
bool read_cap1mci0_fe(void);
bool read_cap1mci0_re(void);
bool read_cap0mci2_fe(void);
bool read_cap0mci2_re(void);
bool read_cap0mci1_fe(void);
bool read_cap0mci1_re(void);
bool read_cap0mci0_fe(void);
bool read_cap0mci0_re(void);
void write_rt2_set(void);
void write_rt1_set(void);
void write_rt0_set(void);
void write_cap2mci2_fe_set(void);
void write_cap2mci2_re_set(void);
void write_cap2mci1_fe_set(void);
void write_cap2mci1_re_set(void);
void write_cap2mci0_fe_set(void);
void write_cap2mci0_re_set(void);
void write_cap1mci2_fe_set(void);
void write_cap1mci2_re_set(void);
void write_cap1mci1_fe_set(void);
void write_cap1mci1_re_set(void);
void write_cap1mci0_fe_set(void);
void write_cap1mci0_re_set(void);
void write_cap0mci2_fe_set(void);
void write_cap0mci2_re_set(void);
void write_cap0mci1_fe_set(void);
void write_cap0mci1_re_set(void);
void write_cap0mci0_fe_set(void);
void write_cap0mci0_re_set(void);
void write_rt2_clr(void);
void write_rt1_clr(void);
void write_rt0_clr(void);
void write_cap2mci2_fe_clr(void);
void write_cap2mci2_re_clr(void);
void write_cap2mci1_fe_clr(void);
void write_cap2mci1_re_clr(void);
void write_cap2mci0_fe_clr(void);
void write_cap2mci0_re_clr(void);
void write_cap1mci2_fe_clr(void);
void write_cap1mci2_re_clr(void);
void write_cap1mci1_fe_clr(void);
void write_cap1mci1_re_clr(void);
void write_cap1mci0_fe_clr(void);
void write_cap1mci0_re_clr(void);
void write_cap0mci2_fe_clr(void);
void write_cap0mci2_re_clr(void);
void write_cap0mci1_fe_clr(void);
void write_cap0mci1_re_clr(void);
void write_cap0mci0_fe_clr(void);
void write_cap0mci0_re_clr(void);
void set_tc0(uint32_t tc0);
uint32_t read_tc0(void);
void set_tc1(uint32_t tc1);
uint32_t read_tc1(void);
void set_tc2(uint32_t tc2);
uint32_t read_tc2(void);
void set_lim0(uint32_t lim0);
uint32_t read_lim0(void);
void set_lim1(uint32_t lim1);
uint32_t read_lim1(void);
void set_lim2(uint32_t lim2);
uint32_t read_lim2(void);
void set_mat0(uint32_t mat0);
uint32_t read_mat0(void);
void set_mat1(uint32_t mat1);
uint32_t read_mat1(void);
void set_mat2(uint32_t mat2);
uint32_t read_mat2(void);
void set_dt0(uint32_t dt0);
uint32_t read_dt0(void);
void set_dt1(uint32_t dt1);
uint32_t read_dt1(void);
void set_dt2(uint32_t dt2);
uint32_t read_dt2(void);
void set_ccpb2(bool en);
bool read_ccpb2(void);
void set_ccpa2(bool en);
bool read_ccpa2(void);
void set_ccpb1(bool en);
bool read_ccpb1(void);
void set_ccpa1(bool en);
bool read_ccpa1(void);
void set_ccpb0(bool en);
bool read_ccpb0(void);
void set_ccpa0(bool en);
bool read_ccpa0(void);
uint32_t read_cap0(void);
uint32_t read_cap1(void);
uint32_t read_cap2(void);
bool read_abort_int(void);
bool read_icap2_int(void);
bool read_imat2_int(void);
bool read_ilim2_int(void);
bool read_icap1_int(void);
bool read_imat1_int(void);
bool read_ilim1_int(void);
bool read_icap0_int(void);
bool read_imat0_int(void);
bool read_ilim0_int(void);
void write_abort_int_set(void);
void write_icap2_int_set(void);
void write_imat2_int_set(void);
void write_ilim2_int_set(void);
void write_icap1_int_set(void);
void write_imat1_int_set(void);
void write_ilim1_int_set(void);
void write_icap0_int_set(void);
void write_imat0_int_set(void);
void write_ilim0_int_set(void);
void write_abort_int_clr(void);
void write_icap2_int_clr(void);
void write_imat2_int_clr(void);
void write_ilim2_int_clr(void);
void write_icap1_int_clr(void);
void write_imat1_int_clr(void);
void write_ilim1_int_clr(void);
void write_icap0_int_clr(void);
void write_imat0_int_clr(void);
void write_ilim0_int_clr(void);
bool read_cntr2(void);
bool read_cntr1(void);
bool read_cntr0(void);
bool read_tc2mci2_fe(void);
bool read_tc2mci2_re(void);
bool read_tc2mci1_fe(void);
bool read_tc2mci1_re(void);
bool read_tc2mci0_fe(void);
bool read_tc2mci0_re(void);
bool read_tc1mci2_fe(void);
bool read_tc1mci2_re(void);
bool read_tc1mci1_fe(void);
bool read_tc1mci1_re(void);
bool read_tc1mci0_fe(void);
bool read_tc1mci0_re(void);
bool read_tc0mci2_fe(void);
bool read_tc0mci2_re(void);
bool read_tc0mci1_fe(void);
bool read_tc0mci1_re(void);
bool read_tc0mci0_fe(void);
bool read_tc0mci0_re(void);
void write_cntr2_set(void);
void write_cntr1_set(void);
void write_cntr0_set(void);
void write_tc2mci2_fe_set(void);
void write_tc2mci2_re_set(void);
void write_tc2mci1_fe_set(void);
void write_tc2mci1_re_set(void);
void write_tc2mci0_fe_set(void);
void write_tc2mci0_re_set(void);
void write_tc1mci2_fe_set(void);
void write_tc1mci2_re_set(void);
void write_tc1mci1_fe_set(void);
void write_tc1mci1_re_set(void);
void write_tc1mci0_fe_set(void);
void write_tc1mci0_re_set(void);
void write_tc0mci2_fe_set(void);
void write_tc0mci2_re_set(void);
void write_tc0mci1_fe_set(void);
void write_tc0mci1_re_set(void);
void write_tc0mci0_fe_set(void);
void write_tc0mci0_re_set(void);
void write_cntr2_clr(void);
void write_cntr1_clr(void);
void write_cntr0_clr(void);
void write_tc2mci2_fe_clr(void);
void write_tc2mci2_re_clr(void);
void write_tc2mci1_fe_clr(void);
void write_tc2mci1_re_clr(void);
void write_tc2mci0_fe_clr(void);
void write_tc2mci0_re_clr(void);
void write_tc1mci2_fe_clr(void);
void write_tc1mci2_re_clr(void);
void write_tc1mci1_fe_clr(void);
void write_tc1mci1_re_clr(void);
void write_tc1mci0_fe_clr(void);
void write_tc1mci0_re_clr(void);
void write_tc0mci2_fe_clr(void);
void write_tc0mci2_re_clr(void);
void write_tc0mci1_fe_clr(void);
void write_tc0mci1_re_clr(void);
void write_tc0mci0_fe_clr(void);
void write_tc0mci0_re_clr(void);
bool read_abort_intf(void);
bool read_icap2_intf(void);
bool read_imat2_intf(void);
bool read_ilim2_intf(void);
bool read_icap1_intf(void);
bool read_imat1_intf(void);
bool read_ilim1_intf(void);
bool read_icap0_intf(void);
bool read_imat0_intf(void);
bool read_ilim0_intf(void);
void write_abort_intf_set(void);
void write_icap2_intf_set(void);
void write_imat2_intf_set(void);
void write_ilim2_intf_set(void);
void write_icap1_intf_set(void);
void write_imat1_intf_set(void);
void write_ilim1_intf_set(void);
void write_icap0_intf_set(void);
void write_imat0_intf_set(void);
void write_ilim0_intf_set(void);
void write_abort_intf_clr(void);
void write_icap2_intf_clr(void);
void write_imat2_intf_clr(void);
void write_ilim2_intf_clr(void);
void write_icap1_intf_clr(void);
void write_imat1_intf_clr(void);
void write_ilim1_intf_clr(void);
void write_icap0_intf_clr(void);
void write_imat0_intf_clr(void);
void write_ilim0_intf_clr(void);
void write_cap_clr2(void);
void write_cap_clr1(void);
void write_cap_clr0(void);

#define MPWM_CHANNEL0_EN		0x1
#define MPWM_CHANNEL1_EN		0x2
#define MPWM_CHANNEL2_EN		0x4

void mpwm_en(uint8_t channel_en);
void mpwm_close(uint8_t channel_en);

#endif

#endif

#endif
