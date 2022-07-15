/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra_sm4.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/19
	Description :   
	Changed Log	:
		Liujian  2019/8/19			- Creation
*****************************************************************************/
#include "cobra4_sm4.h"

#if COBRA4_SM4_EN

/// SM4 queue environment
#if (COBRA_POWER_DOWN_ENABLE)
static struct cobra4_sm4_env_tag   sm4_env		  __attribute__((section("retention_area"),zero_init)); 
static struct cobra4_sm4_env_tag   cobra4_sm4		__attribute__((section("retention_area"),zero_init));
#else
static struct cobra4_sm4_env_tag     sm4_env;
static volatile COBRA4_SM4_TypeDef * cobra4_sm4;
#endif

void cobra4_sm4_init(void)
{
		cobra4_sm4 =  (volatile COBRA4_SM4_TypeDef *)COBRA4_SM4_BASE;
}

/*
"1: decryption enable
 0: encryption enable"
*/
void cobra4_sm4_cfg_encryp0_cypher1(COBRA4_OP_TYPE op)
{
		if(op == OP_DECRYPTION)
		{
				cobra4_sm4->cfg_sm4_basic |= (1<<2);
		}
		else if(op == OP_ENCRYPTION)
		{
				cobra4_sm4->cfg_sm4_basic &= ~(1<<2);
		}
}

/*
when decryption, set it firsly(cfg_compute_ekey_en=1,cfg_sm4_en=0), 
then waitting ekey_vld=1, 
then setting(cfg_compute_ekey_en=0,cfg_sm4_en=1)

*/
void cobra4_sm4_cfg_compute_ekey_en(unsigned int en)
{
		if(en)
		{
			  cobra4_sm4->cfg_sm4_basic |= (1<<1);
		}
		else
		{
			 cobra4_sm4->cfg_sm4_basic &= ~(1<<1);
		}
}

/*
"1: begin to ecryp or cypherthe text
0: idle"
*/
void cobra4_sm4_cfg_sm4_en(unsigned int en)
{
		if(en)
		{
			 cobra4_sm4->cfg_sm4_basic |= 0x01;
		}
		else
		{
				cobra4_sm4->cfg_sm4_basic &= ~0x01;
		}
}

/*
clear the interrupt
"[0] bit
1: clear the y_vld int by upedge
0: normal
[7:1] reserved"

*/
void cobra4_sm4_cfg_int_clr(void)
{
	 cobra4_sm4->IntCntl |= (1<<24);
}

/*

the holding time length of int 

*/
void cobra4_sm4_cfg_int_timeout2(unsigned int timeout)
{
	 volatile unsigned int temp;
	 temp = cobra4_sm4->IntCntl;
	 temp &= ~(0xff << 16);
	 temp |= ((timeout & 0xff) << 16);
	 cobra4_sm4->IntCntl = temp;
}

/*
clock divider
*/
void cobra4_sm4_cfg_int_timeout1(unsigned int timeout)
{
		 volatile unsigned int temp;
		 temp = cobra4_sm4->IntCntl;
		 temp &= ~(0xff << 8);
		 temp |= ((timeout & 0xff) << 8);
		 cobra4_sm4->IntCntl = temp;
	
}


/*
"1: y_vld int is diabled
0: y_vld int is enabled"
*/
void cobra4_sm4_cfg_y_vld_int_mask(unsigned int en)
{
		if(en)
		{
			  cobra4_sm4->IntCntl |= (1<<1);
		}
		else
		{
			 cobra4_sm4->IntCntl &= ~(1<<1);
		}
}

/*

"1: enable stick int
0: non-sticky int"

*/
void cobra4_sm4_cfg_sticky_int_en(unsigned int en)
{
		if(en)
		{
			  cobra4_sm4->IntCntl |= (1<<0);
		}
		else
		{
			 cobra4_sm4->IntCntl &= ~(1<<0);
		}
}

/*
"1. extended keys has been generated
 0: not any extended keys has been generated"

*/
unsigned int cobra4_sm4_ekey_vld(void)
{
	 if(cobra4_sm4->run_status & (1 << 1))
	 {
			return 1;
	 }
	 else
	 {
			return 0;
	 }
}

/*
"1. encryption output or decryption output
 0: not  encryption or  decryption output"
*/
unsigned int cobra4_sm4_y_vld(void)
{
	 if(cobra4_sm4->run_status & (1 << 0))
	 {
			return 1;
	 }
	 else
	 {
			return 0;
	 }
}

/*
"1: int has been generated
0: no int has been generated"

*/
unsigned int cobra4_sm4_interrupt(void)
{
		if(cobra4_sm4->int_status & (1 << 2))
			return 1;
		else
			return 0;	
}

/*
"1:sticky_int is generated 
0: no sticky_int is generated"

*/
unsigned int cobra4_sm4_sticky_int(void)
{
		if(cobra4_sm4->int_status & (1 << 1))
			return 1;
		else
			return 0;	
}


/*
"1. encryption output or decryption output
0: not  encryption or  decryption output"

*/
unsigned int cobra4_sm4_y_vld_int(void)
{
		if(cobra4_sm4->int_status & (1 << 0))
			return 1;
		else
			return 0;	
}

void cobra4_sm4_read_data_out (unsigned char data[])
{
  int i;
  unsigned int temp;
	
	for(i = 3; i >= 0; i--)
	{
			temp = cobra4_sm4->OUTPUT_Y[i];
			*data++ = temp       & 0xff;
			*data++ = temp >>  8 & 0xff;
			*data++ = temp >> 16 & 0xff;
			*data++ = temp >> 24 & 0xff;	
	}

}
void cobra4_sm4_set_key (const uint8_t *key)
{
		int i;
		unsigned int temp;
		for(i = 0; i < 4; i++)
		{
				temp = 0;
				temp <<= 8;
				temp |= key[4*i + 0];
				temp <<= 8;
				temp |= key[4*i + 1];
				temp <<= 8;
				temp |= key[4*i + 2];
				temp <<= 8;
				temp |= key[4*i + 3];			
				cobra4_sm4->KEY[i] = temp;
		}
}

void cobra4_sm4_set_plaintext (const uint8_t *data)
{
		int i;
		unsigned int temp;
		for(i = 0; i < 4; i++)
		{
				temp = 0;
				temp <<= 8;
				temp |= data[4*i + 0];
				temp <<= 8;
				temp |= data[4*i + 1];
				temp <<= 8;
				temp |= data[4*i + 2];
				temp <<= 8;
				temp |= data[4*i + 3];			
				cobra4_sm4->PLAINTEXT_X[i] = temp;
		}
}



void cobra4_sm4_encryption_evt_handler(void)
{
		uint8_t sm4_result[16];
	
		// Clear event
    ke_event_clear(COBRA4_KE_EVENT_SM4_END);
		
		cobra4_sm4_read_data_out(sm4_result);
	
}

void app_cobra4_sm4_event_callback_init(void)
{
		ke_event_callback_set(COBRA4_KE_EVENT_SM4_END, &cobra4_sm4_encryption_evt_handler);
		co_list_init(&(sm4_env.queue));
	  sm4_env.sm4_ongoing = false;
	
		NVIC_SetPriority(AES0_SM4_IRQn, 1);
		NVIC_EnableIRQ(AES0_SM4_IRQn);
	
}

static struct sm4_func_env* cobra4_aes_alloc(uint16_t size, sm4_func_continue_cb sm4_continue_cb, sm4_func_result_cb res_cb)
{
    struct sm4_func_env* p_sm4_env;

    // allocate SM4 memory in non-retention to execute it as soon as possible
    p_sm4_env = ke_malloc(size, KE_MEM_NON_RETENTION);

    // initialize SM4 environment
    p_sm4_env->sm4_continue_cb = sm4_continue_cb;
    p_sm4_env->sm4_res_cb      = res_cb;
    return p_sm4_env;
}

void cobra4_sm4_encrypt_function(const uint8_t* key, const uint8_t *val)
{
			cobra4_sm4_cfg_int_clr();
			cobra4_sm4_set_key(key);
			cobra4_sm4_set_plaintext(val);
			cobra4_sm4_cfg_compute_ekey_en(0);
			cobra4_sm4_cfg_encryp0_cypher1(OP_DECRYPTION);
			cobra4_sm4_cfg_sm4_en(1);
}

void cobra4_sm4_start(struct sm4_func_env* env, const uint8_t* key, const uint8_t *val)
{
    // put function environment at end of list.
     co_list_push_back(&(sm4_env.queue), &(env->hdr));

    // store parameter information
     env->key = key;
     env->val = val;
     
    // AES encryption can be immediately performed
    if(!sm4_env.sm4_ongoing)
    {
        sm4_env.sm4_ongoing = true;
      //  coney_aes_function(param);
        cobra4_sm4_encrypt_function(key, val);
    }
}


void cobra4_sm4_interrupt_function(void)
{
		if(cobra4_sm4_interrupt())
		{
				ke_event_set(COBRA4_KE_EVENT_SM4_END);
		}
}




#endif