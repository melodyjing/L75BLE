#ifndef _REG_IPC_APP_H_
#define _REG_IPC_APP_H_

#include <stdint.h>
#include "_reg_ipc_app.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_IPC_APP_COUNT 17

#define REG_IPC_APP_DECODING_MASK 0x0000007F

/**
 * @brief APP2EMB_TRIGGER register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      APP2EMB_TRIGGER   0x0
 * </pre>
 */
#define IPC_APP2EMB_TRIGGER_ADDR   0x40112000
#define IPC_APP2EMB_TRIGGER_OFFSET 0x00000000
#define IPC_APP2EMB_TRIGGER_INDEX  0x00000000
#define IPC_APP2EMB_TRIGGER_RESET  0x00000000

__INLINE uint32_t ipc_app2emb_trigger_get(void)
{
    return REG_PL_RD(IPC_APP2EMB_TRIGGER_ADDR);
}

__INLINE void ipc_app2emb_trigger_set(uint32_t value)
{
    REG_PL_WR(IPC_APP2EMB_TRIGGER_ADDR, value);
}

// field definitions
#define IPC_APP2EMB_TRIGGER_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP2EMB_TRIGGER_LSB    0
#define IPC_APP2EMB_TRIGGER_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP2EMB_TRIGGER_RST    0x0

__INLINE uint32_t ipc_app2emb_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP2EMB_TRIGGER_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_app2emb_trigger_setf(uint32_t app2embtrigger)
{
    ASSERT_ERR((((uint32_t)app2embtrigger << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_APP2EMB_TRIGGER_ADDR, (uint32_t)app2embtrigger << 0);
}

/**
 * @brief EMB2APP_RAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00    EMB2APP_RAWSTATUS   0x0
 * </pre>
 */
#define IPC_EMB2APP_RAWSTATUS_ADDR   0x40112004
#define IPC_EMB2APP_RAWSTATUS_OFFSET 0x00000004
#define IPC_EMB2APP_RAWSTATUS_INDEX  0x00000001
#define IPC_EMB2APP_RAWSTATUS_RESET  0x00000000

__INLINE uint32_t ipc_emb2app_rawstatus_get(void)
{
    return REG_PL_RD(IPC_EMB2APP_RAWSTATUS_ADDR);
}

// field definitions
#define IPC_EMB2APP_RAWSTATUS_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_EMB2APP_RAWSTATUS_LSB    0
#define IPC_EMB2APP_RAWSTATUS_WIDTH  ((uint32_t)0x00000020)

#define IPC_EMB2APP_RAWSTATUS_RST    0x0

__INLINE uint32_t ipc_emb2app_rawstatus_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB2APP_RAWSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief EMB2APP_ACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          EMB2APP_ACK   0x0
 * </pre>
 */
#define IPC_EMB2APP_ACK_ADDR   0x40112008
#define IPC_EMB2APP_ACK_OFFSET 0x00000008
#define IPC_EMB2APP_ACK_INDEX  0x00000002
#define IPC_EMB2APP_ACK_RESET  0x00000000

__INLINE uint32_t ipc_emb2app_ack_get(void)
{
    return REG_PL_RD(IPC_EMB2APP_ACK_ADDR);
}

__INLINE void ipc_emb2app_ack_clear(uint32_t value)
{
    REG_PL_WR(IPC_EMB2APP_ACK_ADDR, value);
}

// field definitions
#define IPC_EMB2APP_ACK_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_EMB2APP_ACK_LSB    0
#define IPC_EMB2APP_ACK_WIDTH  ((uint32_t)0x00000020)

#define IPC_EMB2APP_ACK_RST    0x0

__INLINE uint32_t ipc_emb2app_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB2APP_ACK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_emb2app_ack_clearf(uint32_t emb2appack)
{
    ASSERT_ERR((((uint32_t)emb2appack << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_EMB2APP_ACK_ADDR, (uint32_t)emb2appack << 0);
}

/**
 * @brief EMB2APP_UNMASK_SET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_UNMASK   0x0
 * </pre>
 */
#define IPC_EMB2APP_UNMASK_SET_ADDR   0x4011200C
#define IPC_EMB2APP_UNMASK_SET_OFFSET 0x0000000C
#define IPC_EMB2APP_UNMASK_SET_INDEX  0x00000003
#define IPC_EMB2APP_UNMASK_SET_RESET  0x00000000

__INLINE uint32_t ipc_emb2app_unmask_get(void)
{
    return REG_PL_RD(IPC_EMB2APP_UNMASK_SET_ADDR);
}

__INLINE void ipc_emb2app_unmask_set(uint32_t value)
{
    REG_PL_WR(IPC_EMB2APP_UNMASK_SET_ADDR, value);
}

// field definitions
#define IPC_EMB2APP_UNMASK_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_EMB2APP_UNMASK_LSB    0
#define IPC_EMB2APP_UNMASK_WIDTH  ((uint32_t)0x00000020)

#define IPC_EMB2APP_UNMASK_RST    0x0

__INLINE uint32_t ipc_emb2app_unmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB2APP_UNMASK_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ipc_emb2app_unmask_setf(uint32_t emb2appunmask)
{
    ASSERT_ERR((((uint32_t)emb2appunmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_EMB2APP_UNMASK_SET_ADDR, (uint32_t)emb2appunmask << 0);
}

/**
 * @brief EMB2APP_UNMASK_CLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_UNMASK   0x0
 * </pre>
 */
#define IPC_EMB2APP_UNMASK_CLEAR_ADDR   0x40112010
#define IPC_EMB2APP_UNMASK_CLEAR_OFFSET 0x00000010
#define IPC_EMB2APP_UNMASK_CLEAR_INDEX  0x00000004
#define IPC_EMB2APP_UNMASK_CLEAR_RESET  0x00000000

__INLINE void ipc_emb2app_unmask_clear(uint32_t value)
{
    REG_PL_WR(IPC_EMB2APP_UNMASK_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
__INLINE void ipc_emb2app_unmask_clearf(uint32_t emb2appunmask)
{
    ASSERT_ERR((((uint32_t)emb2appunmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(IPC_EMB2APP_UNMASK_CLEAR_ADDR, (uint32_t)emb2appunmask << 0);
}

/**
 * @brief EMB2APP_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       EMB2APP_STATUS   0x0
 * </pre>
 */
#define IPC_EMB2APP_STATUS_ADDR   0x4011201C
#define IPC_EMB2APP_STATUS_OFFSET 0x0000001C
#define IPC_EMB2APP_STATUS_INDEX  0x00000007
#define IPC_EMB2APP_STATUS_RESET  0x00000000

__INLINE uint32_t ipc_emb2app_status_get(void)
{
    return REG_PL_RD(IPC_EMB2APP_STATUS_ADDR);
}

// field definitions
#define IPC_EMB2APP_STATUS_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_EMB2APP_STATUS_LSB    0
#define IPC_EMB2APP_STATUS_WIDTH  ((uint32_t)0x00000020)

#define IPC_EMB2APP_STATUS_RST    0x0

__INLINE uint32_t ipc_emb2app_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_EMB2APP_STATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief APP_SIGNATURE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00        APP_SIGNATURE   0x49504332
 * </pre>
 */
#define IPC_APP_SIGNATURE_ADDR   0x40112040
#define IPC_APP_SIGNATURE_OFFSET 0x00000040
#define IPC_APP_SIGNATURE_INDEX  0x00000010
#define IPC_APP_SIGNATURE_RESET  0x49504332

__INLINE uint32_t ipc_app_signature_get(void)
{
    return REG_PL_RD(IPC_APP_SIGNATURE_ADDR);
}

// field definitions
#define IPC_APP_SIGNATURE_MASK   ((uint32_t)0xFFFFFFFF)
#define IPC_APP_SIGNATURE_LSB    0
#define IPC_APP_SIGNATURE_WIDTH  ((uint32_t)0x00000020)

#define IPC_APP_SIGNATURE_RST    0x49504332

__INLINE uint32_t ipc_app_signature_getf(void)
{
    uint32_t localVal = REG_PL_RD(IPC_APP_SIGNATURE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}


#endif // _REG_IPC_APP_H_

