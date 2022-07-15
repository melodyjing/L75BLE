/******************************************************************************
	File Name	:	app_voice_buffer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   
				Buffer °Ìßæß°ßﬂ
	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/

#include "app_voice_buffer.h"

#if (BLE_APP_RDW_VOICE)



I2S_DMA_SIZE_TYPE  i2s_rx_buffer[I2S_BUFFER_NUMBER][I2S_DMA_SIZE];
struct co_list  i2s_rx_free;



struct  i2s_buf_elt  i2s_pool[I2S_BUFFER_NUMBER];

//≥ı ºªØ I2S Buffer
void app_voice_init_buffer(void)
{
		int16_t i;
	
		memset(&i2s_pool, 0, sizeof(i2s_pool));
		for(i = 0 ; i < I2S_BUFFER_NUMBER; i++)
		{
				i2s_pool[i].buf_ptr = (I2S_DMA_SIZE_TYPE *)(&i2s_rx_buffer[i]);
		}
		
//		co_list_pool_init(&i2s_rx_free,
//                      &i2s_pool[0],
//                      sizeof(struct  i2s_buf_elt),  
//                      I2S_BUFFER_NUMBER,
//                      NULL,
//                      POOL_LINKED_LIST);
		
		co_list_pool_init(&i2s_rx_free,
                      &i2s_pool[0],
                      sizeof(struct  i2s_buf_elt),  
                      I2S_BUFFER_NUMBER
                     );
											
}

I2S_DMA_SIZE_TYPE * i2s_buf_rx_alloc(void)
{
    I2S_DMA_SIZE_TYPE  * buf_ptr = NULL;
    struct i2s_buf_elt* elt;

    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    elt = (struct i2s_buf_elt*) co_list_pop_front(&i2s_rx_free);
    GLOBAL_INT_RESTORE();

    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }

    return buf_ptr;
}

void i2s_buf_rx_free(I2S_DMA_SIZE_TYPE * buf)
{
    // Find associated pool element index
    uint8_t index;

    index = (buf - i2s_rx_buffer[0])/I2S_DMA_SIZE;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&i2s_rx_free, &i2s_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}


#endif
