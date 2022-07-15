/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_sfud.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/10/28

	Description :   Serial Flash Universal Driver
	Changed Log	:

		Liujian  2020/10/28		- Creation
*****************************************************************************/

#ifndef __APP_SFUD_H__
#define __APP_SFUD_H__

#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)
#include <sfud.h>
#include "cobra_spi_flash_ctrl.h"

#define SEC_SIZE  4096

#define INSIDE_SPI_FLASH_NVDS   0x00000000    //NVDS µÿ÷∑

#if APP_MESH_CWHSL_LIGHT_FUNTION

#define   SPI_FLASH_COUNT_ADDRESS           0x00003000

#define   SPI_FLASH_COUNT_TIMEOUT_ADDRESS   (SPI_FLASH_COUNT_ADDRESS + 8)

#if (APP_FLASH_LIGHT_NUMBERS)
#define   SPI_FLASH_FLASH_NUMBERS_ADDRESS   (SPI_FLASH_COUNT_ADDRESS + 16)
#endif

#define   SPI_FLASH_OTA_ADDRESS		          (INSIDE_SPI_FLASH_NVDS + M0_M3_NVDS_SIZE + 80)

#define   SPI_FLASH_SCREEN_ADDRESS          (INSIDE_SPI_FLASH_NVDS + M0_M3_NVDS_SIZE+ 100)

#define   SPI_FLASH_MESH_CWHSV_SCENS_DATA    0x00001000
#endif
void app_sfud_init(void);

void cobra_inside_spi_flash_read_nvds(uint8_t *p_temp);

void cobra_inside_spi_flash_save_nvds(uint8_t * buffer);

unsigned int cobra_inside_spi_flash_compare_nvds(uint8_t *p_buffer, uint32_t size);

void cobra_inside_spi_flash_erase_nvds(void);

void cobra_inside_spi_flash_erase(uint32_t spi_address, uint32_t size);

void cobra_inside_spi_flash_write(uint32_t spi_address, uint8_t * p_buffer, uint32_t size);

void cobra_inside_spi_flash_read(uint32_t spi_address, uint8_t * p_buffer, uint32_t size);


#endif

#endif
