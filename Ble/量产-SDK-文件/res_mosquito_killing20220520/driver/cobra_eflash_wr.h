#ifndef _EFLASH_WR_H
#define _EFLASH_WR_H
#include <stdio.h>
#include <string.h>
#include <cobra_config.h>

#include "cobra_eflash_ctrl.h"
uint8_t eflash_identify(uint8_t* id, void (*callback)(void));
unsigned int cobra_flash_erase(unsigned int offset, unsigned int size);

unsigned int cobra_flash_write(unsigned int offset, unsigned int length, unsigned char *buffer);
void eflash_read_all(uint32_t addr,uint16_t num,uint8_t*data_ptr);
#endif

