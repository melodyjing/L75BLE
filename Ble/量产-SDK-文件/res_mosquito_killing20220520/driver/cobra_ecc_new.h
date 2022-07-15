#ifndef _ECC_H
#define _ECC_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cobra_config.h>
#if (COBRA_ECC_NEW_EN)

#define RDW_ECC_ARRAY(x,i) x


#define RDW_ECC_KEYSIZE_MAX             521                                     /**< Maximum key size in bits. Parameterised hardware may be less but not more */
#define RDW_ECC_KEYSIZE32_MAX           ((RDW_ECC_KEYSIZE_MAX+31)/32)           /**< Maximum key size in rounded up 32-bit words */

/** Masks */
#define RDW_ECC_CNTRL_START_MASK        (0x1 << 0)                              /**< Start field mask */
#define RDW_ECC_CNTRL_OP_MASK           (0xF << 1)                              /**< Operation field mask */
#define RDW_ECC_CNTRL_IE_MASK           (0x1 << 5)                              /**< Interrupt enable field mask */
#define RDW_ECC_CNTRL_SE_MASK           (0x1 << 6)                              /**< Enable secret key field mask */
#define RDW_ECC_STAT_DONE_MASK          (0x1 << 0)                              /**< Op complete (done) field mask */
#define RDW_ECC_STAT_BUSY_MASK          (0x1 << 1)                              /**< Busy field mask */
#define RDW_ECC_STAT_VERF_PASS_MASK     (0x1 << 2)                              /**< ECDSA Verification pass */
#define RDW_ECC_STAT_SIGN_RTRY_MASK     (0x1 << 3)                              /**< ECDSA Sign retry */
#define RDW_ECC_STAT_PKV_PASS_MASK      (0x1 << 4)                              /**< PKV Validation pass */

/** Operations */
#define RDW_ECC_OP_ECSM                 (0x0)
#define RDW_ECC_OP_ECA                  (0x1)
#define RDW_ECC_OP_ECD                  (0x2)
#define RDW_ECC_OP_MA                   (0x3)
#define RDW_ECC_OP_MS                   (0x4)
#define RDW_ECC_OP_MM                   (0x5)
#define RDW_ECC_OP_MD                   (0x6)
#define RDW_ECC_OP_MI                   (0x7)
#define RDW_ECC_OP_SIGN                 (0x8)
#define RDW_ECC_OP_VERF                 (0x9)
#define RDW_ECC_OP_PKV                  (0xA)

/** Field values */
#define RDW_ECC_START                   (0x1 << 0)
#define RDW_ECC_IE                      (0x1 << 5)
#define RDW_ECC_SE                      (0x1 << 6)
#define RDW_ECC_DONE                    (0x1 << 0)
#define RDW_ECC_BUSY                    (0x1 << 1)
#define RDW_ECDSA_VPASS                 (0x1 << 2)
#define RDW_ECDSA_SRTRY                 (0x1 << 3)
#define RDW_ECDSA_PKVPASS               (0x1 << 4)

#define RDW_ECC_INF_32                  0xFFFFFFFFUL                            // Assume infinity is all F's upto key_size

/** eSi-ECC registers in hardware. */
typedef struct rdw_ecc
{
    __attribute__ ((aligned (4))) unsigned int control;                /**< Control fields  */
    __attribute__ ((aligned (4))) unsigned int status;                 /**< Status fields */
    __attribute__ ((aligned (4))) unsigned int px;                     /**< EC base point x coordinate or operand */
    __attribute__ ((aligned (4))) unsigned int py;                     /**< EC base point y coordinate or operand */
    __attribute__ ((aligned (4))) unsigned int key;                    /**< Private key */
    __attribute__ ((aligned (4))) unsigned int ca;                     /**< EC "a" parameter */
    __attribute__ ((aligned (4))) unsigned int p;                      /**< EC "prime" or "order" depending on operation */
    __attribute__ ((aligned (4))) unsigned int rx;                     /**< Result x coordinate */
    __attribute__ ((aligned (4))) unsigned int ry;                     /**< Result y coordinate */
    __attribute__ ((aligned (4))) unsigned int sx;                     /**< Second operand in ECA x coordinate */
    __attribute__ ((aligned (4))) unsigned int sy;                     /**< Second operand in ECA y coordinate */
    __attribute__ ((aligned (4))) unsigned int m;                      /**< Hashed message digest for EC DSA */
} RDW_ECC_T;

/** ECC context
 * ecc          - Holds a pointer to the ECC hardware base address
 * key_size     - Run-time key size being operated on
 * key_size_max - Maximum key size the hardware was parameterised with,
 *                this may be less than RDW_ECC_KEY_SIZE_MAX which is the
 *                maximum for any hardware parameterisation. The loading and
 *                unloading functions need to know this information and its not
 *                captured in the ECC registers.
 */
typedef struct rdw_ecc_ctx
{
    volatile RDW_ECC_T *ecc;                                                             /**< Pointer to the ECC hardware base address */
    unsigned int key_size;                                                      /**< Run-time key size in 32-bit words */
    unsigned int key_size_max;                                                  /**< Hardware parameterised maximum key size in 32-bit words */
} RDW_ECC_CTX_T;

extern
int rdw_eccmicro_ecsm(RDW_ECC_CTX_T *ctx,
                      const unsigned int *gx,
                      const unsigned int *gy,
                      const unsigned int *k,
                      const unsigned int *modulus,
                      const unsigned int *ca,
                      unsigned int *rx,
                      unsigned int *ry);
extern
int rdw_eccmicro_eca(RDW_ECC_CTX_T *ctx,
                     const unsigned int *px,
                     const unsigned int *py,
                     const unsigned int *qx,
                     const unsigned int *qy,
                     const unsigned int *modulus,
                     unsigned int  write_mod,
                     unsigned int *rx,
                     unsigned int *ry);
extern
int rdw_eccmicro_ecd(RDW_ECC_CTX_T *ctx,
                     const unsigned int *px,
                     const unsigned int *py,
                     const unsigned int *modulus,
                     unsigned int  write_mod,
                     const unsigned int *ca,
                     unsigned int *rx,
                     unsigned int *ry);
extern
int rdw_ecc_sign(RDW_ECC_CTX_T *ctx,
                 const unsigned int *Gx,
                 const unsigned int *Gy,
                 const unsigned int *A,
                 const unsigned int *P,
                 const unsigned int *Q,
                 const unsigned int *hash,
                 const unsigned int *k,
                 const unsigned int *private_key,
                 unsigned int *r,
                 unsigned int *s);
extern
int rdw_eccmicro_sign(RDW_ECC_CTX_T *ctx,
                      const unsigned int *Gx,
                      const unsigned int *Gy,
                      const unsigned int *A,
                      const unsigned int *P,
                      const unsigned int *Q,
                      const unsigned int *hash,
                      const unsigned int *k,
                      const unsigned int *private_key,
                      unsigned int *r,
                      unsigned int *s);
extern
int rdw_eccmicro_verf(RDW_ECC_CTX_T *ctx,
                      const unsigned int *Gx,
                      const unsigned int *Gy,
                      const unsigned int *A,
                      const unsigned int *P,
                      const unsigned int *Q,
                      const unsigned int *hash,
                      const unsigned int *r,
                      const unsigned int *s,
                      const unsigned int *pubkey_x,
                      const unsigned int *pubkey_y);
extern
int rdw_eccmicro_pkv(RDW_ECC_CTX_T *ctx,
                     const unsigned int *Px,
                     const unsigned int *Py,
                     const unsigned int *modulus,
                     const unsigned int *A,
                     const unsigned int *B);
extern void
rdw_ecc_wait(RDW_ECC_CTX_T *ctx);

/** Start ECC
 * Start the specified operation
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_start(volatile RDW_ECC_T *ecc)
{
    ecc->control |= RDW_ECC_START;
}

/** Enable secret key
 * Key comes from internal secret
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_enable_secret_key(volatile RDW_ECC_T *ecc)
{
    ecc->control |= RDW_ECC_SE;
}

/** Disable secret key
 * Key comes from register map
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_disable_secret_key(volatile RDW_ECC_T *ecc)
{
    ecc->control &= ~RDW_ECC_SE;
}

/** Enable interrupt
 * Enable interrupts
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_enable_interrupt(volatile RDW_ECC_T *ecc)
{
    ecc->control |= RDW_ECC_IE;
}

/** Disable interrupt
 * Disable interrupts
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_disable_interrupt(volatile RDW_ECC_T *ecc)
{
    ecc->control &= ~RDW_ECC_IE;
}

/** Set operation
 * Set the specified operation
 * @param ecc - Pointer to the ECC registers
 * @param op  - Operation to perform
 */
static inline void
rdw_ecc_operation(volatile RDW_ECC_T *ecc, unsigned int op)
{
    ecc->control = (ecc->control & ~RDW_ECC_CNTRL_OP_MASK) | (op << 1);
}

/** Returns the ECC busy status flag
 * @param ecc  - Pointer to the ECC registers
 * @return int - true if busy
 */
static inline int
rdw_ecc_is_busy(volatile RDW_ECC_T *ecc)
{
    return ((ecc->status & RDW_ECC_STAT_BUSY_MASK) == RDW_ECC_BUSY);
}


/** Returns the ECDSA Verification pass bit in the status register
 * @param ecc  - Pointer to the ECC registers
 * @return int - true if busy
 */
static inline int
rdw_ecdsa_verf_pass(volatile RDW_ECC_T *ecc)
{
    return ((ecc->status & RDW_ECC_STAT_VERF_PASS_MASK) >> 2);
}


/** Returns the ECDSA Sign retry bit in the status register
 * @param ecc  - Pointer to the ECC registers
 * @return int - true if busy
 */
static inline int
rdw_ecdsa_sign_retry(volatile RDW_ECC_T *ecc)
{
    return ((ecc->status & RDW_ECC_STAT_SIGN_RTRY_MASK) >> 3);
}


/** Returns the PKV validation pass bit in the status register
 * @param ecc  - Pointer to the ECC registers
 * @return int - true if busy
 */
static inline int
rdw_ecdsa_pkv_pass(volatile RDW_ECC_T *ecc)
{
    return ((ecc->status & RDW_ECC_STAT_PKV_PASS_MASK) >> 4);
}

/** Write 1 to Status bit 0 (OP_COMPLETE) to clear it.
 * @param ecc - Pointer to the ECC registers
 */
//static inline void
//rdw_ecc_op_complete_clear(volatile RDW_ECC_T *ecc)
//{
//    ecc->status &= ~RDW_ECC_DONE;
//}


/** clear Status bit 2 (ECDSA Verification pass)
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_verfpass_clear(volatile RDW_ECC_T *ecc)
{
    ecc->status &= ~RDW_ECDSA_VPASS;
}

/** clear Status bit 3 (ECDSA Sign Retry)
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_signrtry_clear(volatile RDW_ECC_T *ecc)
{
    ecc->status &= ~RDW_ECDSA_SRTRY;
}

/** clear Status bit 4 (PKV pass)
 * @param ecc - Pointer to the ECC registers
 */
static inline void
rdw_ecc_pkvpass_clear(volatile RDW_ECC_T *ecc)
{
    ecc->status &= ~RDW_ECDSA_PKVPASS;
}


/** Wait until ECC core has processed the block
 * @param ecc  - Pointer to the ECC registers.
 */
static inline void
rdw_ecc_wait_complete(volatile RDW_ECC_T *ecc)
{
    while (rdw_ecc_is_busy(ecc));
}

/** Write to the px registers
 * @param ctx          - Pointer to the ECC context.
 * @param px           - Pointer to the px array
 */
static inline void
rdw_ecc_write_px(RDW_ECC_CTX_T *ctx, const unsigned int *px)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->px, i) = px[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->px, i) = 0;
}

/** Write to the py registers
 * @param ctx          - Pointer to the ECC context.
 * @param py           - Pointer to the py array
 */
static inline void
rdw_ecc_write_py(RDW_ECC_CTX_T *ctx, const unsigned int *py)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->py, i) = py[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->py, i) = 0;
}

/** Write to the key registers
 * @param ctx          - Pointer to the ECC context.
 * @param key          - Pointer to the key array
 */
static inline void
rdw_ecc_write_key(RDW_ECC_CTX_T *ctx, const unsigned int *key)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->key, i) = key[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->key, i) = 0;
}

/** Write to the ca registers
 * @param ctx          - Pointer to the ECC context.
 * @param ca           - Pointer to the ca array
 */
static inline void
rdw_ecc_write_ca(RDW_ECC_CTX_T *ctx, const unsigned int *ca)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->ca, i) = ca[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->ca, i) = 0;
}

/** Write to the p registers
 * @param ctx          - Pointer to the ECC context.
 * @param p            - Pointer to the p array
 */
static inline void
rdw_ecc_write_p(RDW_ECC_CTX_T *ctx, const unsigned int *p)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->p, i) = p[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->p, i) = 0;
}

/** Write to the rx registers
 * @param ctx          - Pointer to the ECC context.
 * @param rx           - Pointer to the rx array
 */
static inline void
rdw_ecc_write_rx(RDW_ECC_CTX_T *ctx, const unsigned int *rx)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->rx, i) = rx[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->rx, i) = 0;
}

/** Write to the ry registers
 * @param ctx          - Pointer to the ECC context.
 * @param ry           - Pointer to the ry array
 */
static inline void
rdw_ecc_write_ry(RDW_ECC_CTX_T *ctx, const unsigned int *ry)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->ry, i) = ry[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->ry, i) = 0;
}

/** Write to the sx registers
 * @param ctx          - Pointer to the ECC context.
 * @param sx           - Pointer to the sx array
 */
static inline void
rdw_ecc_write_sx(RDW_ECC_CTX_T *ctx, const unsigned int *sx)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->sx, i) = sx[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->sx, i) = 0;
}

/** Write to the sy registers
 * @param ctx          - Pointer to the ECC context.
 * @param sy           - Pointer to the sy array
 */
static inline void
rdw_ecc_write_sy(RDW_ECC_CTX_T *ctx, const unsigned int *sy)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->sy, i) = sy[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->sy, i) = 0;
}

/** Write to the m registers
 * @param ctx          - Pointer to the ECC context.
 * @param m            - Pointer to the m array
 */
static inline void
rdw_ecc_write_m(RDW_ECC_CTX_T *ctx, const unsigned int *m)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        RDW_ECC_ARRAY(ctx->ecc->m, i) = m[i];

    for (; i < ctx->key_size_max; i++)
        RDW_ECC_ARRAY(ctx->ecc->m, i) = 0;
}

/** Read the px registers
 * @param ctx          - Pointer to the ECC context.
 * @param px           - Pointer to the px array
 */
static inline void
rdw_ecc_read_px(RDW_ECC_CTX_T *ctx, unsigned int *px)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        px[i] = RDW_ECC_ARRAY(ctx->ecc->px, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->px, i);
        (void)dummy;
    }
}

/** Read the py registers
 * @param ctx          - Pointer to the ECC context.
 * @param py           - Pointer to the py array
 */
static inline void
rdw_ecc_read_py(RDW_ECC_CTX_T *ctx, unsigned int *py)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        py[i] = RDW_ECC_ARRAY(ctx->ecc->py, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->py, i);
        (void)dummy;
    }
}

/** Read the key registers
 * @param ctx          - Pointer to the ECC context.
 * @param key          - Pointer to the key array
 */
static inline void
rdw_ecc_read_key(RDW_ECC_CTX_T *ctx, unsigned int *key)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        key[i] = RDW_ECC_ARRAY(ctx->ecc->key, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->key, i);
        (void)dummy;
    }
}

/** Read the ca registers
 * @param ctx          - Pointer to the ECC context.
 * @param ca           - Pointer to the ca array
 */
static inline void
rdw_ecc_read_ca(RDW_ECC_CTX_T *ctx, unsigned int *ca)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        ca[i] = RDW_ECC_ARRAY(ctx->ecc->ca, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->ca, i);
        (void)dummy;
    }
}

/** Read the p registers
 * @param ctx          - Pointer to the ECC context.
 * @param p            - Pointer to the p array
 */
static inline void
rdw_ecc_read_p(RDW_ECC_CTX_T *ctx, unsigned int *p)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        p[i] = RDW_ECC_ARRAY(ctx->ecc->p, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->p, i);
        (void)dummy;
    }
}

/** Read the rx registers
 * @param ctx          - Pointer to the ECC context.
 * @param rx           - Pointer to the rx array
 */
static inline void
rdw_ecc_read_rx(RDW_ECC_CTX_T *ctx, unsigned int *rx)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        rx[i] = RDW_ECC_ARRAY(ctx->ecc->rx, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->rx, i);
        (void)dummy;
    }
}

/** Read the ry registers
 * @param ctx          - Pointer to the ECC context.
 * @param ry           - Pointer to the ry array
 */
static inline void
rdw_ecc_read_ry(RDW_ECC_CTX_T *ctx, unsigned int *ry)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        ry[i] = RDW_ECC_ARRAY(ctx->ecc->ry, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->ry, i);
        (void)dummy;
    }
}

/** Read the sx registers
 * @param ctx          - Pointer to the ECC context.
 * @param sx           - Pointer to the sx array
 */
static inline void
rdw_ecc_read_sx(RDW_ECC_CTX_T *ctx, unsigned int *sx)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        sx[i] = RDW_ECC_ARRAY(ctx->ecc->sx, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->sx, i);
        (void)dummy;
    }
}

/** Read the sy registers
 * @param ctx          - Pointer to the ECC context.
 * @param sy           - Pointer to the sy array
 */
static inline void
rdw_ecc_read_sy(RDW_ECC_CTX_T *ctx, unsigned int *sy)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        sy[i] = RDW_ECC_ARRAY(ctx->ecc->sy, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->sy, i);
        (void)dummy;
    }
}

/** Read the m registers
 * @param ctx          - Pointer to the ECC context.
 * @param m            - Pointer to the m array
 */
static inline void
rdw_ecc_read_m(RDW_ECC_CTX_T *ctx, unsigned int *m)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        m[i] = RDW_ECC_ARRAY(ctx->ecc->m, i);

    for (; i < ctx->key_size_max; i++) {
        dummy = RDW_ECC_ARRAY(ctx->ecc->m, i);
        (void)dummy;
    }
}




#define ECC_BASE				(0x400C0000UL + 0x2000UL)
#define RDW_ECC         ((volatile RDW_ECC_T   *) ECC_BASE )


int ecc_test (void);
#endif
#endif
