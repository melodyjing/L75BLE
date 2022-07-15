;/**************************************************************************//**
; * @file     startup_ARMCM4.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM4 Device Series
; * @version  V5.00
; * @date     02. March 2016
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x0000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x000001000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD		cobra_aon_event0_interrupt			    ;  0     aon_event0               0
                DCD		cobra_m4_gpio_interrupt					; 1    cobra4 GPIO interrupt			;
                DCD		0
                DCD		0
                DCD		0
                DCD		0
                DCD		0													;6
                DCD		0													;7				
                DCD		0								;		;8
                DCD		0													;9
                DCD		cobra_m4_uart0_interrupt									;10
                DCD		0
                DCD		0
                DCD		0								;  ;13
                DCD		cobra_m3_spi0_interrupt												
                DCD		0								;			;15
                DCD		0								;			;16
                DCD		cobra_aon_event3_interrupt								;			;17
                DCD		cobra_aon_event4_interrupt								;			;18
                DCD		0								;			;19
                DCD		0								;			;20
                DCD		0				;					;21
                DCD		cobra4_aes_sm4_interrupt								;			;22
                DCD		cobra_m3_trng_interrupt			;          ;23
                DCD		cobra_m3_timer0_interrupt								;          ;24
                DCD		cobra_m3_timer1_interrupt								;          ;25
                DCD		cobra_dualtimer_interrupt								;          ;26
                DCD		0				          								;27
                DCD		cobra_m3_spi1_interrupt								; 28
                DCD		cobra_m3_eccmicro_interrupt													; 29
                DCD		0								;			;30
                DCD		rwble_isr								;			;31
				DCD		cobra_m4_uart1_interrupt			   										;32
				DCD		cobra_m3_dma_interrupt													;33
				DCD		0																;34
				DCD		0        										;RF_IRQ_TXFIFO 		
				DCD		0                                               ;RF_IRQ_RXFIFO 
				DCD		cobra_csem_irq_tx_interrupt        					;37   RF_IRQ_TX              
				DCD		cobra_csem_irq_rx_stop_interrupt        		   ;38   RF_IRQ_RXSTOP
				DCD		cobra_csem_irq_received_interupt        			;39   RF_IRQ_RECEIVED
				DCD		cobra_csem_irq_sync_interrupt        				;40   RF_IRQ_SYNC

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
				LDR     R0, =0x420F4509
				LDR     R1, =0x4008A004
				STR		R0, [R1]
				LDR     R0, =0x00000001
				LDR     R1, =0x4008B000
				STR		R0, [R1]
				LDR     R0, =0x00000003
				LDR     R1, =0x4008B064
				STR		R0, [R1]
				LDR     R0, =0x00000000
				LDR     R1, =0x4008B000
				STR		R0, [R1]
				LDR     R0, =0x00000004
				LDR     R1, =0x400C6810
				STR		R0, [R1]	
				LDR     R0, =0x00000080
				LDR     R1, =0x400C6850
				STR		R0, [R1]					
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  cobra_aon_event4_interrupt          [WEAK]
				EXPORT  cobra_m3_dma_interrupt		   		[WEAK]
				EXPORT  cobra_m4_uart0_interrupt	   		[WEAK]
				EXPORT  cobra_m3_gpio0_interrupt			[WEAK]
				EXPORT  cobra_m3_gpio1_interrupt			[WEAK]
				EXPORT  cobra_aon_event3_interrupt		   	[WEAK]
				EXPORT	cobra_m3_timer0_interrupt	   		[WEAK]
				EXPORT	cobra_m3_timer1_interrupt	   		[WEAK]
				EXPORT	cobra_dualtimer_interrupt	   		[WEAK]
				EXPORT	cobra_m3_ipc_interrupt				[WEAK]
				EXPORT	cobra_m3_spi0_interrupt				[WEAK]
				EXPORT	cobra_m3_spi1_interrupt				[WEAK]
				EXPORT	cobra_aon_event0_interrupt			[WEAK]
				EXPORT	cobra_watchdog_interrupt			[WEAK]
				EXPORT	cobra_m3_trng_interrupt				[WEAK]
				EXPORT	cobra_m3_eccmicro_interrupt			[WEAK]
					
				EXPORT	cobra_csem_irq_tx_interrupt			[WEAK]
				EXPORT	cobra_csem_irq_received_interupt	[WEAK]
				EXPORT	cobra_csem_irq_rx_stop_interrupt	[WEAK]
                EXPORT	cobra_csem_irq_sync_interrupt 		[WEAK]
				EXPORT  rwble_isr          					[WEAK]
				EXPORT	cobra_m4_gpio_interrupt				[WEAK]
				EXPORT	cobra_m4_uart1_interrupt			[WEAK]
				EXPORT	cobra4_aes_sm4_interrupt			[WEAK]	

rwble_isr
cobra_m3_gpio0_interrupt
cobra_m3_gpio1_interrupt
cobra_m4_uart0_interrupt
cobra_aon_event4_interrupt
cobra_m3_dma_interrupt
cobra_aon_event3_interrupt
cobra_m3_timer0_interrupt
cobra_m3_timer1_interrupt
cobra_dualtimer_interrupt
cobra_m3_ipc_interrupt
cobra_m3_spi0_interrupt
cobra_m3_spi1_interrupt
cobra_aon_event0_interrupt
cobra_watchdog_interrupt
cobra_m3_trng_interrupt
cobra_m3_eccmicro_interrupt
cobra_csem_irq_received_interupt
cobra_csem_irq_rx_stop_interrupt
cobra_csem_irq_sync_interrupt
cobra_csem_irq_tx_interrupt
cobra_m4_uart1_interrupt
cobra_m4_gpio_interrupt
cobra4_aes_sm4_interrupt
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
