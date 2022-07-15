/**
****************************************************************************************
*
* @file rwble.c
*
* @brief RWBLE core interrupt handler
*
* Copyright (C) Radiawave 2017-2021
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"      // stack configuration

#include <string.h>           // for mem* functions
#include "co_version.h"
#include "co_math.h"
#include "rwble.h"            // BLE API definition
#include "rwip.h"             // stack main module

#include "ble_util_buf.h"     // BLE EM buffer management
#include "lld.h"              // link layer driver definition
#include "llc.h"              // link layer controller definition
#include "llm.h"              // link layer manager definition

#if (BLE_ISO_PRESENT)
#if (BLE_HW_50_ISO == 1)
#include "ble_util_isodesc.h" // BLE ISO descriptor management
#endif //(BLE_HW_50_ISO == 1)
#include "lli.h"              // Link Layer ISO definition
#endif // (BLE_ISO_PRESENT)

#include "ke_event.h"         // kernel event definition

#include "sch_arb.h"          // Scheduling Arbiter
#include "sch_prog.h"         // Scheduling Programmer

#include "dbg.h"              // debug definitions

#include "reg_blecore.h"      // BLE Core registers

#if APP_SCAN_DEVICE_ENABLE
#include "device1_scan.h"
#endif
/*
 * DEFINES
 ****************************************************************************************
 */

/// ISR mask for BT and BLE common ISR
#if (BLE_HW_50_ISO == 0)
#define BLE_COMMON_ISR_MASK   (  BLE_FINETGTIMINTSTAT_BIT   | BLE_TIMESTAMPTGTINTSTAT_BIT | BLE_GROSSTGTIMINTSTAT_BIT  \
                               | BLE_CLKNINTSTAT_BIT        | BLE_SLPINTSTAT_BIT          | BLE_CRYPTINTSTAT_BIT       \
                               | BLE_SWINTSTAT_BIT)
#else // (BLE_HW_50_ISO == 1)
#define BLE_COMMON_ISR_MASK   (  BLE_FINETGTIMINTSTAT_BIT | BLE_TIMESTAMPINTSTAT_BIT | BLE_GROSSTGTIMINTSTAT_BIT  \
                               | BLE_CLKNINTSTAT_BIT      | BLE_SLPINTSTAT_BIT       | BLE_CRYPTINTSTAT_BIT       \
                               | BLE_SWINTSTAT_BIT)
#endif // (BLE_HW_50_ISO == 0)
/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// Environment structure for RW BLE interrupt handler
struct rwble_env_tag
{
    uint32_t irq_mask;
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Environment of the RW BLE interrupt handler
#if COBRA_POWER_DOWN_ENABLE
static struct rwble_env_tag rwble_env __attribute__((section("retention_area"),zero_init));
#else
static struct rwble_env_tag rwble_env;
#endif
/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void rwble_init(uint8_t init_type)
{
    if(!init_type)
    {
        uint32_t seed;
        rwip_time_t current_time = rwip_time_get();

        seed =  current_time.hs;
        seed += current_time.hus;

        //Init the random seed
        co_random_init(seed);
    }

    // Initialize buffer management system
    ble_util_buf_init(init_type);

    #if (BLE_ISO_PRESENT && BLE_HW_50_ISO)
    // Initialize ISO descriptor management system
    ble_util_isodesc_init(init_type);
    #endif // (BLE_ISO_PRESENT && BLE_HW_50_ISO)

    // LLD block does nothing at IP INIT state
    if (init_type)
    {
        // Initialize the Link Layer Driver
        lld_init(init_type);
    }

    #if(BLE_CENTRAL || BLE_PERIPHERAL)
    // Initialize the Link Layer Controller
    llc_init(init_type);
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

    // Initialize the Link Layer Manager
    llm_init(init_type);

    #if (BLE_ISO_PRESENT)
    // Initialize the Link Layer ISO
    lli_init(init_type);
    #endif // (BLE_ISO_PRESENT)
}

#if BT_DUAL_MODE
bool rwble_activity_ongoing_check(void)
{
    // check that a BLE activity is ongoing (advertising, scan, initiating, connection)
    return llm_activity_ongoing_check();
}
#endif //BT_DUAL_MODE

#if (BLE_HW_50_ISO == 1)
__BLEIRQ void rwble_isr(void)
{
    DBG_SWDIAG(ISR, BLE, 1);

    // Loop until no more interrupts have to be handled
    while (1)
    {
        // Check BLE interrupt status and call the appropriate handlers
        uint32_t irq_stat      = ble_intstat0_get();
        // Nothing to do, immediately stop
        if (irq_stat == 0)  break;

        #if (BLE_BIS | BLE_CIS)
        // Hopping scheme computation
        if (irq_stat & BLE_HOPINTACK_BIT)
        {
            // Clear the interrupt
            ble_intack0_hopintack_clearf(1);

            // Handle end of hopping HW accelerator
            lld_iso_hop_accel_isr();
        }
        #endif // (BLE_BIS | BLE_CIS)

        // Error interrupt
        if (irq_stat & BLE_ERRORINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack0_errorintack_clearf(1);

            ASSERT_INFO(0, ble_errortypestat_get(), 0);
        }
    }

    DBG_SWDIAG(ISR, BLE, 0);
}

#else // ! (BLE_HW_50_ISO == 1)
__BLEIRQ void rwble_isr(void)
{
    DBG_SWDIAG(ISR, BLE, 1);

    // Loop until no more interrupts have to be handled
    while (1)
    {
        // Check BLE interrupt status and call the appropriate handlers
        uint32_t irq_stat      = ble_intstat_get();

        // Nothing to do, immediately stop
        if (irq_stat == 0)
            break;

        #if (!BT_DUAL_MODE)
        if ((irq_stat & BLE_COMMON_ISR_MASK) != 0)
        {
            // handle general platform interrupts
            rwip_isr();
        }
        #endif // (!BT_DUAL_MODE)

        // Error interrupt
        if (irq_stat & BLE_ERRORINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack_errorintack_clearf(1);

            ASSERT_INFO(0, ble_errortypestat_get(), 0);
        }

        // TX
        if (irq_stat & BLE_TXINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, TXINT, 1);

            // Ack Tx interrupt
            ble_intack_txintack_clearf(1);

            // Call handler
            sch_prog_tx_isr(0xFF);

            DBG_SWDIAG(BLE_ISR, TXINT, 0);
        }
        // RX
        if (irq_stat & BLE_RXINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, RXINT, 1);

            // Ack Rx interrupt
            ble_intack_rxintack_clearf(1);

            // Call handler
            sch_prog_rx_isr(0xFF);

            DBG_SWDIAG(BLE_ISR, RXINT, 0);
        }

        #if (BLE_ISO_PRESENT)
        // Audio channel 0 interrupt
        if (irq_stat & BLE_AUDIOINT0STAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, AUDIO0INT, 1);

            // Ack audio interrupt
            ble_intack_clear(BLE_AUDIOINT0STAT_BIT);

            // Handle audio IRQ
            sch_prog_audio_isr(0);

            DBG_SWDIAG(BLE_ISR, AUDIO0INT, 0);
        }
        // Audio channel 1 interrupt
        if (irq_stat & BLE_AUDIOINT1STAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, AUDIO1INT, 1);

            // Ack audio interrupt
            ble_intack_clear(BLE_AUDIOINT1STAT_BIT);

            // Handle audio IRQ
            sch_prog_audio_isr(1);

            DBG_SWDIAG(BLE_ISR, AUDIO1INT, 0);
        }
        // Audio channel 2 interrupt
        if (irq_stat & BLE_AUDIOINT2STAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, AUDIO2INT, 1);

            // Ack audio interrupt
            ble_intack_clear(BLE_AUDIOINT2STAT_BIT);

            // Handle audio IRQ
            sch_prog_audio_isr(2);

            DBG_SWDIAG(BLE_ISR, AUDIO2INT, 0);
        }
        #endif // (BLE_ISO_PRESENT)

        // Skip event
        if (irq_stat & BLE_SKIPEVTINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, SKIPINT, 1);

            // Clear the interrupt
            ble_intack_skipevtintack_clearf(1);

            // Handle skip interrupt
            sch_prog_skip_isr(0xFF);

            DBG_SWDIAG(BLE_ISR, SKIPINT, 0);
        }

        // End of event
        if (irq_stat & BLE_ENDEVTINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, EVENTINT, 1);

            // Clear the interrupt
            ble_intack_endevtintack_clearf(1);

//						app_device1_watchdog_timer_set(0);
            // Handle end of frame
            sch_prog_end_isr(0xFF);

            DBG_SWDIAG(BLE_ISR, EVENTINT, 0);
        }
    }

    DBG_SWDIAG(ISR, BLE, 0);
}
#endif // (BLE_HW_50_ISO == 1)


extern void app_deep_sleep_disable_interrupt(void);

extern void app_deep_sleep_enable_interrupt(void);

void rwble_sleep_enter(void)
{
    #if (BLE_HW_50_ISO == 1)
    // Keep currently enabled interrupts
    rwble_env.irq_mask = ble_intcntl1_get();
    // Mask all interrupts
    ble_intcntl1_set(0);
    // Clear possible pending IRQs
    ble_intack1_clear(0xFFFFFFFF);
    #else // (BLE_HW_50_ISO == 0)
    // Keep currently enabled interrupts
  //  printf("E");
	
    rwble_env.irq_mask = ble_intcntl_get();
    // Mask all interrupts
    ble_intcntl_set(0);
    // Clear possible pending IRQs
    ble_intack_clear(0xFFFFFFFF);
	
//	  printf("1 0x%x \n", rwble_env.irq_mask);
    #endif // (BLE_HW_50_ISO == 1)
}


void rwble_sleep_wakeup_end(void)
{
    // Restore enabled interrupts
    #if (BLE_HW_50_ISO == 1)
    ble_intcntl1_set(rwble_env.irq_mask);
    #else // (BLE_HW_50_ISO == 0)
    ble_intcntl_set(rwble_env.irq_mask);
//    printf("2 0x%x \n", rwble_env.irq_mask);
    #endif // (BLE_HW_50_ISO == 1)
}

///@} RWBTINT
