/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_irda.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/7

	Description :   
	

	Changed Log	:

		Liujian  2019/3/28		- Creation
        lianbo   2021/2/5       - modified


*****************************************************************************/

#ifndef __COBRA_IRDA_H__
#define __COBRA_IRDA_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_IRDA_EN
typedef struct cobra_irda_type
{
		    __attribute__ ((aligned (4))) volatile unsigned int DEC_D0;         				/*0x000*//*32bit*/
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D1;          			/*0x004*//*32bit*/
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D2;      	  			/*0x008*//*32bit*/        
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D3;  							/*0x00C*//*32bit*/           
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D4;   							/*0x010*//*32bit*/
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D5;     						/*0x014*//*32bit*/ 
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D6;      	  			/*0x018*//*32bit*/
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D7;  							/*0x01C*//*32bit*/      
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D8;      					/*0x020*//*32bit*/ 
			__attribute__ ((aligned (4))) volatile unsigned int DEC_D9;      			  	/*0x024*//*32bit*/ 
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_FILT_THRES;         /*0x028*//*4bit*/
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_MAX_LIFE;       		/*0x02C*//*15bit*/
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_MAX_REC_LEN;        /*0x030*//*7bit*/  
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_DIV;    				    /*0x034*//*4bit*/ 
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_DEC_LEN;    				/*0x038*//*7bit*/ 
	
			__attribute__ ((aligned (4))) volatile unsigned int RX_EN;    							/*0x03C*//*1bit*/ 
			
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_CFG0;    				/*0x040*//*32bit*/ 	
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_CFG1;    				/*0x044*//*32bit*/ 	
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_CFG2;    				/*0x048*//*32bit*/ 	
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_CFG3;    				/*0x04C*//*32bit*/ 
			
			__attribute__ ((aligned (4))) volatile unsigned int TX_EN;    							/*0x050*//*1bit*/ 
			
			__attribute__ ((aligned (4))) volatile unsigned int TX_SWBTN;    					/*0x054*//*1bit*/ 
/*
	 16'b0
     tx_key_ind[5:0],                R
     3'b0,
     tx_keyup_val ,                  RW         //1:key[4:0]取反
     tx_kb_sel[1:0],                 RW         //键盘来源 0:PAD  1:uCPU
     tx_keymode,                     RW
     tx_repeat_en,                   RW
     tx_mode,                        RW
     tx_carrier_sel                  RW

*/
			__attribute__ ((aligned (4))) volatile unsigned int TX_CONFIG;    					/*0x058*//*4bit*/ 
			
/*
     intr_tx_keychanged,
     intr_rx_wakeup  ,  
     intr_rx_done    ,
     intr_tx_done    ,                           //tx_done中断时,还有最后一段数据正在开始发送
     intr_tx_blk_done,
     intr_rx_toolonglife            RW    0x05C Write 1 to clear that bit
*/
			
			__attribute__ ((aligned (4))) volatile unsigned int INTR_STATUS;    						/*0x05C*//*5bit*/ 
/*
	8'b0                   0
    tx_car_rev             23                    //1: carrier取反
    txd_rev                22                    //1: txd取反
    rxd_rev                21                    //1: i_rxd取反
    use_aonpad             20                    //1:rxd使用来自模拟侧rx/tx复用PAD  0:来自独立PAD
    wakeup_code_mask[5:0]  19:14                 //assign o_wakeup_cpu=i_rx_wakeup || 
    wakeup_code[5:0]       13:8                 //((i_flt_en==1'b0 || key_vld) && ((i_tx_key[5:0] & i_wakeup_code_mask)==i_wakeup_code));
    flt_en                 7                    //
    rx_tx_clk_sel[2:0]     6:4                  //内部时钟（o_tx_clk,o_rx_clk,o_rx_ctrl_clk）的分频控制
                                                 //3'd0:   i_clk
                                                 //3'd1:   i_clk/2
                                                 //3'd2:   i_clk/4
                                                 //3'd3:   i_clk/8
                                                 //3'd4:   i_clk/16
                                                 //3'd5:   i_clk/32
                                                 //3'd6:   i_clk/64
                                                 //3'd7:   i_clk/128
    ram_clk_sven           3                     //1: ram clk 允许门控以省电
    tx_clk_en              2                     //1：打开门控时钟
    rx_clk_en              1                     //1：打开门控时钟
    rx_ctrl_clk_en         0                     //1：打开门控时钟
*/
			__attribute__ ((aligned (4))) volatile unsigned int TX_RX_CLK_CONTROL;    //0x60
			
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_DOWN_IND;   		//0x64
//flt_en==1时，对输入按键的滤抖	
			__attribute__ ((aligned (4))) volatile unsigned int TX_KEY_FILT_THRES; 		//0x68
			
} COBRA_IRDA_TypeDef; 



typedef enum {
	 TX_INTERNAL_CARRIER,
	 TX_PWM
}TX_CARRIER;

typedef enum {
	 TX_RAW_MODE,
	 TX_ENCODE_MODE
}TX_MODE;


typedef enum {
	 TX_KEY_DIRECT_MAP,
	 TX_KEY_ENCODE_MAP
}TX_KEYMODE;

typedef enum{
	TX_KEY_NO_UCPU,
	TX_KEY_FROM_UCPU
}TX_KEYSOURCE;

typedef enum{
	CLK_NO_DIV,
	CLK_DIV2,
	CLK_DIV4,
	CLK_DIV8,
	CLK_DIV16,
	CLK_DIV32,
	CLK_DIV64,
	CLK_DIV128,
}CLK_DIV;

typedef enum{
	RXD_FROM_RX_TX,
	RXD_FROM_INDIVIDUAL_PAD
}PAD_SOURCE;

#ifdef COBRA_IRDA_NEW
#define COBRA_IRDA_BASE				(0x400AB000UL)
#define COBRA_IRDA_DATA_RAM			(0x400AB000UL + 0x800UL)
#else
#define COBRA_IRDA_BASE				(0x40081000UL)
#define COBRA_IRDA_DATA_RAM			(0x40081000UL + 0x800UL)
#endif

#define IRDA_INTERRIPT_TX_KEYCHANGED	(1 << 5)
#define IRDA_INTERRIPT_RX_WAKEUP		(1 << 4)
#define IRDA_INTERRUPT_RX_DONE			(1 << 3)
#define IRDA_INTERRUPT_TX_DONE			(1 << 2)
#define IRDA_INTERRUPT_TX_BLK_DONE		(1 << 1)
#define IRDA_INTERRUPT_RX_TOOLONG		(1 << 0)


#ifdef COBRA_IRDA_NEW

#define NEC_FRAME_TIME 	   27500
#define NEC_HEADER_HIGH_US    2250         /*!< NEC protocol header: positive 9ms */
#define NEC_HEADER_LOW_US     1125         /*!< NEC protocol header: negative 4.5ms*/

#define NEC_BIT_ONE_HIGH_US    140                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US    (562-NEC_BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */

#define NEC_BIT_ZERO_HIGH_US   140                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US   (280-NEC_BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */

#define NEC_BIT_END            140                         /*!< NEC protocol end: positive 0.56ms */

#define NEC_REPEAT0_HIGH	2250
#define NEC_REPEAT0_LOW		562
#define NEC_REPEAT1_HIGH	140
#define NEC_REPEAT1_LOW		24578

#define EOF_ENABLE_MASK		0x100

#define WAKEUP_RX_DIV_2     1
#define WAKEUP_RX_DIV_4     2
#define WAKEUP_RX_DIV_8     3
#define WAKEUP_RX_DIV_16    4
#define WAKEUP_RX_DIV_32    5
#define WAKEUP_RX_DIV_64    6
#define WAKEUP_RX_DIV_128   7
#define WAKEUP_RX_DIV_256   8
#define WAKEUP_RX_DIV_512   9
#define WAKEUP_RX_DIV_1024  10
#else
#define NEC_HEADER_HIGH_US    9000         /*!< NEC protocol header: positive 9ms */
#define NEC_HEADER_LOW_US     4500         /*!< NEC protocol header: negative 4.5ms*/

#define NEC_BIT_ONE_HIGH_US    560                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US    (2250-NEC_BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */

#define NEC_BIT_ZERO_HIGH_US   560                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US   (1120-NEC_BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */

#define NEC_BIT_END            560                         /*!< NEC protocol end: positive 0.56ms */
#endif

#define NEC_BIT_MARGIN        40           /*!< NEC parse margin time */

#define NEC_DATA_ITEM_NUM   34  /*!< NEC code item number: header + 32bit data + end */

void app_cobra_irda_rx_init(void);
#ifdef COBRA_IRDA_NEW
void app_cobra_irda_tx_init(void);
void irda_wake_up_init(void);
#endif
void cobra_irda_init(void);
void cobra_irda_tx_key_invert(unsigned int en);
void cobra_app_irda_task_loop(void);
void cobra_irda_interrupt(void);
#endif   //COBRA_IRDA_EN



#endif

