/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_eflash_ctrl.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/21

	Description :   
					Eflash control erase, program .....


	Changed Log	:

		Liujian  2019/3/21			- Creation


*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cobra_config.h>

#include "cobra4_aon_top.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_cache_ctrl.h"

#if COBRA_EFLASH_CTRL_EN

static volatile COBRA_EFLASH_T * cobra_eflash;

#if (COBRA_POWER_DOWN_ENABLE)
static volatile struct cobra_flash_env_tag cobra_eflash_env __attribute__((section("retention_area"),zero_init)); 
#else
static volatile struct cobra_flash_env_tag cobra_eflash_env;
#endif

void cobra_eflash_init(void)
{
		 cobra_eflash = (COBRA_EFLASH_T *)COBRA_EFLASH_BASE;  
		 cobra_eflash_env.base_addr = 0x800000;
		 cobra_eflash_env.length    = 0x50000;      //256K byte
}

/*
unlock key is 0x48fcd451. 

when other value be set, the unlock1 will be disable, 

the control key is used to protect auto_mode write operation

*/
void cobra_eflash_unlock1_enable(unsigned int en)
{
		if(en)
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
		if(en)
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
		if(en)
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
		if(operate == EFLASH_STANDBY)
		{
				cobra_eflash->auto_mode_reg = 0x00;
		}
		else if(operate == EFLASH_READ)
		{
				cobra_eflash->auto_mode_reg = 0x01;
		}
		else if(operate == EFLASH_SLEEP)
		{
				cobra_eflash->auto_mode_reg = 0x02;
		}
		else if(operate == EFLASH_REFERENCE_CELL_ERASE)
		{
				cobra_eflash->auto_mode_reg = 0x04;
		}
		else if(operate == EFLASH_MASS_ERASE)
		{
				cobra_eflash->auto_mode_reg = 0x05;
		}
		else if(operate == EFLASH_PAGE_ERASE)
		{
				cobra_eflash->auto_mode_reg = 0x06;
		}
		else if(operate == EFLASH_PROGRAM)
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
void cobra_eflash_tacc(unsigned int  wait_time)
{
		cobra_eflash->elash_parameter1 &= ~0x0f;
		cobra_eflash->elash_parameter1 |= wait_time&0x0f;
}

/*
	number of pclk cycle to meet 10ns

default is 0
*/
void cobra_eflash_delay_10n_val(unsigned int value)
{
		cobra_eflash->elash_parameter1 &= ~(0x0f << 4);
		cobra_eflash->elash_parameter1 |= ((value&0x0f) << 4);
}

/*
number of pclk cycle to meet 100ns
default is  1
*/
void cobra_eflash_delay_100n_val(unsigned int value)
{
		cobra_eflash->elash_parameter1 &= ~(0xff << 8);
		cobra_eflash->elash_parameter1 |= ((value&0xff) << 8);
}

/*
number of pclk cycle to meet 5us

default is 0x40  64
*/
void cobra_eflash_delay_5u_val(unsigned int value)
{
		cobra_eflash->elash_parameter1 &= ~((unsigned int)0xffff << 16);
		cobra_eflash->elash_parameter1 |= ((unsigned int)(value&0xffff) << 16);
}

/*
number of pclk cycle to meet 100us

default is 0x500  1280

*/
void cobra_eflash_delay_100u_val(unsigned int value)
{
		cobra_eflash->elash_parameter2 &= ~((unsigned int)0xffff << 0);
		cobra_eflash->elash_parameter2 |= ((unsigned int)(value&0xffff) << 0);
}

/*
number of pclk cycle to meet program timing

default is 0x78
*/
void cobra_eflash_delay_porg_val(unsigned int value)
{
		cobra_eflash->elash_parameter2 &= ~((unsigned int)0xffff << 16);
		cobra_eflash->elash_parameter2 |= ((unsigned int)(value&0xffff) << 16);
}

/*
number of pclk cycle to meet erase timing\

default is 001312D0   1250000


*/
void cobra_eflash_delay_erase_val(unsigned int value)
{
		cobra_eflash->elash_parameter3 &= ~((unsigned int)0x1ffffff );
		cobra_eflash->elash_parameter3 |= ((unsigned int)(value&0x1ffffff));
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
		cobra_eflash->flash_control_op_and_parameter |= value&0x07;
}

/*
1: 清除 flash cache 内容
0: 保持住
*/
void cobra_eflash_flash_cache_flush_en(unsigned int en)
{
		if(en)
		{
				cobra_eflash->flash_control_op_and_parameter |= 1<<3;
		}
		else
		{
				cobra_eflash->flash_control_op_and_parameter &= ~(1<<3);
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
		if(op_ctrl == CTRL_NORMAL)
		{
				cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
		}
		else if(op_ctrl == CTRL_REPONSE_ERR_FOR_INVALID_ACCESS)
		{
				cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
				cobra_eflash->flash_control_op_and_parameter |=  (0x01 << 4);
		}
		else if(op_ctrl == CTRL_HREADYOUT_LOW_FOR_INVALID_ACCESS)
		{
				cobra_eflash->flash_control_op_and_parameter &= ~(0x03 << 4);
				cobra_eflash->flash_control_op_and_parameter |=  (0x02 << 4);
		}
	
}


/*
XE/YE static read control

0: dynamic control, 1: static control(high)

*/
void cobra_eflash_flashxye_sel(FLASHXYE_CONTROL sel)
{
	if(sel == STATIC_CONTROL)
	{
			cobra_eflash->flash_control_op_and_parameter |= (1<<8);
	}
	else if(sel == DYNAMIC_CONTROL)
	{
			cobra_eflash->flash_control_op_and_parameter &= ~(1<<8);
	}
}


/*
1: apb read operation insert one cycle

*/
void cobra_eflash_rd_multicycle(unsigned int  en)
{
	if(en)
	{
			cobra_eflash->flash_control_op_and_parameter |= (1<<12);
	}
	else
	{
			cobra_eflash->flash_control_op_and_parameter &= ~(1<<12);
	}
}

/*
1: apb write operation insert one cycle

*/
void cobra_eflash_wr_multicycle(unsigned int  en)
{
	if(en)
	{
			cobra_eflash->flash_control_op_and_parameter |= (1<<13);
	}
	else
	{
			cobra_eflash->flash_control_op_and_parameter &= ~(1<<13);
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
		if(cobra_eflash->flash_state & 0x01) return 1;
		else return 0;
}


/*
auto_op_en

*/
unsigned int cobra_eflash_auto_op_en(void)
{
		if(cobra_eflash->flash_state & (1 << 1))
				return 1;
		else	
				return 0;	
}

/*
unlock1  state

*/
unsigned int cobra_eflash_unlock1_state(void)
{
		if(cobra_eflash->flash_state & (1 << 2))
				return 1;
		else	
				return 0;	
}

/*
unlock2  state

*/
unsigned int cobra_eflash_unlock2_state(void)
{
		if(cobra_eflash->flash_state & (1 << 3))
				return 1;
		else	
				return 0;	
}

/*
unlock3  state

*/
unsigned int cobra_eflash_unlock3_state(void)
{
		if(cobra_eflash->flash_state & (1 << 4))
				return 1;
		else	
				return 0;	
}

/*
bor_invalid  state

*/
unsigned int cobra_eflash_bor_invalid_state(void)
{
		if(cobra_eflash->flash_state & (1 << 5))
				return 1;
		else	
				return 0;	
}

/*

operation interrupt mask, 1: int enable, 0: int disable

*/
void cobra_eflash_operation_interrupt_enable(unsigned int en)
{
		if(en)
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
		if(en)
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
		cobra_eflash->flash_state_clear |= 0x01;
}

/*
bor_invalid_clr

*/
void cobra_eflash_bor_invalid_clr(void)
{
		cobra_eflash->flash_state_clear |= (0x01 << 5);
}


void  cobra_eflash_parameter_init(void)
{

	if(cobra4_aon_pd_top_hf_clock_source() == HF_XTAL48M)
	{
			cobra_eflash_embeded_flash_access_wait(1); //insert 1 cycle
			cobra_eflash_tacc(0);
			cobra_eflash_delay_10n_val(1);     //0
			cobra_eflash_delay_100n_val(5);
			cobra_eflash_delay_5u_val(0xF0);  // 240
		
			cobra_eflash_delay_100u_val(0x12C0);  //4800
		
			cobra_eflash_delay_porg_val(0x240);   //576
		
			cobra_eflash_delay_erase_val(0x57E400); //(0x57E400);   //5760000
	}
	else if(cobra4_aon_pd_top_hf_clock_source() == HF_RC_OSC24M)
	{
				cobra_eflash_embeded_flash_access_wait(0); //insert 0 cycle
				cobra_eflash_tacc(0);
				cobra_eflash_delay_10n_val(0);
		
				cobra_eflash_delay_100n_val(3);
				cobra_eflash_delay_5u_val(0x78);  //64
		
				cobra_eflash_delay_100u_val(0x960);   //256
		
				cobra_eflash_delay_porg_val(0x120);  //120
		
				cobra_eflash_delay_erase_val(0x2BF200); //1250000			
	}

	
}

#if(APP_RAINBOW_ENABLE == 1)
void  cobra_eflash_rainbow_parameter_init(void)
{
	    cobra_eflash_embeded_flash_access_wait(1); //insert 1 cycle
      cobra_eflash_tacc(0);
      cobra_eflash_delay_10n_val(1);
      cobra_eflash_delay_100n_val(5);
      cobra_eflash_delay_5u_val(0xF0);  // 240
      cobra_eflash_delay_100u_val(0x12C0);  //4800
      cobra_eflash_delay_porg_val(0x240);   //576
      cobra_eflash_delay_erase_val(0x57E400);   //5760000
}
#endif

#if(APP_MOSQUITO_KILLING_ENABLE == 1)
void  cobra_eflash_mosquito_killing_parameter_init(void)
{
	  cobra_eflash_embeded_flash_access_wait(1); //insert 1 cycle
      cobra_eflash_tacc(0);
      cobra_eflash_delay_10n_val(1);
      cobra_eflash_delay_100n_val(5);
      cobra_eflash_delay_5u_val(0xF0);  // 240
      cobra_eflash_delay_100u_val(0x12C0);  //4800
      cobra_eflash_delay_porg_val(0x240);   //576
      cobra_eflash_delay_erase_val(0x57E400);   //5760000
}
#endif

//void eflash_read(uint32_t addr,uint32_t*read_data)
//{
//    EFLASH_CTRL->STATUS_CONTROL |= 0x300;
//    EFLASH_CTRL->UNLOCK1_ENABLE = UNLOCK1_ENABLE_VALUE;		//unlock1 enable

//    EFLASH_CTRL->ADDR_R = addr;
//    EFLASH_CTRL->UNLOCK2_ENABLE = UNLOCK2_ENABLE_VALUE;
//    EFLASH_CTRL->UNLOCK3_ENABLE = UNLOCK3_ENABLE_VALUE;
//    EFLASH_CTRL->AUTO_MODE_R = AUTO_MODE_READ;		//config AUTO_MODE_PROGRAM control
//    while(!(EFLASH_CTRL->STATUS_CONTROL&AUTO_OP_END));
//    read_data[0] = EFLASH_CTRL->READ_DATA0;
//    read_data[1] = EFLASH_CTRL->READ_DATA1;
//    read_data[2] = EFLASH_CTRL->READ_DATA2;
//    read_data[3] = EFLASH_CTRL->READ_DATA3;
//    EFLASH_CTRL->STATUS_CLR |= AUTO_OPEND_CLR;
//    EFLASH_CTRL->UNLOCK1_ENABLE = 0x00;
//}

void cobra_eflash_read(uint32_t addr,uint32_t*read_data)
{
		cobra_eflash_operation_interrupt_enable(1);
		cobra_eflash_bor_interrupt_enable(1);
		cobra_eflash_unlock1_enable(1);
	
		cobra_eflash->flash_address = addr;
	
		cobra_eflash_unlock2_enable(1);
		cobra_eflash_unlock3_enable(1);
	
		cobra_eflash_flash_auto_operation_mode(EFLASH_READ);
	
		while(!cobra_eflash_auto_op_end());
	
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
		
		while(!cobra_eflash_auto_op_end());

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

		

		while(!cobra_eflash_auto_op_end());     //wait auto operator end flag
	
		cobra_eflash_auto_opend_clr();          //clear auto operator end flag
	
		cobra_eflash_unlock1_enable(0);

}

void cobra_eflash_erase_all(void)
{
		cobra_eflash_operation_interrupt_enable(1);
		cobra_eflash_bor_interrupt_enable(1);
	
		cobra_eflash_unlock1_enable(1);
	
		cobra_eflash->flash_address = 0x0000;
	
		cobra_eflash_unlock2_enable(1);
		cobra_eflash_unlock3_enable(1);
	
		cobra_eflash_flash_auto_operation_mode(EFLASH_MASS_ERASE);

		while(!cobra_eflash_auto_op_end());
	
		cobra_eflash_auto_opend_clr();
	
		cobra_eflash_unlock1_enable(0);
	
}

void cobra_eflash_enter_cache_access(void)
{
//		cobra_eflash->flash_address = 0x0000;
//	
//		cobra_eflash->flash_program_word0 = 0x00000000;
////		cobra_eflash->flash_program_word1 = 0x00000000;
////		cobra_eflash->flash_program_word2 = 0x00000000;
////		cobra_eflash->flash_program_word3 = 0x00000000;
//	
//		cobra_eflash_operation_interrupt_enable(1);
//		cobra_eflash_bor_interrupt_enable(1);
//	
//		cobra_eflash_unlock1_enable(1);
		cobra_eflash_flash_cache_flush_en(1);
		cobra_eflash_flash_cache_flush_en(0);
//		cobra_eflash_unlock1_enable(1);
//	
//		cobra_eflash_flash_auto_operation_mode(EFLASH_CACHE_ACCESS);

//		cobra_eflash_unlock1_enable(0);
	
}

void cobra_eflash_enter_sleep(void)
{
			cobra_eflash_operation_interrupt_enable(1);
			cobra_eflash_bor_interrupt_enable(1);
			
			cobra_eflash_unlock1_enable(1);
			cobra_eflash_flash_auto_operation_mode(EFLASH_SLEEP);
	
			cobra_eflash_unlock1_enable(0);
}

void cobra_eflash_wakeup_from_sleep(void)
{
			volatile unsigned int  i;
			cobra_eflash_operation_interrupt_enable(1);
			cobra_eflash_bor_interrupt_enable(1);
			
			cobra_eflash_unlock1_enable(1);
			cobra_eflash_flash_auto_operation_mode(EFLASH_STANDBY);
		
			cobra_eflash_unlock1_enable(0);
	
			for(i = 0; i < 100; i++);   //3us
}


//-----------------------------------------------------------
unsigned int cobra_flash_erase(unsigned int offset, unsigned int size)
{
    unsigned int  end_offset_size;

    end_offset_size = offset + size;

    // Check parameters
    if (end_offset_size > cobra_eflash_env.length)
    {
					return 0;
    }
		
		offset = offset & ~(COBRA_EFLASH_BOUNDARY4K - 1);
		
    while((offset < end_offset_size) && (offset < cobra_eflash_env.length))
    {
				cobra_eflash_page_erase(offset);
			
				offset += COBRA_EFLASH_BOUNDARY4K;
    }

    return 1;
}

unsigned int cobra_flash_write(unsigned int offset, unsigned int length, unsigned char *buffer)
{
	  unsigned int i, program_loop, remain_number;
    unsigned int program_data;
		unsigned char *p_temp;

    // Check parameters
    if (((offset + length) >= cobra_eflash_env.length) )
    {
        return (0);
    }
		
		if(length == 0) return 0;
		
		if((offset & 0x03) != 0)
		{
				if((offset & 0x03) == 1)
				{
						program_data = ((buffer[0]<<8) | 0x00FF);
						buffer++;
						length--;
						if(length > 1)
						{
								program_data |= buffer[0] << 16;
								buffer++;
								length--;		
								if(length > 1)
								{
										program_data |= buffer[0] << 24;
										buffer++;
										length--;		
								}
								else
								{
										program_data |= ((unsigned int)0xFF << 24);
								}
						}
						else
						{
								program_data |= ((unsigned int)0xFFFF << 16);
						}
						
				}
				else if((offset & 0x03) == 2)
				{
						program_data = ((buffer[0]<<16) | 0xFFFF);
						buffer++;
						length--;
						if(length > 1)
						{
								program_data |= (buffer[0]<<24);
								buffer++;
								length--;							
						}
						else
						{
								program_data |= ((unsigned int)0xFF<<24);
						}
				}
				else
				{
						program_data = ((buffer[0]<<24) | 0xFFFFFF);
						buffer++;
						length--;
				}	
				offset &= ~0x03;
			
				cobra_eflash_program(offset&(~0x80000) , program_data);
		}
		
		program_loop = length>>2;
		remain_number = length&0x03;
		
		p_temp = (unsigned char *)&program_data;
		for(i = 0; i < program_loop; i++)
		{
				memmove(p_temp, buffer, 4);
			
				cobra_eflash_program(offset , program_data);
				
				buffer += 4;
				offset += 4;
		}
		
		if(remain_number > 0)
		{
				memset(p_temp, 0xff, 4);
				memmove(p_temp, buffer, remain_number);
				cobra_eflash_program(offset , program_data);
		}
		
    return 1;
}

//void cobra_information_flash_read(unsigned int address, uint32_t *p_buffer)
//{
//    uint32_t data_tmp[4] = {0};
//    cobra_eflash_read(address|EFLASH_INFOMATION_ADD_CONTROL_FLAG, data_tmp);
//    memcpy(p_buffer, data_tmp, 4*sizeof(uint32_t));
//		return ;
//}



void cobra_information_flash_read(uint32_t address, uint16_t number, uint8_t *p_buffer)				//addr<0x4000
{
    uint32_t  read_num;
    uint32_t  i, j;
	  uint32_t  addr_tmp;
	
    uint32_t  data_tmp[4] = {0};
		
		if(p_buffer == NULL) return;
		
    if(number%16 == 0) 
		{
        read_num = number/16;
    } 
		else 
		{
        read_num = number/16 + 1;
    }
		
    for(i = 0;  i < read_num;  i++)
    {
        addr_tmp = (address+i*16) | EFLASH_INFOMATION_ADD_CONTROL_FLAG;
			
        cobra_eflash_read(addr_tmp,data_tmp);
			
        for(j=0; j < 16; j++)
        {
            if((j + i*16) < number)
						{
                p_buffer[j+i*16] = data_tmp[j/4] >> ((j%4)*8);
						}
        }
    }
}




#if(TUYA_BLE_SDK_ENABLE == 1)
static unsigned char pack_erase[COBRA_EFLASH_BOUNDARY4K];

int cobra_tuya_write_nv_data(uint32_t addr,const uint8_t *p_data, uint32_t size)
{
	  uint32_t length, i, length_end;
	
	  uint32_t  rem_length;
	
		uint32_t sect = addr&(~(COBRA_EFLASH_BOUNDARY4K - 1));
		
		uint32_t offset = addr - sect;
		
		cobra_cache_invalidate_the_cache();
	
	  length = size + offset;
	
	  uint32_t total_sector;
	
	  if(length <=  COBRA_EFLASH_BOUNDARY4K)
		{
			  memcpy(pack_erase, (unsigned char *)(sect), COBRA_EFLASH_BOUNDARY4K);
			
			  memcpy(pack_erase + offset, p_data, size);
			
			  cobra_flash_erase(sect, COBRA_EFLASH_BOUNDARY4K);
	
		    cobra_flash_write(sect, COBRA_EFLASH_BOUNDARY4K, &pack_erase[0]);
		}
		else
		{

			  rem_length = COBRA_EFLASH_BOUNDARY4K - offset;
			
			  memcpy(pack_erase, (unsigned char *)(sect), COBRA_EFLASH_BOUNDARY4K);
			
			  memcpy(pack_erase + offset, p_data, rem_length);
			
			  cobra_flash_erase(sect, COBRA_EFLASH_BOUNDARY4K);
	
		    cobra_flash_write(sect, COBRA_EFLASH_BOUNDARY4K, &pack_erase[0]);
			
			  length = size + offset - rem_length;
			
			  total_sector = length / COBRA_EFLASH_BOUNDARY4K;
			
			  for(i = 0; i < total_sector; i++)
			  {
				    memcpy(pack_erase, (p_data + rem_length + i*COBRA_EFLASH_BOUNDARY4K) , COBRA_EFLASH_BOUNDARY4K);
					
					  cobra_flash_erase(sect + (i+1)*COBRA_EFLASH_BOUNDARY4K, COBRA_EFLASH_BOUNDARY4K);
					
					  cobra_flash_write(sect + (i+1)*COBRA_EFLASH_BOUNDARY4K, COBRA_EFLASH_BOUNDARY4K, &pack_erase[0]);
			  }
				
				length_end = size - rem_length - total_sector * COBRA_EFLASH_BOUNDARY4K;
				
				if(length_end > 0)
				{
				   memcpy(pack_erase, (unsigned char *)(sect + (total_sector+1)*COBRA_EFLASH_BOUNDARY4K), COBRA_EFLASH_BOUNDARY4K);
				
				   memcpy(pack_erase, (p_data + rem_length + total_sector*COBRA_EFLASH_BOUNDARY4K) , length_end);
					
					 cobra_flash_erase(sect + (total_sector + 1)*COBRA_EFLASH_BOUNDARY4K, COBRA_EFLASH_BOUNDARY4K);
					
					 cobra_flash_write(sect + (total_sector + 1)*COBRA_EFLASH_BOUNDARY4K, COBRA_EFLASH_BOUNDARY4K, &pack_erase[0]);				
				}	
		}
		
		return 0;
}

#endif


//unsigned char test_buf[256];
//void app_cobra4_eflash_test(void)
//{
//			uint32_t i;
//			uint32_t temp;
//			unsigned char *p_temp;
//	
//	    cobra_eflash_parameter_init();
//	
//	    cobra_eflash_flash_cache_flush_en(1);
//	    // cobra_eflash_erase_all();
//			//cobra_flash_erase(0, 256);
//	    //cobra_eflash_flash_cache_flush_en(1);
//			for(i = 0; i < 256; i++)
//			{
//					test_buf[i] = i;
//			}
//			cobra_flash_write(0, 256, test_buf);
//	
//			cobra_cache_invalidate_the_cache();
//			p_temp = (unsigned char *)(cobra_eflash_env.base_addr);
//			for(i = 0; i < 256; i++)
//			{
//					printf("0x%02x ", *p_temp++);
//			}
//			cobra_flash_erase(0, 256);
//			//cobra_eflash_flash_cache_flush_en(1);
//			printf("\n----------------------------------------------------------\n");
//			
//			//cobra_eflash_flash_cache_flush_en(1);
//			for(i = 0; i < 256; i++)
//			{
//					test_buf[i] = 0x12;
//			}
//			cobra_flash_write(0, 256, test_buf);
//			cobra_eflash_flash_cache_flush_en(1);
//			cobra_eflash_flash_cache_flush_en(0);
//			cobra_cache_invalidate_the_cache();
//			p_temp = (unsigned char *)(cobra_eflash_env.base_addr);
//			for(i = 0; i < 256; i++)
//			{
//					printf("0x%02x ", *p_temp++);
//			}
//			
//		
//	
//}


//unsigned int cobra_flash_read_infomation_page(uint32_t offset, uint16_t num, uint8_t*data_ptr)
//{
//    uint32_t read_num = 0,i = 0,j = 0,addr_tmp = 0;
//    uint32_t data_tmp[4] = {0};
//		
//		if((offset % 16) != 0) return 0;
//		
//    if(num%16==0) {
//        read_num = num/16;
//    } else {
//        read_num = num/16+1;
//    }
//    for(i=0; i<read_num; i++)
//    {
//        addr_tmp = (offset+i*16)|EFLASH_INFOMATION_ADD_CONTROL_FLAG;
//        cobra_eflash_read(addr_tmp,data_tmp);
//        for(j=0; j<16; j++)
//        {
//            if(j+i*16<num)
//                data_ptr[j+i*16]=data_tmp[j/4]>>(j%4)*8;
//        }
//    }
//		
//		return 1;
//}

#endif


#if (COBRA_POWER_DOWN_ENABLE)
void eflash_read_all(uint32_t addr,uint16_t num,uint8_t*data_ptr)			//addr<0x40000
{
    uint32_t read_num = 0,i = 0,j = 0,addr_tmp = 0,num_per = 0,cnt_per = 0;
	
    uint32_t data_tmp[4] = {0};
  
		uint8_t *data = 0;

    if((num+addr%16)%16==0) 
		{
        read_num = (num+addr%16)/16;
    } 
		else 
		{
        read_num = (num+addr%16)/16+1;
    }
		
    for(i=0; i<read_num; i++)
    {
        addr_tmp = (addr+i*16)&(~0xF);
			
        cobra_eflash_read(addr_tmp,data_tmp);
			
        data = (uint8_t *)data_tmp;
			
        if(num <= (16-addr%16))
        {
            num_per = num;
        } 
				else
        {
            if(i==0)
            {
                num_per = 16-addr%16;
            }
            else
            {
                num_per = (((num-cnt_per)<=16)?num-cnt_per:16);
            }
        }
				
        for(j=0; j<num_per; j++)
        {
            if(i==0)
            {
                data_ptr[cnt_per++]=data[addr-addr_tmp+j];
            }
            else
            {
                data_ptr[cnt_per++]=data[j];
            }
        }
    }
}
#endif











