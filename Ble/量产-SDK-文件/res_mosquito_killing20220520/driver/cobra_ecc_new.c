#include "cobra_ecc_new.h"
#if (COBRA_ECC_NEW_EN)
/** Wrapper function for eSi-ECC Scalar Multiply R = kG
 * @param ctx          - Pointer to the ECC context.
 * @param gx           - Basepoint x
 * @param gy           - Basepoint y
 * @param k            - Key
 * @param modulus      - modulus
 * @param ca           - The "a" parameter
 * @param rx           - Result x coordinate
 * @param ry           - Result y coordinate
 */
int rdw_eccmicro_ecsm(RDW_ECC_CTX_T *ctx,
                      const unsigned int *gx,
                      const unsigned int *gy,
                      const unsigned int *k,
                      const unsigned int *modulus,
                      const unsigned int *ca,
                      unsigned int *rx,
                      unsigned int *ry)
{
    int i;
    int j;             // needs to be signed to prevent counting below 0!
    unsigned ki;
    int ret1 = 0;
    volatile int ret2 = 0;

    unsigned int Q1x[RDW_ECC_KEYSIZE32_MAX];        // Temportary array storage
    unsigned int Q1y[RDW_ECC_KEYSIZE32_MAX];        // Temportary array storage
    unsigned int Q2x[RDW_ECC_KEYSIZE32_MAX];        // Temportary array storage
    unsigned int Q2y[RDW_ECC_KEYSIZE32_MAX];        // Temportary array storage
    //unsigned int temp[RDW_ECC_KEYSIZE32_MAX];       // Temportary array storage

    // Initialise Montgomery ladder variable
    for (i = 0; i < (int)ctx->key_size; i++) {
        Q1x[i] = RDW_ECC_INF_32;
        Q1y[i] = RDW_ECC_INF_32;
        Q2x[i] = gx[i];
        Q2y[i] = gy[i];
    }

    // Write modulous to the core once
    rdw_ecc_write_p(ctx, modulus);

    // Montgomery ladder
    for (i = (((int)ctx->key_size)-1); i>=0 ; i--) {
        for (j = 31; j >= 0 ; j--) {

            ki = (k[i] >> j) & 0x1;

            if (ki == 1) {
                ret1 = rdw_eccmicro_eca(ctx, Q1x, Q1y, Q2x, Q2y, modulus, 0, Q1x, Q1y);
                ret2 = rdw_eccmicro_ecd(ctx, Q2x, Q2y, modulus, 0, ca, Q2x, Q2y);
            }
            else {
                ret2 = rdw_eccmicro_eca(ctx, Q1x, Q1y, Q2x, Q2y, modulus, 0, Q2x, Q2y);
                ret1 = rdw_eccmicro_ecd(ctx, Q1x, Q1y, modulus, 0, ca, Q1x, Q1y);
            }
        }
    }

    for (i = 0; i < (int)ctx->key_size; i++) {
        rx[i] = Q1x[i];
        ry[i] = Q1y[i];
    }

    return ret1;

}

/** Wrapper function for eSi-ECCMIC Point Addition R = P + Q
 * @param ctx          - Pointer to the ECCMIC context.
 * @param px           - EC point Px
 * @param py           - EC point Py
 * @param qx           - EC point Qx
 * @param qy           - EC point Qy
 * @param modulus      - modulus
 * @param write_mod    - When Set to 1 modulus will be written to the core. Otherwise assume modulus already has been written.
 * @param rx           - Result x coordinate
 * @param ry           - Result y coordinate
 */
int rdw_eccmicro_eca(RDW_ECC_CTX_T *ctx,
                     const unsigned int *px,
                     const unsigned int *py,
                     const unsigned int *qx,
                     const unsigned int *qy,
                     const unsigned int *modulus,
                     unsigned int  write_mod,
                     unsigned int *rx,
                     unsigned int *ry)
{

    unsigned int i;
    unsigned int inf_1;
    unsigned int inf_2;
    unsigned int inf_out;
    unsigned int D[RDW_ECC_KEYSIZE32_MAX];        // Temporary array storage
    unsigned int E[RDW_ECC_KEYSIZE32_MAX];
//    unsigned int F[RDW_ECC_KEYSIZE32_MAX];

    // Initialise temporary storage with 0s
    for (i = 0; i < ctx->key_size; i++) {
        D[i] = 0;
        //  E[i] = 0;
        //  F[i] = 0;
    }

    // Check if input curve points to be added equal infinity
    inf_1 = 1;
    inf_2 = 1;
    i = 0;
    while (((inf_1 == 1) || (inf_2 == 1)) && (i<ctx->key_size)) {
        if ((px[i] != RDW_ECC_INF_32) || (py[i] != RDW_ECC_INF_32))  //detect if (Px,Py) != (INF,INF)
            inf_1 = 0;

        if ((qx[i] != RDW_ECC_INF_32) || (qy[i] != RDW_ECC_INF_32))  //detect if (Qx,Qy) != (INF,INF)
            inf_2 = 0;

        i++;
    }

    // If (Px,Py) = (INF,INF), assign (Rx,Ry) = (Qx,Qy)
    if (inf_1 == 1) {
        for (i = 0; i < ctx->key_size; i++) {
            rx[i] = qx[i];
            ry[i] = qy[i];
        }
        return 0;
    }

    // If (Qx,Qy) = (INF,INF), assign (Rx,Ry) = (Px,Py)
    if (inf_2 == 1) {
        for (i = 0; i < ctx->key_size; i++) {
            rx[i] = px[i];
            ry[i] = py[i];
        }
        return 0;
    }

    // If modulous has not been written to the core externally, write it now.
    if (write_mod == 1)
        rdw_ecc_write_p(ctx, modulus);

    //rdw_ecc_ms(ctx, D, qy, modulus, D);    // compute -qy
    rdw_ecc_write_px(ctx, D);
    rdw_ecc_write_py(ctx, qy);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, D);

    inf_out = 1;
    i = 0;
    while ((inf_out == 1) && (i<ctx->key_size)) {
        if ((px[i] != qx[i]) || (py[i] != D[i]))     //detect if (Px,Py) != (Qx,-Qy)
            inf_out = 0;

        i++;
    }

    // If (Px,Py) = (Qx,-Qy), return INF point
    if (inf_out == 1) {
        for (i = 0; i < ctx->key_size; i++) {
            rx[i] = RDW_ECC_INF_32;
            ry[i] = RDW_ECC_INF_32;
        }
        return 1;                                   // Return 1 to signal infinity
    }

    // If no special cases detected for P,Q, perform ECA routine

    // STEP 1: rdw_ecc_ms(ctx, qy, py, modulus, D);     // D = Qy - Py
    rdw_ecc_write_px(ctx, qy);
    rdw_ecc_write_py(ctx, py);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, D);

    // STEP 2: rdw_ecc_ms(ctx, qx, px, modulus, E);     // E = Qx - Px
    rdw_ecc_write_px(ctx, qx);
    rdw_ecc_write_py(ctx, px);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, E);

    /// STEP 3: rdw_ecc_md(ctx, E, D, modulus, D);       // D = D/E
    //rdw_ecc_write_px(ctx, E);
    rdw_ecc_write_py(ctx, D);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MD);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, D);

    // STEP 4: rdw_ecc_mm(ctx, D, D, modulus, E);       // E = D*D
    //rdw_ecc_write_px(ctx, D);
    //rdw_ecc_write_py(ctx, D);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, E);

    // STEP 5: rdw_ecc_ma(ctx, px, qx, modulus, F);     // F = Px + Qx
    rdw_ecc_write_px(ctx, px);
    rdw_ecc_write_py(ctx, qx);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, F);

    // STEP 6: rdw_ecc_ms(ctx, E, F, modulus, E);       // E = E-F
    rdw_ecc_write_px(ctx, E);
    //rdw_ecc_write_py(ctx, F);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, E);

    // STEP 7: rdw_ecc_ms(ctx, qx, E, modulus, F);      // F = Qx - E
    rdw_ecc_write_px(ctx, qx);
    //rdw_ecc_write_py(ctx, E);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, F);

    // STEP 8: rdw_ecc_mm(ctx, D, F, modulus, F);       // F = D*F
    rdw_ecc_write_px(ctx, D);
    //rdw_ecc_write_py(ctx, F);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, F);

    // STEP 9: rdw_ecc_ms(ctx, F, qy, modulus, F);      // F = F - Q1y
    //rdw_ecc_write_px(ctx, F);
    rdw_ecc_write_py(ctx, qy);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_py(ctx, ry);


    for (i = 0; i < ctx->key_size; i++) {
        rx[i] = E[i];
        // ry[i] = F[i];
    }

    return 0;
}

/** Wrapper function for eSi-ECC Point Doubling R = 2Q
 * @param ctx          - Pointer to the ECC context.
 * @param px           - EC point Px
 * @param py           - EC point Py
 * @param modulus      - modulus
 * @param write_mod    - When Set to 1 modulus will be written to the core. Otherwise assume modulus already has been written.
 * @param ca           - The "a" parameter
 * @param rx           - Result x coordinate
 * @param ry           - Result y coordinate
 */
int rdw_eccmicro_ecd(RDW_ECC_CTX_T *ctx,
                     const unsigned int *px,
                     const unsigned int *py,
                     const unsigned int *modulus,
                     unsigned int  write_mod,
                     const unsigned int *ca,
                     unsigned int *rx,
                     unsigned int *ry)
{
    unsigned int i;
    int inf_out;
    unsigned int A[RDW_ECC_KEYSIZE32_MAX];        // Temporary array storage
    unsigned int B[RDW_ECC_KEYSIZE32_MAX];        // Temporary array storage
//    unsigned int C[RDW_ECC_KEYSIZE32_MAX];        // Temporary array storage


    // Initialise temporary storage with 0s
    // for (i = 0; i < RDW_ECC_KEYSIZE32_MAX; i++){
    //     A[i] = 0;
    //     B[i] = 0;                     // rx = B
    //     C[i] = 0;                     // ry = C
    // }

    inf_out = 1;
    i = 0;
    while ((inf_out == 1) && (i<ctx->key_size)) {
        if ((px[i] != RDW_ECC_INF_32) || (py[i] != RDW_ECC_INF_32))     //detect if (Px,Py) != (INFx,INFy)
            inf_out = 0;

        i++;
    }

    // If (Px,Py) = (INFx,INFy), return INF point
    if (inf_out == 1) {
        for (i = 0; i < ctx->key_size; i++) {
            rx[i] = RDW_ECC_INF_32;
            ry[i] = RDW_ECC_INF_32;
        }
        return 1;                                  // Return 1 to signal infinity
    }

    // If modulus has not been written to the core externally, write it now.
    if (write_mod == 1)
        rdw_ecc_write_p(ctx, modulus);

    // If no special cases detected perform ECD routine
    // STEP 1: rdw_ecc_mm(ctx, px, px, modulus, A);  // A = Px*Px
    rdw_ecc_write_px(ctx, px);
    rdw_ecc_write_py(ctx, px);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, A);

    // STEP 2: rdw_ecc_ma(ctx, A, A, modulus, B);    // B = A + A
    //rdw_ecc_write_px(ctx, A);
    //rdw_ecc_write_py(ctx, A);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, B);

    // STEP 3: rdw_ecc_ma(ctx, B, A, modulus, A);    // A = B + A
    //rdw_ecc_write_px(ctx, B);
    rdw_ecc_write_py(ctx, A);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, A);

    // STEP 4: rdw_ecc_ma(ctx, A, ca, modulus, A);   // A = A + ca
    //rdw_ecc_write_px(ctx, A);
    rdw_ecc_write_py(ctx, ca);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, A);

    // STEP 5: rdw_ecc_ma(ctx, py, py, modulus, B);  // B = Py + Py
    rdw_ecc_write_px(ctx, py);
    rdw_ecc_write_py(ctx, py);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, B);

    // STEP 6: rdw_ecc_md(ctx, B, A, modulus, A);    // A = A/B
    //rdw_ecc_write_px(ctx, B);
    rdw_ecc_write_py(ctx, A);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MD);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, A);

    // STEP 7: rdw_ecc_mm(ctx, A, A, modulus, B);    // B = A*A
    //rdw_ecc_write_px(ctx, A);
    //rdw_ecc_write_py(ctx, A);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, B);

    // STEP 8: rdw_ecc_ma(ctx, px, px, modulus, C);  // C = Px + Px
    rdw_ecc_write_px(ctx, px);
    rdw_ecc_write_py(ctx, px);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, C);

    // STEP 9: rdw_ecc_ms(ctx, B, C, modulus, B);    // B = B - C
    rdw_ecc_write_px(ctx, B);
    //rdw_ecc_write_py(ctx, C);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, B);

    // STEP 10: rdw_ecc_ms(ctx, px, B, modulus, C);   // C = Px - B
    rdw_ecc_write_px(ctx, px);
    //rdw_ecc_write_py(ctx, B);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, C);

    // STEP 11: rdw_ecc_mm(ctx, A, C, modulus, C);    // C = A*C
    rdw_ecc_write_px(ctx, A);
    //rdw_ecc_write_py(ctx, C);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, C);

    // STEP 12: rdw_ecc_ms(ctx, C, py, modulus, C);   // C = C - PY
    //rdw_ecc_write_px(ctx, C);
    rdw_ecc_write_py(ctx, py);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_py(ctx, ry);


    for (i = 0; i < ctx->key_size; i++) {
        rx[i] = B[i];
        //     ry[i] = C[i];
    }

    return 0;

}

/** Wrapper function for eSi-ECC sign
 * @param ctx          - Pointer to the ECC context.
 * @param Gx           - Basepoint x co-ordinate
 * @param Gy           - Basepoint y co-ordinate
 * @param A            - Curve "A"
 * @param P            - Curve "prime"
 * @param Q            - Curve "order"
 * @param hash         - Hash
 * @param k            - Randomly generated key
 * @param private_key  - Secret private key
 * @param r            - Signature r component return value
 * @param s            - Signature s component return value
 * @return             - Return 0 on success, non-zero if public key validation fails
 */
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
                      unsigned int *s)
{
    unsigned int i;
    unsigned int zero_test;
    unsigned int temp[RDW_ECC_KEYSIZE32_MAX];
    unsigned int *pubkey_x = r; // use external available temporary storage
    unsigned int *pubkey_y = s; // use external available temporary storage

    /* generate a vector of all zeros in temp */
    for (i = 0; i < ctx->key_size; i++)
        temp[i] = 0;

    /* generate a public key from the randomly generated key */
    rdw_eccmicro_ecsm(ctx, Gx, Gy, k, P, A, pubkey_x, pubkey_y);

    /* r = (pubkey_x + 0) mod n */
    //rdw_ecc_ma(ctx, pubkey_x, temp, Q, r);
    rdw_ecc_write_px(ctx, pubkey_x);
    rdw_ecc_write_py(ctx, temp);
    rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, r);

    /* abort if r == 0 */
    zero_test = 0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= r[i];
    if (zero_test == 0)
        return -1;
    /* temp = 1/k */
    //rdw_ecc_mi(ctx, k, Q, temp);
    rdw_ecc_write_px(ctx, k);
    //rdw_ecc_write_py(ctx, k);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MI);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp);

    /* xr */
    //rdw_ecc_mm(ctx, r, private_key, Q, s);
    rdw_ecc_write_px(ctx, r);
    rdw_ecc_write_py(ctx, private_key);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, s);

    /* (e + xr) */
    //rdw_ecc_ma(ctx, hash, s, Q, s);
    rdw_ecc_write_px(ctx, hash);
    //rdw_ecc_write_py(ctx, s);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, s);


    /* s = (e + xr) * (1/k) */
    //rdw_ecc_mm(ctx, s, temp, Q, s);
    //rdw_ecc_write_px(ctx, s);
    rdw_ecc_write_py(ctx, temp);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, s);

    /* abort if s == 0 */
    zero_test = 0x0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= s[i];
    if (zero_test == 0)
        return -1;
    else
        return 0;
}

/** Wrapper function for eSi-ECC verify
 * @param ctx          - Pointer to the ECC context.
 * @param Gx           - Basepoint x co-ordinate
 * @param Gy           - Basepoint y co-ordinate
 * @param A            - Curve "A"
 * @param P            - Curve "prime"
 * @param Q            - Curve "order"
 * @param hash         - Hash
 * @param r            - Signature r component
 * @param s            - Signature s component
 * @param pubkey_x     - Public key x component
 * @param pubkey_y     - Public key y component
 * @return             - Return 0 on success, non-zero if signature verification fails
 */


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
                      const unsigned int *pubkey_y)
{
    unsigned int i;
    unsigned int zero_test;
    //unsigned int check_equal;
    unsigned int temp1[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp2[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp3[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp4[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp5[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp6[RDW_ECC_KEYSIZE32_MAX];

    /* generate a vector of all zeros in temp */
    for (i = 0; i < ctx->key_size; i++)
        temp1[i] = 0;

    /* abort if r == 0 */
    zero_test = 0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= r[i];
    if (zero_test == 0)
        return -1;

    /* abort if s == 0 */
    zero_test = 0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= s[i];
    if (zero_test == 0)
        return -1;

    /* (r + 0) mod  Q*/
    //rdw_ecc_ma(ctx, r, temp1, Q, temp2);
    rdw_ecc_write_px(ctx, r);
    rdw_ecc_write_py(ctx, temp1);
    rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp2);

    //check_equal = 1;
    i = 0;
    while ((i<ctx->key_size)) {
        if (temp2[i] != r[i]) {
            //check_equal = 0;
            return -1;
        }
        i++;
    }

    /* (s + 0) mod  Q*/
    //rdw_ecc_ma(ctx, s, temp1, Q, temp2);
    rdw_ecc_write_px(ctx, s);
    rdw_ecc_write_py(ctx, temp1);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp2);

    //check_equal = 1;
    i = 0;
    while ((i<ctx->key_size)) {
        if (temp2[i] != s[i]) {
            //check_equal = 0;
            return -1;
        }
        i++;
    }

    // compute temp1 = 1/s mod n
    //rdw_ecc_mi(ctx, s, Q, temp1);
    rdw_ecc_write_px(ctx, s);
    //rdw_ecc_write_py(ctx, temp1);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MI);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    // compute temp2 = 1/s * hash mod n
    //rdw_ecc_mm(ctx, temp1, hash, Q, temp2);
    //rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, hash);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp2);

    // compute temp1 = 1/s * r mod n
    //rdw_ecc_mm(ctx, temp1, r, Q, temp1);
    rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, r);
    //rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    // compute temp2*G (ECSM)
    rdw_eccmicro_ecsm(ctx, Gx, Gy, temp2, P, A, temp3, temp4);
    // compute temp1*Pubkey (ECSM)
    rdw_eccmicro_ecsm(ctx, pubkey_x, pubkey_y, temp1, P, A, temp5, temp6);
    // compute temp2*G + temp1*Pubkey
    rdw_eccmicro_eca(ctx, temp3, temp4, temp5, temp6, P, 1, temp1, temp2);

    for (i = 0; i < ctx->key_size; i++)
        temp2[i] = 0;

    /* (temp1 + 0) mod  Q*/
    //rdw_ecc_ma(ctx, temp1, temp2, Q, temp2);
    rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, temp2);
    rdw_ecc_write_p(ctx, Q);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp2);

    //check_equal = 1;
    i = 0;
    while ((i<ctx->key_size)) {
        if (temp2[i] != r[i]) {
            //check_equal = 0;
            return -1;
        }
        i++;
    }

    return 0;
}


/** Wrapper function for eSi-ECC-micro PKV
 * @param ctx          - Pointer to the ECC context.
 * @param Px           - Public Key x co-ordinate
 * @param Py           - Public Key y co-ordinate
 * @param modulus      - modulus
 * @param A            - Curve "A"
 * @param A            - Curve "B"
 */
int rdw_eccmicro_pkv(RDW_ECC_CTX_T *ctx,
                     const unsigned int *Px,
                     const unsigned int *Py,
                     const unsigned int *modulus,
                     const unsigned int *A,
                     const unsigned int *B)
{
    unsigned int i;
    unsigned int zero_test;
    unsigned int temp1[RDW_ECC_KEYSIZE32_MAX];
    unsigned int temp2[RDW_ECC_KEYSIZE32_MAX];

    /* generate a vector of all zeros in temp */
    for (i = 0; i < ctx->key_size; i++)
        temp2[i] = 0;


    // STEP 1 : Check Px is in the [0,..,p-1] range
    /* temp1 = Px + 0 mod p */
    rdw_ecc_write_px(ctx, Px);
    rdw_ecc_write_py(ctx, temp2);
    rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp1);

    /* temp1 = temp1 - Px mod p */
    //rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, Px);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    /* abort if temp1 != 0 */
    zero_test = 0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= temp1[i];
    if (zero_test != 0)
        return -1;

    // STEP 2 : Check Py is in the [0,..,p-1] range
    /* temp1 = Py + 0 mod p */
    rdw_ecc_write_px(ctx, Py);
    rdw_ecc_write_py(ctx, temp2);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp1);

    /* temp1 = temp1 - Py mod p */
    //rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, Py);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    /* abort if temp1 != 0 */
    zero_test = 0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= temp1[i];
    if (zero_test != 0)
        return -1;

    // STEP 3 : Compute Px^3 + A*Px + B   (X^3 + aX + b)
    /* temp1 = Px*Px mod p */
    rdw_ecc_write_px(ctx, Px);
    rdw_ecc_write_py(ctx, Px);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp1);

    /* temp1 = temp1 + A mod p */
    //rdw_ecc_write_px(ctx, Px);
    rdw_ecc_write_py(ctx, A);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp1);

    /* temp1 = temp1*Px mod p */
    //rdw_ecc_write_px(ctx, temp1);
    rdw_ecc_write_py(ctx, Px);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp1);

    /* temp1 = temp1 + B mod p */
    //rdw_ecc_write_px(ctx, temp2);
    rdw_ecc_write_py(ctx, B);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MA);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    // Compute Py^2
    /* temp2 = Py*Py mod p */
    rdw_ecc_write_px(ctx, Py);
    rdw_ecc_write_py(ctx, Py);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MM);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    //rdw_ecc_read_px(ctx, temp2);

    // Compute Py^2  - [ Px^3 + A*Px + B] mod p
    /* temp1 = temp1 - temp2 mod p */
    rdw_ecc_write_px(ctx, temp1);
    //rdw_ecc_write_py(ctx, temp2);
    //rdw_ecc_write_p(ctx, modulus);
    rdw_ecc_operation(ctx->ecc, RDW_ECC_OP_MS);
    rdw_ecc_start(ctx->ecc);
    rdw_ecc_wait(ctx);
    rdw_ecc_read_px(ctx, temp1);

    /* Check result is 0 else PKV fails */
    zero_test = 0x0;
    for (i = 0; i < ctx->key_size; i++)
        zero_test |= temp1[i];
    if (zero_test == 0)
        return 0;
    else
        return -1;
}


/* resolve the extern rdw_ecc_wait because the ISS models hardware behaviour */
void rdw_ecc_wait(RDW_ECC_CTX_T *ctx)
{
    rdw_ecc_wait_complete(ctx->ecc);
    /* write 1 to clear the DONE (OP_COMPLETE) flag */
    ctx->ecc->status |= RDW_ECC_DONE;             // clearing status bit is necessary before starting a new operation in the core.
    //rdw_ecc_op_complete_clear(ctx->ecc);
}
#endif