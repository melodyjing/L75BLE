//liujian add 
//2018/7/17
//control uMCU

#ifndef __RDW_UMCU_H___
#define __RDW_UMCU_H___
#include <stdint.h>
#include <stdbool.h>

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

//typedef struct RDW_UMCU_STRUCT {
//	__attribute__ ((aligned (4))) volatile unsigned int IRAM_CTRL;
//	
//	
//};

#define  RDW_UMCU_ADDRESS	   0x40080000 + 0x1000*3

#define  RDW_UMCU_IRAM_CTRL		RDW_UMCU_ADDRESS + 0x000
	
#define  RDW_UMCU_PAD_CTRL		RDW_UMCU_ADDRESS + 0x04
	
#define  RDW_UMCU_IRAM_KEY		RDW_UMCU_ADDRESS + 0x10
	
#define  RDW_UMCU_IRAM_INT		RDW_UMCU_ADDRESS + 0x5C
	
#define  UMCU_PROGRAM_ADDRESS	RDW_UMCU_ADDRESS + 0x800


#ifdef __cplusplus
}
#endif

#endif 
