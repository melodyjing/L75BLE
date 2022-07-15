/**@file			cobra4A_eflash_ctrl.C
* @brief            
* @details          
* @author      			
* @date				2020-03-19
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* Ӳ��ƽ̨:			rdw2150 \n
* SDK �汾:			cobra4a-ble-lib-v1.0.0
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/03/19  <td>1.0      <td>liujian   <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <cobra_config.h>

#include "cobra4_aon_top.h"
#include "cobra_eflash_ctrl.h"
#include "cobra_cache_ctrl.h"


#if COBRA_EFLASH_CTRL_EN

volatile COBRA_EFLASH_T *cobra_eflash = (COBRA_EFLASH_T *)COBRA_EFLASH_BASE;
/*
unlock key is 0x48fcd451. 

when other value be set, the unlock1 will be disable, 

the control key is used to protect auto_mode write operation

*/
void cobra_eflash_unlock1_enable(unsigned int en)
{
	if (en)
	{
		cobra_eflash->unlock1_enable = 0x48fcd451UL;
	}
	else
	{
		cobra_eflash->unlock1_enable = 0x00000000;
	}
}

/*

unlock key is 0xf3c7081a


when write offset8 or offsetC, the unlock2 will be disable. 

Used to portect unlock3 or debug_mode write operation 

*/
void cobra_eflash_unlock2_enable(unsigned int en)
{
	if (en)
	{
		cobra_eflash->unlock2_enable = 0xf3c7081aUL;
	}
	else
	{
		cobra_eflash->unlock2_enable = 0x00000000;
	}
}

/*
unlock key is 0x357028bf

"disable condition:
1. unlock1 disable
2. brown out 
3. the next writen address is not auto_mode
4. auto_mode is not set to erase or program mode. 
the control key is used to protect erase and program operation for auto_mode"


*/
void cobra_eflash_unlock3_enable(unsigned int en)
{
	if (en)
	{
		cobra_eflash->unlock3_enable = 0x357028bfUL;
	}
	else
	{
		cobra_eflash->unlock3_enable = 0x00000000;
	}
}

/*

0: no-operation(standby), 
1: read, 
2: sleep
4: reference cell erase(only for test)
5: mass erase, 
6: page erase, 
7: program
8: cache access

*/
void cobra_eflash_flash_auto_operation_mode(EFLASH_AUTO_OPERATE operate)
{
    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();
    __nop();
    __nop(); 

	if (operate == EFLASH_STANDBY)
	{
		cobra_eflash->auto_mode_reg = 0x00;
	}
	else if (operate == EFLASH_READ)
	{
		cobra_eflash->auto_mode_reg = 0x01;
	}
	else if (operate == EFLASH_SLEEP)
	{
		cobra_eflash->auto_mode_reg = 0x02;
	}
	else if (operate == EFLASH_REFERENCE_CELL_ERASE)
	{
		cobra_eflash->auto_mode_reg = 0x04;
	}
	else if (operate == EFLASH_MASS_ERASE)
	{
		cobra_eflash->auto_mode_reg = 0x05;
	}
	else if (operate == EFLASH_PAGE_ERASE)
	{
		cobra_eflash->auto_mode_reg = 0x06;
	}
	else if (operate == EFLASH_PROGRAM)
	{
		cobra_eflash->auto_mode_reg = 0x07;
	}
	//		else if(operate == EFLASH_CACHE_ACCESS)
	//		{
	//				cobra_eflash->auto_mode_reg = 0x08;
	//		}
}

/*
read wait state

default is 0
*/
void cobra_eflash_tacc(unsigned int wait_time)
{
	cobra_eflash->elash_parameter1 &= ~0x0f;
	cobra_eflash->elash_parameter1 |= wait_time & 0x0f;
}

/*
	number of pclk cycle to meet 10ns

default is 0
*/
void cobra_eflash_delay_10n_val(unsigned int value)
{
	cobra_eflash->elash_parameter1 &= ~(0x0f << 4);
	cobra_eflash->elash_parameter1 |= ((value & 0x0f) << 4);
}

/*
number of pclk cycle to meet 100ns
default is  1
*/
void cobra_eflash_delay_100n_val(unsigned int value)
{
	cobra_eflash->elash_parameter1 &= ~(0xff << 8);
	cobra_eflash->elash_parameter1 |= ((value & 0xff) << 8);
}

/*
number of pclk cycle to meet 5us

default is 0x40  64
*/
void cobra_eflash_delay_5u_val(unsigned int value)
{
	cobra_eflash->elash_parameter1 &= ~((unsigned int)0xffff << 16);
	cobra_eflash->elash_parameter1 |= ((unsigned int)(value & 0xffff) << 16);
}

/*
number of pclk cycle to meet 100us

default is 0x500  1280

*/
void cobra_eflash_delay_100u_val(unsigned int value)
{
	cobra_eflash->elash_parameter2 &= ~((unsigned int)0xffff << 0);
	cobra_eflash->elash_parameter2 |= ((unsigned int)(value & 0xffff) << 0);
}

/*
number of pclk cycle to meet program timing

default is 0x78
*/
void cobra_eflash_delay_porg_val(unsigned int value)
{
	cobra_eflash->elash_parameter2 &= ~((unsigned int)0xffff << 16);
	cobra_eflash->elash_parameter2 |= ((unsigned int)(value & 0xffff) << 16);
}

/*
number of pclk cycle to meet erase timing\

default is 001312D0   1250000


*/
void cobra_eflash_delay_erase_val(unsigned int value)
{
	cobra_eflash->elash_parameter3 &= ~((unsigned int)0x1ffffff);
	cobra_eflash->elash_parameter3 |= ((unsigned int)(value & 0x1ffffff));
}

/*
embeded Flash access wait state

0: insert 0 cycle, 
1: insert 1 cycle
7: insert 7 cycle


*/
void cobra_eflash_embeded_flash_access_wait(unsigned int value)
{
	cobra_eflash->flash_control_op_and_parameter &= ~0x07;
	cobra_eflash->flash_control_op_and_parameter |= value & 0x07;
}

/*
1: ��� flash cache ����
0: ����ס
*/
void cobra_eflash_flash_cache_flush_en(unsigned int en)
{
	if (en)
	{
		cobra_eflash->flash_control_op_and_parameter |= 1 << 3;
	}
	else
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(1 << 3);
	}
}

/*
"00: normal, not any reponse
01: reponse error for invalid access
10: hreadyout set to low when access is invalid
11: forbidden"



*/
void cobra_eflash_embeded_flash_cross_control(EFLASH_CROSS_CTRL op_ctrl)
{
	if (op_ctrl == CTRL_NORMAL)
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
	}
	else if (op_ctrl == CTRL_REPONSE_ERR_FOR_INVALID_ACCESS)
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
		cobra_eflash->flash_control_op_and_parameter |= (0x01 << 4);
	}
	else if (op_ctrl == CTRL_HREADYOUT_LOW_FOR_INVALID_ACCESS)
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
		cobra_eflash->flash_control_op_and_parameter |= (0x02 << 4);
	}
}

/*
XE/YE static read control

0: dynamic control, 1: static control(high)

*/
void cobra_eflash_flashxye_sel(FLASHXYE_CONTROL sel)
{
	if (sel == STATIC_CONTROL)
	{
		cobra_eflash->flash_control_op_and_parameter |= (1 << 8);
	}
	else if (sel == DYNAMIC_CONTROL)
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(1 << 8);
	}
}

/*
1: apb read operation insert one cycle

*/
void cobra_eflash_rd_multicycle(unsigned int en)
{
	if (en)
	{
		cobra_eflash->flash_control_op_and_parameter |= (1 << 12);
	}
	else
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(1 << 12);
	}
}

/*
1: apb write operation insert one cycle

*/
void cobra_eflash_wr_multicycle(unsigned int en)
{
	if (en)
	{
		cobra_eflash->flash_control_op_and_parameter |= (1 << 13);
	}
	else
	{
		cobra_eflash->flash_control_op_and_parameter &= ~(1 << 13);
	}
}

/*

set 1 to clear auto_op_int

*/
void cobra_eflash_clear_auto_op_int(void)
{
	cobra_eflash->flash_state |= 0x01;
}

/*

auto operate end ????
*/
unsigned int cobra_eflash_auto_op_end(void)
{
	if (cobra_eflash->flash_state & 0x01)
		return 1;
	else
		return 0;
}

/*
auto_op_en

*/
unsigned int cobra_eflash_auto_op_en(void)
{
	if (cobra_eflash->flash_state & (1 << 1))
		return 1;
	else
		return 0;
}

/*
unlock1  state

*/
unsigned int cobra_eflash_unlock1_state(void)
{
	if (cobra_eflash->flash_state & (1 << 2))
		return 1;
	else
		return 0;
}

/*
unlock2  state

*/
unsigned int cobra_eflash_unlock2_state(void)
{
	if (cobra_eflash->flash_state & (1 << 3))
		return 1;
	else
		return 0;
}

/*
unlock3  state

*/
unsigned int cobra_eflash_unlock3_state(void)
{
	if (cobra_eflash->flash_state & (1 << 4))
		return 1;
	else
		return 0;
}

/*
bor_invalid  state

*/
unsigned int cobra_eflash_bor_invalid_state(void)
{
	if (cobra_eflash->flash_state & (1 << 5))
		return 1;
	else
		return 0;
}

/*

operation interrupt mask, 1: int enable, 0: int disable

*/
void cobra_eflash_operation_interrupt_enable(unsigned int en)
{
	if (en)
	{
		cobra_eflash->flash_state |= (1 << 8);
	}
	else
	{
		cobra_eflash->flash_state &= ~(1 << 8);
	}
}

//cobra2 no this function .........
void cobra_eflash_bor_interrupt_enable(unsigned int en)
{
	if (en)
	{
		cobra_eflash->flash_state |= (1 << 9);
	}
	else
	{
		cobra_eflash->flash_state &= ~(1 << 9);
	}
}

/*
auto_op_end_clr

*/
void cobra_eflash_auto_opend_clr(void)
{
	cobra_eflash->flash_state_clear = 0x01;
}

/*
bor_invalid_clr

*/
void cobra_eflash_bor_invalid_clr(void)
{
	cobra_eflash->flash_state_clear = (0x01 << 5);
}


void cobra_eflash_parameter_init(void)
{

	if (cobra4_aon_pd_top_hf_clock_source() == HF_XTAL48M)
	{
		cobra_eflash_embeded_flash_access_wait(1); //insert 1 cycle
		cobra_eflash_tacc(0);
		cobra_eflash_delay_10n_val(0);
		cobra_eflash_delay_100n_val(5);
		cobra_eflash_delay_5u_val(0xF0); // 240

		cobra_eflash_delay_100u_val(0x12C0); //4800

		cobra_eflash_delay_porg_val(0x240); //576

		cobra_eflash_delay_erase_val(0x57E400); //5760000
	}
	else if (cobra4_aon_pd_top_hf_clock_source() == HF_RC_OSC24M)
	{
		cobra_eflash_embeded_flash_access_wait(0); //insert 0 cycle
		cobra_eflash_tacc(0);
		cobra_eflash_delay_10n_val(0);

		cobra_eflash_delay_100n_val(3);
		cobra_eflash_delay_5u_val(0x78); //64

		cobra_eflash_delay_100u_val(0x960); //256

		cobra_eflash_delay_porg_val(0x120); //120

		cobra_eflash_delay_erase_val(0x2BF200); //1250000
	}
}

void cobra_eflash_read(uint32_t addr, uint32_t *read_data)
{
	cobra_eflash_operation_interrupt_enable(1);
	cobra_eflash_bor_interrupt_enable(1);
	cobra_eflash_unlock1_enable(1);

	cobra_eflash->flash_address = addr;

	cobra_eflash_unlock2_enable(1);
	cobra_eflash_unlock3_enable(1);

	cobra_eflash_flash_auto_operation_mode(EFLASH_READ);
    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();
    __nop();
    __nop(); 

	while (!cobra_eflash_auto_op_end())
		;

	read_data[0] = cobra_eflash->flash_read_word0;
	read_data[1] = cobra_eflash->flash_read_word1;
	read_data[2] = cobra_eflash->flash_read_word2;
	read_data[3] = cobra_eflash->flash_read_word3;

	cobra_eflash_auto_opend_clr();

	cobra_eflash_unlock1_enable(0);
}

void cobra_eflash_page_erase(uint32_t offset)
{

	cobra_eflash_operation_interrupt_enable(1);
	cobra_eflash_bor_interrupt_enable(1);

	cobra_eflash_unlock1_enable(1);

	cobra_eflash->flash_address = offset;

	cobra_eflash_unlock2_enable(1);
	cobra_eflash_unlock3_enable(1);

	cobra_eflash_flash_auto_operation_mode(EFLASH_PAGE_ERASE);

    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();
    __nop();
    __nop(); 
	while (!cobra_eflash_auto_op_end())
		;

	cobra_eflash_auto_opend_clr();

	cobra_eflash_unlock1_enable(0);
}

void cobra_eflash_program(uint32_t offset, uint32_t program_data)
{

	cobra_eflash_operation_interrupt_enable(1);
	cobra_eflash_bor_interrupt_enable(1);

	cobra_eflash_unlock1_enable(1);

	cobra_eflash->flash_address = offset;
	cobra_eflash->flash_program_word0 = program_data;

	cobra_eflash_unlock2_enable(1);
	cobra_eflash_unlock3_enable(1);

	cobra_eflash_flash_auto_operation_mode(EFLASH_PROGRAM);

    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();
    __nop();
    __nop(); 
	while (!cobra_eflash_auto_op_end())
		; //wait auto operator end flag

	cobra_eflash_auto_opend_clr(); //clear auto operator end flag

	cobra_eflash_unlock1_enable(0);
}

//void get_ftid(void)
//{
//    cobra_eflash_parameter_init();
//    uint32_t infomation_page0[20] = {0};
//    p_eflash_read_all(0x0000,80,(uint8_t*)infomation_page0);
////    printf("ftID:%08x,%08x,%08x,%08x\n",infomation_page0[19],infomation_page0[18],infomation_page0[17],infomation_page0[16]);
//}//Add By Lilianbo 2020/04/01

#if 0
unsigned char test_buf[256];
void app_cobra4_eflash_test(void)
{
			uint32_t i;
			uint32_t temp;
			unsigned char *p_temp;
	
	    cobra_eflash_parameter_init();
	
	    cobra_eflash_flash_cache_flush_en(1);
	    // cobra_eflash_erase_all();
			//cobra_flash_erase(0, 256);
	    //cobra_eflash_flash_cache_flush_en(1);
			for(i = 0; i < 256; i++)
			{
					test_buf[i] = i;
			}
			p_cobra_flash_write(0, 256, test_buf);
	
			cobra_cache_invalidate_the_cache();
			p_temp = (unsigned char *)(cobra_eflash_env.base_addr);
			for(i = 0; i < 256; i++)
			{
					printf("0x%02x ", *p_temp++);
			}
			p_cobra_flash_erase(0, 256);
			//cobra_eflash_flash_cache_flush_en(1);
			printf("\n----------------------------------------------------------\n");
			
			//cobra_eflash_flash_cache_flush_en(1);
			for(i = 0; i < 256; i++)
			{
					test_buf[i] = 0x12;
			}
			p_cobra_flash_write(0, 256, test_buf);
			cobra_eflash_flash_cache_flush_en(1);
			cobra_eflash_flash_cache_flush_en(0);
			cobra_cache_invalidate_the_cache();
			p_temp = (unsigned char *)(cobra_eflash_env.base_addr);
			for(i = 0; i < 256; i++)
			{
					printf("0x%02x ", *p_temp++);
			}
			
		
	
}
#endif

#endif
