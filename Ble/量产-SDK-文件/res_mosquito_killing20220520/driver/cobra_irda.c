/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_irda.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/7

	Description :   


	Changed Log	:

		Liujian   2019/5/7			- Creation
        Lianbo    2021/2/5          - modified


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_system.h"
#include "cobra_aon_pmu.h"

#include "cobra_irda.h"
#include "cobra_pinmux.h"




#if COBRA_IRDA_EN

#include "app_bk_task.h"

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_IRDA_TypeDef * cobra_irda		__attribute__((section("retention_area"),zero_init)); 
static volatile unsigned int *cobra_irda_data		  __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_IRDA_TypeDef * cobra_irda; 
static volatile unsigned int *cobra_irda_data;
#endif

void cobra_irda_init(void)
{
		cobra_irda = (COBRA_IRDA_TypeDef * )COBRA_IRDA_BASE;
		cobra_irda_data = (unsigned int *)COBRA_IRDA_DATA_RAM;
}

void cobra_irda_rx_filt_thres(unsigned int rx_filt_thres)
{
		cobra_irda->RX_FILT_THRES = rx_filt_thres&0x0f;	
}

void cobra_irda_rx_max_life(unsigned int rx_max_life)
{
		cobra_irda->RX_MAX_LIFE = rx_max_life&0x7fff;	
}

void cobra_irda_rx_max_rec_len(unsigned int rx_max_rec_len)
{
		cobra_irda->RX_MAX_REC_LEN = rx_max_rec_len&0x7f;
}


void cobra_irda_rx_div(unsigned int rx_div)
{
		cobra_irda->RX_DIV = rx_div&0x0f;
	
}


void cobra_irda_rx_dec_len(unsigned int rx_dec_len)
{
		cobra_irda->RX_DEC_LEN = rx_dec_len&0x7f;
	
}

void cobra_irda_rx_en(unsigned int enable)
{
		if(enable)
		{
				cobra_irda->RX_EN = 1;
		}
		else
		{
				cobra_irda->RX_EN = 0;
		}	
}
#ifdef COBRA_IRDA_NEW

void cobra_irda_set_dec0(uint32_t dec0)
{
    cobra_irda->DEC_D0 = dec0;
}

void cobra_irda_set_dec1(uint32_t dec1)
{
    cobra_irda->DEC_D1 = dec1;
}

void cobra_irda_set_dec2(uint32_t dec2)
{
    cobra_irda->DEC_D2 = dec2;
}

void cobra_irda_set_dec3(uint32_t dec3)
{
    cobra_irda->DEC_D3 = dec3;
}

void cobra_irda_set_dec4(uint32_t dec4)
{
    cobra_irda->DEC_D4 = dec4;
}

void cobra_irda_set_dec5(uint32_t dec5)
{
    cobra_irda->DEC_D5 = dec5;
}

void cobra_irda_set_dec6(uint32_t dec6)
{
    cobra_irda->DEC_D6 = dec6;
}

void cobra_irda_set_dec7(uint32_t dec7)
{
    cobra_irda->DEC_D7 = dec7;
}

void cobra_irda_set_dec8(uint32_t dec8)
{
    cobra_irda->DEC_D8 = dec8;
}

void cobra_irda_set_dec9(uint32_t dec9)
{
    cobra_irda->DEC_D9 = dec9;
}

#endif
void cobra_irda_tx_key_cfg0(unsigned int key_cfg)
{
		cobra_irda->TX_KEY_CFG0 = key_cfg;	
}

void cobra_irda_tx_key_cfg1(unsigned int key_cfg)
{
		cobra_irda->TX_KEY_CFG1 = key_cfg;	
}

void cobra_irda_tx_key_cfg2(unsigned int key_cfg)
{
		cobra_irda->TX_KEY_CFG2 = key_cfg;	
}

void cobra_irda_tx_key_cfg3(unsigned int key_cfg)
{
		cobra_irda->TX_KEY_CFG3 = key_cfg;	
}


void cobra_irda_tx_en(unsigned int enable)
{
		if(enable)
		{
				cobra_irda->TX_EN = 1;
		}
		else
		{
				cobra_irda->TX_EN = 0;
		}	
}

void cobra_irda_tx_software_button(unsigned int enable)
{
		if(enable)
		{
				cobra_irda->TX_SWBTN = 1;
		}
		else
		{
				cobra_irda->TX_SWBTN = 0;
		}	
}

// carrier_sel= 0:ÄÚÖÃÔØ²¨ 1:PWMÔØ²¨

void cobra_irda_tx_carrier_select(TX_CARRIER index)
{
		if(index == TX_INTERNAL_CARRIER)
		{
				cobra_irda->TX_CONFIG &= ~0x01;
		}
		else if(index == TX_PWM)
		{
				cobra_irda->TX_CONFIG |= 0x01;
		}
}

//TX RAWÄ£Ê½   tx_mode=0
//TX ±àÂëÄ£Ê½  tx_mode=1
void cobra_irda_tx_mode_select(TX_MODE index)
{
		if(index == TX_RAW_MODE)
		{
				cobra_irda->TX_CONFIG &= ~0x02;
		}
		else if(index == TX_ENCODE_MODE)
		{
				cobra_irda->TX_CONFIG |= 0x02;
		}
}

void cobra_irda_tx_repeat_en(unsigned int  en)
{
		if(en)
		{
				cobra_irda->TX_CONFIG |= (1 << 2);
		}
		else
		{
				cobra_irda->TX_CONFIG &= ~(1 << 2);
		}
}

/*

TX °´¼üÄ£Ê½
tx_keymode=0 
key0-62 Ö±½Ó¶ÔÓ¦Íâ²¿63¸ö°´¼ü£¬ key63¶ÔÓ¦Èí¼þÐéÄâ°´¼ü£¨CobraÏîÄ¿ÊÜPADÏÞÖÆ£¬Ö»Ö§³Ö32¸öÎïÀí°´¼üºÍ1¸öÈí¼þ°´¼ü£©
tx_keymode=1 
Ó¦ÓÃÖÐ£¬ÍùÍùÎÞ·¨Âú×ã¸øÃ¿¸ö°´¼ü·ÖÅäÒ»¸öIO PAD¡£¶Ô°´¼ü±àÂë£¬ÒÔ¼õÉÙ¶ÔIOÐèÇó¡£×î´óÖ§³Ö6¸öIO¶ÔÓ¦62¸ö°´¼ü£º
6¡¯d0: ÎÞ°´¼ü°´ÏÂ
6¡¯d1-6¡¯d62: 62¸ö¼ü°´
6¡¯d63: Èí¼þÐéÄâ°´¼ü

*/

void cobra_irda_tx_keymode_select(TX_KEYMODE mode)
{
		if(mode == TX_KEY_DIRECT_MAP)
		{
				cobra_irda->TX_CONFIG &= ~(1 << 3);
		}
		else if(mode == TX_KEY_ENCODE_MAP)
		{
				cobra_irda->TX_CONFIG |= (1 << 3);
		}
}

//TX °´¼üÀ´Ô´
/*
tx_kb_sel=0
PAD ¡ú ÂË¶¶ ¡ú ½ÓÊÕ
tx_kb_sel=1
PAD ¡ú ÂË¶¶ ¡ú uCPU ¡ú ½ÓÊÕ (i_uc_getkey[5:0])
´ËÊ±ÐèÒªÉèÖÃtx_keymode=1

*/
void cobra_irda_tx_key_come_from(TX_KEYSOURCE source)
{
		if(source == TX_KEY_NO_UCPU)
		{
				cobra_irda->TX_CONFIG &= ~(0x03 << 4);
		}
		else if(source == TX_KEY_FROM_UCPU)
		{
				cobra_irda->TX_CONFIG &= ~(0x03 << 4);
				cobra_irda->TX_CONFIG |= (0x01 << 4);
		}
	
}

// //1:key[4:0]È¡·´
void cobra_irda_tx_key_invert(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_CONFIG |= (1 << 6);
		}
		else
		{
				cobra_irda->TX_CONFIG &= ~(1 << 6);
		}
	
}

/*
tx_key_ind±íÊ¾µÄ°´¼üºÅÂë£¬
*/
unsigned int cobra_irda_receive_key_value(void)
{
		return (cobra_irda->TX_CONFIG >> 10)&0x3f;
}

unsigned int cobra_irda_intr_state(void)
{
		return cobra_irda->INTR_STATUS & 0x3f;	
}

void cobra_irda_clear_intr_state(unsigned int value)
{
		cobra_irda->INTR_STATUS = value&0x3f;
}

//rx

void cobra_irda_rx_ctrl_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= 0x01;
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~0x01;
		}	
}

void cobra_irda_rx_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= 0x02;
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~0x02;
		}
	
}


void cobra_irda_tx_clk_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= 0x04;
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~0x04;
		}
	
}

void cobra_irda_ram_clk_save_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= 0x08;
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~0x08;
		}
}

void cobra_irda_rx_tx_clk_sel(CLK_DIV div)
{
		if(div == CLK_NO_DIV)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
		}
		else if(div == CLK_DIV2)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 1 << 4;
		}
		else if(div == CLK_DIV4)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 2 << 4;
		}
		else if(div == CLK_DIV8)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 3 << 4;
		}
		else if(div == CLK_DIV16)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 4 << 4;
		}
		else if(div == CLK_DIV32)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 5 << 4;
		}
		else if(div == CLK_DIV64)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 6 << 4;
		}
		else if(div == CLK_DIV128)
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(0x07 << 4);
				cobra_irda->TX_RX_CLK_CONTROL |= 7 << 4;
		}
			
}

void cobra_irda_fliter_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= (1 << 7);
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(1 << 7);
		}
}


void cobra_irda_use_aon_pad_select(PAD_SOURCE source)
{
		if(source == RXD_FROM_RX_TX)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= (1 << 20);
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(1 << 20);
		}
}

////1: i_rxdÈ¡·´
void cobra_irda_rxd_invert_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= (1 << 21);
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(1 << 21);
		}
}

////1: txdÈ¡·´
void cobra_irda_txd_invert_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= (1 << 22);
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(1 << 22);
		}
}

////1: carrierÈ¡·´
void cobra_irda_tx_carrier_invert_en(unsigned int en)
{
		if(en)
		{
				cobra_irda->TX_RX_CLK_CONTROL |= (1 << 23);
		}
		else
		{
				cobra_irda->TX_RX_CLK_CONTROL &= ~(1 << 23);
		}
}

unsigned int cobra_irda_tx_key_down_ind(void)
{
		return cobra_irda->TX_KEY_DOWN_IND;	
}

void cobra_irda_tx_key_filter_thres(unsigned int value)
{
		cobra_irda->TX_KEY_FILT_THRES = value&0x7ff;	
}

#ifdef COBRA_IRDA_NEW
/*
 * 计算32位数据中1的个数
 *	@para uint32_t saw_32num 为要计算的32位数据
 *	@return 返回1的个数
 */
uint8_t _1_num_32(uint32_t saw_32num)
{
    uint32_t _32num;
    uint8_t num = 0,i;
    _32num = saw_32num;
    for(i=0; i<32; i++)
    {
        if(_32num&0x01)
        {
            num++;
        }
        _32num >>= 1;
    }
    return num;
}

/*
 *将32位数据进行翻转后输出
 *@para uint32_t saw_32num 要翻转的数据
 *@return 翻转后的32位数据
 */
uint32_t flip_32(uint32_t saw_32num)
{
    uint32_t flip = 0;
    uint8_t i;
    for(i=0; i<32; i++)
    {
        flip |=  ((saw_32num&(1<<i))>>i)<<(31-i);
    }
    return flip;
}
void cobra_irda_encoded_mode_setpin_command(uint8_t keynum,uint32_t command)
{
    uint16_t eof_low = 0;
    uint32_t *tx_addr;
    tx_addr = (uint32_t *)cobra_irda_data;
    tx_addr += keynum*8;
    *tx_addr = 32|EOF_ENABLE_MASK;
    tx_addr++;
    *tx_addr = flip_32(command);
    tx_addr++;
    eof_low = NEC_FRAME_TIME-
              (_1_num_32(command)*(NEC_BIT_ONE_LOW_US+NEC_BIT_ONE_HIGH_US)\
               +(32-_1_num_32(command))*(NEC_BIT_ZERO_LOW_US+NEC_BIT_ZERO_HIGH_US)\
               +NEC_HEADER_HIGH_US+NEC_HEADER_LOW_US+NEC_REPEAT1_HIGH);
    *tx_addr =  eof_low<<16|(NEC_REPEAT1_HIGH|0x8000);
}

/*
 *
 *set addr high_time ,low_time
 */
void fill_item_level(uint32_t * addr, uint16_t high_time , uint16_t low_time)
{
    * addr = low_time<<16|(high_time|0x8000);
}

/*
 * @brief Generate NEC header value: active 9ms + negative 4.5ms
 */
static void fill_item_header(uint32_t * addr)
{
    fill_item_level(addr, NEC_HEADER_HIGH_US, NEC_HEADER_LOW_US);
}


/*
 * @brief Generate data bit 1: positive 0.56ms + negative 1.69ms
 */
static void fill_item_bit_one(uint32_t * addr)
{
    fill_item_level(addr, NEC_BIT_ONE_HIGH_US, NEC_BIT_ONE_LOW_US);
}

/*
 * @brief Generate data bit 0: positive 0.56ms + negative 0.56ms
 */
static void fill_item_bit_zero(uint32_t * addr)
{
    fill_item_level(addr, NEC_BIT_ZERO_HIGH_US, NEC_BIT_ZERO_LOW_US);
}

/*
 * @fill_item_end
 */
static void fill_item_end(uint32_t * addr)
{
    * addr = 0;
}

/*
 *标准模式，标准协议编码
 *@para uint32_t *data 编码完成的数组
 *@para uint32_t code 要编码的数据
 *
 */
uint8_t irda_encoded(uint32_t *data, uint32_t code)
{
    uint8_t i;
    uint16_t eof_low;
    fill_item_header(data++);
    for(i=1; i<33; i++)
    {
        if(code&0x1)
        {
            fill_item_bit_one(data++);
        }
        else
        {
            fill_item_bit_zero(data++);
        }
        code >>= 1;
    }
    eof_low = \
              NEC_FRAME_TIME-(_1_num_32(code)*(NEC_BIT_ONE_LOW_US+NEC_BIT_ONE_HIGH_US)\
                               +(32-_1_num_32(code))*(NEC_BIT_ZERO_LOW_US+NEC_BIT_ZERO_HIGH_US)\
                               +NEC_HEADER_HIGH_US+NEC_HEADER_LOW_US+NEC_REPEAT1_HIGH);
    *data  =  eof_low<<16|(NEC_REPEAT1_HIGH|0x8000);
    i++;
    return i ;
}

uint8_t set_num[32] = {0x0},item[32] = {0x0};
void clear_item_cnt(uint8_t keynum)
{
    set_num[keynum] = 0;
    item[keynum] = 0;
}

void irda_tx_fillmode_set_memory(uint8_t keynum,uint32_t command,uint8_t itemnum)
{
    uint16_t eof_low = 0;
    uint32_t *tx_addr;
    tx_addr = (uint32_t *)cobra_irda_data;
    tx_addr += keynum*8;      
    if(set_num[keynum]==0){
        fill_item_header(tx_addr++);
        item[keynum]++;
        for(;item[keynum]<(set_num[keynum]+1)*8;item[keynum]++)
        {
            if((command>>(item[keynum]-1))&0x1){
                fill_item_bit_one(tx_addr++);
            }
            else{
                fill_item_bit_zero(tx_addr++);
            }       
        }
    }else{
        for(;item[keynum]<(set_num[keynum]+1)*8;item[keynum]++)
        {
            if(item[keynum]==itemnum-1){
                eof_low = \
                NEC_FRAME_TIME-(_1_num_32(command)*(NEC_BIT_ONE_LOW_US+NEC_BIT_ONE_HIGH_US)\
                               +(32-_1_num_32(command))*(NEC_BIT_ZERO_LOW_US+NEC_BIT_ZERO_HIGH_US)\
                               +NEC_HEADER_HIGH_US+NEC_HEADER_LOW_US+NEC_REPEAT1_HIGH);
                fill_item_level(tx_addr++, NEC_REPEAT1_HIGH, eof_low);
                fill_item_end(tx_addr);
                break;
            }
            else{
                if((command>>(item[keynum]-1))&0x1){
                    fill_item_bit_one(tx_addr++);
                }
                else{
                    fill_item_bit_zero(tx_addr++);
                }
            }           
        }   
    } 
    set_num[keynum]++;  
    if(set_num[keynum] == (((itemnum%8)==0)?(itemnum/8):(itemnum/8+1)))
    {
        clear_item_cnt(keynum);
    }
}

#if (APP_COBRA_IRDA_WAKEUP_EN)
/*
 *
 *set wakeup from irda rx data
 */
void irda_wake_up_init(void)
{
    uint32_t dec_data = 0,wake_up_data[34] = {0};
    uint32_t command = 0xfc03ef00;
    uint16_t rx_div = 256;
    uint8_t i,j,dec_dat[68];
    //小遥控的OK键
    irda_encoded(wake_up_data,command);
    for(i=0; i<66; i=i+2)
    {
        dec_dat[i] = (wake_up_data[i/2]&0x7FFF)/rx_div;
        dec_dat[i+1] = (wake_up_data[i/2]>>16&0x7FFF)/rx_div;
//		printf("%d ",wake_up_data[i/2]&0x7FFF);
//		printf("%d\n",(wake_up_data[i/2]>>16&0x7FFF));
    }
    cobra_pcrm_irda_active_mode_clock_enable(1);
    cobra_pcrm_irda_sleep_mode_clock_enable(1);
    cobra_pcrm_irda_deep_sleep_mode_clock_enable(1);
	cobra_irda_rx_filt_thres(3);    //time no less than filt thres
	cobra_irda_rx_max_life(32700);  //20ms 
	cobra_irda_rx_max_rec_len(66);
	cobra_irda_rxd_invert_en(1);
	cobra_irda_rx_tx_clk_sel(CLK_DIV64);   //iclk  = 16000000/64, 因为在cobra_system.c 中已经将48M 分频到了 16M
                                          //we max_life max value is 32767,the last item time mey be more then 32767 us,
                                          //so we use 4us value,PS:NEC_BIT_ONE_HIGH = 560(us)/4;
	cobra_irda_rx_ctrl_clk_en(1);
	cobra_irda_rx_clk_en(1);
	cobra_irda_tx_clk_en(1);
	
	cobra_irda_rx_en(1);
	
	cobra_map_pinmodeset(PIN30_INDEX, PIN30_RMT0_RXD);
    cobra_map_pinmodeset(PIN17_INDEX, PIN17_M0_UART0_CTS);//close pin17 RMT_RXD
    cobra_irda_rx_dec_len(66);
    cobra_irda_rx_div(WAKEUP_RX_DIV_256);
    for(i=0; i < 10; i++)
    {
        for(j=0; j<8; j++)
        {
            if(8*i+j<68)
                dec_data |=dec_dat[8*i+j]<<j*4;
        }
        switch(i)
        {
        case 0:
            cobra_irda_set_dec0(dec_data);
            break;
        case 1:
            cobra_irda_set_dec1(dec_data);
            break;
        case 2:
            cobra_irda_set_dec2(dec_data);
            break;
        case 3:
            cobra_irda_set_dec3(dec_data);
            break;
        case 4:
           cobra_irda_set_dec4(dec_data);
            break;
        case 5:
            cobra_irda_set_dec5(dec_data);
            break;
        case 6:
            cobra_irda_set_dec6(dec_data);
            break;
        case 7:
            cobra_irda_set_dec7(dec_data);
            break;
        case 8:
            cobra_irda_set_dec8(dec_data);
            break;
        case 9:
            cobra_irda_set_dec9(dec_data);
            break;
        }
        dec_data = 0;
    }
    NVIC_SetPriority((IRQn_Type)INT_RTMT0_WAKEUP_IRQ, 3);
    NVIC_ClearPendingIRQ((IRQn_Type)INT_RTMT0_WAKEUP_IRQ);
    NVIC_EnableIRQ((IRQn_Type)INT_RTMT0_WAKEUP_IRQ);
}
#endif

#if APP_COBRA_IRDA_EN
#define MAX_IRDA_READ_NUMBER	68
void app_cobra_irda_rx_init(void)
{

        cobra_pcrm_irda_active_mode_clock_enable(1);
		cobra_irda_rx_filt_thres(3);
		cobra_irda_rx_max_life(32700);  //20ms 
		cobra_irda_rx_max_rec_len(MAX_IRDA_READ_NUMBER);
		cobra_irda_rxd_invert_en(1);
		cobra_irda_rx_tx_clk_sel(CLK_DIV64);  //iclk  = 16000000/64, 因为在cobra_system.c 中已经将48M 分频到了 16M
                                             //we max_life max value is 32767,the last item time mey be more then 32767 us,
                                             //so we use 4us value,PS:NEC_BIT_ONE_HIGH = 560(us)/4;
	
		cobra_irda_rx_ctrl_clk_en(1);
		cobra_irda_rx_clk_en(1);
		cobra_irda_tx_clk_en(1);
	
		cobra_irda_rx_en(1);
	
	    cobra_map_pinmodeset(PIN30_INDEX, PIN30_RMT0_RXD);
        cobra_map_pinmodeset(PIN17_INDEX, PIN17_M0_UART0_CTS);//close pin17 RMT_RXD
	
	
		NVIC_SetPriority((IRQn_Type)RMT0_IRQ, 3);
		NVIC_ClearPendingIRQ((IRQn_Type)RMT0_IRQ);
		NVIC_EnableIRQ((IRQn_Type)RMT0_IRQ);
		
		
		
//		printf("m3_apb1_frequency = %d \n", cobra_system_get_m3_apb1_clk());
	
}
#endif

#if (APP_COBRA_IRDATX_EN)

//#define ENCODED_MODE 1
//#define SPWM_CLK_IN  1
//#define PAD_SOURCE_UCPU 1
void app_cobra_irda_tx_init(void)
{
    uint32_t *tx_addr;
    printf("m3_apb1_frequency = %d \n", cobra_system_get_m3_apb1_clk());
    cobra_pcrm_irda_active_mode_clock_enable(1);
    cobra_map_pinmodeset(PIN31_INDEX, PIN31_RMT0_TXD);
    cobra_map_pinmodeset(PIN18_INDEX, PIN18_M0_UART0_RTS);//close pin18 RMT_TXD
//    cobra_map_pinmodeset(PIN27_INDEX, PIN27_RMT0_TX_KEY0);
    cobra_map_pinmodeset(PIN28_INDEX, PIN28_RMT0_TX_KEY1);
    cobra_map_pinmodeset(PIN29_INDEX, PIN29_RMT0_TX_KEY2);
//    cobra_pin_pullup_enable(PIN27_INDEX,1);
    cobra_pin_pullup_enable(PIN28_INDEX,1);
    cobra_pin_pullup_enable(PIN29_INDEX,1);
//    cobra_pin_inverse_input_enable(PIN27_INDEX,1);
    cobra_pin_inverse_input_enable(PIN28_INDEX,1);
    cobra_pin_inverse_input_enable(PIN29_INDEX,1);
#ifdef ENCODED_MODE
    
    cobra_irda_tx_mode_select(TX_ENCODE_MODE);
    
    tx_addr = (uint32_t *)cobra_irda_data;
    tx_addr +=3;
    *tx_addr = NEC_HEADER_LOW_US<<16|(NEC_HEADER_HIGH_US|0x8000);
    tx_addr++;
    *tx_addr = NEC_BIT_ZERO_LOW_US<<16|(NEC_BIT_ZERO_HIGH_US|0x8000);
    tx_addr++;
    *tx_addr = NEC_BIT_ONE_LOW_US<<16|(NEC_BIT_ONE_HIGH_US|0x8000);
    tx_addr++;
    *tx_addr = NEC_REPEAT0_LOW<<16|(NEC_REPEAT0_HIGH|0x8000);
    tx_addr++;
    *tx_addr = NEC_REPEAT1_LOW<<16|(NEC_REPEAT1_HIGH|0x8000);
//    cobra_irda_encoded_mode_setpin_command(0,0x4cb35583);
    cobra_irda_encoded_mode_setpin_command(1,0x4db25583);
    cobra_irda_encoded_mode_setpin_command(2,0x4fb05583);
//    cobra_irda_encoded_mode_setpin_command(3,0x4eb15583);
    cobra_irda_tx_key_cfg3(0x00000000);
    cobra_irda_tx_repeat_en(1);
#ifdef SPWM_CLK_IN
    cobra_pcrm_spwm_active_mode_clock_enable(1);
    PWM_Init((PWM_ENABLE)(PORTA_ENABLE|PORTB_ENABLE),630,200,0);
    cobra_irda_tx_carrier_select(TX_PWM);
#endif
#if PAD_SOURCE_UCPU
    cobra_irda_tx_key_come_from(TX_KEY_FROM_UCPU);
    cobra_irda_tx_keymode_select(TX_KEY_ENCODE_MAP);
#endif
//iclk  = 16000000/64, 因为在cobra_system.c 中已经将48M 分频到了 16M
//we max_life max value is 32767,the last item time mey be more then 32767 us,
//so we use 4us value,PS:NEC_BIT_ONE_HIGH = 560(us)/4;
    cobra_irda_rx_tx_clk_sel(CLK_DIV64);    
	cobra_irda_rx_ctrl_clk_en(1);
	cobra_irda_rx_clk_en(1);
	cobra_irda_tx_clk_en(1);
    cobra_irda_tx_en(1);
	NVIC_SetPriority((IRQn_Type)RMT0_IRQ, 3);
	NVIC_ClearPendingIRQ((IRQn_Type)RMT0_IRQ);
	NVIC_EnableIRQ((IRQn_Type)RMT0_IRQ);    
#endif

#ifndef ENCODED_MODE

//    irda_tx_fillmode_set_memory(0,0x4cb35583,34);
    irda_tx_fillmode_set_memory(1,0x4db25583,34);
    irda_tx_fillmode_set_memory(2,0x4fb05583,34);
//    irda_tx_fillmode_set_memory(3,0x4eb15583,34);
    tx_addr = (uint32_t *)cobra_irda_data;
    tx_addr += 62*8;
    *tx_addr = NEC_REPEAT0_LOW<<16|(NEC_REPEAT0_HIGH|0x8000);
    tx_addr++;
    *tx_addr = NEC_REPEAT1_LOW<<16|(NEC_REPEAT1_HIGH|0x8000);
    tx_addr++;
    *tx_addr = 0;
    cobra_irda_tx_repeat_en(1);
#ifdef SPWM_CLK_IN
    cobra_pcrm_spwm_active_mode_clock_enable(1);
    PWM_Init((PWM_ENABLE)(PORTA_ENABLE|PORTB_ENABLE),630,315,0);
    cobra_irda_tx_carrier_select(TX_PWM);
#endif
#if PAD_SOURCE_UCPU
    cobra_irda_tx_key_come_from(TX_KEY_FROM_UCPU);
    cobra_irda_tx_keymode_select(TX_KEY_ENCODE_MAP);
#endif
    cobra_irda_tx_mode_select(TX_RAW_MODE);
//iclk  = 16000000/64, 因为在cobra_system.c 中已经将48M 分频到了 16M
//we max_life max value is 32767,the last item time mey be more then 32767 us,
//so we use 4us value,PS:NEC_BIT_ONE_HIGH = 560(us)/4;    
    cobra_irda_rx_tx_clk_sel(CLK_DIV64);
	cobra_irda_rx_ctrl_clk_en(1);
	cobra_irda_rx_clk_en(1);
	cobra_irda_tx_clk_en(1);
    cobra_irda_tx_en(1);
	NVIC_SetPriority((IRQn_Type)RMT0_IRQ, 3);
	NVIC_ClearPendingIRQ((IRQn_Type)RMT0_IRQ);
	NVIC_EnableIRQ((IRQn_Type)RMT0_IRQ);
#endif

}
#endif

#else

#if APP_COBRA_IRDA_EN
#define MAX_IRDA_READ_NUMBER	100
void app_cobra_irda_rx_init(void)
{

		cobra_irda_rx_filt_thres(3);
		cobra_irda_rx_max_life(20000);  //20ms 
		cobra_irda_rx_max_rec_len(MAX_IRDA_READ_NUMBER);
		cobra_irda_rxd_invert_en(1);
		cobra_irda_rx_tx_clk_sel(CLK_DIV4);   //iclk  = 1000000, ÒòÎªÔÚcobra_system.c ÖÐÒÑ¾­½«48M ·ÖÆµµ½ÁË 4M
	
		cobra_irda_rx_ctrl_clk_en(1);
		cobra_irda_rx_clk_en(1);
		cobra_irda_tx_clk_en(1);
	
		cobra_irda_rx_en(1);
	
	    cobra_map_pinmodeset(PIN30_INDEX, PIN30_RMT0_RXD);
		cobra_aon_pmu_protection_en(1);
		cobra_aon_pmu_aon2mcu_source4_selection(RMT0_INT_EVENT);
		cobra_aon_pmu_protection_en(0);
	
	
		NVIC_SetPriority(AON_EVENT4_IRQn, 1);
		NVIC_ClearPendingIRQ(AON_EVENT4_IRQn);
		NVIC_EnableIRQ(AON_EVENT4_IRQn);
		
		
		
	//	printf("m3_apb0_frequency = %d \n", cobra_system_get_m3_apb0_clk());
	
}
#endif
#endif
//nec irda decode
/*
 * @brief Check whether duration is around target_us
 */
bool nec_check_in_range(short duration_ticks,  short target_us,  short margin_us)
{
    if((duration_ticks < (target_us + margin_us))
        && (duration_ticks > (target_us - margin_us))) {
        return true;
    } else {
        return false;
    }
}

/*
 * @brief Check whether this value represents an NEC header
 */
static bool nec_header_if(int * item)
{
		short *p0;
		short *p1;
		p0 = (short *)item;
		p1 = p0 + 1;
	//	printf("p0 = %x , p1 = %x \n", *p0, *p1);
	
    if(((*p0&0x8000) == 0x8000) && ((*p1&0x8000) != 0x8000)
        && nec_check_in_range(*p0&0x7fff, NEC_HEADER_HIGH_US, 200)
        && nec_check_in_range(*p1&0x7fff, NEC_HEADER_LOW_US, 200)) {
        return true;
    }
    return false;
}

/*
 * @brief Check whether this value represents an NEC data bit 1
 */
static bool nec_bit_one_if(int* item)
{
		short *p0;
		short *p1;
		p0 = (short *)item;
		p1 = p0 + 1;
	
	//	printf("p0 = %x , p1 = %x \n", *p0, *p1);
    if(((*p0&0x8000) == 0x8000) && ((*p1&0x8000) != 0x8000)
        && nec_check_in_range(*p0&0x7fff, NEC_BIT_ONE_HIGH_US, 150)
        && nec_check_in_range(*p1&0x7fff, NEC_BIT_ONE_LOW_US, 150)) {
        return true;
    }
    return false;
}

/*
 * @brief Check whether this value represents an NEC data bit 0
 */
static bool nec_bit_zero_if(int* item)
{
    short *p0;
		short *p1;
		p0 = (short *)item;
		p1 = p0 + 1;
	
	//	printf("p0 = %x , p1 = %x \n", *p0, *p1);
	
    if(((*p0&0x8000) == 0x8000) && ((*p1&0x8000) != 0x8000)
        && nec_check_in_range(*p0&0x7fff, NEC_BIT_ZERO_HIGH_US, 150)
        && nec_check_in_range(*p1&0x7fff, NEC_BIT_ZERO_LOW_US, 150)) {
        return true;
    }
    return false;
}


/*
 * @brief Parse NEC 32 bit waveform to address and command.
 */
static int nec_parse_items(int * item, int item_num, uint16_t* addr, uint16_t* data)
{
    int w_len = item_num;
    if(w_len < NEC_DATA_ITEM_NUM) {
        return -1;
    }
    int i = 0, j = 0;
    if(!nec_header_if(item++)) {
				//printf("nec_header_if  \n");
        return -1;
    }
    uint16_t addr_t = 0;
    for(j = 0; j < 16; j++) {
        if(nec_bit_one_if(item)) {
            addr_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            addr_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    uint16_t data_t = 0;
    for(j = 0; j < 16; j++) {
        if(nec_bit_one_if(item)) {
            data_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            data_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    *addr = addr_t;
    *data = data_t;
    return i;
}


void cobra_app_irda_task_loop(void)
{
			int ret;
			int *p_data = (int *)cobra_irda_data;
			uint16_t addr, data;
			addr = 0;
			data = 0;
			ret = nec_parse_items(p_data, NEC_DATA_ITEM_NUM,  &addr, &data);
			if(ret > 0)
			{
					printf("addr= %x , data = %x \n", addr, data);
			}
	
//	if (0){
//		unsigned int i;
//		volatile unsigned short * p = cobra_irda_data;
//			
//		i = 0;
//			
//		while(*p & 0x7fff)
//		{
//			printf("%d, %d\n", *p>>15, *p&0x7fff);	
//			i++;
//			p++;
//		}
//		
//		printf("i = %d \n", i);
//	}
		cobra_irda_rx_en(1);
}

void cobra_irda_interrupt(void)
{
		volatile uint32_t irda_interrupt_state = 0;
		irda_interrupt_state = cobra_irda_intr_state();
		cobra_irda_clear_intr_state(irda_interrupt_state);
	//	printf("irda_interrupt_state = 0x%x \n", irda_interrupt_state);
#ifdef COBRA_IRDA_NEW
		if(irda_interrupt_state & IRDA_INTERRUPT_TX_BLK_DONE)
		{
#ifndef ENCODED_MODE            
            switch(cobra_irda_receive_key_value())
            {
//                case 0:irda_tx_fillmode_set_memory(0,0x4cb35583,34);break;
                case 1:irda_tx_fillmode_set_memory(1,0x4db25583,34);break;
                case 2:irda_tx_fillmode_set_memory(2,0x4fb05583,34);break;
//                case 3:irda_tx_fillmode_set_memory(3,0x4eb15583,34);break;                
                default:break;
            }           
#endif            
        }
        if(irda_interrupt_state & IRDA_INTERRUPT_TX_DONE)
        {
#ifndef ENCODED_MODE 
//            clear_item_cnt(0);
            clear_item_cnt(1); 
            clear_item_cnt(2);
//            clear_item_cnt(3); 
//            irda_tx_fillmode_set_memory(0,0x4cb35583,34);
            irda_tx_fillmode_set_memory(1,0x4db25583,34);
            irda_tx_fillmode_set_memory(2,0x4fb05583,34);
//            irda_tx_fillmode_set_memory(3,0x4eb15583,34);            
#endif            
        }            
#endif
		if(irda_interrupt_state & IRDA_INTERRUPT_RX_DONE)
		{
				cobra_irda_rx_en(0);
#ifdef COBRA_IRDA_NEW
            cobra_app_irda_task_loop();
#else
				cobra_app_set_irda_task();
#endif            
		}		
}
#ifdef COBRA_IRDA_NEW
void cobra_irda_wake_interrupt(void)
{
    volatile uint32_t irda_interrupt_state = 0;
    irda_interrupt_state = cobra_irda_intr_state();
    cobra_irda_clear_intr_state(irda_interrupt_state);
    printf("irda_interrupt_state = 0x%x \n", irda_interrupt_state);
}
#endif

#endif














