/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_ipc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/8

	Description :   


	Changed Log	:

		Liujian  2019/3/28			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_ipc.h"

#if COBRA_IPC_EN

static volatile COBRA_IPC_HOST_TypeDef * cobra_ipc_host;  //cortex-M3

static volatile COBRA_IPC_EMB_TypeDef * cobra_ipc_emb;   //cortex-M0

void cobra_ipc_host_init(void)
{
		cobra_ipc_host = (COBRA_IPC_HOST_TypeDef * )COBRA_IPC_HOST_BASE;	
	
		cobra_ipc_emb = (COBRA_IPC_EMB_TypeDef *)COBRA_IPC_EMB_BASE;
}

/*
The IPC enables the cortex-M3 to assert up to thirty interrupt sources towards the cortex-M0

Written bit to 1'b1 asserts the corresponding application to embedded IPC interrupt source.
*/
uint32_t cobra_ipc_m3_app2emb_trigger_get(void)
{
    return cobra_ipc_host->app2emb_trigger;
}

void cobra_ipc_m3_app2emb_trigger_set(uint32_t value)
{
    cobra_ipc_host->app2emb_trigger = value;
}

/*
The register reports the active interrupt sources triggered by the M0 processor. 
This register is not affected by the mask applied on the interrupt sources.
The cortex-M0 can assert up to eight interrupt sources towards the cortex-M3.


A bit set to 1'b1 indicates that the corresponding interrupt source is pending.

*/
uint32_t cobra_ipc_m3_emb2app_rawstatus_get(void)
{
    return cobra_ipc_host->emb2app_rawstatus;
}


/*
The register enables cortex-M3 to acknowledge the interrupt sources 
asserted by cortex-M0

A written bit set to 1'b1 acknowledges the corresponding interrupt pending source.
*/
uint32_t cobra_ipc_m3_emb2app_ack_get(void)
{
    return cobra_ipc_host->emb2app_ack;
}

void cobra_ipc_m3_emb2app_ack_clear(uint32_t value)
{
    cobra_ipc_host->emb2app_ack = value;
}


/*
The register enables the cortex-M3 to un-mask interrupt sources from cortex-M0

A written bit set to 1'b1 un-masks the corresponding interrupt source.

*/
uint32_t cobra_ipc_m3_emb2app_unmask_get(void)
{
    return cobra_ipc_host->emb2app_unmask_set;
}

//enable interrupt
void cobra_ipc_m3_emb2app_unmask_set(uint32_t value)
{
    cobra_ipc_host->emb2app_unmask_set = value;
}

/*
The register enables the cortex-M3 to mask interrupt sources from the cortex-M0.

A written bit set to 1'b1 masks the corresponding interrupt source.
*/
void cobra_ipc_m3_emb2app_unmask_clear(uint32_t value)
{
    cobra_ipc_host->emb2app_unmask_clear = value;
}


/*

The register reports the active and un-masked interrupt sources triggered by the cortex-M0. 
The cortex-M0 can assert up to eight interrupt sources towards the host processor.

*/

uint32_t cobra_ipc_m3_emb2app_status_get(void)
{
    return cobra_ipc_host->emb2app_status;
}

//get 0x49504332
uint32_t cobra_ipc_m3_app_signature_get(void)
{
    return cobra_ipc_host->app_signature;
}


//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//cortex-M0 emb 

/*
The IPC enables cortex-M0 to assert up to thirty interrupt sources towards the cortex-M3.

Written bit to 1'b1 asserts the corresponding embedded to application IPC interrupt source.

*/
uint32_t cobra_ipc_m0_emb2app_trigger_get(void)
{
    return cobra_ipc_emb->emb2app_trigger;
}

void cobra_ipc_m0_emb2app_trigger_set(uint32_t value)
{
    cobra_ipc_emb->emb2app_trigger = value;
}


/*
The register reports what are the active interrupt sources triggered by the cortex-M3.
This register is not affected by the mask applied to interrupt sources. 
The cortex-M3 can assert up to 32 interrupt sources towards the cortex-M0.

//A bit set to 1'b1 indicates that the corresponding interrupt source is pending.
*/
uint32_t cobra_ipc_m0_app2emb_rawstatus_get(void)
{
    return cobra_ipc_emb->app2emb_rawstatus;
}


/*
The register enables the cortex-M0 to acknowledge the interrupt sources asserted by cortex-M3

*/
uint32_t cobra_ipc_m0_app2emb_ack_get(void)
{
    return cobra_ipc_emb->app2emb_ack;
}

//A written bit set to 1'b1 acknowledges the corresponding interrupt pending source.
void cobra_ipc_m0_app2emb_ack_clear(uint32_t value)
{
    cobra_ipc_emb->app2emb_ack = value;
}

/*

The register enables cortex-M0 to un-mask interrupt sources from the cortex-M3
A written bit set to 1'b1 un-masks the corresponding interrupt source.
*/
uint32_t cobra_ipc_m0_app2emb_unmask_get(void)
{
    return cobra_ipc_emb->app2emb_unmask_set;
}

void cobra_ipc_m0_app2emb_unmask_set(uint32_t value)
{
    cobra_ipc_emb->app2emb_unmask_set = value;
}

/*
The register enables cortex-M0 to mask interrupt sources from cortex-M3

A written bit set to 1'b1 masks the corresponding interrupt source.

*/
void cobra_ipc_m0_app2emb_unmask_clear(uint32_t value)
{
    cobra_ipc_emb->app2emb_unmask_clear = value;
}


/*
The register enables the cortex-M0 to assign the lower sixteen interrupt sources 
to the four IPC interrupt lines ipc_irq[3:0].

*/
uint32_t cobra_ipc_m0_app2emb_line_sel_low_get(void)
{
    return cobra_ipc_emb->app2emb_line_sel_low;
}

void cobra_ipc_m0_app2emb_line_sel_low_set(uint32_t value)
{
    cobra_ipc_emb->app2emb_line_sel_low = value;
}

/*
The register enables cortex-M0 to assign the upper sixteen interrupt sources 

to the four IPC interrupt lines ipc_irq[3:0].
*/
uint32_t cobra_ipc_m0_app2emb_line_sel_high_get(void)
{
    return cobra_ipc_emb->app2emb_line_sel_high;
}

void cobra_ipc_m0_app2emb_line_sel_high_set(uint32_t value)
{
    cobra_ipc_emb->app2emb_line_sel_high = value;
}

/*

The register reports what are the active and un-masked interrupt sources triggered by cortex-M3. 

The cortex-M3 can assert up to 31 interrupt sources towards the cortex-M0

*/
uint32_t cobra_ipc_m0_app2emb_status_get(void)
{
    return cobra_ipc_emb->app2emb_status;
}

//0x49504332
uint32_t cobra_ipc_m0_emb_signature_get(void)
{
    return cobra_ipc_emb->emb_signature;
}


void cobra_ipc_m0_set_line_and_trigger_interrupt(unsigned int line_number, IPC_IRQ_NUMBER irq)
{
    if(line_number >= 32) return;
		if(line_number <= 15)
		{
				cobra_ipc_emb->app2emb_line_sel_low &= ~(0x03 << 2*line_number);
				cobra_ipc_emb->app2emb_line_sel_low |= irq << 2*line_number;			
		}
		else
		{
				cobra_ipc_emb->app2emb_line_sel_high &= ~(0x03 << 2*(line_number - 16));
				cobra_ipc_emb->app2emb_line_sel_high |= (irq << 2*(line_number - 16));		
		}
}



#endif














