//liujian add 
//2018/8/6

#ifndef __RDW_PDM_H__
#define __RDW_PDM_H__


#ifdef __cplusplus
extern "C"
{
#endif
#include "pdm.h"
#include "rdw_gpio.h"
//#include "pcrm.h"

#define RDW_PDM_GPIO		GPIO_PIN_7

extern void rdw_init_pdm(void);

extern void rdw_pdm_disable(void);	
	
extern void rdw_init_sys_pdm(char pdm_type);

extern void rdw_pdm_init(uint8_t dma_en,uint8_t osr,float pclk_kHz);

extern void rdw_pdm_enable(void);
#ifdef __cplusplus
}
#endif

#endif 
