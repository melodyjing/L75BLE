/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_gpadc.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/9

	Description :   
	
	Changed Log	:

		Liujian  2019/5/9		- Creation

*****************************************************************************/

#ifndef __COBRA_GPADC_H__
#define __COBRA_GPADC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_GPADC_EN
typedef struct cobra_gpadc_type
{
		__attribute__ ((aligned (4))) volatile unsigned int GPADC_CTRL;          													/*0x000*/
		__attribute__ ((aligned (4))) volatile unsigned int CFG_SAMPLING_CTRL;        										/*0x004*/        
		__attribute__ ((aligned (4))) volatile unsigned int CFG_WAIT_TIME;  															/*0x008*/           
		__attribute__ ((aligned (4))) volatile unsigned int CFG_MAX_POLL_COUNT;  													/*0x00C*/
		__attribute__ ((aligned (4))) volatile unsigned int CFG_TEMP_POLL_COUNT_VALUE;      							/*0x010*/ 
		__attribute__ ((aligned (4))) volatile unsigned int CFG_BATT_POLL_COUNT_VALUE;     								/*0x014*/
		__attribute__ ((aligned (4))) volatile unsigned int CFG_HAND_OPT;   															/*0x018*/      
		__attribute__ ((aligned (4))) volatile unsigned int CFG_CALIBRATION_CTRL;      										/*0x01C*/ 
		__attribute__ ((aligned (4))) volatile unsigned int CFG_ANA_VAR_CTRL;       											/*0x020*/ 
		__attribute__ ((aligned (4))) volatile unsigned int COMPARE_A_CTRL;     								 					/*0x024*/
		__attribute__ ((aligned (4))) volatile unsigned int COMPARE_B_CTRL;       												/*0x028*/
		__attribute__ ((aligned (4))) volatile unsigned int CFG_HAND_COMPARE_B;         									/*0x02C*/ 
		__attribute__ ((aligned (4))) volatile unsigned int RESERVED0[3];         												/*0x030-0x38*/		
		__attribute__ ((aligned (4))) volatile unsigned int UPDATA;         															/*0x03C*/     
		__attribute__ ((aligned (4))) volatile unsigned int CALIBRATION_STATUS;        										/*0x040*/     
		__attribute__ ((aligned (4))) volatile unsigned int DC_OFFSET;    					   						 				/*0x044*/   
		__attribute__ ((aligned (4))) volatile unsigned int COMP_A_HISTORY_REG;														/*0x048*/  
		__attribute__ ((aligned (4))) volatile unsigned int COMP_B_HISTORY_REG;    									  		/*0x04C*/   
		__attribute__ ((aligned (4))) volatile unsigned int UP_COMPARE;    									  						/*0x050*/  
		
} COBRA_GPADC_TypeDef; 



typedef enum {
		TEMP_MON_MODE,
		BAT_MON_MODE,
		GPADC_MODE,
		IDLE_MODE,
	 
}BTADC_MODE;


typedef enum {
		VANA_DIFF_MODE,
		VANA_PIN0_PIN1,
		VANA_PIN2_PIN3,	 
}ADC_DIFF_MODE;


typedef enum {
		VANA_PAD0,
		VANA_PAD1,
		VANA_PAD2,
		VANA_PAD3,
		
}ADC_SEL_MODE;


typedef enum {
		FIRST_DIFF_THEN_SINGLE,
		ONLY_DIFF,
		SINGLE_CAL,
		NO_CAL,
		SINGLE_ENDED_CAL,
		DIFFERENTAL_CAL,	
}ADC_CAL_MODE;


#define COBRA_GPADC_BASE				(0x40084000UL)



void cobra_gpadc_init(void);
void app_cobra_gpadc_init(void);
#endif   //COBRA_GPADC_EN



#endif

