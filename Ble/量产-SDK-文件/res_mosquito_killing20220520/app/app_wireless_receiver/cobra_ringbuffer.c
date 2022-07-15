/******************************************************************************
	File Name	:	cobra_ringbuffer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/12

	Description :   

	Changed Log	:
		Liujian  2020/03/12			- Creation

*****************************************************************************/
#include "cobra_ringbuffer.h"
#include "ll.h"
#if COBRA_RINGBUFFER_EN

int cobra_ring_buffer_init(struct cobra_ringbuffer *buffer, uint8_t *data, uint32_t len) 
{
			 if(!(len && !(len & (len - 1)))) 
			 {
						return 0;
			 }

			 buffer->tail = 0;
			 buffer->head = 0;
			 buffer->size_mask = len-1;
			 buffer->data = data;
			 return 1;
}

//得到buffer 使用了的空间
uint32_t cobra_ring_buffer_len(struct cobra_ringbuffer *buffer) 
{
			if(buffer->tail >= buffer->head) 
			{
					return buffer->tail - buffer->head;
			}

			return buffer->size_mask - (buffer->head - buffer->tail) + 1;
}

//判断 buffer 是否为空
uint8_t cobra_ring_buffer_empty(struct cobra_ringbuffer *buffer) 
{
			return (buffer->tail == buffer->head);
}

uint32_t cobra_ring_buffer_len_available(struct cobra_ringbuffer *buffer)
{
			return buffer->size_mask - cobra_ring_buffer_len(buffer);
}

uint32_t cobra_ring_buffer_max_size(struct cobra_ringbuffer *buffer) 
{
			return buffer->size_mask;
}

void cobra_ring_buffer_append_one(struct cobra_ringbuffer *buffer, uint8_t data)
{
			buffer->data[buffer->tail] = data;
			buffer->tail = (buffer->tail + 1) & buffer->size_mask;
}

void cobra_ring_buffer_append_multiple(struct cobra_ringbuffer *buffer, uint8_t *data, uint32_t len)
{
			
			if(buffer->tail + len > buffer->size_mask) 
			{
					uint32_t lenToTheEnd = buffer->size_mask - buffer->tail + 1;
				
					uint32_t lenFromBegin = len - lenToTheEnd;
					
					memcpy(buffer->data + buffer->tail, data, lenToTheEnd);
					
					memcpy(buffer->data, data + lenToTheEnd, lenFromBegin);
			 }
			 else 
			 {
					memcpy(buffer->data + buffer->tail, data, len);
			 }
			 
			 GLOBAL_INT_DISABLE();
			 
			 buffer->tail = (buffer->tail + len) & buffer->size_mask;
			 
			 GLOBAL_INT_RESTORE();
}

uint8_t cobra_ring_buffer_peek_one(struct cobra_ringbuffer *buffer)
{
			return buffer->data[buffer->head];
}

uint8_t cobra_ring_buffer_get_one(struct cobra_ringbuffer *buffer)
{
			uint8_t data =  buffer->data[buffer->head];
			buffer->head = (buffer->head + 1) & buffer->size_mask;
			return data;
}

void cobra_ring_buffer_peek_multiple(struct cobra_ringbuffer *buffer, uint8_t *dst, uint32_t len)
{
		 
		 if(buffer->head + len > buffer->size_mask) 
		 {
					uint32_t lenToTheEnd = buffer->size_mask - buffer->head + 1;
			 
					uint32_t lenFromBegin = len - lenToTheEnd;
				
					memcpy(dst, buffer->data + buffer->head, lenToTheEnd);
				
					memcpy(dst + lenToTheEnd, buffer->data, lenFromBegin);
		 }
		 else 
		 {
					memcpy(dst, buffer->data + buffer->head, len);
		 }
		 
}

void cobra_ring_buffer_get_multiple(struct cobra_ringbuffer *buffer, uint8_t *dst, uint32_t len) 
{
			
			cobra_ring_buffer_peek_multiple(buffer, dst, len);
	
			GLOBAL_INT_DISABLE();
			buffer->head = (buffer->head + len) & buffer->size_mask;
	    GLOBAL_INT_RESTORE();
}



void cobra_ring_buffer_discard_multiple(struct cobra_ringbuffer *buffer, uint32_t len)
{
			buffer->head = (buffer->head + len) + buffer->size_mask;
}


void cobra_ring_buffer_clear(struct cobra_ringbuffer *buffer)
{
			buffer->head = buffer->tail = 0;
}




#endif
