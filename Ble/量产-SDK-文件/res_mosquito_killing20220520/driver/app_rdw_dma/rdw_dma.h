//liujian add 
//2018/6/28

#ifndef __RDW_DMA_H__
#define __RDW_DMA_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DMA_CHANNEL_SEL_CONTROL_BASE		0X400E0180

#define CHANNEL0  		0
#define CHANNEL1  		1
#define CHANNEL2  		2
#define CHANNEL3  		3
#define CHANNEL4  		4
#define CHANNEL5  		5
#define CHANNEL6  		6
#define CHANNEL7  		7
#define CHANNEL8  		8
#define CHANNEL9  		9
#define CHANNEL10  		10
#define CHANNEL11  		11
#define CHANNEL12  		12
#define CHANNEL13  		13
#define CHANNEL14  		14
#define CHANNEL15  		15
#define CHANNEL16  		16
#define CHANNEL17  		17
#define CHANNEL18  		18
#define CHANNEL19  		19

#define MAX_CHANNEL_NUMBER  20

enum channel_usage
{
		CH0_SOFTWARE,
		CH0_IPC_IRQ,
		CH0_SHA_INT,
		CH0_AON_EVT0,	
		
		CH1_SOFTWARE,
		CH1_BT_SIRQ,
		CH1_GP_ADC,
		CH1_AON_EVT2,
		
		CH2_UART0_RX,
		CH2_UART10_RX,
		CH2_PDM0,
		CH2_SPIF_RX,
		
		CH3_UART0_TX,
		CH3_UART10_TX,
		CH3_PDM2PCM,
		CH3_SPIF_TX,	
	
		CH4_SPI0_RX,
		CH4_UART11_RX,
		CH4_AON_EVT0,
		CH4_DTIMER0,	
		
		CH5_SPI0_TX,
		CH5_UART11_TX,
		CH5_AON_EVT2,
		CH5_DTIMER1,	
		
		CH6_I2C0_RX,
		CH6_SHA_INT,
		CH6_AES0_RX,	
		
		CH7_I2C0_TX,
		CH7_GP_ADC,
		CH7_AES0_TX,
		
		CH8_UART1_RX,
		CH8_TIMER0,
		CH8_AON_EVT4,
		CH8_AES0_RX,
		
		CH9_UART1_TX,
		CH9_TIMER1,
		CH9_AON_EVT3,
		CH9_AES0_TX,	
		
		CH10_SPI1_RX,
		CH10_TIMER0,
		CH10_AON_EVT4,	
		
		CH11_SPI1_TX,
		CH11_TIMER1,
		CH11_AON_EVT3,	
	
		CH12_I2C1_RX,
		CH12_TIMER0,
		CH12_AON_EVT4,
		CH12_UART10_RX,
		
		CH13_I2C1_TX,
		CH13_TIMER1,
		CH13_AON_EVT3,
		CH13_UART10_TX,
		
		CH14_I2S0_RX,
		CH14_AON_EVT4,
		CH14_IPC_IRQ,
		CH14_UART11_RX,
		
		CH15_I2S0_TX,
		CH15_AON_EVT3,
		CH15_BT_SIRQ,
		CH15_UART11_TX,
		
		CH16_I2S1_RX,
		CH16_AON_EVT4,
		CH16_DTIMER0,
		
		CH17_I2S1_TX,
		CH17_AON_EVT3,
		CH17_DTIMER1,
		
		CH18_SPIF_RX,
		CH18_AON_EVT4,
		CH18_PDM0,
		
		CH19_SPIF_RX,
		CH19_AON_EVT3,
		CH19_PDM2PCM,
		
		
};

char app_rdw_udma_get_channel(char ch, uint8_t usage);

void app_rdw_udma_init(void);

void app_rdw_udma_register_callback(unsigned long ulIntChannel, void (*pfnHandler)(void));


void app_rdw_udma_unregister_callback(unsigned long ulIntChannel);




#ifdef __cplusplus
}
#endif

#endif 
