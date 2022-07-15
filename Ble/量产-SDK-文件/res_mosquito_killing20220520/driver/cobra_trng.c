/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_trng.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/21

	Description :   


	Changed Log	:

		Liujian  2019/5/21			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_trng.h"
#include "cobra_irda.h"
#include "cobra_aon_pmu.h"
#include "cobra_pcrm.h"
#include "rdw_mcu.h"

#if COBRA_TRNG_EN


static rand_func_result_cb  cobra_trng_interrupt_callback ;

static volatile COBRA_TRNG_TypeDef * cobra_trng; 

void cobra_trng_init(void)
{
		cobra_trng = (COBRA_TRNG_TypeDef * )COBRA_TRNG_BASE;
}


unsigned int  cobra_trng_rand_read(void)
{
		unsigned int temp;

		cobra_pcrm_trng_active_mode_clock_enable(1);
	
		while((cobra_trng->TRNG_BUSY & 0x01));

		cobra_trng->TRNG_CONFIG = 0x01;

		cobra_trng->SAMPLE_CNT1 = 300;

		cobra_trng->RND_SOURCE_ENABLE = 0x01;

		while(!(cobra_trng->TRNG_VALID & 0x01));

		temp = cobra_trng->EHR_DATA0;
	
	  cobra_trng->TRNG_CONFIG = 0x00;
    cobra_trng->RND_SOURCE_ENABLE = 0x00;
    cobra_pcrm_trng_active_mode_clock_enable(0);
	
		return temp;

}



void cobra_trng_read(rand_func_result_cb func_cb)
{
	
		cobra_pcrm_trng_active_mode_clock_enable(1);
	
//		cobra_pcrm_trng_sleep_mode_clock_enable(1);
	
		cobra_trng_interrupt_callback = func_cb;

		cobra_trng->RNG_IMR = cobra_trng->RNG_IMR & ~0x01; //Enable valid interrupt

		cobra_trng->TRNG_CONFIG = 0x01;

		cobra_trng->SAMPLE_CNT1 = 300;

		cobra_trng->RND_SOURCE_ENABLE = 0x01;

		NVIC_SetPriority(TRNG_IRQn, 1);
		NVIC_ClearPendingIRQ(TRNG_IRQn);
		NVIC_EnableIRQ(TRNG_IRQn);
	
}

void cobra_m3_trng_interrupt(void)
{
	uint32_t interrupt_data[6];

	if(cobra_trng->RNG_ISR & 0x01)
	{
		
		cobra_trng->RNG_ICR = 0x0f;
		cobra_trng->RNG_IMR = cobra_trng->RNG_IMR | 0x01;
		cobra_trng->RND_SOURCE_ENABLE = 0x00;
		
		if(cobra_trng_interrupt_callback != NULL)
		{
				interrupt_data[0] = cobra_trng->EHR_DATA0;
				interrupt_data[1] = cobra_trng->EHR_DATA1;
				interrupt_data[2] = cobra_trng->EHR_DATA2;
				interrupt_data[3] = cobra_trng->EHR_DATA3;
				interrupt_data[4] = cobra_trng->EHR_DATA4;
				interrupt_data[5] = cobra_trng->EHR_DATA5;

				cobra_trng_interrupt_callback(interrupt_data);
			//	cobra_trng_interrupt_callback = NULL;
		}
		
	}
}

void app_cobra_trng_rand_callback(uint32_t * result)
{

		int i;
		for(i = 0; i < 6; i++)
		{
				printf("rand %d\n", result[i]%1000);
		}
	  cobra_trng_read(app_cobra_trng_rand_callback);
}

#ifdef COBRA_TRNG_NEW
void rdw_trng_set_config(uint8_t config)
{
    trng_writel(HW_TRNG_CONFIG_REG_ADDR,config);
}

bool rdw_trng_read_busy(void)
{
    return (trng_readl(HW_RNG_BUSY_REG_ADDR) & TRNG_BUSY_REG);
}

bool rdw_trng_read_valid(void)
{
    return (trng_readl(HW_TRNG_VALID_REG_ADDR) & EHR_VALID);
}

uint32_t rdw_trng_read_int_status(void)
{
    return (trng_readl(HW_RNG_ISR_REG_ADDR));
}

void rdw_trng_clear_int_status(uint32_t clear)
{
    trng_writel(HW_RNG_ICR_REG_ADDR,clear);
}

void rdw_trng_set_int_mask(uint32_t mask)
{
    trng_writel(HW_RNG_IMR_REG_ADDR,mask);
}

void rdw_trng_set_sample_count(uint32_t count)
{
    trng_writel(HW_SAMPLE_CNT1_REG_ADDR,count);
}

uint32_t read_rand_data(uint8_t datax)
{
    uint32_t rand_data = 0;
    switch(datax)
    {
    case 0:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_0_REG_ADDR);
        break;
    case 1:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_1_REG_ADDR);
        break;
    case 2:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_2_REG_ADDR);
        break;
    case 3:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_3_REG_ADDR);
        break;
    case 4:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_4_REG_ADDR);
        break;
    case 5:
        rand_data = trng_readl(HW_EHR_DATA_ADDR_5_REG_ADDR);
        break;
    default:
        break;
    }
    return rand_data;
}

void rdw_trng_set_source_enable(uint8_t config)
{
    trng_writel(HW_RND_SOURCE_ENABLE_REG_ADDR,config);
}

void rdw_trng_reset(void)
{
    trng_writel(HW_TRNG_SW_RESET_REG_ADDR,0x1);
}

void rdw_trng_reset_bits_counter(void)
{
    trng_writel(HW_RST_BITS_COUNTER_REG_ADDR,0x1);
}


uint32_t rdw_trng_rand(uint32_t rand[6])
{
//	uint32_t retry = 0;
    uint32_t rand_data = 0;
    while(rdw_trng_read_busy());
    rdw_trng_set_config(SHORTEST_INVERTER_CHAIN_LENGTH);
    rdw_trng_set_sample_count(200);
    rdw_trng_set_source_enable(RND_SRC_EN);
    while(!(rdw_trng_read_valid()&EHR_VALID))
    {
//		retry++;
//		if(retry == 50000)																					//2ms
//		{
//			printf("Time out!\n");
//			rdw_trng_set_source_enable(RND_SRC_DISABLE);
//			rdw_trng_reset();
//			rdw_trng_reset_bits_counter();
//			return rand_data;
//		}
    }
    rand_data = read_rand_data(0);
//		rand[0] = rand_data;
//		rand[1] = read_rand_data(1);
//		rand[2] = read_rand_data(2);
//		rand[3] = read_rand_data(3);
//		rand[4] = read_rand_data(4);
//		rand[5] = read_rand_data(5);
//	rdw_trng_clear_int_status(EHR_VALID_INT);
    rdw_trng_set_source_enable(RND_SRC_DISABLE);
    return rand_data;
}
#endif

#endif













