/**
 ****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_

#ifndef __arm__
#error "File only included with RVDS!"
#endif // __arm__

#include <stdint.h>
#include "arch.h"
#include "ARMCM4.h"

#if 0
/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
#ifndef CFG_CPU_CORTEX_M1
#define GLOBAL_INT_START()                                                  \
do {                                                                        \
    uint32_t __l_cpsr_tmp;                                                  \
    __asm("                                                                 \
        MRS     __l_cpsr_tmp, CPSR;                                         \
        BIC     __l_cpsr_tmp, __l_cpsr_tmp, #0x80;                          \
        MSR     CPSR_cxsf, __l_cpsr_tmp;                                    \
    ");                                                                     \
} while(0)
#else
#define GLOBAL_INT_START()
#endif

/** @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 */
#ifndef CFG_CPU_CORTEX_M1
#define GLOBAL_INT_STOP()                                                   \
do {                                                                        \
    uint32_t __l_cpsr_tmp;                                                  \
    __asm("                                                                 \
        MRS     __l_cpsr_tmp, CPSR;                                         \
        ORR     __l_cpsr_tmp, __l_cpsr_tmp, #0x80;                          \
        MSR     CPSR_cxsf, __l_cpsr_tmp;                                    \
    ");                                                                     \
} while(0)
#else
#define GLOBAL_INT_STOP()
#endif


/** @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */

#ifndef CFG_CPU_CORTEX_M1
#define GLOBAL_INT_DISABLE()                                                \
do {                                                                        \
    uint32_t __l_cpsr_tmp;                                                  \
    uint32_t __l_irq_rest;                                                  \
    __asm("                                                                 \
        MRS     __l_cpsr_tmp, CPSR;                                         \
        AND     __l_irq_rest, __l_cpsr_tmp, #0x80;                          \
        ORR     __l_cpsr_tmp, __l_cpsr_tmp, #0x80;                          \
        MSR     CPSR_cxsf, __l_cpsr_tmp;                                    \
    ")
#else
#define GLOBAL_INT_DISABLE()
#endif



/** @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 */

#ifndef CFG_CPU_CORTEX_M1
#define GLOBAL_INT_RESTORE()                                                \
    __asm("                                                                 \
        MRS     __l_cpsr_tmp, CPSR;                                         \
        BIC     __l_cpsr_tmp, __l_cpsr_tmp, #0x80;                          \
        ORR     __l_cpsr_tmp, __l_cpsr_tmp, __l_irq_rest;                   \
        MSR     CPSR_cxsf, __l_cpsr_tmp;                                    \
    ");                                                                     \
} while(0)
#else
#define GLOBAL_INT_RESTORE()
#endif

/** @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *
 */
#define WFI()                                                                            \
do {                                                                                     \
    GLOBAL_INT_DISABLE();                                                                \
    while(intc_irqstatus_get() == 0);                                                    \
    GLOBAL_INT_RESTORE();                                                                \
} while (0)

#endif

//liujian add 2017/12/6

#define GLOBAL_INT_START()  __enable_irq();
#define GLOBAL_INT_STOP()		__disable_irq();



#define GLOBAL_INT_DISABLE()										\
		uint32_t  volatile __l_irq_rest = __get_PRIMASK();  	\
		__set_PRIMASK(1); 													

#define GLOBAL_INT_RESTORE()										\
		if(__l_irq_rest == 0)    										\
		{																						\
				__set_PRIMASK(0);												\
		}																						\
		else																				\
		{																						\
				__set_PRIMASK(1);												\
		}																						
		
//} while(0)




#define WFI() __WFI(); 
#define WFE()	__WFE();

#define SEV() __SEV();		




#endif // LL_H_
