/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_cortex_m4.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/5

	Description :  used for cortex m4 interrupt,
				   and we need cmis core_m4.h 
				   so please include the file , may be at the 
		
	
	Changed Log	:

		Liujian  2019/8/5 		- Creation


*****************************************************************************/

#ifndef __COBRA_CORTEX_M4_H__
#define __COBRA_CORTEX_M4_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if MCU_RUN_SYSTEM == MCU_M4

/**
 * @brief Cobra M4  Interrupt Number Definition, according to the selected device 
 *        in @ref Library_configuration_section 
 */
typedef enum
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

	AON_EVENT0_IRQn					= 0,   //aon_event0
	
	PAD_TRIGGER_SINGLE_IRQ				 	= 1,							//pad_trigger_single
	
	INT_RTMT0_WAKEUP_IRQ					= 2, 					// int_rmt0_wakeup
	
	BUS_EVENT1_IRQn					= 3,				//bus_event1
	
	SPIF_INT_IRQn						= 4,	       //spif_int_n
	
	SHA_INTERRUPT_IRQn			= 5,				//sha_interrupt_n
	
	I2SPCM0_IRQn						= 6, 				//i2spcm0_int
	
	I2SPCM1_IRQn						= 7,				//i2spcm1_int
	
	DMA_ERR_IRQn						= 8,				//dma_err
	
	PDM0_INT_IRQn						= 9,				//pdm0_int
	
	UART0_IRQn							= 10,				//int_uart0
	
	RMT0_RXD_SYNC_IRQ							= 11,				//rmt0_rxd_sync 
	
	MPWM_IRQn								= 12,				//int_mpwm
	
	SPWM_IRQn								= 13,				//int_spwm
	
	SPI0_IRQn								= 14, 			//int_spi0
	
	EFC_IRQn								= 15,				//efc_int   EFlash interrupt
	
	VA_VOICE_AWKAE_IRQ					= 16,				//va_voice_awake           
	
	AON_EVENT3_IRQn					= 17,				//aon_event3               
	
	AON_EVENT4_IRQn					= 18,				//aon_event4               
	
	I2C0_IRQn								= 19,				//int_i2c0
	
	I2C1_IRQn								= 20,				//int_i2c1
	
	RMT0_IRQ							= 21,				//int_rmt0                 
	
	AES0_IRQn								= 22,				//int_aes0
	
	TRNG_IRQn								= 23,				//int_trng
	
	TIMER0_IRQn							= 24,				//int_timer0
	
	TIMER1_IRQn							= 25,				//int_timer1
	
	DUALTIMER2_IRQn					= 26,				//int_dualtimer2
	
	AON_EVENT5_IRQn							= 27,				//aon_event5
	
	SPI1_IRQn								= 28,				//int_spi1
	
	ECCMICRO0_IRQn					= 29,				//int_eccmicro0
	
	QDEC0_IRQn							= 30,				//qdec0_int
	
	BT_IRQn									= 31,				//Bluethooth interrupt, bt_sirq
	
	UART1_IRQn						= 32,				//int_uart1                
	
	DMADONE_IRQn					= 33,				//dmadone_int
	
	VIMS_CACHE_IRQn				= 34,				//Cache interrupt
	
	CSEM_IRQ_TXFIFO				= 35,				//icytrx_irq_4
	
	CSEM_IRQ_RXFIFO				= 36,				//icytrx_irq_5
	
	CSEM_IRQ_TX						= 37,				//icytrx_irq_0
	
	CSEM_IRQ_RXSTOP				= 38,				//icytrx_irq_1
	
	CSEM_IRQ_RECEIVED			= 39,				//icytrx_irq_2
	
	CSEM_IRQ_SYNC					= 40				//icytrx_irq_3
	

}IRQn_Type;	
	
	
	

/* ----------------Configuration of the Cortex-M4 Processor and Core Peripherals---------------- */
#define __CM4_REV                 0x0001            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               3            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT                  1            /*!< FPU present or not    */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm4.h"

void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
void NVIC_SetDeepSleep(void);
void NVIC_Systick_enable(unsigned int en);
void NVIC_Systick_set_value(unsigned int value);
void NVIC_Systick_interrupt_enable(unsigned int en);
void NVIC_Systick_system_clock_enable(unsigned int en);
unsigned int NVIC_Systick_get_current_value(void);



#endif

#ifdef __cplusplus
}
#endif
#endif


