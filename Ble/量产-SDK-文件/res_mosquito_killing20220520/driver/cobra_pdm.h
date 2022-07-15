/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_pdm.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/28

	Description :   

	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/

#ifndef __COBRA_PDM_H__
#define __COBRA_PDM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_PDM_EN

typedef struct cobra_pdm_type
{
	__attribute__ ((aligned (4))) uint32_t reserver1[9];
	__attribute__ ((aligned (4))) uint32_t cfg_pdm_ctrl;
	
	__attribute__ ((aligned (4))) uint32_t cfg_pdm_ctrl1;
	__attribute__ ((aligned (4))) uint32_t cfg_clk_gen;
	__attribute__ ((aligned (4))) uint32_t upfifo_interrupt;

	__attribute__ ((aligned (4))) uint32_t upfifo_threshold;
	__attribute__ ((aligned (4))) uint32_t upfifo_ctrl;

	__attribute__ ((aligned (4))) uint32_t log2_cic_decimation;

	__attribute__ ((aligned (4))) uint32_t cfg_bypass;
	__attribute__ ((aligned (4))) uint32_t hp_iir_coff0;

	__attribute__ ((aligned (4))) uint32_t cfg_pdm_data_delay_phase;
	__attribute__ ((aligned (4))) uint32_t cfg_gain_steps;
	__attribute__ ((aligned (4))) uint32_t cfg_sampling;

	__attribute__ ((aligned (4))) uint32_t reserver2[19];

	__attribute__ ((aligned (4))) uint32_t read_upfifo;

	__attribute__ ((aligned (4))) uint32_t upfifo_status;
	
}COBRA_PDM_TypeDef;



typedef enum pdm_sample_clk_edge{
	PDM_SAMPLE_CLK_NEGEDGE,
	PDM_SAMPLE_CLK_POSEDGE

}PDM_SAMPLE_CLK_EDGE;

typedef enum pdm_who_is_stero {
	LEFT_PIN_IS_STERO,
	RIGHT_PIN_IS_STERO
}PDM_WHO_IS_STERO;

/*
typedef enum pdm_who_is_mono {
	PDM0_DATA0_IS_MONO,
	PDM1_DATA1_IS_MONO
}PDM_WHO_IS_MONO;
*/

typedef enum pdm_gain_step{
	GAIN_STEP_NO_ATTENUATION,
	GAIN_STEP_6DB_ATTENUATION,
	GAIN_STEP_12DB_ATTENUATION,
	GAIN_STEP_18DB_ATTENUATION,
	GAIN_STEP_24DB_ATTENUATION,
	GAIN_STEP_30DB_ATTENUATION,
	GAIN_STEP_36DB_ATTENUATION,
	GAIN_STEP_42DB_ATTENUATION,

	GAIN_STEP_6DB_INCREASE,
	GAIN_STEP_12DB_INCREASE,
	GAIN_STEP_18DB_INCREASE,
	GAIN_STEP_24DB_INCREASE,
	GAIN_STEP_30DB_INCREASE,
	GAIN_STEP_36DB_INCREASE,
	GAIN_STEP_42DB_INCREASE,
	GAIN_STEP_48DB_INCREASE
}PDM_GAIN_STEP;

typedef enum pdm_sampling_object{
	SAMPLING_CIC_OUT,
	SAMPLING_CIC_OUT1,
	SAMPLING_CIC_OUT2,
	SAMPLING_HBF1_OUT,
	SAMPLING_HBF2_OUT,
	SAMPLING_HP_IIR_OUT,
	SAMPLING_GAIN_OUT,
	SAMPLING_PDM_PIN
}PDM_SAMPLING_OBJECT;




#define PDM_STATE_UPFIFO_OVERFLOW   	0x01
#define PDM_STATE_UPFIFO_UNDERFLOW   	0x02
#define PDM_STATE_UPFIFO_FULL	   		0x04
#define PDM_STATE_UPFIFO_ALMOSTFULL   	0x08
#define PDM_STATE_UPFIFO_EMPTY		    0x10

typedef enum
{
	PDM0,
}pdm_id;



void cobra_pdm_init(void);
void app_cobra_pdm_init(pdm_id index, uint8_t dma_en, uint8_t osr, float pclk_kHz);
void app_cobra_pdm_disable(pdm_id index);
void app_cobra_pdm_enable(pdm_id index);
unsigned int * cobra_pdm_read_upfifo_address(pdm_id index);

void cobra_pdm_cfg_pdm_bit_width(pdm_id index,  unsigned int bitwidth);

#define COBRA_PDM_BASE				((0x400CC000UL))

#endif   //COBRA_PDM_EN



#endif

