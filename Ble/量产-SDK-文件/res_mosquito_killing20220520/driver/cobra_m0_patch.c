/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_m0_patch.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/27

	Description :   
					

	Changed Log	:

		Liujian  2019/3/27			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_m0_patch.h"


#if COBRA_M0_PATCH_EN
static volatile COBRA_M0_PATCH_T * cobra_m0_patch;

static __attribute__ ((aligned (1024))) volatile unsigned int instruction_base[6]; 

static uint32_t patch_table[6];

 
void cobra_m0_patch_init(void)
{
		cobra_m0_patch = (COBRA_M0_PATCH_T *)COBRA_M0_PATCH_BASE; 

		cobra_m0_patch_ctrl(0);
	
		cobra_m0_patch->remap_base = (unsigned int)instruction_base;
} 


//en = 1, enable m0  patch
//en = 0, disable m0 patch
void cobra_m0_patch_ctrl(unsigned int en)
{
		if(en)
		{
				cobra_m0_patch->ctrl = 0x01;		
		}
		else
		{
				cobra_m0_patch->ctrl = 0x00;		
		}	
}

//要打补丁的ROM 的基地址的高 14 位 [31:18]
void cobra_m0_patch_rom_base(unsigned int patch_base)
{
		cobra_m0_patch->rom_base = patch_base&0xfffc0000;	
}

/*
index: 要打补丁的序列号,最多6个
patch_rom_addr: 要打补丁的ROM 地址, 要 4 字节对齐    
instruction:    要打补丁的 ROM  插入的2条 thumb 指令
*/
void cobra_m0_patch_addr_and_instruction(PATCH_INDEX index, unsigned int patch_rom_addr, unsigned int instruction)
{
		if(index == PATCH_INDEX0)
		{
				cobra_m0_patch->rom_index[0] = patch_rom_addr;
				instruction_base[0] = instruction;
				cobra_m0_patch->enable_index |= 0x01;
		}
		else if(index == PATCH_INDEX1)
		{
				cobra_m0_patch->rom_index[1] = patch_rom_addr;
				instruction_base[1] = instruction;
				cobra_m0_patch->enable_index |= (1 << 1);
		}
		else if(index == PATCH_INDEX2)
		{
				cobra_m0_patch->rom_index[2] = patch_rom_addr;
				instruction_base[2] = instruction;
				cobra_m0_patch->enable_index |= (1 << 2);
		}
		else if(index == PATCH_INDEX3)
		{
				cobra_m0_patch->rom_index[3] = patch_rom_addr;
				instruction_base[3] = instruction;
				cobra_m0_patch->enable_index |= (1 << 3);
		}
		else if(index == PATCH_INDEX4)
		{
				cobra_m0_patch->rom_index[4] = patch_rom_addr;
				instruction_base[4] = instruction;
				cobra_m0_patch->enable_index |= (1 << 4);
		}
		else if(index == PATCH_INDEX5)
		{
				cobra_m0_patch->rom_index[5] = patch_rom_addr;
				instruction_base[5] = instruction;
				cobra_m0_patch->enable_index |= (1 << 5);
		}
}

//---------------------APP ---------------------------------------------------
/*
index, 表示要patch 的 index, patch_function 是要更改为正确的函数的地址
*/
void cobra_m0_patch_function(PATCH_INDEX index, unsigned int patch_function)
{
		if(index == PATCH_INDEX0)
		{
				patch_table[0] = patch_function;
		}
		else if(index == PATCH_INDEX1)
		{
				patch_table[1] = patch_function;
		}
		else if(index == PATCH_INDEX2)
		{
				patch_table[2] = patch_function;
		}
		else if(index == PATCH_INDEX3)
		{
				patch_table[3] = patch_function;
		}
		else if(index == PATCH_INDEX4)
		{
				patch_table[4] = patch_function;
		}
		else if(index == PATCH_INDEX5)
		{
				patch_table[5] = patch_function;
		}
}


/*
// 
// pwdSF[0] = R0 , pwdSF[1] = R1
// pwdSF[2] = R2 , pwdSF[3] = R3
// pwdSF[4] = R12, pwdSF[5] = LR
// pwdSF[6] = 返回地址 (进入stack 的 PC)
// pwdSF[7] = xPSR
*/

void svc_handler_function(unsigned int * pwdSF)
{
		unsigned int svc_number;
			
		svc_number = ((char *)pwdSF[6])[-2];
			
		if (svc_number < (sizeof patch_table)/4)
				pwdSF[6] = (uint32_t)patch_table[svc_number];
		else
				while(1);

		return;
}

#if 0
// 使用例子

void patch_function1(void)
{

}

void patch_function2(void)
{

}

void patch_app_test(void)
{
		cobra_m0_patch_init();
	  cobra_m0_patch_rom_base(0x00400000);    // cobra ROM base address
		cobra_m0_patch_addr_and_instruction(PATCH_INDEX0, 0x00017a14, 0x4770df00);  // svc 0
		cobra_m0_patch_function(PATCH_INDEX0, (unsigned int)patch_function1);

		cobra_m0_patch_addr_and_instruction(PATCH_INDEX1, 0x00018a14, 0x4770df01);  // svc 1
		cobra_m0_patch_function(PATCH_INDEX1, (unsigned int)patch_function2);
		
		cobra_m0_patch_ctrl(1);
		
		NVIC_DisableIRQ(SVCall_IRQn);
    NVIC_SetPriority(SVCall_IRQn, 1);
    NVIC_EnableIRQ(SVCall_IRQn);
		
}


#endif


//这段代码要替换启动代码中的 SVC CALL 处理函数
/*
IMPORT SVC_HandlerC
SVC_Handler     PROC
                movs r0, #4
                mov r1, lr
                tst r0, r1
                beq SVC_stacking_used_MSP
                mrs r0, psp
                ldr r1,=SVC_HandlerC
                bx r1
SVC_stacking_used_MSP
                mrs r0, msp
                ldr r1,=SVC_HandlerC
                bx r1
				
                ENDP
*/

#endif














