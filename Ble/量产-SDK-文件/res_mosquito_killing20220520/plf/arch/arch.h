/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup PLATFORM_DRIVERS PLATFORM_DRIVERS
 * @ingroup MACSW
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup ARCH_AT91SAM9261 ARCH_AT91SAM9261
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// required to define GLOBAL_INT_** macros as inline assembly
//#include "boot.h"
//#include "ll.h"
#include "compiler.h"
//#include "rwnx_config.h"

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE          1


#define    RESET_NO_ERROR         0x00000000
#define    RESET_MEM_ALLOC_FAIL   0xF2F2F2F2

/// Reset platform and stay in ROM
#define    RESET_TO_ROM           0xA5A5A5A5
/// Reset platform and reload FW
#define    RESET_AND_LOAD_FW      0xC3C3C3C3


/*
 * ASSERTION CHECK
 ****************************************************************************************
 */
void force_trigger(const char *msg);
void assert_rec(const char *condition, const char * file, int line);
void assert_err(const char *condition, const char * file, int line);
void assert_warn(const char *condition, const char * file, int line);
#if 0
#if (NX_DEBUG > 1)
#define ASSERT(type, cond) assert_##type(cond, __MODULE__, __LINE__)
#elif (NX_DEBUG > 0)
#define ASSERT(type, cond) assert_##type("", __MODULE__, __LINE__)
#else
#define ASSERT(type, cond) assert_##type("", "", 0)
#endif

#if NX_DEBUG
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(err, #cond);                       \
        }                                             \
    } while(0)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR2(cond, param0, param1) ASSERT_ERR(cond)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond)                             \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(warn, #cond);                      \
        }                                             \
    } while(0)
#else
#define ASSERT_ERR(cond)
#define ASSERT_ERR2(cond, param0, param1)
#define ASSERT_WARN(cond)
#endif

#if NX_RECOVERY
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)                              \
    ({                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return;                                   \
        }                                             \
    })

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)                     \
    ({                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return (ret);                             \
        }                                             \
    })

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)                       \
    ({                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
        }                                             \
    })

#else
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)             ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)    ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)      ASSERT_ERR(cond)
#endif

#endif

#if PLF_DEBUG
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            assert_err(#cond, __MODULE__, __LINE__);  \
        }                                             \
    } while(0)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)             \
    do {                                              \
        if (!(cond)) {                                \
            assert_param((int)param0, (int)param1, __MODULE__, __LINE__);  \
        }                                             \
    } while(0)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1)             \
    do {                                              \
        if (!(cond)) {                                \
            assert_warn((int)param0, (int)param1, __MODULE__, __LINE__); \
        }                                             \
    } while(0)

#define DUMP_DATA(data, length) \
    dump_data((uint8_t*)data, length)

#else
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1)

/// DUMP data array present in the SW.
#define DUMP_DATA(data, length)
#endif //PLF_DEBUG

/// Object allocated in shared memory - check linker script
#define __SHARED __attribute__ ((section("shram")))

/// Macro checking if a pointer is part of the shared RAM __data_start__
#ifdef 	 ARM_CORTEX_CM3
#define _sshram  ((volatile unsigned long *)(0x20009b8c))
#define _eshram  ((volatile unsigned long *)(0x20089b8c))
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)_eshram))
#else			
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)_eshram))
#endif
/// Macro checking if a pointer is part of the shared RAM
#define CHK_SHRAM_PTR(ptr) { if (TST_SHRAM_PTR(ptr)) return;}

#include "ll.h"
/// @}
/// @}
#endif // _ARCH_H_
