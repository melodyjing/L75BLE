#include "cobra_eflash_wr.h"
#include "co_error.h"
#include <cobra_config.h>

#include "cobra4_aon_top.h"
#include "cobra_eflash_ctrl.h"
#include "cobra_cache_ctrl.h"

extern volatile COBRA_EFLASH_T *cobra_eflash;
static volatile struct cobra_flash_env_tag cobra_eflash_env __attribute__((section("retention_area"),zero_init));
void cobra_eflash_init(void)
{
//	cobra_eflash = (COBRA_EFLASH_T *)COBRA_EFLASH_BASE;
	cobra_eflash_env.base_addr = 0x800000;
	cobra_eflash_env.length = 0x50000; //256K byte
}


uint8_t eflash_identify(uint8_t* id, void (*callback)(void))
{
    uint16_t id_code;
    id_code = EFLASH_ID;
    *id = id_code;
//    switch(id_code)
//    {
//    case EFLASH_ID:
//        cobra_eflash_env.base_addr = 0x800000;
//        cobra_eflash_env.length = 0x50000; //256K byte
//        break;
//    default:
//        break;
//    }
    return id_code;//	return CO_ERROR_NO_ERROR;
}

//-----------------------------------------------------------
/*
 EFALSH offset  size 
 eflash_addr EFALSH
 size
*/
unsigned int cobra_flash_erase(unsigned int eflash_addr, unsigned int size)
{
	unsigned int end_offset_size;

	end_offset_size = eflash_addr + size;

	// Check parameters
	if (end_offset_size > cobra_eflash_env.length)
	{
		return 0;
	}

	eflash_addr = eflash_addr & ~(COBRA_EFLASH_BOUNDARY4K - 1);

	while ((eflash_addr < end_offset_size) && (eflash_addr < cobra_eflash_env.length))
	{
		cobra_eflash_page_erase(eflash_addr);

		eflash_addr += COBRA_EFLASH_BOUNDARY4K;
	}

	return 1;
}

/*
  ??Eflash ???? eflash_addr, ?????
  ???????buffer, ????? length

*/
unsigned int cobra_flash_write(unsigned int eflash_addr, unsigned int length, unsigned char *buffer)
{
	unsigned int i, program_loop, remain_number;
	unsigned int program_data;
	unsigned char *p_temp;

	// Check parameters
	if (((eflash_addr + length) >= cobra_eflash_env.length))
	{
		return (0);
	}

	if (length == 0)
		return 0;

	if ((eflash_addr & 0x03) != 0)
	{
		if ((eflash_addr & 0x03) == 1)
		{
			program_data = ((buffer[0] << 8) | 0x00FF);
			buffer++;
			length--;
			if (length > 1)
			{
				program_data |= buffer[0] << 16;
				buffer++;
				length--;
				if (length > 1)
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
		else if ((eflash_addr & 0x03) == 2)
		{
			program_data = ((buffer[0] << 16) | 0xFFFF);
			buffer++;
			length--;
			if (length > 1)
			{
				program_data |= (buffer[0] << 24);
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
			program_data = ((buffer[0] << 24) | 0xFFFFFF);
			buffer++;
			length--;
		}
		eflash_addr &= ~0x03;

		cobra_eflash_program(eflash_addr & (~0x80000), program_data);
		eflash_addr += 4;
	}

	program_loop = length >> 2;
	remain_number = length & 0x03;

	p_temp = (unsigned char *)&program_data;
	for (i = 0; i < program_loop; i++)
	{
		memmove(p_temp, buffer, 4);

		cobra_eflash_program(eflash_addr, program_data);

		buffer += 4;
		eflash_addr += 4;
	}

	if (remain_number > 0)
	{
		memset(p_temp, 0xff, 4);
		memmove(p_temp, buffer, remain_number);
		cobra_eflash_program(eflash_addr, program_data);
	}

	return 1;
}

void eflash_read_all(uint32_t addr,uint16_t num,uint8_t*data_ptr)			//addr<0x40000
{
    uint32_t read_num = 0,i = 0,j = 0,addr_tmp = 0,num_per = 0,cnt_per = 0;
    uint32_t data_tmp[4] = {0};
    uint8_t *data = 0;
    if((num+addr%16)%16==0) {
        read_num = (num+addr%16)/16;
    } else {
        read_num = (num+addr%16)/16+1;
    }
    for(i=0; i<read_num; i++)
    {
        addr_tmp = (addr+i*16)&(~0xF);
        cobra_eflash_read(addr_tmp,data_tmp);
        data = (uint8_t *)data_tmp;
        if(num<=(16-addr%16))
        {
            num_per = num;
        } else
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

uint32_t cobra_information_flash_read(unsigned int address)
{
	uint32_t data_tmp[4] = {0};
	cobra_eflash_read(address | EFLASH_INFOMATION_ADD_CONTROL_FLAG, data_tmp);
	return data_tmp[0];
}



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


void cobra_eflash_erase_all(void)
{
	cobra_eflash_operation_interrupt_enable(1);
	cobra_eflash_bor_interrupt_enable(1);

	cobra_eflash_unlock1_enable(1);

	cobra_eflash->flash_address = 0x0000;

	cobra_eflash_unlock2_enable(1);
	cobra_eflash_unlock3_enable(1);

	cobra_eflash_flash_auto_operation_mode(EFLASH_MASS_ERASE);

//    __nop();
//    __nop();
//    __nop();
//    __nop(); 
//    __nop();
//    __nop();
//    __nop();
//    __nop(); 
	while (!cobra_eflash_auto_op_end())
		;

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
	volatile unsigned int i;
	cobra_eflash_operation_interrupt_enable(1);
	cobra_eflash_bor_interrupt_enable(1);

	cobra_eflash_unlock1_enable(1);
	cobra_eflash_flash_auto_operation_mode(EFLASH_STANDBY);

	cobra_eflash_unlock1_enable(0);

	for (i = 0; i < 100; i++)
		; //3us
}