/******************************************************************************
	File Name	:	cobra_ringbuffer.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/12

	Description :   

	Changed Log	:
		Liujian  2020/03/12			- Creation

*****************************************************************************/

#ifndef __COBRA_RINGBUFFER_H__
#define __COBRA_RINGBUFFER_H__
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cobra_config.h>

#ifdef  __cplusplus
extern  "C" {
#endif
	
#if COBRA_RINGBUFFER_EN

struct cobra_ringbuffer{
	
   uint32_t tail;
	
   uint32_t head;
	
   uint32_t size_mask;
	
   uint8_t *data;
	
};



int cobra_ring_buffer_init(struct cobra_ringbuffer *buffer, uint8_t *data, uint32_t len) ;

void cobra_ring_buffer_get_multiple(struct cobra_ringbuffer *buffer, uint8_t *dst, uint32_t len) ;

uint8_t cobra_ring_buffer_empty(struct cobra_ringbuffer *buffer) ;

void cobra_ring_buffer_append_multiple(struct cobra_ringbuffer *buffer, uint8_t *data, uint32_t len);

uint32_t cobra_ring_buffer_len(struct cobra_ringbuffer *buffer) ;

uint32_t cobra_ring_buffer_len_available(struct cobra_ringbuffer *buffer);

#endif

#ifdef  __cplusplus
      }
#endif
			
#endif
	
