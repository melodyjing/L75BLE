#ifndef _REG_IPC_EMB_H_
#define _REG_IPC_EMB_H_

#include <stdint.h>
#include "_reg_ipc_emb.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_IPC_EMB_COUNT 17

#define REG_IPC_EMB_DECODING_MASK 0x0000007F

/**
 * @brief EMB2APP_TRIGGER register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      EMB2APP_TRIGGER   0x0
 * </pre>
 */
#define IPC_EMB2APP_TRIGGER_ADDR   0x40112100
#define IPC_EMB2APP_TRIGGER_OFFSET 0x00000000
#define IPC_EMB2APP_TRIGGER_INDEX  0x00000000
#define IPC_EMB2APP_TRIGGER_RESET  0x00000000

__INLINE uint32_t ipc_emb2app_trigger_get(void)
{
    return REG_PL_RD(IPC_EMB2APP_TRIGGER_ADDR);
}

__INLINE void ipc_emb2app_trigger_set(uint32_t value)
{
    REG_PL_WR(IPC_EMB2APP_TRIGGER_ADDR, value);
}

// field definitions
#define IPC_EMB2APP_TRIGGER_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_EMB2APP_TRIGGER_LSB    0
#define IPC_EMB2APP_TRIGGER_WIDTH  ((uint32_t)0x00000020)

#define IPC_EMB2APP_TRIGGER_RST    0x0

__INLINE uint32_t ipc_emb2app_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB2APP_TRIGGER_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_emb2app_trigger_setf(uint32_t emb2apptrigger)
{
    ASSERT_ERR((((uint32_t)emb2apptrigger << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_EMB2APP_TRIGGER_ADDR, (uint32_t)emb2apptrigger << 0);
}

/**
 * @brief APP2EMB_RAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00    APP2EMB_RAWSTATUS   0x0
 * </pre>
 */
#define IPC_APP2EMB_RAWSTATUS_ADDR   0x40112104
#define IPC_APP2EMB_RAWSTATUS_OFFSET 0x00000004
#define IPC_APP2EMB_RAWSTATUS_INDEX  0x00000001
#define IPC_APP2EMB_RAWSTATUS_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_rawstatus_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_RAWSTATUS_ADDR);
}

// field definitions
#define IPC_APP2EMB_RAWSTATUS_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP2EMB_RAWSTATUS_LSB    0
#define IPC_APP2EMB_RAWSTATUS_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP2EMB_RAWSTATUS_RST    0x0

__INLINE uint32_t ipc_app2emb_rawstatus_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_RAWSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief APP2EMB_ACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          APP2EMB_ACK   0x0
 * </pre>
 */
#define IPC_APP2EMB_ACK_ADDR   0x40112108
#define IPC_APP2EMB_ACK_OFFSET 0x00000008
#define IPC_APP2EMB_ACK_INDEX  0x00000002
#define IPC_APP2EMB_ACK_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_ack_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_ACK_ADDR);
}

__INLINE void ipc_app2emb_ack_clear(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_ACK_ADDR, value);
}

// field definitions
#define IPC_APP2EMB_ACK_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP2EMB_ACK_LSB    0
#define IPC_APP2EMB_ACK_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP2EMB_ACK_RST    0x0

__INLINE uint32_t ipc_app2emb_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_ACK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_app2emb_ack_clearf(uint32_t app2emback)
{
    ASSERT_ERR((((uint32_t)app2emback << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_APP2EMB_ACK_ADDR, (uint32_t)app2emback << 0);
}

/**
 * @brief APP2EMB_UNMASK_SET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       APP2EMB_UNMASK   0x0
 * </pre>
 */
#define IPC_APP2EMB_UNMASK_SET_ADDR   0x4011210C
#define IPC_APP2EMB_UNMASK_SET_OFFSET 0x0000000C
#define IPC_APP2EMB_UNMASK_SET_INDEX  0x00000003
#define IPC_APP2EMB_UNMASK_SET_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_unmask_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_UNMASK_SET_ADDR);
}

__INLINE void ipc_app2emb_unmask_set(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_UNMASK_SET_ADDR, value);
}

// field definitions
#define IPC_APP2EMB_UNMASK_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP2EMB_UNMASK_LSB    0
#define IPC_APP2EMB_UNMASK_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP2EMB_UNMASK_RST    0x0

__INLINE uint32_t ipc_app2emb_unmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_UNMASK_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_app2emb_unmask_setf(uint32_t app2embunmask)
{
    ASSERT_ERR((((uint32_t)app2embunmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_APP2EMB_UNMASK_SET_ADDR, (uint32_t)app2embunmask << 0);
}

/**
 * @brief APP2EMB_UNMASK_CLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       APP2EMB_UNMASK   0x0
 * </pre>
 */
#define IPC_APP2EMB_UNMASK_CLEAR_ADDR   0x40112110
#define IPC_APP2EMB_UNMASK_CLEAR_OFFSET 0x00000010
#define IPC_APP2EMB_UNMASK_CLEAR_INDEX  0x00000004
#define IPC_APP2EMB_UNMASK_CLEAR_RESET  0x00000000

__INLINE void ipc_app2emb_unmask_clear(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_UNMASK_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
__INLINE void ipc_app2emb_unmask_clearf(uint32_t app2embunmask)
{
    ASSERT_ERR((((uint32_t)app2embunmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_APP2EMB_UNMASK_CLEAR_ADDR, (uint32_t)app2embunmask << 0);
}

/**
 * @brief APP2EMB_LINE_SEL_LOW register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:30        APP2EMB15_SEL   0x0
 *  29:28        APP2EMB14_SEL   0x0
 *  27:26        APP2EMB13_SEL   0x0
 *  25:24        APP2EMB12_SEL   0x0
 *  23:22        APP2EMB11_SEL   0x0
 *  21:20        APP2EMB10_SEL   0x0
 *  19:18         APP2EMB9_SEL   0x0
 *  17:16         APP2EMB8_SEL   0x0
 *  15:14         APP2EMB7_SEL   0x0
 *  13:12         APP2EMB6_SEL   0x0
 *  11:10         APP2EMB5_SEL   0x0
 *  09:08         APP2EMB4_SEL   0x0
 *  07:06         APP2EMB3_SEL   0x0
 *  05:04         APP2EMB2_SEL   0x0
 *  03:02         APP2EMB1_SEL   0x0
 *  01:00         APP2EMB0_SEL   0x0
 * </pre>
 */
#define IPC_APP2EMB_LINE_SEL_LOW_ADDR   0x40112114
#define IPC_APP2EMB_LINE_SEL_LOW_OFFSET 0x00000014
#define IPC_APP2EMB_LINE_SEL_LOW_INDEX  0x00000005
#define IPC_APP2EMB_LINE_SEL_LOW_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_line_sel_low_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
}

__INLINE void ipc_app2emb_line_sel_low_set(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, value);
}

// field definitions
#define IPC_APP2EMB15_SEL_MASK   ((uint32_t)0xC0000000)
#define IPC_APP2EMB15_SEL_LSB    30
#define IPC_APP2EMB15_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB14_SEL_MASK   ((uint32_t)0x30000000)
#define IPC_APP2EMB14_SEL_LSB    28
#define IPC_APP2EMB14_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB13_SEL_MASK   ((uint32_t)0x0C000000)
#define IPC_APP2EMB13_SEL_LSB    26
#define IPC_APP2EMB13_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB12_SEL_MASK   ((uint32_t)0x03000000)
#define IPC_APP2EMB12_SEL_LSB    24
#define IPC_APP2EMB12_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB11_SEL_MASK   ((uint32_t)0x00C00000)
#define IPC_APP2EMB11_SEL_LSB    22
#define IPC_APP2EMB11_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB10_SEL_MASK   ((uint32_t)0x00300000)
#define IPC_APP2EMB10_SEL_LSB    20
#define IPC_APP2EMB10_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB9_SEL_MASK    ((uint32_t)0x000C0000)
#define IPC_APP2EMB9_SEL_LSB     18
#define IPC_APP2EMB9_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB8_SEL_MASK    ((uint32_t)0x00030000)
#define IPC_APP2EMB8_SEL_LSB     16
#define IPC_APP2EMB8_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB7_SEL_MASK    ((uint32_t)0x0000C000)
#define IPC_APP2EMB7_SEL_LSB     14
#define IPC_APP2EMB7_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB6_SEL_MASK    ((uint32_t)0x00003000)
#define IPC_APP2EMB6_SEL_LSB     12
#define IPC_APP2EMB6_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB5_SEL_MASK    ((uint32_t)0x00000C00)
#define IPC_APP2EMB5_SEL_LSB     10
#define IPC_APP2EMB5_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB4_SEL_MASK    ((uint32_t)0x00000300)
#define IPC_APP2EMB4_SEL_LSB     8
#define IPC_APP2EMB4_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB3_SEL_MASK    ((uint32_t)0x000000C0)
#define IPC_APP2EMB3_SEL_LSB     6
#define IPC_APP2EMB3_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB2_SEL_MASK    ((uint32_t)0x00000030)
#define IPC_APP2EMB2_SEL_LSB     4
#define IPC_APP2EMB2_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB1_SEL_MASK    ((uint32_t)0x0000000C)
#define IPC_APP2EMB1_SEL_LSB     2
#define IPC_APP2EMB1_SEL_WIDTH   ((uint32_t)0x00000002)
#define IPC_APP2EMB0_SEL_MASK    ((uint32_t)0x00000003)
#define IPC_APP2EMB0_SEL_LSB     0
#define IPC_APP2EMB0_SEL_WIDTH   ((uint32_t)0x00000002)

#define IPC_APP2EMB15_SEL_RST    0x0
#define IPC_APP2EMB14_SEL_RST    0x0
#define IPC_APP2EMB13_SEL_RST    0x0
#define IPC_APP2EMB12_SEL_RST    0x0
#define IPC_APP2EMB11_SEL_RST    0x0
#define IPC_APP2EMB10_SEL_RST    0x0
#define IPC_APP2EMB9_SEL_RST     0x0
#define IPC_APP2EMB8_SEL_RST     0x0
#define IPC_APP2EMB7_SEL_RST     0x0
#define IPC_APP2EMB6_SEL_RST     0x0
#define IPC_APP2EMB5_SEL_RST     0x0
#define IPC_APP2EMB4_SEL_RST     0x0
#define IPC_APP2EMB3_SEL_RST     0x0
#define IPC_APP2EMB2_SEL_RST     0x0
#define IPC_APP2EMB1_SEL_RST     0x0
#define IPC_APP2EMB0_SEL_RST     0x0

__INLINE void ipc_app2emb_line_sel_low_pack(uint8_t app2emb15sel, uint8_t app2emb14sel, uint8_t app2emb13sel, uint8_t app2emb12sel, uint8_t app2emb11sel, uint8_t app2emb10sel, uint8_t app2emb9sel, uint8_t app2emb8sel, uint8_t app2emb7sel, uint8_t app2emb6sel, uint8_t app2emb5sel, uint8_t app2emb4sel, uint8_t app2emb3sel, uint8_t app2emb2sel, uint8_t app2emb1sel, uint8_t app2emb0sel)
{
    ASSERT_ERR((((uint32_t)app2emb15sel << 30) & ~((uint32_t)0xC0000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb14sel << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb13sel << 26) & ~((uint32_t)0x0C000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb12sel << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb11sel << 22) & ~((uint32_t)0x00C00000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb10sel << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb9sel << 18) & ~((uint32_t)0x000C0000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb8sel << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb7sel << 14) & ~((uint32_t)0x0000C000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb6sel << 12) & ~((uint32_t)0x00003000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb5sel << 10) & ~((uint32_t)0x00000C00)) == 0);
    ASSERT_ERR((((uint32_t)app2emb4sel << 8) & ~((uint32_t)0x00000300)) == 0);
    ASSERT_ERR((((uint32_t)app2emb3sel << 6) & ~((uint32_t)0x000000C0)) == 0);
    ASSERT_ERR((((uint32_t)app2emb2sel << 4) & ~((uint32_t)0x00000030)) == 0);
    ASSERT_ERR((((uint32_t)app2emb1sel << 2) & ~((uint32_t)0x0000000C)) == 0);
    ASSERT_ERR((((uint32_t)app2emb0sel << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR,  ((uint32_t)app2emb15sel << 30) | ((uint32_t)app2emb14sel << 28) | ((uint32_t)app2emb13sel << 26) | ((uint32_t)app2emb12sel << 24) | ((uint32_t)app2emb11sel << 22) | ((uint32_t)app2emb10sel << 20) | ((uint32_t)app2emb9sel << 18) | ((uint32_t)app2emb8sel << 16) | ((uint32_t)app2emb7sel << 14) | ((uint32_t)app2emb6sel << 12) | ((uint32_t)app2emb5sel << 10) | ((uint32_t)app2emb4sel << 8) | ((uint32_t)app2emb3sel << 6) | ((uint32_t)app2emb2sel << 4) | ((uint32_t)app2emb1sel << 2) | ((uint32_t)app2emb0sel << 0));
}

__INLINE void ipc_app2emb_line_sel_low_unpack(uint8_t* app2emb15sel, uint8_t* app2emb14sel, uint8_t* app2emb13sel, uint8_t* app2emb12sel, uint8_t* app2emb11sel, uint8_t* app2emb10sel, uint8_t* app2emb9sel, uint8_t* app2emb8sel, uint8_t* app2emb7sel, uint8_t* app2emb6sel, uint8_t* app2emb5sel, uint8_t* app2emb4sel, uint8_t* app2emb3sel, uint8_t* app2emb2sel, uint8_t* app2emb1sel, uint8_t* app2emb0sel)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);

    *app2emb15sel = (localVal & ((uint32_t)0xC0000000)) >> 30;
    *app2emb14sel = (localVal & ((uint32_t)0x30000000)) >> 28;
    *app2emb13sel = (localVal & ((uint32_t)0x0C000000)) >> 26;
    *app2emb12sel = (localVal & ((uint32_t)0x03000000)) >> 24;
    *app2emb11sel = (localVal & ((uint32_t)0x00C00000)) >> 22;
    *app2emb10sel = (localVal & ((uint32_t)0x00300000)) >> 20;
    *app2emb9sel = (localVal & ((uint32_t)0x000C0000)) >> 18;
    *app2emb8sel = (localVal & ((uint32_t)0x00030000)) >> 16;
    *app2emb7sel = (localVal & ((uint32_t)0x0000C000)) >> 14;
    *app2emb6sel = (localVal & ((uint32_t)0x00003000)) >> 12;
    *app2emb5sel = (localVal & ((uint32_t)0x00000C00)) >> 10;
    *app2emb4sel = (localVal & ((uint32_t)0x00000300)) >> 8;
    *app2emb3sel = (localVal & ((uint32_t)0x000000C0)) >> 6;
    *app2emb2sel = (localVal & ((uint32_t)0x00000030)) >> 4;
    *app2emb1sel = (localVal & ((uint32_t)0x0000000C)) >> 2;
    *app2emb0sel = (localVal & ((uint32_t)0x00000003)) >> 0;
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb15_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0xC0000000)) >> 30);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb15_sel_setf(uint8_t app2emb15sel)
{
    ASSERT_ERR((((uint32_t)app2emb15sel << 30) & ~((uint32_t)0xC0000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0xC0000000)) | ((uint32_t)app2emb15sel << 30));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb14_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb14_sel_setf(uint8_t app2emb14sel)
{
    ASSERT_ERR((((uint32_t)app2emb14sel << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)app2emb14sel << 28));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb13_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x0C000000)) >> 26);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb13_sel_setf(uint8_t app2emb13sel)
{
    ASSERT_ERR((((uint32_t)app2emb13sel << 26) & ~((uint32_t)0x0C000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x0C000000)) | ((uint32_t)app2emb13sel << 26));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb12_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb12_sel_setf(uint8_t app2emb12sel)
{
    ASSERT_ERR((((uint32_t)app2emb12sel << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)app2emb12sel << 24));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb11_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00C00000)) >> 22);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb11_sel_setf(uint8_t app2emb11sel)
{
    ASSERT_ERR((((uint32_t)app2emb11sel << 22) & ~((uint32_t)0x00C00000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00C00000)) | ((uint32_t)app2emb11sel << 22));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb10_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb10_sel_setf(uint8_t app2emb10sel)
{
    ASSERT_ERR((((uint32_t)app2emb10sel << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)app2emb10sel << 20));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb9_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x000C0000)) >> 18);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb9_sel_setf(uint8_t app2emb9sel)
{
    ASSERT_ERR((((uint32_t)app2emb9sel << 18) & ~((uint32_t)0x000C0000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x000C0000)) | ((uint32_t)app2emb9sel << 18));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb8_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb8_sel_setf(uint8_t app2emb8sel)
{
    ASSERT_ERR((((uint32_t)app2emb8sel << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)app2emb8sel << 16));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb7_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x0000C000)) >> 14);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb7_sel_setf(uint8_t app2emb7sel)
{
    ASSERT_ERR((((uint32_t)app2emb7sel << 14) & ~((uint32_t)0x0000C000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x0000C000)) | ((uint32_t)app2emb7sel << 14));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb6_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb6_sel_setf(uint8_t app2emb6sel)
{
    ASSERT_ERR((((uint32_t)app2emb6sel << 12) & ~((uint32_t)0x00003000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00003000)) | ((uint32_t)app2emb6sel << 12));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb5_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00000C00)) >> 10);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb5_sel_setf(uint8_t app2emb5sel)
{
    ASSERT_ERR((((uint32_t)app2emb5sel << 10) & ~((uint32_t)0x00000C00)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00000C00)) | ((uint32_t)app2emb5sel << 10));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb4_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb4_sel_setf(uint8_t app2emb4sel)
{
    ASSERT_ERR((((uint32_t)app2emb4sel << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00000300)) | ((uint32_t)app2emb4sel << 8));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb3_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x000000C0)) >> 6);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb3_sel_setf(uint8_t app2emb3sel)
{
    ASSERT_ERR((((uint32_t)app2emb3sel << 6) & ~((uint32_t)0x000000C0)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x000000C0)) | ((uint32_t)app2emb3sel << 6));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb2_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb2_sel_setf(uint8_t app2emb2sel)
{
    ASSERT_ERR((((uint32_t)app2emb2sel << 4) & ~((uint32_t)0x00000030)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00000030)) | ((uint32_t)app2emb2sel << 4));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb1_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x0000000C)) >> 2);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb1_sel_setf(uint8_t app2emb1sel)
{
    ASSERT_ERR((((uint32_t)app2emb1sel << 2) & ~((uint32_t)0x0000000C)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x0000000C)) | ((uint32_t)app2emb1sel << 2));
}

__INLINE uint8_t ipc_app2emb_line_sel_low_app2emb0_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

__INLINE void ipc_app2emb_line_sel_low_app2emb0_sel_setf(uint8_t app2emb0sel)
{
    ASSERT_ERR((((uint32_t)app2emb0sel << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_LOW_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_LOW_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)app2emb0sel << 0));
}

/**
 * @brief APP2EMB_LINE_SEL_HIGH register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:30        APP2EMB31_SEL   0x0
 *  29:28        APP2EMB30_SEL   0x0
 *  27:26        APP2EMB29_SEL   0x0
 *  25:24        APP2EMB28_SEL   0x0
 *  23:22        APP2EMB27_SEL   0x0
 *  21:20        APP2EMB26_SEL   0x0
 *  19:18        APP2EMB25_SEL   0x0
 *  17:16        APP2EMB24_SEL   0x0
 *  15:14        APP2EMB23_SEL   0x0
 *  13:12        APP2EMB22_SEL   0x0
 *  11:10        APP2EMB21_SEL   0x0
 *  09:08        APP2EMB20_SEL   0x0
 *  07:06        APP2EMB19_SEL   0x0
 *  05:04        APP2EMB18_SEL   0x0
 *  03:02        APP2EMB17_SEL   0x0
 *  01:00        APP2EMB16_SEL   0x0
 * </pre>
 */
#define IPC_APP2EMB_LINE_SEL_HIGH_ADDR   0x40112118
#define IPC_APP2EMB_LINE_SEL_HIGH_OFFSET 0x00000018
#define IPC_APP2EMB_LINE_SEL_HIGH_INDEX  0x00000006
#define IPC_APP2EMB_LINE_SEL_HIGH_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_line_sel_high_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
}

__INLINE void ipc_app2emb_line_sel_high_set(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, value);
}

// field definitions
#define IPC_APP2EMB31_SEL_MASK   ((uint32_t)0xC0000000)
#define IPC_APP2EMB31_SEL_LSB    30
#define IPC_APP2EMB31_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB30_SEL_MASK   ((uint32_t)0x30000000)
#define IPC_APP2EMB30_SEL_LSB    28
#define IPC_APP2EMB30_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB29_SEL_MASK   ((uint32_t)0x0C000000)
#define IPC_APP2EMB29_SEL_LSB    26
#define IPC_APP2EMB29_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB28_SEL_MASK   ((uint32_t)0x03000000)
#define IPC_APP2EMB28_SEL_LSB    24
#define IPC_APP2EMB28_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB27_SEL_MASK   ((uint32_t)0x00C00000)
#define IPC_APP2EMB27_SEL_LSB    22
#define IPC_APP2EMB27_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB26_SEL_MASK   ((uint32_t)0x00300000)
#define IPC_APP2EMB26_SEL_LSB    20
#define IPC_APP2EMB26_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB25_SEL_MASK   ((uint32_t)0x000C0000)
#define IPC_APP2EMB25_SEL_LSB    18
#define IPC_APP2EMB25_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB24_SEL_MASK   ((uint32_t)0x00030000)
#define IPC_APP2EMB24_SEL_LSB    16
#define IPC_APP2EMB24_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB23_SEL_MASK   ((uint32_t)0x0000C000)
#define IPC_APP2EMB23_SEL_LSB    14
#define IPC_APP2EMB23_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB22_SEL_MASK   ((uint32_t)0x00003000)
#define IPC_APP2EMB22_SEL_LSB    12
#define IPC_APP2EMB22_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB21_SEL_MASK   ((uint32_t)0x00000C00)
#define IPC_APP2EMB21_SEL_LSB    10
#define IPC_APP2EMB21_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB20_SEL_MASK   ((uint32_t)0x00000300)
#define IPC_APP2EMB20_SEL_LSB    8
#define IPC_APP2EMB20_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB19_SEL_MASK   ((uint32_t)0x000000C0)
#define IPC_APP2EMB19_SEL_LSB    6
#define IPC_APP2EMB19_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB18_SEL_MASK   ((uint32_t)0x00000030)
#define IPC_APP2EMB18_SEL_LSB    4
#define IPC_APP2EMB18_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB17_SEL_MASK   ((uint32_t)0x0000000C)
#define IPC_APP2EMB17_SEL_LSB    2
#define IPC_APP2EMB17_SEL_WIDTH  ((uint32_t)0x00000002)
#define IPC_APP2EMB16_SEL_MASK   ((uint32_t)0x00000003)
#define IPC_APP2EMB16_SEL_LSB    0
#define IPC_APP2EMB16_SEL_WIDTH  ((uint32_t)0x00000002)

#define IPC_APP2EMB31_SEL_RST    0x0
#define IPC_APP2EMB30_SEL_RST    0x0
#define IPC_APP2EMB29_SEL_RST    0x0
#define IPC_APP2EMB28_SEL_RST    0x0
#define IPC_APP2EMB27_SEL_RST    0x0
#define IPC_APP2EMB26_SEL_RST    0x0
#define IPC_APP2EMB25_SEL_RST    0x0
#define IPC_APP2EMB24_SEL_RST    0x0
#define IPC_APP2EMB23_SEL_RST    0x0
#define IPC_APP2EMB22_SEL_RST    0x0
#define IPC_APP2EMB21_SEL_RST    0x0
#define IPC_APP2EMB20_SEL_RST    0x0
#define IPC_APP2EMB19_SEL_RST    0x0
#define IPC_APP2EMB18_SEL_RST    0x0
#define IPC_APP2EMB17_SEL_RST    0x0
#define IPC_APP2EMB16_SEL_RST    0x0

__INLINE void ipc_app2emb_line_sel_high_pack(uint8_t app2emb31sel, uint8_t app2emb30sel, uint8_t app2emb29sel, uint8_t app2emb28sel, uint8_t app2emb27sel, uint8_t app2emb26sel, uint8_t app2emb25sel, uint8_t app2emb24sel, uint8_t app2emb23sel, uint8_t app2emb22sel, uint8_t app2emb21sel, uint8_t app2emb20sel, uint8_t app2emb19sel, uint8_t app2emb18sel, uint8_t app2emb17sel, uint8_t app2emb16sel)
{
    ASSERT_ERR((((uint32_t)app2emb31sel << 30) & ~((uint32_t)0xC0000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb30sel << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb29sel << 26) & ~((uint32_t)0x0C000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb28sel << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb27sel << 22) & ~((uint32_t)0x00C00000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb26sel << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb25sel << 18) & ~((uint32_t)0x000C0000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb24sel << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb23sel << 14) & ~((uint32_t)0x0000C000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb22sel << 12) & ~((uint32_t)0x00003000)) == 0);
    ASSERT_ERR((((uint32_t)app2emb21sel << 10) & ~((uint32_t)0x00000C00)) == 0);
    ASSERT_ERR((((uint32_t)app2emb20sel << 8) & ~((uint32_t)0x00000300)) == 0);
    ASSERT_ERR((((uint32_t)app2emb19sel << 6) & ~((uint32_t)0x000000C0)) == 0);
    ASSERT_ERR((((uint32_t)app2emb18sel << 4) & ~((uint32_t)0x00000030)) == 0);
    ASSERT_ERR((((uint32_t)app2emb17sel << 2) & ~((uint32_t)0x0000000C)) == 0);
    ASSERT_ERR((((uint32_t)app2emb16sel << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR,  ((uint32_t)app2emb31sel << 30) | ((uint32_t)app2emb30sel << 28) | ((uint32_t)app2emb29sel << 26) | ((uint32_t)app2emb28sel << 24) | ((uint32_t)app2emb27sel << 22) | ((uint32_t)app2emb26sel << 20) | ((uint32_t)app2emb25sel << 18) | ((uint32_t)app2emb24sel << 16) | ((uint32_t)app2emb23sel << 14) | ((uint32_t)app2emb22sel << 12) | ((uint32_t)app2emb21sel << 10) | ((uint32_t)app2emb20sel << 8) | ((uint32_t)app2emb19sel << 6) | ((uint32_t)app2emb18sel << 4) | ((uint32_t)app2emb17sel << 2) | ((uint32_t)app2emb16sel << 0));
}

__INLINE void ipc_app2emb_line_sel_high_unpack(uint8_t* app2emb31sel, uint8_t* app2emb30sel, uint8_t* app2emb29sel, uint8_t* app2emb28sel, uint8_t* app2emb27sel, uint8_t* app2emb26sel, uint8_t* app2emb25sel, uint8_t* app2emb24sel, uint8_t* app2emb23sel, uint8_t* app2emb22sel, uint8_t* app2emb21sel, uint8_t* app2emb20sel, uint8_t* app2emb19sel, uint8_t* app2emb18sel, uint8_t* app2emb17sel, uint8_t* app2emb16sel)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);

    *app2emb31sel = (localVal & ((uint32_t)0xC0000000)) >> 30;
    *app2emb30sel = (localVal & ((uint32_t)0x30000000)) >> 28;
    *app2emb29sel = (localVal & ((uint32_t)0x0C000000)) >> 26;
    *app2emb28sel = (localVal & ((uint32_t)0x03000000)) >> 24;
    *app2emb27sel = (localVal & ((uint32_t)0x00C00000)) >> 22;
    *app2emb26sel = (localVal & ((uint32_t)0x00300000)) >> 20;
    *app2emb25sel = (localVal & ((uint32_t)0x000C0000)) >> 18;
    *app2emb24sel = (localVal & ((uint32_t)0x00030000)) >> 16;
    *app2emb23sel = (localVal & ((uint32_t)0x0000C000)) >> 14;
    *app2emb22sel = (localVal & ((uint32_t)0x00003000)) >> 12;
    *app2emb21sel = (localVal & ((uint32_t)0x00000C00)) >> 10;
    *app2emb20sel = (localVal & ((uint32_t)0x00000300)) >> 8;
    *app2emb19sel = (localVal & ((uint32_t)0x000000C0)) >> 6;
    *app2emb18sel = (localVal & ((uint32_t)0x00000030)) >> 4;
    *app2emb17sel = (localVal & ((uint32_t)0x0000000C)) >> 2;
    *app2emb16sel = (localVal & ((uint32_t)0x00000003)) >> 0;
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb31_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0xC0000000)) >> 30);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb31_sel_setf(uint8_t app2emb31sel)
{
    ASSERT_ERR((((uint32_t)app2emb31sel << 30) & ~((uint32_t)0xC0000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0xC0000000)) | ((uint32_t)app2emb31sel << 30));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb30_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb30_sel_setf(uint8_t app2emb30sel)
{
    ASSERT_ERR((((uint32_t)app2emb30sel << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)app2emb30sel << 28));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb29_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x0C000000)) >> 26);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb29_sel_setf(uint8_t app2emb29sel)
{
    ASSERT_ERR((((uint32_t)app2emb29sel << 26) & ~((uint32_t)0x0C000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x0C000000)) | ((uint32_t)app2emb29sel << 26));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb28_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb28_sel_setf(uint8_t app2emb28sel)
{
    ASSERT_ERR((((uint32_t)app2emb28sel << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)app2emb28sel << 24));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb27_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00C00000)) >> 22);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb27_sel_setf(uint8_t app2emb27sel)
{
    ASSERT_ERR((((uint32_t)app2emb27sel << 22) & ~((uint32_t)0x00C00000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00C00000)) | ((uint32_t)app2emb27sel << 22));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb26_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb26_sel_setf(uint8_t app2emb26sel)
{
    ASSERT_ERR((((uint32_t)app2emb26sel << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)app2emb26sel << 20));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb25_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x000C0000)) >> 18);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb25_sel_setf(uint8_t app2emb25sel)
{
    ASSERT_ERR((((uint32_t)app2emb25sel << 18) & ~((uint32_t)0x000C0000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x000C0000)) | ((uint32_t)app2emb25sel << 18));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb24_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb24_sel_setf(uint8_t app2emb24sel)
{
    ASSERT_ERR((((uint32_t)app2emb24sel << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)app2emb24sel << 16));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb23_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x0000C000)) >> 14);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb23_sel_setf(uint8_t app2emb23sel)
{
    ASSERT_ERR((((uint32_t)app2emb23sel << 14) & ~((uint32_t)0x0000C000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x0000C000)) | ((uint32_t)app2emb23sel << 14));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb22_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb22_sel_setf(uint8_t app2emb22sel)
{
    ASSERT_ERR((((uint32_t)app2emb22sel << 12) & ~((uint32_t)0x00003000)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00003000)) | ((uint32_t)app2emb22sel << 12));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb21_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00000C00)) >> 10);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb21_sel_setf(uint8_t app2emb21sel)
{
    ASSERT_ERR((((uint32_t)app2emb21sel << 10) & ~((uint32_t)0x00000C00)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00000C00)) | ((uint32_t)app2emb21sel << 10));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb20_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb20_sel_setf(uint8_t app2emb20sel)
{
    ASSERT_ERR((((uint32_t)app2emb20sel << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00000300)) | ((uint32_t)app2emb20sel << 8));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb19_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x000000C0)) >> 6);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb19_sel_setf(uint8_t app2emb19sel)
{
    ASSERT_ERR((((uint32_t)app2emb19sel << 6) & ~((uint32_t)0x000000C0)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x000000C0)) | ((uint32_t)app2emb19sel << 6));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb18_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb18_sel_setf(uint8_t app2emb18sel)
{
    ASSERT_ERR((((uint32_t)app2emb18sel << 4) & ~((uint32_t)0x00000030)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00000030)) | ((uint32_t)app2emb18sel << 4));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb17_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x0000000C)) >> 2);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb17_sel_setf(uint8_t app2emb17sel)
{
    ASSERT_ERR((((uint32_t)app2emb17sel << 2) & ~((uint32_t)0x0000000C)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x0000000C)) | ((uint32_t)app2emb17sel << 2));
}

__INLINE uint8_t ipc_app2emb_line_sel_high_app2emb16_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

__INLINE void ipc_app2emb_line_sel_high_app2emb16_sel_setf(uint8_t app2emb16sel)
{
    ASSERT_ERR((((uint32_t)app2emb16sel << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(IPC_APP2EMB_LINE_SEL_HIGH_ADDR, (REG_PL_RD(IPC_APP2EMB_LINE_SEL_HIGH_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)app2emb16sel << 0));
}

/**
 * @brief APP2EMB_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       APP2EMB_STATUS   0x0
 * </pre>
 */
#define IPC_APP2EMB_STATUS_ADDR   0x4011211C
#define IPC_APP2EMB_STATUS_OFFSET 0x0000001C
#define IPC_APP2EMB_STATUS_INDEX  0x00000007
#define IPC_APP2EMB_STATUS_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_status_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_STATUS_ADDR);
}

// field definitions
#define IPC_APP2EMB_STATUS_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP2EMB_STATUS_LSB    0
#define IPC_APP2EMB_STATUS_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP2EMB_STATUS_RST    0x0

__INLINE uint32_t ipc_app2emb_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_STATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief EMB_SIGNATURE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            SIGNATURE   0x49504332
 * </pre>
 */
#define IPC_EMB_SIGNATURE_ADDR   0x40112140
#define IPC_EMB_SIGNATURE_OFFSET 0x00000040
#define IPC_EMB_SIGNATURE_INDEX  0x00000010
#define IPC_EMB_SIGNATURE_RESET  0x49504332

__INLINE uint32_t ipc_emb_signature_get(void)
{
    return REG_PL_RD(IPC_EMB_SIGNATURE_ADDR);
}

// field definitions
#define IPC_SIGNATURE_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_SIGNATURE_LSB    0
#define IPC_SIGNATURE_WIDTH  ((uint32_t)0x00000020)

#define IPC_SIGNATURE_RST    0x49504332

__INLINE uint32_t ipc_emb_signature_signature_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB_SIGNATURE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}


#endif // _REG_IPC_EMB_H_

