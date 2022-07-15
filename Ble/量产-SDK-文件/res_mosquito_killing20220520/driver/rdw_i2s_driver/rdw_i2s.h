//liujian add 
//2018/6/29

#ifndef __RDW_I2S_H___
#define __RDW_I2S_H___

#include "hw_i2s.h"
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

void rdw_init_i2s(void);

//void rdw_set_i2s_bits_per_word(uint32_t bits_per_word);

//void rdw_set_i2s_control(uint16_t ctl);

//uint16_t rdw_get_i2s_status(void);

//uint32_t rdw_read_i2s_data(void);

//void rdw_write_i2s_data(uint32_t data);


void cfg_txrx_common_mode_en(bool en);
	
void rdw_i2s_rx_init(uint8_t mode,uint8_t width,uint8_t sampling_point,bool dma_en,bool isMaster);

void cfg_txrx_common_mode_start(void);

void rdw_i2s_rx_disable(bool isMaster);	

#ifdef __cplusplus
}
#endif

#endif 
