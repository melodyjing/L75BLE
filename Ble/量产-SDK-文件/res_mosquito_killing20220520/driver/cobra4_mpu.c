/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_mpu.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/10/25

	Description :   
         cortex-M4 MPU driver

	Changed Log	:

		Liujian  2021/10/25		- Creation

*****************************************************************************/

#include "cobra4_mpu.h"
#include "cobra4_aon_top.h"
#include "cobra_watchdog.h"

#if (COBRA_MPU_EN == 1)

void HAL_MPU_Disable(void)
{
    __DMB();

    SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;

    MPU->CTRL = 0;
	
}

void HAL_MPU_Enable(uint32_t mpu_control)
{
	  MPU->CTRL = mpu_control | MPU_CTRL_ENABLE_Msk;
	
	  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	
	  __DSB();
	  __ISB();
}

void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init)
{
  /* Check the parameters */
  assert_param(IS_MPU_REGION_NUMBER(MPU_Init->Number));
  assert_param(IS_MPU_REGION_ENABLE(MPU_Init->Enable));
 
  /* Set the Region number */
  MPU->RNR = MPU_Init->Number;
 
  if ((MPU_Init->Enable) != RESET)
  {
    /* Check the parameters */
    assert_param(IS_MPU_INSTRUCTION_ACCESS(MPU_Init->DisableExec));
    assert_param(IS_MPU_REGION_PERMISSION_ATTRIBUTE(MPU_Init->AccessPermission));
    assert_param(IS_MPU_TEX_LEVEL(MPU_Init->TypeExtField));
    assert_param(IS_MPU_ACCESS_SHAREABLE(MPU_Init->IsShareable));
    assert_param(IS_MPU_ACCESS_CACHEABLE(MPU_Init->IsCacheable));
    assert_param(IS_MPU_ACCESS_BUFFERABLE(MPU_Init->IsBufferable));
    assert_param(IS_MPU_SUB_REGION_DISABLE(MPU_Init->SubRegionDisable));
    assert_param(IS_MPU_REGION_SIZE(MPU_Init->Size));
 
    MPU->RBAR = MPU_Init->BaseAddress;
    MPU->RASR = ((uint32_t)MPU_Init->DisableExec             << MPU_RASR_XN_Pos)   |
                ((uint32_t)MPU_Init->AccessPermission        << MPU_RASR_AP_Pos)   |
                ((uint32_t)MPU_Init->TypeExtField            << MPU_RASR_TEX_Pos)  |
                ((uint32_t)MPU_Init->IsShareable             << MPU_RASR_S_Pos)    |
                ((uint32_t)MPU_Init->IsCacheable             << MPU_RASR_C_Pos)    |
                ((uint32_t)MPU_Init->IsBufferable            << MPU_RASR_B_Pos)    |
                ((uint32_t)MPU_Init->SubRegionDisable        << MPU_RASR_SRD_Pos)  |
                ((uint32_t)MPU_Init->Size                    << MPU_RASR_SIZE_Pos) |
                ((uint32_t)MPU_Init->Enable                  << MPU_RASR_ENABLE_Pos);
  }
  else
  {
    MPU->RBAR = 0x00;
    MPU->RASR = 0x00;
  }
}


/*
*********************************************************************************************************
*	函 数 名: cobra4_mpu_config
*	功能说明: 配置MPU
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void cobra4_mpu_config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;
 
	/* 禁止 MPU */
	HAL_MPU_Disable();
	

	/* 配置VIMS_ROM */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	
	if(cobra4_aon_pd_top_get_mcu_remap2flash_cfg())  //bootload 影射到了 0x100000
	{
	     MPU_InitStruct.BaseAddress      = 0x100000;
	}
	else
	{
		   MPU_InitStruct.BaseAddress      = 0x000000;   //bootload 影射到了 0x00000
	}
	
	MPU_InitStruct.Size             = MPU_REGION_SIZE_1MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
 
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
 
//	/* 配置AXI SRAM的MPU属性为Write back, Read allocate，Write allocate */
//	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
//	MPU_InitStruct.BaseAddress      = 0x24000000;
//	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
//	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
//	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
//	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
//	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
//	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
//	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
//	MPU_InitStruct.SubRegionDisable = 0x00;
//	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
// 
//	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
	
//	/* 配置FMC扩展IO的MPU属性为Device或者Strongly Ordered */
//	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
//	MPU_InitStruct.BaseAddress      = 0x60000000;
//	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_64KB;	
//	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
//	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
//	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;	
//	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
//	MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
//	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
//	MPU_InitStruct.SubRegionDisable = 0x00;
//	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
//	
//	HAL_MPU_ConfigRegion(&MPU_InitStruct);
 
	/*使能 MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void test_enter_bootload(void)
{
	  //typedef void (*fp_test)(void);
	
    uint32_t  pc, sp;
	
		uint32_t *rebootaddr =(uint32_t *)(0x00000);		//CACHE_ADDR  FLASH_ADDR
		
		sp = rebootaddr[0];
		pc = rebootaddr[1];


    cobra_system_reboot_jump(sp,pc);

}

void MemManage_Handler(void)
{
	  printf("memory fault \n");
	  app_cobra_watchdog_init(20, WATCHDOG_RESET);
	  while(1);
}

#endif



