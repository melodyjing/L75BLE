/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_ipc.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/8

	Description :   
	
	Changed Log	:

		Liujian  2019/4/8		- Creation



*****************************************************************************/

#ifndef __COBRA_IPC_H__
#define __COBRA_IPC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_IPC_EN

typedef struct cobra_ipc_host_type
{
			__attribute__ ((aligned (4))) unsigned int  app2emb_trigger;
	
			__attribute__ ((aligned (4))) unsigned int  emb2app_rawstatus;
	
			__attribute__ ((aligned (4))) unsigned int  emb2app_ack;
	
			__attribute__ ((aligned (4))) unsigned int  emb2app_unmask_set;
	
			__attribute__ ((aligned (4))) unsigned int  emb2app_unmask_clear;
	
			__attribute__ ((aligned (4))) unsigned int  reserver1[2];
	
			__attribute__ ((aligned (4))) unsigned int  emb2app_status;
	
			__attribute__ ((aligned (4))) unsigned int  reserver2[8] ;  //0x20
	
			__attribute__ ((aligned (4))) unsigned int  app_signature;   //0x40
			
	
	
}COBRA_IPC_HOST_TypeDef;


typedef struct cobra_ipc_emb_type
{
			__attribute__ ((aligned (4))) unsigned int  emb2app_trigger;
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_rawstatus;
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_ack;
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_unmask_set;
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_unmask_clear;
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_line_sel_low; 
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_line_sel_high; 
	
			__attribute__ ((aligned (4))) unsigned int  app2emb_status;
	
			__attribute__ ((aligned (4))) unsigned int  reserver[8] ;  //0x20
	
			__attribute__ ((aligned (4))) unsigned int  emb_signature;   //0x40
			
	
	
}COBRA_IPC_EMB_TypeDef;

typedef enum {
		IPC_IRQ0,
		IPC_IRQ1,
		IPC_IRQ2,
		IPC_IRQ3
}IPC_IRQ_NUMBER;


#define COBRA_IPC_HOST_BASE				(0x40112000UL)

#define COBRA_IPC_EMB_BASE				(0x40112100UL)

void cobra_ipc_host_init(void);

void cobra_ipc_m3_emb2app_unmask_set(uint32_t value);

void cobra_ipc_m3_app2emb_trigger_set(uint32_t value);

uint32_t cobra_ipc_m3_emb2app_ack_get(void);

void cobra_ipc_m3_emb2app_ack_clear(uint32_t value);

uint32_t cobra_ipc_m3_emb2app_status_get(void);



//EMB
void cobra_ipc_m0_app2emb_line_sel_low_set(uint32_t value);
void cobra_ipc_m0_app2emb_line_sel_high_set(uint32_t value);

void cobra_ipc_m0_app2emb_unmask_set(uint32_t value);
void cobra_ipc_m0_set_line_and_trigger_interrupt(unsigned int line_number, IPC_IRQ_NUMBER irq);

uint32_t cobra_ipc_m0_app2emb_status_get(void);

void cobra_ipc_m0_app2emb_unmask_clear(uint32_t value);
void cobra_ipc_m0_app2emb_ack_clear(uint32_t value);
uint32_t cobra_ipc_m0_app2emb_ack_get(void);
void cobra_ipc_m0_emb2app_trigger_set(uint32_t value);

#endif   //COBRA_IPC_EN



#endif

