/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_ecc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/29

	Description :   


	Changed Log	:

		Liujian  2019/5/29			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_ecc.h"
#include "cobra_irda.h"
#include "cobra_aon_pmu.h"
#include "cobra_pcrm.h"
#include "rwip_config.h"  
#include "ke_event.h"
#include "ke_mem.h"         // Used to allocate memory environment
#include "rdw_mcu.h"

#if COBRA_ECC_EN

//struct ecc_env_tag 	ecc_env;
struct ecc_func_env * p_ecc_func_env;
static ecc_func_result_cb eccmic_callback;

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_ECC_TypeDef * cobra_ecc		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_ECC_TypeDef * cobra_ecc; 
#endif

void cobra_ecc_init(void)
{
		cobra_ecc = (COBRA_ECC_TypeDef * )COBRA_ECC_BASE;
		eccmic_callback = NULL;
		p_ecc_func_env  = NULL;
//		co_list_init(&(ecc_env.queue));
//		ecc_env.ecc_ongoing = false;
}


/** Start ECC
 * Start the specified operation
 */
void cobra_ecc_start(void)
{
    cobra_ecc->control |= 0x01;
}


///** Enable secret key
// * Key comes from internal secret
// */
//void cobra_ecc_enable_secret_key(void)
//{
//    cobra_ecc->control |= (0x1 << 6);
//}


///** Disable secret key
// * Key comes from register map
// */
//void cobra_ecc_disable_secret_key(void)
//{
//    cobra_ecc->control &= ~(0x1 << 6);
//}

/** Enable interrupt
 * Enable interrupts
 */
void cobra_ecc_enable_interrupt(void)
{
    cobra_ecc->control |= (0x1 << 5);
}

/** Disable interrupt
 * Disable interrupts
 */

void cobra_ecc_disable_interrupt(void)
{
    cobra_ecc->control &= ~(0x1 << 5);
}


/** Set operation
 * Set the specified operation
 * @param op_index  - Operation to perform
 */

void cobra_ecc_operation(ECC_OPERATION op_index)
{
		switch(op_index)
		{
			case ECC_MODULAR_ADDITION:
				cobra_ecc->control = (cobra_ecc->control & ~(0xF << 1)) | (0x03 << 1);
				break;
			case ECC_MODULAR_SUBTRACTION:
				cobra_ecc->control = (cobra_ecc->control & ~(0xF << 1)) | (0x04 << 1);
				break;
			case ECC_MODULAR_MULTIPLICATION:
				cobra_ecc->control = (cobra_ecc->control & ~(0xF << 1)) | (0x05 << 1);
				break;
			case ECC_MODULAR_DIVISION:
				cobra_ecc->control = (cobra_ecc->control & ~(0xF << 1)) | (0x06 << 1);
				break;
			case ECC_MODULAR_INVERSION:
				cobra_ecc->control = (cobra_ecc->control & ~(0xF << 1)) | (0x07 << 1);
				break;
			default:
				break;		
		}
    
}


/** Returns the ECC busy status flag
 * @return int - true if busy
 */

int cobra_ecc_is_busy(void)
{
    return ((cobra_ecc->status & (0x1 << 1) ) == (0x1 << 1) );
}


/** Write 1 to Status bit 0 (OP_COMPLETE) to clear it.
 * @param ecc - Pointer to the ECC registers
 */

void cobra_ecc_op_complete_clear(void)
{
    cobra_ecc->status |= 0x01;
}

/** Wait until ECC core has processed the block
 * @param ecc  - Pointer to the ECC registers.
 */

void cobra_ecc_wait_complete(void)
{
    while (cobra_ecc_is_busy());
}


/** Write to the px registers
 * @param ctx          - Pointer to the ECC context.
 * @param px           - Pointer to the px array
 */

void cobra_ecc_write_px(COBRA_ECC_CTX_T *ctx, const unsigned int *px)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
	  {
        cobra_ecc->px = px[i];
		}
    for (; i < ctx->key_size_max; i++)
		{
        cobra_ecc->px = 0;
		}
}


/** Write to the py registers
 * @param ctx          - Pointer to the ECC context.
 * @param py           - Pointer to the py array
 */

void cobra_ecc_write_py(COBRA_ECC_CTX_T *ctx, const unsigned int *py)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        cobra_ecc->py = py[i];

    for (; i < ctx->key_size_max; i++)
        cobra_ecc->py = 0;
}

/** Write to the p registers
 * @param ctx          - Pointer to the ECC context.
 * @param p            - Pointer to the p array
 */

void cobra_ecc_write_p(COBRA_ECC_CTX_T *ctx, const unsigned int *p)
{
    unsigned int i;

    for (i = 0; i < ctx->key_size; i++)
        cobra_ecc->p = p[i];

    for (; i < ctx->key_size_max; i++)
        cobra_ecc->p = 0;
}

/** Read the px registers
 * @param ctx          - Pointer to the ECC context.
 * @param px           - Pointer to the px array
 */

void cobra_ecc_read_px(COBRA_ECC_CTX_T *ctx, unsigned int *px)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        px[i] = cobra_ecc->px;

    for (; i < ctx->key_size_max; i++) {
        dummy = cobra_ecc->px;
        (void)dummy;
    }
}

/** Read the py registers
 * @param ctx          - Pointer to the ECC context.
 * @param py           - Pointer to the py array
 */

void cobra_ecc_read_py(COBRA_ECC_CTX_T *ctx, unsigned int *py)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        py[i] = cobra_ecc->py;

    for (; i < ctx->key_size_max; i++) {
        dummy = cobra_ecc->py;
        (void)dummy;
    }
}


/** Read the p registers
 * @param ctx          - Pointer to the ECC context.
 * @param p            - Pointer to the p array
 */

void cobra_ecc_read_p(COBRA_ECC_CTX_T *ctx, unsigned int *p)
{
    unsigned int i;
    volatile unsigned int dummy;

    for (i = 0; i < ctx->key_size; i++)
        p[i] = cobra_ecc->p;

    for (; i < ctx->key_size_max; i++) {
        dummy = cobra_ecc->p;
        (void)dummy;
    }
}

/* resolve the extern rdw_ecc_wait because the ISS models hardware behaviour */
void cobra_ecc_wait(void)
{
    cobra_ecc_wait_complete();
    /* write 1 to clear the DONE (OP_COMPLETE) flag */
    // clearing status bit is necessary before starting a new operation in the core.
    cobra_ecc_op_complete_clear();
}

//**********************漂亮的分割线   1     *********************************************************************************
//**********************漂亮的分割线   1     *********************************************************************************
//**********************漂亮的分割线   1     *********************************************************************************
//**********************漂亮的分割线   1     *********************************************************************************


void ecc_result_handler(void)
{
		unsigned int result_px[COBRA_ECC_KEYSIZE32_MAX];        // Temporary array storage
    unsigned int result_py[COBRA_ECC_KEYSIZE32_MAX];        // Temporary array storage
	
    if(p_ecc_func_env)
    {
        bool finished = true;

				// continue function computation
				if(p_ecc_func_env->ecc_continue_cb != NULL)
				{
						finished = p_ecc_func_env->ecc_continue_cb(p_ecc_func_env, result_px, result_py);
				}
    

        // if function execution is over
        if(finished)
        {
            // Inform requester of end of ECC based algorithm
            if(p_ecc_func_env->ecc_res_cb != NULL)
            {
                p_ecc_func_env->ecc_res_cb(result_px, result_py, p_ecc_func_env->src_info);  //src_info 可以使调用者的函数
            }
    
            // remove function environment
            ke_free(p_ecc_func_env);
        }
    }
}


static void cobra_eccmicro_task_callback(void)
{
		ke_event_clear(KE_EVENT_ECC_END);
		ecc_result_handler();
}

void cobra_app_eccmicro_task_init(void)
{
    ke_event_callback_set(KE_EVENT_ECC_END, &cobra_eccmicro_task_callback);
}

void cobra_m3_eccmicro_interrupt(void)
{
		cobra_ecc_op_complete_clear();
		
		ke_event_set(KE_EVENT_ECC_END);
	
}



void app_cobra_eccmicro_operation(COBRA_ECC_CTX_T *ctx, 
																	const unsigned int  *px_operation, 
																  const unsigned int  *py_operation, 
																	const unsigned int  *prime_factor, 
																	ECC_OPERATION  operation
																	)
{
		cobra_pcrm_ecc_active_mode_clock_enable(1);
	
		if(px_operation != NULL)
		{
				cobra_ecc_write_px(ctx, px_operation);
		}
		if(py_operation != NULL)
		{
				cobra_ecc_write_py(ctx, py_operation);
		}
		if(prime_factor != NULL)
		{
				cobra_ecc_write_p(ctx, prime_factor);
		}

    cobra_ecc_operation(operation);
		cobra_ecc_enable_interrupt();
    cobra_ecc_start();
		
		NVIC_SetPriority(ECCMICRO0_IRQn, 1);
		NVIC_ClearPendingIRQ(ECCMICRO0_IRQn);
		NVIC_EnableIRQ(ECCMICRO0_IRQn);
		
}

struct ecc_func_env* ecc_alloc(uint16_t size, ecc_func_continue_cb p_ecc_continue_cb, ecc_func_result_cb p_res_cb,
                               uint32_t p_src_info)
{
    struct ecc_func_env* p_ecc_env;

    // allocate AES memory in non-retention to execute it as soon as possible
    p_ecc_env = ke_malloc(size, KE_MEM_NON_RETENTION);

    // initialize ECC environment
    p_ecc_env->ecc_continue_cb = p_ecc_continue_cb;
    p_ecc_env->ecc_res_cb      = p_res_cb;
    p_ecc_env->src_info        = p_src_info;

    return p_ecc_env;
}

void ecc_start(struct ecc_func_env* env, COBRA_ECC_CTX_T *ctx, const unsigned int  *px_operation,  const unsigned int  *py_operation,  
														const unsigned int  *prime_factor, ECC_OPERATION  operation)
{
	
     app_cobra_eccmicro_operation(ctx, px_operation, py_operation, prime_factor, operation);
	
}




bool 	cobra_eccmicro_eca_continue( struct ecc_func_env *env, uint32_t* ecc_result1, uint32_t* ecc_result2)
{
		unsigned int i;
//    unsigned int inf_1;
//    unsigned int inf_2;
    unsigned int inf_out;

		struct eccmicro_eca_env *p_eccmicro_eca_env = (struct eccmicro_eca_env *)env;
	

		if(p_eccmicro_eca_env->step == 1)   //STEP 1  // D = Qy - Py
		{
				cobra_ecc_read_px(p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->D);
			
				inf_out = 1;
				i = 0;
				while ((inf_out == 1) && (i<p_eccmicro_eca_env->ecc_eca_env.ctx->key_size)){
						if ((p_eccmicro_eca_env->ecc_eca_env.px[i] != p_eccmicro_eca_env->qx[i]) || (p_eccmicro_eca_env->ecc_eca_env.py[i] != p_eccmicro_eca_env->D[i]))     //detect if (Px,Py) != (Qx,-Qy)
								inf_out = 0;

						i++;
				}
				
				 // If (Px,Py) = (Qx,-Qy), return INF point
				if (inf_out == 1){
						for (i = 0; i < p_eccmicro_eca_env->ecc_eca_env.ctx->key_size; i++){
								ecc_result1[i] = COBRA_ECC_INF_32;
								ecc_result2[i] = COBRA_ECC_INF_32;
						}
						return 1;                                   // Return 1 to signal infinity
				}
						
				ecc_start((struct ecc_func_env* )env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->qy,
											p_eccmicro_eca_env->ecc_eca_env.py, NULL, ECC_MODULAR_SUBTRACTION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
				
						
		}
		else if(p_eccmicro_eca_env->step == 2)  // E = Qx - Px
		{
					cobra_ecc_read_px(p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->D);
					ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->qx,
											p_eccmicro_eca_env->ecc_eca_env.px, NULL, ECC_MODULAR_SUBTRACTION);
					p_eccmicro_eca_env->step++;
					return 0;   //continue;
				
		}
		else if(p_eccmicro_eca_env->step == 3)   // D = D/E
		{
					ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, NULL,
											p_eccmicro_eca_env->D, NULL, ECC_MODULAR_DIVISION);
					p_eccmicro_eca_env->step++;
					return 0;   //continue;
		}
		else if(p_eccmicro_eca_env->step == 4)  // E = D*D
		{
			
				cobra_ecc_read_px(p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->D);
			
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, NULL,
											NULL, NULL, ECC_MODULAR_MULTIPLICATION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
				
		}
		else if(p_eccmicro_eca_env->step == 5)  // // F = Px + Qx
		{
				cobra_ecc_read_px(p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->E);
			
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->ecc_eca_env.px,
											p_eccmicro_eca_env->qx, NULL, ECC_MODULAR_ADDITION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
				
		}
		else if(p_eccmicro_eca_env->step == 6) 
		{
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->E,
											NULL, NULL, ECC_MODULAR_SUBTRACTION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
		}
		else if(p_eccmicro_eca_env->step == 7)  // F = Qx - E
		{
				cobra_ecc_read_px(p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->E);
			
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->qx,
											NULL, NULL, ECC_MODULAR_SUBTRACTION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
		}
		else if(p_eccmicro_eca_env->step == 8)  // F = D*F
		{
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, p_eccmicro_eca_env->D,
											NULL, NULL, ECC_MODULAR_MULTIPLICATION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
		}
		else if(p_eccmicro_eca_env->step == 9)  // F = F - Q1y
		{
				ecc_start((struct ecc_func_env* )p_eccmicro_eca_env, p_eccmicro_eca_env->ecc_eca_env.ctx, NULL,
											p_eccmicro_eca_env->qy, NULL, ECC_MODULAR_SUBTRACTION);
				p_eccmicro_eca_env->step++;
				return 0;   //continue;
		}
		else
		{
			 for (i = 0; i < p_eccmicro_eca_env->ecc_eca_env.ctx->key_size; i++){
					ecc_result1[i] = p_eccmicro_eca_env->E[i];
			 }
			 cobra_ecc_read_py(p_eccmicro_eca_env->ecc_eca_env.ctx, ecc_result2);
			 return 1;
		}
}

void 	cobra_eccmicro_eca_start(struct eccmicro_eca_env *env)
{
		unsigned int i;
    unsigned int inf_1;
    unsigned int inf_2;
//    unsigned int inf_out;
		// Initialise temporary storage with 0s
		for (i = 0; i < env->ecc_eca_env.ctx->key_size; i++)
		{
					env->D[i] = 0;
		}
		
		// Check if input curve points to be added equal infinity
		inf_1 = 1;
		inf_2 = 1;
		i = 0;
		while (((inf_1 == 1) || (inf_2 == 1)) && (i<env->ecc_eca_env.ctx->key_size))
		{
				if ((env->ecc_eca_env.px[i] != COBRA_ECC_INF_32) || (env->ecc_eca_env.py[i] != COBRA_ECC_INF_32))  //detect if (Px,Py) != (INF,INF)
						inf_1 = 0;

				if ((env->qx[i] != COBRA_ECC_INF_32) || (env->qy[i] != COBRA_ECC_INF_32))  //detect if (Qx,Qy) != (INF,INF)
						inf_2 = 0;

				i++;
		}
		// If (Px,Py) = (INF,INF), assign (Rx,Ry) = (Qx,Qy)
		if (inf_1 == 1)
		{
				for (i = 0; i < env->ecc_eca_env.ctx->key_size; i++){
						env->rx[i] = env->qx[i];
						env->ry[i] = env->qy[i];
				}
				//回调函数,通知应用程序得到计算结果
				env->ecc_eca_env.ecc_res_cb(env->rx, env->ry, env->ecc_eca_env.src_info);
				ke_free(env);
				return ;
		}
		// If (Qx,Qy) = (INF,INF), assign (Rx,Ry) = (Px,Py)
		if (inf_2 == 1)
		{
				for (i = 0; i < env->ecc_eca_env.ctx->key_size; i++){
						env->rx[i] = env->ecc_eca_env.px[i];
						env->ry[i] = env->ecc_eca_env.py[i];
				}
				//回调函数,通知应用程序得到计算结果
				env->ecc_eca_env.ecc_res_cb(env->rx, env->ry, env->ecc_eca_env.src_info);
				ke_free(env);
				return ;
			}
			 
			// compute -qy
			ecc_start((struct ecc_func_env* )env, env->ecc_eca_env.ctx, env->D,
									env->qy, env->ecc_eca_env.p_factor, ECC_MODULAR_SUBTRACTION);
			env->step++;
	
	
}






int app_cobra_eccmicro_eca(COBRA_ECC_CTX_T *ctx,
                  const unsigned int *px,
                  const unsigned int *py,
                  const unsigned int *qx,
                  const unsigned int *qy,
                  const unsigned int *modulus,
                  unsigned int *rx,
                  unsigned int *ry,
								  eccmicro_eca_res_cb res_cb,
								  ecc_func_result_cb res_cb1)
{
		uint32_t src_info;
	
		src_info = (uint32_t)res_cb;
	
		struct eccmicro_eca_env *eccmic_eca = (struct eccmicro_eca_env *)ecc_alloc(sizeof(struct eccmicro_eca_env) ,
																					(ecc_func_continue_cb)(cobra_eccmicro_eca_continue), res_cb1, src_info);
		
		eccmic_eca->ecc_eca_env.px = px;
		eccmic_eca->ecc_eca_env.py = py;
		eccmic_eca->ecc_eca_env.p_factor = modulus;
		eccmic_eca->ecc_eca_env.ctx = ctx;
	
		
		eccmic_eca->qx = qx;
		eccmic_eca->qy = qy;
		eccmic_eca->rx = rx;
		eccmic_eca->ry = ry;
		
		eccmic_eca->step = 0;
		
		p_ecc_func_env = (struct ecc_func_env *)(&(eccmic_eca->ecc_eca_env));
		
		cobra_eccmicro_eca_start(eccmic_eca);
}

//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//**********************漂亮的分割线*******************************************************************************************
//----------------------------------------------------------------------------------------------------------------------

///** Wrapper function for eSi-ECC Point Doubling R = 2Q
// * @param ctx          - Pointer to the ECC context.
// * @param px           - EC point Px
// * @param py           - EC point Py
// * @param modulus      - modulus
// * @param write_mod    - When Set to 1 modulus will be written to the core. Otherwise assume modulus already has been written.
// * @param ca           - The "a" parameter
// * @param rx           - Result x coordinate
// * @param ry           - Result y coordinate
// */
//int app_cobra_eccmicro_ecd(COBRA_ECC_CTX_T *ctx,
//                 const unsigned int *px,
//                 const unsigned int *py,
//                 const unsigned int *modulus,
//                 unsigned int  write_mod,
//                 const unsigned int *ca,
//                 unsigned int *rx,
//                 unsigned int *ry)
//{
//    unsigned int i;
//    int inf_out;
//    unsigned int A[COBRA_ECC_KEYSIZE32_MAX];        // Temporary array storage
//    unsigned int B[COBRA_ECC_KEYSIZE32_MAX];        // Temporary array storage
////    unsigned int C[RDW_ECC_KEYSIZE32_MAX];        // Temporary array storage


//    // Initialise temporary storage with 0s
//   // for (i = 0; i < RDW_ECC_KEYSIZE32_MAX; i++){
//   //     A[i] = 0;
//   //     B[i] = 0;                     // rx = B
//   //     C[i] = 0;                     // ry = C
//   // }

//    inf_out = 1;
//    i = 0;
//    while ((inf_out == 1) && (i<ctx->key_size)){
//        if ((px[i] != COBRA_ECC_INF_32) || (py[i] != COBRA_ECC_INF_32))     //detect if (Px,Py) != (INFx,INFy)
//            inf_out = 0;

//        i++;
//    }

//    // If (Px,Py) = (INFx,INFy), return INF point
//    if (inf_out == 1){
//        for (i = 0; i < ctx->key_size; i++){
//            rx[i] = COBRA_ECC_INF_32;
//            ry[i] = COBRA_ECC_INF_32;
//        }
//        return 1;                                  // Return 1 to signal infinity
//    }

//    // If modulus has not been written to the core externally, write it now.
//    if (write_mod == 1)
//        cobra_ecc_write_p(ctx, modulus);

//    // If no special cases detected perform ECD routine
//    // STEP 1: rdw_ecc_mm(ctx, px, px, modulus, A);  // A = Px*Px
//    cobra_ecc_write_px(ctx, px);
//    cobra_ecc_write_py(ctx, px);
//    cobra_ecc_operation(ECC_MODULAR_MULTIPLICATION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_px(ctx, A);

//    // STEP 2: rdw_ecc_ma(ctx, A, A, modulus, B);    // B = A + A
//    //rdw_ecc_write_px(ctx, A);
//    //rdw_ecc_write_py(ctx, A);
//    cobra_ecc_operation(ECC_MODULAR_ADDITION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, B);

//    // STEP 3: rdw_ecc_ma(ctx, B, A, modulus, A);    // A = B + A
//    //rdw_ecc_write_px(ctx, B);
//    cobra_ecc_write_py(ctx, A);
//    cobra_ecc_operation(ECC_MODULAR_ADDITION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, A);

//    // STEP 4: rdw_ecc_ma(ctx, A, ca, modulus, A);   // A = A + ca
//    //rdw_ecc_write_px(ctx, A);
//    cobra_ecc_write_py(ctx, ca);
//    cobra_ecc_operation( ECC_MODULAR_ADDITION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_px(ctx, A);

//    // STEP 5: rdw_ecc_ma(ctx, py, py, modulus, B);  // B = Py + Py
//    cobra_ecc_write_px(ctx, py);
//    cobra_ecc_write_py(ctx, py);
//    cobra_ecc_operation(ECC_MODULAR_ADDITION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, B);

//    // STEP 6: rdw_ecc_md(ctx, B, A, modulus, A);    // A = A/B
//    //rdw_ecc_write_px(ctx, B);
//    cobra_ecc_write_py(ctx, A);
//    cobra_ecc_operation( ECC_MODULAR_DIVISION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_px(ctx, A);

//    // STEP 7: rdw_ecc_mm(ctx, A, A, modulus, B);    // B = A*A
//    //rdw_ecc_write_px(ctx, A);
//    //rdw_ecc_write_py(ctx, A);
//    cobra_ecc_operation( ECC_MODULAR_MULTIPLICATION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_px(ctx, B);

//    // STEP 8: rdw_ecc_ma(ctx, px, px, modulus, C);  // C = Px + Px
//    cobra_ecc_write_px(ctx, px);
//    cobra_ecc_write_py(ctx, px);
//    cobra_ecc_operation(ECC_MODULAR_ADDITION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, C);

//    // STEP 9: rdw_ecc_ms(ctx, B, C, modulus, B);    // B = B - C
//    cobra_ecc_write_px(ctx, B);
//    //rdw_ecc_write_py(ctx, C);
//    cobra_ecc_operation(ECC_MODULAR_SUBTRACTION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_px(ctx, B);

//    // STEP 10: rdw_ecc_ms(ctx, px, B, modulus, C);   // C = Px - B
//    cobra_ecc_write_px(ctx, px);
//    //rdw_ecc_write_py(ctx, B);
//    cobra_ecc_operation(ECC_MODULAR_SUBTRACTION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, C);

//    // STEP 11: rdw_ecc_mm(ctx, A, C, modulus, C);    // C = A*C
//    cobra_ecc_write_px(ctx, A);
//    //rdw_ecc_write_py(ctx, C);
//    cobra_ecc_operation(ECC_MODULAR_MULTIPLICATION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    //rdw_ecc_read_px(ctx, C);

//    // STEP 12: rdw_ecc_ms(ctx, C, py, modulus, C);   // C = C - PY
//    //rdw_ecc_write_px(ctx, C);
//    cobra_ecc_write_py(ctx, py);
//    cobra_ecc_operation(ECC_MODULAR_SUBTRACTION);
//    cobra_ecc_start();
//    cobra_ecc_wait();
//    cobra_ecc_read_py(ctx, ry);


//    for (i = 0; i < ctx->key_size; i++){
//        rx[i] = B[i];
//   //     ry[i] = C[i];
//    }

//    return 0;

//}


///** Wrapper function for eSi-ECC Scalar Multiply R = kG
// * @param ctx          - Pointer to the ECC context.
// * @param gx           - Basepoint x
// * @param gy           - Basepoint y
// * @param k            - Key
// * @param modulus      - modulus
// * @param ca           - The "a" parameter
// * @param rx           - Result x coordinate
// * @param ry           - Result y coordinate
// */
//int app_cobra_eccmicro_ecsm(COBRA_ECC_CTX_T *ctx,
//                  const unsigned int *gx,
//                  const unsigned int *gy,
//                  const unsigned int *k,
//                  const unsigned int *modulus,
//                  const unsigned int *ca,
//                  unsigned int *rx,
//                  unsigned int *ry)
//{
//    int i;
//    int j;             // needs to be signed to prevent counting below 0!
//    unsigned ki;
//    int ret1 = 0;
//		volatile int ret2 = 0;

//    unsigned int Q1x[COBRA_ECC_KEYSIZE32_MAX];        // Temportary array storage
//    unsigned int Q1y[COBRA_ECC_KEYSIZE32_MAX];        // Temportary array storage
//    unsigned int Q2x[COBRA_ECC_KEYSIZE32_MAX];        // Temportary array storage
//    unsigned int Q2y[COBRA_ECC_KEYSIZE32_MAX];        // Temportary array storage
//    //unsigned int temp[RDW_ECC_KEYSIZE32_MAX];       // Temportary array storage

//    // Initialise Montgomery ladder variable
//    for (i = 0; i < (int)ctx->key_size; i++){
//        Q1x[i] = COBRA_ECC_INF_32;
//        Q1y[i] = COBRA_ECC_INF_32;
//        Q2x[i] = gx[i];
//        Q2y[i] = gy[i];
//    }

//    // Write modulous to the core once
//    cobra_ecc_write_p(ctx, modulus);

//    // Montgomery ladder
//    for (i = (((int)ctx->key_size)-1); i>=0 ; i--){
//        for (j = 31; j >= 0 ; j--){

//            ki = (k[i] >> j) & 0x1;

//            if (ki == 1){
//                ret1 = app_cobra_eccmicro_eca(ctx, Q1x, Q1y, Q2x, Q2y, modulus, 0, Q1x, Q1y);
//                ret2 = app_cobra_eccmicro_ecd(ctx, Q2x, Q2y, modulus, 0, ca, Q2x, Q2y);
//            }
//            else {
//                ret2 = app_cobra_eccmicro_eca(ctx, Q1x, Q1y, Q2x, Q2y, modulus, 0, Q2x, Q2y);
//                ret1 = app_cobra_eccmicro_ecd(ctx, Q1x, Q1y, modulus, 0, ca, Q1x, Q1y);
//            }
//        }
//    }

//    for (i = 0; i < (int)ctx->key_size; i++){
//        rx[i] = Q1x[i];
//        ry[i] = Q1y[i];
//    }

//    return ret1;

//}

/* SEC2 Curve parameters 192 r1 */
const unsigned int Gx_192[6] = {0x82ff1012, 0xf4ff0afd, 0x43a18800, 0x7cbf20eb, 0xb03090f6, 0x188da80e}; /* basepoint x */
const unsigned int Gy_192[6] = {0x1e794811, 0x73f977a1, 0x6b24cdd5, 0x631011ed, 0xffc8da78, 0x07192b95}; /* basepoint y */
const unsigned int A_192[6]  = {0xfffffffc, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff}; /* A parameter */
const unsigned int B_192[6]  = {0xc146b9b1, 0xfeb8deec, 0x72243049, 0x0fa7e9ab, 0xe59c80e7, 0x64210519}; /* B parameter */
const unsigned int P_192[6]  = {0xffffffff, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff}; /* prime */
const unsigned int Q_192[6]  = {0xb4d22831, 0x146bc9b1, 0x99def836, 0xffffffff, 0xffffffff, 0xffffffff}; /* order */
/* SEC2 Test data 192 r1 */
const unsigned int Sx_192[6]     = {0x3e97acf8, 0x53a01207, 0xd2467693, 0x0c330266, 0x27ae671b, 0xd458e7d1}; /* Sx testpoint */
const unsigned int Sy_192[6]     = {0x5086df3b, 0x5673a164, 0xcf7fb11b, 0x6bddc050, 0x0d851f33, 0x32593050}; /* Sy testpoint */
const unsigned int Tx_192[6]     = {0x9b9753a4, 0xbe16fb05, 0x87fdbd01, 0x67ddecdd, 0x213e9ebe, 0xf22c4395}; /* Tx testpoint */
const unsigned int Ty_192[6]     = {0x691a9c79, 0xa9cecc97, 0xf8dfb41f, 0x7796db48, 0x6af2b359, 0x26442409}; /* Ty testpoint */
const unsigned int key_192[6]    = {0xf64c74ee, 0x8255391a, 0xa542463a, 0x5dd41b33, 0x60baec0c, 0xa78a236d}; /* key testpoint */


unsigned int Rx[COBRA_ECC_KEYSIZE32_MAX];
unsigned int Ry[COBRA_ECC_KEYSIZE32_MAX];
COBRA_ECC_CTX_T  ctx;


void app_cobra_eccmicro_eca_result_cb(uint32_t* ecc_res1,  uint32_t* ecc_res2, uint32_t src_info)
{
		unsigned int i;
//    m_al_sec_aes_res_cb res_cb = (m_al_sec_aes_res_cb)src_info;

//    // execute proper result callback
//    res_cb(aes_res);
		printf("app_cobra_eccmicro_eca_result_cb \n");
		for(i = 0; i < ctx.key_size ; i++)
		{
				printf("RRRx[%d] = %x \n", i, ecc_res1[i]);
				printf("RRRy[%d] = %x \n", i, ecc_res2[i]);
		}
}



void app_cobra_eccmicro_test(void)
{

		unsigned int i;

		ctx.key_size = 192/32;
		ctx.key_size_max = 12;
	
	
		app_cobra_eccmicro_eca(&ctx, Sx_192, Sy_192, Tx_192, Ty_192, P_192, Rx, Ry, NULL, app_cobra_eccmicro_eca_result_cb);
}
#endif














