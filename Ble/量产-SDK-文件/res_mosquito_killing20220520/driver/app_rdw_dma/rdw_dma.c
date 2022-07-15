/*
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	//liujian 2018/2/16
*/	
#include "rdw_mcu.h"
#include "debug.h"
#include "rdw_dma.h"
#include "cobra_udma.h"
#include "cobra_pcrm.h"

#define MEMORY_DMA_COPY_TEST		0
#define UART_DMA_TEST						0


/*
DMA Channel  	Peripheral 0   	Peripheral 1    Peripheral 2  	Peripheral  		Mode waitonreq 
0  			 		 	Software  			IPC_IRQ  				SHA_INT  				AON_EVT0  
1  			 			Software  			BT_SIRQ  				GP_ADC  				AON_EVT2  
2  			 			UART0_RX  			UART10_RX  			PDM0  					SPIF_RX  
3  			 			UART0_TX  			UART10_TX  			PDM2PCM  				SPIF_TX   
4  			 			SPI0_RX  				UART11_RX  			AON_EVT0  			DTimer0  
5  			 			SPI0_TX  				UART11_TX  			AON_EVT2  			DTimer1   
6  			 			I2C0_RX  				SHA_INT  				AES0_RX    		 	------
7  			 			I2C0_TX  				GP_ADC  				AES0_TX   			------
8  			 			UART1_RX  			Timer0  				AON_EVT4  			AES0_RX   
9  			 			UART1_TX  			Timer1  				AON_EVT3  			AES0_TX 
10  		 			SPI1_RX  				Timer0  				AON_EVT4   			-------
11  		 			SPI1_TX  				Timer1  				AON_EVT3   			-------
12  		 			I2C1_RX  				Timer0  				AON_EVT4  			UART10_RX   
13  		 			I2C1_TX  				Timer1  				AON_EVT3  			UART10_TX    
14  		 			I2S0_RX  				AON_EVT4  			IPC_IRQ  				UART11_RX    
15  		 			I2S0_TX  				AON_EVT3  			BT_SIRQ  				UART11_TX  
16  		 			I2S1_RX  				AON_EVT4  			DTimer0   			--------
17  		 			I2S1_TX  				AON_EVT3  			DTimer1    			--------
18  		 			SPIF_RX  				AON_EVT4  			PDM0   					--------
19  		 			SPIF_TX  				AON_EVT3  			PDM2PCM  				--------

*/
#if COBRA_UDMA_EN

void (*g_pfnDMAVectors[MAX_CHANNEL_NUMBER])(void);

#if (COBRA_POWER_DOWN_ENABLE == 1)
uint8_t ui8ControlTable[1024] __attribute__  ((section("retention_area"),zero_init)); //((aligned(1024)))

#else
uint8_t ui8ControlTable[1024] __attribute__ ((aligned(1024)));
#endif

//static int patch_dma = 0;

void app_rwd_udma_reset(void)
{
	
	cobra_pcrm_udma_soft_reset();
	__nop();
	__nop();
	__nop();
	__nop();
	
}

void app_rdw_udma_init(void)
{
	
		char i;
	
		app_rwd_udma_reset();
		
		cobra_udma_enable();
		
		cobra_umda_set_control_base(ui8ControlTable);           // 设置 DMA control Base address
		
		for(i = 0; i < 32; i++)
		{
				cobra_udma_channel_attribute_enable(i, UDMA_ATTR_REQMASK);
				//cobra_udma_enable_channel(i);
			  cobra_udma_disable_channel(i);
				cobra_udma_clear_interrupt(i);
		}
		
		for(i = 0; i < MAX_CHANNEL_NUMBER; i++)
		{
				g_pfnDMAVectors[i] = NULL;
		}


//		patch_dma = 0;
			
}    




void app_rdw_udma_register_callback(unsigned long ulIntChannel, void (*pfnHandler)(void))
{
		g_pfnDMAVectors[ulIntChannel] = pfnHandler;
	
}

void app_rdw_udma_unregister_callback(unsigned long ulIntChannel)
{

		g_pfnDMAVectors[ulIntChannel] = NULL;
}


void cobra_m3_dma_interrupt(void)
{
		uint32_t  i;
		
		for(i = 0; i < MAX_CHANNEL_NUMBER; i++)
		{
				if(cobra_udma_get_status()& (1<< i))
				{
						if(g_pfnDMAVectors[i] != NULL)
						{
								(*g_pfnDMAVectors[i])();
						}
						cobra_udma_clear_interrupt(i);
						
				}
		}

			
}




/*
input: ch : CHANNEL0, CHANNEL1, CHANNEL2 ... CHANNEL19
	   ch_usage:  enum channel_usage


*/

char app_rdw_udma_get_channel(char ch, uint8_t usage)
{
	 char temp_ch = -1;
	 uint32_t * p_ctrl = (uint32_t *)(DMA_CHANNEL_SEL_CONTROL_BASE);

	 switch(ch)
	 {
//channel 0 		 
		 case CHANNEL0:
		 {
			 switch (usage)
			 {
				case CH0_SOFTWARE:
					temp_ch = CHANNEL0;
					*(p_ctrl + CHANNEL0) = 0x00;
					break;
				case CH0_IPC_IRQ:
					temp_ch = CHANNEL0;
					*(p_ctrl + CHANNEL0) = 0x01;
					break;
				case CH0_SHA_INT:
					temp_ch = CHANNEL0;
					*(p_ctrl + CHANNEL0) = 0x02;
					break;
				case CH0_AON_EVT0:
					temp_ch = CHANNEL0;
					*(p_ctrl + CHANNEL0) = 0x03;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 1		 
		 case CHANNEL1:
		 {
			 switch (usage)
			 {
				case CH1_SOFTWARE:
					temp_ch = CHANNEL1;
					*(p_ctrl + CHANNEL1) = 0;
					break;
				case CH1_BT_SIRQ:
					temp_ch = CHANNEL1;
					*(p_ctrl + CHANNEL1) = 1;
					break;
				case CH1_GP_ADC:
					temp_ch = CHANNEL1;
					*(p_ctrl + CHANNEL1) = 2;
					break;
				case CH1_AON_EVT2:
					temp_ch = CHANNEL1;
					*(p_ctrl + CHANNEL1) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 2		 
		 case CHANNEL2:
		 {
			 switch (usage)
			 {
				case CH2_UART0_RX:
					temp_ch = CHANNEL2;
					*(p_ctrl + CHANNEL2) = 0;
					break;
				case CH2_UART10_RX:
					temp_ch = CHANNEL2;
					*(p_ctrl + CHANNEL2) = 1;
					break;
				case CH2_PDM0:
					temp_ch = CHANNEL2;
					*(p_ctrl + CHANNEL2) = 2;
					break;
				case CH2_SPIF_RX:
					temp_ch = CHANNEL2;
					*(p_ctrl + CHANNEL2) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 3		 
		 case CHANNEL3:
		 {
			 switch (usage)
			 {
				case CH3_UART0_TX:
					temp_ch = CHANNEL3;
					*(p_ctrl + CHANNEL3) = 0;
					break;
				case CH3_UART10_TX:
					temp_ch = CHANNEL3;
					*(p_ctrl + CHANNEL3) = 1;
					break;
				case CH3_PDM2PCM:
					temp_ch = CHANNEL3;
					*(p_ctrl + CHANNEL3) = 2;
					break;
				case CH3_SPIF_TX:
					temp_ch = CHANNEL3;
					*(p_ctrl + CHANNEL3) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 4		 
		 case CHANNEL4:
		 {
			 switch (usage)
			 {
				case CH4_SPI0_RX:
					temp_ch = CHANNEL4;
					*(p_ctrl + CHANNEL4) = 0;
					break;
				case CH4_UART11_RX:
					temp_ch = CHANNEL4;
					*(p_ctrl + CHANNEL4) = 1;
					break;
				case CH4_AON_EVT0:
					temp_ch = CHANNEL4;
					*(p_ctrl + CHANNEL4) = 2;
					break;
				case CH4_DTIMER0:
					temp_ch = CHANNEL4;
					*(p_ctrl + CHANNEL4) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 5		 
		 case CHANNEL5:
		 {
			 switch (usage)
			 {
				case CH5_SPI0_TX:
					temp_ch = CHANNEL5;
					*(p_ctrl + CHANNEL5) = 0;
					break;
				case CH5_UART11_TX:
					temp_ch = CHANNEL5;
					*(p_ctrl + CHANNEL5) = 1;
					break;
				case CH5_AON_EVT2:
					temp_ch = CHANNEL5;
					*(p_ctrl + CHANNEL5) = 2;
					break;
				case CH5_DTIMER1:
					temp_ch = CHANNEL5;
					*(p_ctrl + CHANNEL5) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }		 
//channel 6		 
		 case CHANNEL6:
		 {
			 switch (usage)
			 {
				case CH6_I2C0_RX:
					temp_ch = CHANNEL6;
					*(p_ctrl + CHANNEL6) = 0;
					break;
				case CH6_SHA_INT:
					temp_ch = CHANNEL6;
					*(p_ctrl + CHANNEL6) = 1;
					break;
				case CH6_AES0_RX:
					temp_ch = CHANNEL6;
					*(p_ctrl + CHANNEL6) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }		 
//channel 7		 
		 case CHANNEL7:
		 {
			 switch (usage)
			 {
				case CH7_I2C0_TX:
					temp_ch = CHANNEL7;
					*(p_ctrl + CHANNEL7) = 0;
					break;
				case CH7_GP_ADC:
					temp_ch = CHANNEL7;
					*(p_ctrl + CHANNEL7) = 1;
					break;
				case CH7_AES0_TX:
					temp_ch = CHANNEL7;
					*(p_ctrl + CHANNEL7) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }		 
//channel 8		 
		 case CHANNEL8:
		 {
			 switch (usage)
			 {
				case CH8_UART1_RX:
					temp_ch = CHANNEL8;
					*(p_ctrl + CHANNEL8) = 0;
					break;
				case CH8_TIMER0:
					temp_ch = CHANNEL8;
					*(p_ctrl + CHANNEL8) = 1;
					break;
				case CH8_AON_EVT4:
					temp_ch = CHANNEL8;
					*(p_ctrl + CHANNEL8) = 2;
					break;
				case CH8_AES0_RX:
					temp_ch = CHANNEL8;
					*(p_ctrl + CHANNEL8) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }		 

//channel 9		 
		 case CHANNEL9:
		 {
			 switch (usage)
			 {
				case CH9_UART1_TX:
					temp_ch = CHANNEL9;
					*(p_ctrl + CHANNEL9) = 0;
					break;
				case CH9_TIMER1:
					temp_ch = CHANNEL9;
					*(p_ctrl + CHANNEL9) = 1;
					break;
				case CH9_AON_EVT3:
					temp_ch = CHANNEL9;
					*(p_ctrl + CHANNEL9) = 2;
					break;
				case CH9_AES0_TX:
					temp_ch = CHANNEL9;
					*(p_ctrl + CHANNEL9) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 10		 
		 case CHANNEL10:
		 {
			 switch (usage)
			 {
				case CH10_SPI1_RX:
					temp_ch = CHANNEL10;
					*(p_ctrl + CHANNEL10) = 0;
					break;
				case CH10_TIMER0:
					temp_ch = CHANNEL10;
					*(p_ctrl + CHANNEL10) = 1;
					break;
				case CH10_AON_EVT4:
					temp_ch = CHANNEL10;
					*(p_ctrl + CHANNEL10) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 11		 
		 case CHANNEL11:
		 {
			 switch (usage)
			 {
				case CH11_SPI1_TX:
					temp_ch = CHANNEL11;
					*(p_ctrl + CHANNEL11) = 0;
					break;
				case CH11_TIMER1:
					temp_ch = CHANNEL11;
					*(p_ctrl + CHANNEL11) = 1;
					break;
				case CH11_AON_EVT3:
					temp_ch = CHANNEL11;
					*(p_ctrl + CHANNEL11) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 12		 
		 case CHANNEL12:
		 {
			 switch (usage)
			 {
				case CH12_I2C1_RX:
					temp_ch = CHANNEL12;
					*(p_ctrl + CHANNEL12) = 0;
					break;
				case CH12_TIMER0:
					temp_ch = CHANNEL12;
					*(p_ctrl + CHANNEL12) = 1;
					break;
				case CH12_AON_EVT4:
					temp_ch = CHANNEL12;
					*(p_ctrl + CHANNEL12) = 2;
					break;
				case CH12_UART10_RX:
					temp_ch = CHANNEL12;
					*(p_ctrl + CHANNEL12) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 13		 
		 case CHANNEL13:
		 {
			 switch (usage)
			 {
				case CH13_I2C1_TX:
					temp_ch = CHANNEL13;
					*(p_ctrl + CHANNEL13) = 0;
					break;
				case CH13_TIMER1:
					temp_ch = CHANNEL13;
					*(p_ctrl + CHANNEL13) = 1;
					break;
				case CH13_AON_EVT3:
					temp_ch = CHANNEL13;
					*(p_ctrl + CHANNEL13) = 2;
					break;
				case CH13_UART10_TX:
					temp_ch = CHANNEL13;
					*(p_ctrl + CHANNEL13) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 14		 
		 case CHANNEL14:
		 {
			 switch (usage)
			 {
				case CH14_I2S0_RX:
					temp_ch = CHANNEL14;
					*(p_ctrl + CHANNEL14) = 0;
					break;
				case CH14_AON_EVT4:
					temp_ch = CHANNEL14;
					*(p_ctrl + CHANNEL14) = 1;
					break;
				case CH14_IPC_IRQ:
					temp_ch = CHANNEL14;
					*(p_ctrl + CHANNEL14) = 2;
					break;
				case CH14_UART11_RX:
					temp_ch = CHANNEL14;
					*(p_ctrl + CHANNEL14) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 15		 
		 case CHANNEL15:
		 {
			 switch (usage)
			 {
				case CH15_I2S0_TX:
					temp_ch = CHANNEL15;
					*(p_ctrl + CHANNEL15) = 0;
					break;
				case CH15_AON_EVT3:
					temp_ch = CHANNEL15;
					*(p_ctrl + CHANNEL15) = 1;
					break;
				case CH15_BT_SIRQ:
					temp_ch = CHANNEL15;
					*(p_ctrl + CHANNEL15) = 2;
					break;
				case CH15_UART11_TX:
					temp_ch = CHANNEL15;
					*(p_ctrl + CHANNEL15) = 3;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 16
		 case CHANNEL16:
		 {
			 switch (usage)
			 {
				case CH16_I2S1_RX:
					temp_ch = CHANNEL16;
					*(p_ctrl + CHANNEL16) = 0;
					break;
				case CH16_AON_EVT4:
					temp_ch = CHANNEL16;
					*(p_ctrl + CHANNEL16) = 1;
					break;
				case CH16_DTIMER0:
					temp_ch = CHANNEL16;
					*(p_ctrl + CHANNEL16) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 17		 
		 case CHANNEL17:
		 {
			 switch (usage)
			 {
				case CH17_I2S1_TX:
					temp_ch = CHANNEL17;
					*(p_ctrl + CHANNEL17) = 0;
					break;
				case CH17_AON_EVT3:
					temp_ch = CHANNEL17;
					*(p_ctrl + CHANNEL17) = 1;
					break;
				case CH17_DTIMER1:
					temp_ch = CHANNEL17;
					*(p_ctrl + CHANNEL17) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }
//channel 18		 
		 case CHANNEL18:
		 {
			 switch (usage)
			 {
				case CH18_SPIF_RX:
					temp_ch = CHANNEL18;
					*(p_ctrl + CHANNEL18) = 0;
					break;
				case CH18_AON_EVT4:
					temp_ch = CHANNEL18;
					*(p_ctrl + CHANNEL18) = 1;
					break;
				case CH18_PDM0:
					temp_ch = CHANNEL18;
					*(p_ctrl + CHANNEL18) = 2;
					break;
			
				default:
					break;
			 }
			 break;
		 }
//channel 19	 
		 case CHANNEL19:
		 {
			 switch (usage)
			 {
				case CH19_SPIF_RX:
					temp_ch = CHANNEL19;
					*(p_ctrl + CHANNEL19) = 0;
					break;
				case CH19_AON_EVT3:
					temp_ch = CHANNEL19;
					*(p_ctrl + CHANNEL19) = 1;
					break;
				case CH19_PDM2PCM:
					temp_ch = CHANNEL19;
					*(p_ctrl + CHANNEL19) = 2;
					break;
				default:
					break;
			 }
			 break;
		 }
		default:
			break;
	 }
	 
	 return temp_ch;
	
}




#if MEMORY_DMA_COPY_TEST
//liujian test
#define MEM_BUFFER_SIZE         128
static uint32_t g_ui32SrcBuf[MEM_BUFFER_SIZE];
static uint32_t g_ui32DstBuf[MEM_BUFFER_SIZE];

char sw_channel;

void InitSWTransfer(void)
{
			sw_channel = app_rdw_udma_get_channel(CHANNEL0, CH0_SOFTWARE);
	
			if(sw_channel < 0) return;
				

	
			NVIC_EnableIRQ(DMADONE_IRQn);   //DMA   中断打开

			//
			// Put the attributes in a known state for the uDMA software channel.
			// These should already be disabled by default.
			//
			cobra_udma_channel_attribute_disable(sw_channel,
                                    UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT |
                                    (UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK));

    //
    // Configure the control parameters for the SW channel.  The SW channel
    // will be used to transfer between two memory buffers, 32 bits at a time.
    // Therefore the data size is 32 bits, and the address increment is 32 bits
    // for both source and destination.  The arbitration size will be set to 8,
    // which causes the uDMA controller to rearbitrate after 8 items are
    // transferred.  This keeps this channel from hogging the uDMA controller
    // once the transfer is started, and allows other channels cycles if they
    // are higher priority.
    //
    cobra_udma_set_channel_control(sw_channel | UDMA_PRI_SELECT,
                              UDMA_SIZE_32 | UDMA_SRC_INC_32 | UDMA_DST_INC_32 |
                              UDMA_ARB_8);

    //
    // Set up the transfer parameters for the software channel.  This will
    // configure the transfer buffers and the transfer size.  Auto mode must be
    // used for software transfers.
    //
    cobra_udma_set_channel_transfer(sw_channel | UDMA_PRI_SELECT,
                               UDMA_MODE_AUTO, g_ui32SrcBuf, g_ui32DstBuf,
                               MEM_BUFFER_SIZE);

    //
    // Now the software channel is primed to start a transfer.  The channel
    // must be enabled.  For software based transfers, a request must be
    // issued.  After this, the uDMA memory transfer begins.
    //
    cobra_udma_enable_channel(sw_channel);
    cobra_udma_channel_request(sw_channel);
}
#endif



#if UART_DMA_TEST

#include "cobra_uart.h"
#define UART_TXBUF_SIZE         128
#define UART_RXBUF_SIZE         128

static uint8_t g_ui8TxBuf[UART_TXBUF_SIZE];
static uint8_t g_ui8RxBufA[UART_RXBUF_SIZE];
static uint8_t g_ui8RxBufB[UART_RXBUF_SIZE];

char uart0_rx_ch;
char uart0_tx_ch;

//static volatile RDW_UART_T *m3_uart=UART0;

void uart_dma_callback(void)
{
		  uint32_t ui32Status;
			uint32_t ui32Mode;

    //
    // Read the interrupt status of the UART.
    //
   // ui32Status = ROM_UARTIntStatus(UART1_BASE, 1);

    //
    // Clear any pending status, even though there should be none since no UART
    // interrupts were enabled.  If UART error interrupts were enabled, then
    // those interrupts could occur here and should be handled.  Since uDMA is
    // used for both the RX and TX, then neither of those interrupts should be
    // enabled.
    //
   // ROM_UARTIntClear(UART1_BASE, ui32Status);

    //
    // Check the DMA control table to see if the ping-pong "A" transfer is
    // complete.  The "A" transfer uses receive buffer "A", and the primary
    // control structure.
    //
    ui32Mode = cobra_udma_get_channel_mode(uart0_rx_ch | UDMA_PRI_SELECT);

    //
    // If the primary control structure indicates stop, that means the "A"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "B" buffer.
    //
    if(ui32Mode == UDMA_MODE_STOP)
    {
        //
        // Increment a counter to indicate data was received into buffer A.  In
        // a real application this would be used to signal the main thread that
        // data was received so the main thread can process the data.
        //
     //   g_ui32RxBufACount++;

        //
        // Set up the next transfer for the "A" buffer, using the primary
        // control structure.  When the ongoing receive into the "B" buffer is
        // done, the uDMA controller will switch back to this one.  This
        // example re-uses buffer A, but a more sophisticated application could
        // use a rotating set of buffers to increase the amount of time that
        // the main thread has to process the data in the buffer before it is
        // reused.
        //
        cobra_udma_set_channel_transfer(uart0_rx_ch | UDMA_PRI_SELECT,
                                   UDMA_MODE_PINGPONG,
                                   (void *)cobra_uart_get_rx_address(M3_UART0),
                                   g_ui8RxBufA, sizeof(g_ui8RxBufA));
																	 
				printf("%d \n", cobra_udma_channel_is_enable(uart0_rx_ch));
//			 uDMAChannelEnable(uart0_rx_ch);
    }

    //
    // Check the DMA control table to see if the ping-pong "B" transfer is
    // complete.  The "B" transfer uses receive buffer "B", and the alternate
    // control structure.
    //
    ui32Mode = cobra_udma_get_channel_mode(uart0_rx_ch | UDMA_ALT_SELECT);

    //
    // If the alternate control structure indicates stop, that means the "B"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "A" buffer.
    //
    if(ui32Mode == UDMA_MODE_STOP)
    {
        //
        // Increment a counter to indicate data was received into buffer A.  In
        // a real application this would be used to signal the main thread that
        // data was received so the main thread can process the data.
        //
      //  g_ui32RxBufBCount++;

        //
        // Set up the next transfer for the "B" buffer, using the alternate
        // control structure.  When the ongoing receive into the "A" buffer is
        // done, the uDMA controller will switch back to this one.  This
        // example re-uses buffer B, but a more sophisticated application could
        // use a rotating set of buffers to increase the amount of time that
        // the main thread has to process the data in the buffer before it is
        // reused.
        //
        cobra_udma_set_channel_transfer(uart0_rx_ch | UDMA_ALT_SELECT,
                                   UDMA_MODE_PINGPONG,
                                   (void *)cobra_uart_get_rx_address(M3_UART0),
                                   g_ui8RxBufB, sizeof(g_ui8RxBufB));
																	 
		//		uDMAChannelEnable(uart0_rx_ch);
					printf("%d \n", cobra_udma_channel_is_enable(uart0_rx_ch));
																	 
    }

    //
    // If the UART1 DMA TX channel is disabled, that means the TX DMA transfer
    // is done.
    //
    if(!cobra_udma_channel_is_enable(uart0_tx_ch))
    {
        //
        // Start another DMA transfer to UART1 TX.
        //
        cobra_udma_set_channel_transfer(uart0_tx_ch | UDMA_PRI_SELECT,
                                   UDMA_MODE_BASIC, g_ui8TxBuf,
                                   (void *)cobra_uart_get_tx_address(M3_UART0),
                                   sizeof(g_ui8TxBuf));

        //
        // The uDMA TX channel must be re-enabled.
        //
        cobra_udma_enable_channel(uart0_tx_ch);
    }

	
}


uint8_t uart_dma_test(void)
{
			uart0_rx_ch = app_rdw_udma_get_channel(CHANNEL2, CH2_UART0_RX);

    //
    // Put the attributes in a known state for the uDMA UART1RX channel.  These
    // should already be disabled by default.
    //
    cobra_udma_channel_attribute_disable(uart0_rx_ch,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);

    //
    // Configure the control parameters for the primary control structure for
    // the UART RX channel.  The primary contol structure is used for the "A"
    // part of the ping-pong receive.  The transfer data size is 8 bits, the
    // source address does not increment since it will be reading from a
    // register.  The destination address increment is byte 8-bit bytes.  The
    // arbitration size is set to 4 to match the RX FIFO trigger threshold.
    // The uDMA controller will use a 4 byte burst transfer if possible.  This
    // will be somewhat more effecient that single byte transfers.
    //
    cobra_udma_set_channel_control(uart0_rx_ch | UDMA_PRI_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 |
                              UDMA_ARB_1);

    //
    // Configure the control parameters for the alternate control structure for
    // the UART RX channel.  The alternate contol structure is used for the "B"
    // part of the ping-pong receive.  The configuration is identical to the
    // primary/A control structure.
    //
    cobra_udma_set_channel_control(uart0_rx_ch | UDMA_ALT_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 |
                              UDMA_ARB_1);

    //
    // Set up the transfer parameters for the UART RX primary control
    // structure.  The mode is set to ping-pong, the transfer source is the
    // UART data register, and the destination is the receive "A" buffer.  The
    // transfer size is set to match the size of the buffer.
    //
    cobra_udma_set_channel_transfer(uart0_rx_ch | UDMA_PRI_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)cobra_uart_get_rx_address(M3_UART0),
                               g_ui8RxBufA, sizeof(g_ui8RxBufA));

    //
    // Set up the transfer parameters for the UART RX alternate control
    // structure.  The mode is set to ping-pong, the transfer source is the
    // UART data register, and the destination is the receive "B" buffer.  The
    // transfer size is set to match the size of the buffer.
    //
    cobra_udma_set_channel_transfer(uart0_rx_ch | UDMA_ALT_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)cobra_uart_get_rx_address(M3_UART0),
                               g_ui8RxBufB, sizeof(g_ui8RxBufB));

    //
    // Put the attributes in a known state for the uDMA UART1TX channel.  These
    // should already be disabled by default.
    //
    cobra_udma_channel_attribute_disable(uart0_tx_ch,
                                    UDMA_ATTR_ALTSELECT |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);

    //
    // Set the USEBURST attribute for the uDMA UART TX channel.  This will
    // force the controller to always use a burst when transferring data from
    // the TX buffer to the UART.  This is somewhat more effecient bus usage
    // than the default which allows single or burst transfers.
    //
    cobra_udma_channel_attribute_enable(uart0_tx_ch, UDMA_ATTR_USEBURST);

    //
    // Configure the control parameters for the UART TX.  The uDMA UART TX
    // channel is used to transfer a block of data from a buffer to the UART.
    // The data size is 8 bits.  The source address increment is 8-bit bytes
    // since the data is coming from a buffer.  The destination increment is
    // none since the data is to be written to the UART data register.  The
    // arbitration size is set to 4, which matches the UART TX FIFO trigger
    // threshold.
    //
    cobra_udma_set_channel_control(uart0_tx_ch | UDMA_PRI_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
                              UDMA_ARB_1);

    //
    // Set up the transfer parameters for the uDMA UART TX channel.  This will
    // configure the transfer source and destination and the transfer size.
    // Basic mode is used because the peripheral is making the uDMA transfer
    // request.  The source is the TX buffer and the destination is the UART
    // data register.
    //
    cobra_udma_set_channel_transfer(uart0_tx_ch | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC, g_ui8TxBuf,
                               (void *)cobra_uart_get_tx_address(M3_UART0),
                               sizeof(g_ui8TxBuf));

    //
    // Now both the uDMA UART TX and RX channels are primed to start a
    // transfer.  As soon as the channels are enabled, the peripheral will
    // issue a transfer request and the data transfers will begin.
    //
    cobra_udma_enable_channel(uart0_rx_ch);
   // uDMAChannelEnable(uart0_tx_ch);
															 
		app_rdw_udma_register_callback(uart0_rx_ch, uart_dma_callback);
		
		cobra_udma_enable_interrupt(CHANNEL2);	
															 
		NVIC_EnableIRQ(DMADONE_IRQn);      //DMA   中断打开
}



#endif




/*
void DMADONE_IRQHandler(void)
{
    
		if(!patch_dma)
		{
				patch_dma = 1;
				return;
		}
    //
    // Check for the primary control structure to indicate complete.
    //
#if MEMORY_DMA_COPY_TEST
		uint32_t ui32Mode; 
		ui32Mode = uDMAChannelModeGet(sw_channel);
    if(ui32Mode == UDMA_MODE_STOP)
    {
        //
        // Configure it for another transfer.
        //
        uDMAChannelTransferSet(sw_channel, UDMA_MODE_AUTO,
                                     g_ui32SrcBuf, g_ui32DstBuf,
                                     MEM_BUFFER_SIZE);

        //
        // Initiate another transfer.
        //
        uDMAChannelEnable(sw_channel);
        uDMAChannelRequest(sw_channel);
			
			 printf("DMADONE_IRQHandler \n");
    }
#endif
		
#if UART_DMA_TEST
		  uint32_t ui32Status;
			uint32_t ui32Mode;

    //
    // Read the interrupt status of the UART.
    //
   // ui32Status = ROM_UARTIntStatus(UART1_BASE, 1);

    //
    // Clear any pending status, even though there should be none since no UART
    // interrupts were enabled.  If UART error interrupts were enabled, then
    // those interrupts could occur here and should be handled.  Since uDMA is
    // used for both the RX and TX, then neither of those interrupts should be
    // enabled.
    //
   // ROM_UARTIntClear(UART1_BASE, ui32Status);

    //
    // Check the DMA control table to see if the ping-pong "A" transfer is
    // complete.  The "A" transfer uses receive buffer "A", and the primary
    // control structure.
    //
    ui32Mode = uDMAChannelModeGet(uart0_rx_ch | UDMA_PRI_SELECT);

    //
    // If the primary control structure indicates stop, that means the "A"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "B" buffer.
    //
    if(ui32Mode == UDMA_MODE_STOP)
    {
        //
        // Increment a counter to indicate data was received into buffer A.  In
        // a real application this would be used to signal the main thread that
        // data was received so the main thread can process the data.
        //
     //   g_ui32RxBufACount++;

        //
        // Set up the next transfer for the "A" buffer, using the primary
        // control structure.  When the ongoing receive into the "B" buffer is
        // done, the uDMA controller will switch back to this one.  This
        // example re-uses buffer A, but a more sophisticated application could
        // use a rotating set of buffers to increase the amount of time that
        // the main thread has to process the data in the buffer before it is
        // reused.
        //
        uDMAChannelTransferSet(uart0_rx_ch | UDMA_PRI_SELECT,
                                   UDMA_MODE_PINGPONG,
                                   (void *)&(m3_uart->RX_DATA),
                                   g_ui8RxBufA, sizeof(g_ui8RxBufA));
				printf("%d \n", uDMAChannelIsEnabled(uart0_rx_ch));
//			 uDMAChannelEnable(uart0_rx_ch);
    }

    //
    // Check the DMA control table to see if the ping-pong "B" transfer is
    // complete.  The "B" transfer uses receive buffer "B", and the alternate
    // control structure.
    //
    ui32Mode = uDMAChannelModeGet(uart0_rx_ch | UDMA_ALT_SELECT);

    //
    // If the alternate control structure indicates stop, that means the "B"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "A" buffer.
    //
    if(ui32Mode == UDMA_MODE_STOP)
    {
        //
        // Increment a counter to indicate data was received into buffer A.  In
        // a real application this would be used to signal the main thread that
        // data was received so the main thread can process the data.
        //
      //  g_ui32RxBufBCount++;

        //
        // Set up the next transfer for the "B" buffer, using the alternate
        // control structure.  When the ongoing receive into the "A" buffer is
        // done, the uDMA controller will switch back to this one.  This
        // example re-uses buffer B, but a more sophisticated application could
        // use a rotating set of buffers to increase the amount of time that
        // the main thread has to process the data in the buffer before it is
        // reused.
        //
        uDMAChannelTransferSet(uart0_rx_ch | UDMA_ALT_SELECT,
                                   UDMA_MODE_PINGPONG,
                                   (void *)&(m3_uart->RX_DATA),
                                   g_ui8RxBufB, sizeof(g_ui8RxBufB));
																	 
		//		uDMAChannelEnable(uart0_rx_ch);
					printf("%d \n", uDMAChannelIsEnabled(uart0_rx_ch));
																	 
    }

    //
    // If the UART1 DMA TX channel is disabled, that means the TX DMA transfer
    // is done.
    //
    if(!uDMAChannelIsEnabled(uart0_tx_ch))
    {
        //
        // Start another DMA transfer to UART1 TX.
        //
        uDMAChannelTransferSet(uart0_tx_ch | UDMA_PRI_SELECT,
                                   UDMA_MODE_BASIC, g_ui8TxBuf,
                                   (void *)&(m3_uart->TX_DATA),
                                   sizeof(g_ui8TxBuf));

        //
        // The uDMA TX channel must be re-enabled.
        //
        uDMAChannelEnable(uart0_tx_ch);
    }



#endif		
		

}

*/
#endif



