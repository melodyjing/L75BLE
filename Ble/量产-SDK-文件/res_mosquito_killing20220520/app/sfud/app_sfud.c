/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_sfud.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/10/28

	Description :   Serial Flash Universal Driver
	Changed Log	:

		Liujian  2020/10/28		- Creation
*****************************************************************************/

#include "app_sfud.h"

#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)



const sfud_flash *cobra_inside_spi_flash;
		
void app_sfud_init(void)
{
	  if (sfud_init() == SFUD_SUCCESS) 
	  {  
			 cobra_inside_spi_flash = sfud_get_device_table() + 0;
    }
		else
		{
			 cobra_inside_spi_flash = NULL; 
		}
		  
}


unsigned int cobra_inside_spi_flash_disk_write(const uint8_t *buff, uint32_t sector, uint32_t count)
{
	   if(cobra_inside_spi_flash == NULL) return 0;
	
	   sfud_erase(cobra_inside_spi_flash, sector*SEC_SIZE, count*SEC_SIZE);
	   sfud_write(cobra_inside_spi_flash, sector*SEC_SIZE, count*SEC_SIZE, buff);
	
	   return 1;
	
}

unsigned int  cobra_inside_spi_flash_disk_read(uint8_t *buff, uint32_t sector, uint32_t count)
{
	   if(cobra_inside_spi_flash == NULL) return 0;
	
	   sfud_read(cobra_inside_spi_flash, sector*SEC_SIZE, count*SEC_SIZE, buff);
	
	   return 1;
}

//从 spi flash 中读取保存的数据
void cobra_inside_spi_flash_read_nvds(uint8_t *p_temp)
{
	  sfud_read(cobra_inside_spi_flash, INSIDE_SPI_FLASH_NVDS, M0_M3_NVDS_SIZE, p_temp);
}

//保存nvds
void cobra_inside_spi_flash_save_nvds(uint8_t * buffer)
{
	   sfud_erase(cobra_inside_spi_flash, INSIDE_SPI_FLASH_NVDS, M0_M3_NVDS_SIZE);
	   sfud_write(cobra_inside_spi_flash, INSIDE_SPI_FLASH_NVDS, M0_M3_NVDS_SIZE, buffer);
}

void cobra_inside_spi_flash_erase(uint32_t spi_address, uint32_t size)
{
	 sfud_erase(cobra_inside_spi_flash, spi_address, size);
}

void cobra_inside_spi_flash_write(uint32_t spi_address, uint8_t * p_buffer, uint32_t size)
{
	 sfud_write(cobra_inside_spi_flash, spi_address, size, p_buffer);
}

void cobra_inside_spi_flash_read(uint32_t spi_address, uint8_t * p_buffer, uint32_t size)
{
	 sfud_read(cobra_inside_spi_flash, spi_address, size, p_buffer);
}

void cobra_inside_spi_flash_erase_nvds(void)
{
	cobra_inside_spi_flash_erase(INSIDE_SPI_FLASH_NVDS, M0_M3_NVDS_SIZE);
}

unsigned int cobra_inside_spi_flash_compare_nvds(uint8_t *p_buffer, uint32_t size)
{
	  uint32_t i;
	  uint8_t  ch;
	  for(i = 0; i < size; i++)
	  {
			  sfud_read(cobra_inside_spi_flash, INSIDE_SPI_FLASH_NVDS + i, 1,  &ch);
			  //printf("0x%02x ", ch);
			  if(ch != p_buffer[i]) 
				{
					printf("\n\n\n i = %d , ch = 0x%02x , 0x%02x \n\n", i, ch, p_buffer[i] ) ;
					return 1;
				}
		}
		
		return 1;
	
}


#endif
