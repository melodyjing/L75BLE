/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_trng.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/21

	Description :   
	
	Changed Log	:

		Liujian  2019/5/21		- Creation



*****************************************************************************/


#ifndef __COBRA_TRNG_H__
#define __COBRA_TRNG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_TRNG_EN
typedef struct cobra_trng_type
{
		__attribute__ ((aligned (4))) unsigned int   Reserved1[64];
		__attribute__ ((aligned (4))) unsigned int   RNG_IMR;
		__attribute__ ((aligned (4))) unsigned int   RNG_ISR;
		__attribute__ ((aligned (4))) unsigned int   RNG_ICR;
		__attribute__ ((aligned (4))) unsigned int   TRNG_CONFIG;
		__attribute__ ((aligned (4))) unsigned int   TRNG_VALID;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA0;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA1;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA2;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA3;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA4;
		__attribute__ ((aligned (4))) unsigned int   EHR_DATA5;
		__attribute__ ((aligned (4))) unsigned int   RND_SOURCE_ENABLE;
		__attribute__ ((aligned (4))) unsigned int   SAMPLE_CNT1;
		__attribute__ ((aligned (4))) unsigned int   AUTOCORR_STATISTIC;
		__attribute__ ((aligned (4))) unsigned int   TRNG_DEBUG_CONTROL;
		__attribute__ ((aligned (4))) unsigned int   Reserved2;
		__attribute__ ((aligned (4))) unsigned int   TRNG_SW_RESET;
		__attribute__ ((aligned (4))) unsigned int   Reserved3[29];
		__attribute__ ((aligned (4))) unsigned int   TRNG_BUSY;
		__attribute__ ((aligned (4))) unsigned int   RST_BITS_COUNTER;
		__attribute__ ((aligned (4))) unsigned int   Reserved4[8];
		__attribute__ ((aligned (4))) unsigned int   RNG_BIST_CNTR[3];

} COBRA_TRNG_TypeDef; 



typedef void (*rand_func_result_cb) (uint32_t* rand_res);


void  cobra_trng_init(void);

unsigned int  cobra_trng_rand_read(void);

void cobra_m3_trng_interrupt(void);

void cobra_trng_read(rand_func_result_cb func_cb);

void app_cobra_trng_rand_callback(uint32_t * result);

#define  COBRA_TRNG_BASE			(0x400C4000UL)

#ifdef COBRA_TRNG_NEW

#define EHR_VALID_INT												0x01
#define AUTOCORR_ERR_INT										0x02
#define CRNGT_ERR_INT												0x04
#define VN_ERR_INT													0x08

/*Configuration register*/
#define SHORTEST_INVERTER_CHAIN_LENGTH			0x00
#define SHORT_INVERTER_CHAIN_LENGTH					0x01
#define LONG_INVERTER_CHAIN_LENGTH					0x02
#define LONGEST_INVERTER_CHAIN_LENGTH				0x03

/*Valid register*/
#define EHR_VALID														0x01

/*Random Source Enable register*/
#define RND_SRC_EN													0x01
#define RND_SRC_DISABLE											0x00

/*Debug Control register*/
#define VNC_BYPASS													0x02
#define TRNG_CRNGT_BYPASS										0x04
#define AUTO_CORRELATE_BYPASS								0x08

/*Reset register*/
#define TRNG_SW_RESET												0x01

/*Busy register*/
#define TRNG_BUSY_REG												0x01

/*Reset Bits Counter register*/
#define RST_BITS_COUNTER										0x01

#define HW_RNG_IMR_REG_ADDR 0x100UL
#define HW_RNG_ISR_REG_ADDR 0x104UL
#define HW_RNG_ICR_REG_ADDR 0x108UL
#define HW_TRNG_CONFIG_REG_ADDR 0x10CUL
#define HW_TRNG_VALID_REG_ADDR 0x110UL
#define HW_EHR_DATA_ADDR_0_REG_ADDR 0x114UL
#define HW_EHR_DATA_ADDR_1_REG_ADDR 0x118UL
#define HW_EHR_DATA_ADDR_2_REG_ADDR 0x11CUL
#define HW_EHR_DATA_ADDR_3_REG_ADDR 0x120UL
#define HW_EHR_DATA_ADDR_4_REG_ADDR 0x124UL
#define HW_EHR_DATA_ADDR_5_REG_ADDR 0x128UL
#define HW_RND_SOURCE_ENABLE_REG_ADDR 0x12CUL
#define HW_SAMPLE_CNT1_REG_ADDR 0x130UL
#define HW_AUTOCORR_STATISTIC_REG_ADDR 0x134UL
#define HW_TRNG_DEBUG_CONTROL_REG_ADDR 0x138UL
#define HW_TRNG_SW_RESET_REG_ADDR 0x140UL
#define HW_RNG_BUSY_REG_ADDR 0x1B8UL
#define HW_RST_BITS_COUNTER_REG_ADDR 0x1BCUL
#define HW_RNG_BIST_CNTR0_REG_ADDR 0x1E0UL
#define HW_RNG_BIST_CNTR1_REG_ADDR 0x1E4UL
#define HW_RNG_BIST_CNTR2_REG_ADDR 0x1E8UL

static inline void trng_writel(uint32_t reg, uint32_t val)
{
    *( uint32_t *) (COBRA_TRNG_BASE + reg) = val;
}

static inline uint32_t trng_readl(uint32_t reg)
{
    return *(const uint32_t *) (COBRA_TRNG_BASE+ reg);
}
uint32_t rdw_trng_rand(uint32_t rand[6]);
#endif

#endif   //COBRA_TRNG_EN


#endif

