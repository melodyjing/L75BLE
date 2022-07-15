/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */
/*Confidential and copyright 2019-2025 Radiawave Ltd.
	
	 Author:Liujian
	 
	 2020/11/20
*/

#include <sfud.h>
#include <stdarg.h>
#include "cobra_spi_flash_ctrl.h"

#if (COBRA4C_INSIDE_SPI_FLASH_ENABLE)

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }
		
     /* 开始读写数据 */
		cobra_spi_flash_config_set_cs(1);
		
    for (size_t i = 0, retry_times; i < write_size + read_size; i++) 
	  {
        /* 先写缓冲区中的数据到 SPI 总线，数据写完后，再写 dummy(0xFF) 到 SPI 总线 */
        if (i < write_size) 
				{
            send_data = *write_buf++;
        } 
				else 
				{
            send_data = SFUD_DUMMY_DATA;
        }

        read_data =cobra_spi_flash_config_write_and_read (send_data);

        if (i >= write_size) 
				{
            *read_buf++ = read_data;
        }
    }
		
		cobra_spi_flash_config_set_cs(0);

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */
    uint16_t i;
		uint8_t * p_qspi_flash = (uint8_t *)(0x60000000 + addr);
		memcpy(read_buf, p_qspi_flash, read_size);
					
    return result;
}
#endif /* SFUD_USING_QSPI */


static void spi_lock(const sfud_spi *spi) 
{
 //   __disable_irq();
}

static void spi_unlock(const sfud_spi *spi)
{
 //   __enable_irq();
}


/* about 100 microsecond delay */
static void app_cobra_retry_delay_100us(void) 
{
    cobra_soft_delay_us(100);
}

sfud_err sfud_spi_port_init(sfud_flash *flash) 
{
    sfud_err result = SFUD_SUCCESS;
	
	  app_cobra_sfud_port_flash_init();
    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
	
       /* 同步 Flash 移植所需的接口及数据 */
        flash->spi.wr = spi_write_read;
	      flash->spi.qspi_read = qspi_read;	
        flash->spi.lock = spi_lock;
        flash->spi.unlock = spi_unlock;
        flash->spi.user_data = NULL;
        /* about 100 microsecond delay */
        flash->retry.delay = app_cobra_retry_delay_100us;
        /* adout 60 seconds timeout */
        flash->retry.times = 60 * 10000;
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

#endif

