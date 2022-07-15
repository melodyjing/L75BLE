;2019/4/15
;cobra startup program
;
;
; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size		EQU     0x1000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size      EQU     0x1000
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

__Vectors       DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     MemoryManagement_Handler       ; Reserved
                DCD     BusFault_Handler               ; Reserved
                DCD     UsageFault_Handler             ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     DebugMon_Handler               ; Reserved
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler

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
                DCD		cobra_m3_uart0_interrupt									;10
                DCD		0
                DCD		0
                DCD		0								;  ;13
                DCD		cobra_m3_spi0_interrupt												
                DCD		0								;			;15
                DCD		0								;			;16
                DCD		0								;			;17
                DCD		0								;			;18
                DCD		0								;			;19
                DCD		0								;			;20
                DCD		0				;					;21
                DCD		0								;			;22
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
				DCD		0        										;CSEM_IRQ_TXFIFO 		
				DCD		0                                               ;CSEM_IRQ_RXFIFO 
				DCD		cobra_csem_irq_tx_interrupt        					;37   CSEM_IRQ_TX              
				DCD		cobra_csem_irq_rx_stop_interrupt        		   ;38   CSEM_IRQ_RXSTOP
				DCD		cobra_csem_irq_received_interupt        			;39   CSEM_IRQ_RECEIVED
				DCD		cobra_csem_irq_sync_interrupt        				;40   CSEM_IRQ_SYNC


__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler routine
Reset_Handler    PROC
                 EXPORT  Reset_Handler                 [WEAK]
        IMPORT  __main
        IMPORT  SystemInit 
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BLX     R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

                
; Dummy Exception Handlers (infinite loops which can be modified)
                IMPORT NMI_HandlerC
NMI_Handler     PROC
                movs r0, #4
                mov r1, lr
                tst r0, r1
                beq NMI_stacking_used_MSP
                mrs r0, psp
                ldr r1,=NMI_HandlerC
                bx r1
NMI_stacking_used_MSP
                mrs r0, msp
                ldr r1,=NMI_HandlerC
                bx r1
                ENDP
					
                IMPORT HardFault_HandlerC
HardFault_Handler\
                PROC
                movs r0, #4
                mov r1, lr
                tst r0, r1
                beq HardFault_stacking_used_MSP
                mrs r0, psp
                ldr r1,=HardFault_HandlerC
                bx r1
HardFault_stacking_used_MSP
                mrs r0, msp
                ldr r1,=HardFault_HandlerC
                bx r1
				ENDP
                IMPORT SVC_HandlerC
SVC_Handler     PROC
				LDR		R0,=SVC_HandlerC
				BX		R0
                ENDP

                IMPORT PendSV_HandlerC
PendSV_Handler  PROC
				LDR		R0,=PendSV_HandlerC
				BX		R0
                ENDP
					
SysTick_Handler PROC
                EXPORT  SysTick_Handler                [WEAK]
                B       .
                ENDP

                IMPORT MemoryManagement_HandlerC
MemoryManagement_Handler  PROC
				LDR		R0,=MemoryManagement_HandlerC
				BX		R0
                ENDP
	
                IMPORT BusFault_HandlerC
BusFault_Handler	PROC	
				LDR		R0,=BusFault_HandlerC
				BX		R0
                ENDP
					
                IMPORT UsageFault_HandlerC
UsageFault_Handler	PROC	
				LDR		R0,=UsageFault_HandlerC
				BX		R0
                ENDP
	
                IMPORT DebugMon_HandlerC
DebugMon_Handler	PROC	
				LDR		R0,=DebugMon_HandlerC
				BX		R0
                ENDP
					

					
					
Default_Handler PROC

                EXPORT  cobra_aon_event4_interrupt          [WEAK]
				EXPORT  cobra_m3_dma_interrupt		   		[WEAK]
				EXPORT  cobra_m3_uart0_interrupt	   		[WEAK]
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

rwble_isr
cobra_m3_gpio0_interrupt
cobra_m3_gpio1_interrupt
cobra_m3_uart0_interrupt
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
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE*****
