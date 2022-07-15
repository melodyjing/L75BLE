/**
 ****************************************************************************************
 *
 * @file gnuarm/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_



#if 0
#ifndef __GNUC__
#error "File only included with ARM GCC"
#endif
#endif

#undef __INLINE
/// define the force inlining attribute for this compiler
#define __INLINE static __attribute__((__always_inline__)) inline
	
#ifndef  __CC_ARM
/// define the IRQ handler attribute for this compiler
#define __IRQ __attribute__((__interrupt__("IRQ")))

/// define the FIQ handler attribute for this compiler

#define __FIQ __attribute__((__interrupt__("FIQ")))
#else
#define __IRQ
#define __FIQ

#endif
/// Function returns struct in registers (4 in rvds, var with gnuarm).
/// With Gnuarm, feature depends on command line options and
/// impacts ALL functions returning 2-words max structs
/// (check -freg-struct-return and -mabi=xxx)
#define __VIR

/// function has no side effect and return depends only on arguments
#define __PURE __attribute__((const))

/// symbol is defined weakly (silently replaced by NULL at link if not defined)
/// GCC 3.4 for ARM may not accept it, must be tested. Error meanwhile.
//#if __GNUC__ >= 4
//#define __WEAK __attribute__((weak))
//#else
//#error "check weak with gnuarm 3.4"
//#endif

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))
#define __ALIGN2 __attribute__((aligned(2)))
#define __ALIGN1 __attribute__((aligned(1)))

/// Pack a structure field
#define __PACKED16 __attribute__ ((__packed__))

#undef __PACKED
#define __PACKED __attribute__ ((__packed__))

/// __MODULE__ comes from the RVDS compiler that supports it
#ifndef  __CC_ARM
#define __MODULE__ __BASE_FILE__
#endif
// Mapping of these different elements is already handled in the map.txt file, so no need
// to define anything here
#define __SHAREDRAMIPC
#define __SHAREDRAM			
#define __LARAMMAC
#define __MIB

/// define the static keyword for this compiler
#define __STATIC static

/// define the force inlining attribute for this compiler
//#define __INLINE __forceinline static

/// define the IRQ handler attribute for this compiler
//#define __IRQ __irq

/// define the BLE IRQ handler attribute for this compiler
#define __BTIRQ

/// define the BLE IRQ handler attribute for this compiler
#define __BLEIRQ

/// define the FIQ handler attribute for this compiler
//#define __FIQ __irq

#define __ARRAY_EMPTY


#endif // _COMPILER_H_
