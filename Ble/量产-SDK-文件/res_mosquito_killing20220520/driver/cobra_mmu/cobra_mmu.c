/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  cobra_mmu.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/1/25

	Description :   
        内存管理模块, 把空闲的内存管理起来, 参考的代码为  rt-thread os

	Changed Log	:

		Liujian  2021/1/25			- Creation

*****************************************************************************/
#include "cobra_mmu.h"
#include "cobra_system.h"

#if COBRA4_MMU_ENABLE



#define MIN_SIZE 12
#define HEAP_MAGIC 0x1ea0

struct heap_mem
{
    /* magic and used flag */
    uint16_t magic;
    uint16_t used;
    unsigned long next, prev;
};

#ifdef COBRA_MEM_STATS
static unsigned long used_mem, max_mem;
#endif

/** pointer to the heap: for alignment, heap_ptr is now a pointer instead of an array */
static uint8_t *heap_ptr;

/** the last entry, always unused! */
static struct heap_mem *heap_end;

static struct heap_mem *lfree;   /* pointer to the lowest free block */

static unsigned long   mem_size_aligned;

#define MIN_SIZE_ALIGNED     COBRA_ALIGN(MIN_SIZE, MMU_ALIGN_SIZE)
#define SIZEOF_STRUCT_MEM    COBRA_ALIGN(sizeof(struct heap_mem), MMU_ALIGN_SIZE)

static void plug_holes(struct heap_mem *mem)
{
    struct heap_mem *nmem;
    struct heap_mem *pmem;

    /* plug hole forward */
    nmem = (struct heap_mem *)&heap_ptr[mem->next];
    if (mem != nmem &&
        nmem->used == 0 &&
        (uint8_t *)nmem != (uint8_t *)heap_end)
    {
        /* if mem->next is unused and not end of heap_ptr,
         * combine mem and mem->next
         */
        if (lfree == nmem)
        {
            lfree = mem;
        }
        mem->next = nmem->next;
        ((struct heap_mem *)&heap_ptr[nmem->next])->prev = (uint8_t *)mem - heap_ptr;
    }

    /* plug hole backward */
    pmem = (struct heap_mem *)&heap_ptr[mem->prev];
    if (pmem != mem && pmem->used == 0)
    {
        /* if mem->prev is unused, combine mem and mem->prev */
        if (lfree == mem)
        {
            lfree = pmem;
        }
        pmem->next = mem->next;
        ((struct heap_mem *)&heap_ptr[mem->next])->prev = (uint8_t *)pmem - heap_ptr;
    }
}

/**
 * @ingroup SystemInit
 *
 * This function will initialize system heap memory.
 *
 * @param begin_addr the beginning address of system heap memory.
 * @param end_addr the end address of system heap memory.
 */
void cobra4_system_heap_init(void *begin_addr, void *end_addr)
{
    struct heap_mem *mem;
    unsigned long begin_align = COBRA_ALIGN((unsigned long)begin_addr, MMU_ALIGN_SIZE);
    unsigned long end_align   = COBRA_ALIGN_DOWN((unsigned long)end_addr, MMU_ALIGN_SIZE);


    /* alignment addr */
    if ((end_align > (2 * SIZEOF_STRUCT_MEM)) &&
        ((end_align - 2 * SIZEOF_STRUCT_MEM) >= begin_align))
    {
        /* calculate the aligned memory size */
        mem_size_aligned = end_align - begin_align - 2 * SIZEOF_STRUCT_MEM;
    }
    else
    {
        printf("mem init, error begin address 0x%x, and end address 0x%x\n",
                   (unsigned long)begin_addr, (unsigned long)end_addr);

        return;
    }

    /* point to begin address of heap */
    heap_ptr = (uint8_t *)begin_align;


    /* initialize the start of the heap */
    mem        = (struct heap_mem *)heap_ptr;
    mem->magic = HEAP_MAGIC;
    mem->next  = mem_size_aligned + SIZEOF_STRUCT_MEM;
    mem->prev  = 0;
    mem->used  = 0;

    /* initialize the end of the heap */
    heap_end        = (struct heap_mem *)&heap_ptr[mem->next];
    heap_end->magic = HEAP_MAGIC;
    heap_end->used  = 1;
    heap_end->next  = mem_size_aligned + SIZEOF_STRUCT_MEM;
    heap_end->prev  = mem_size_aligned + SIZEOF_STRUCT_MEM;

    /* initialize the lowest-free pointer to the start of the heap */
    lfree = (struct heap_mem *)heap_ptr;
}

/**
 * @addtogroup MM
 */

/**@{*/

/**
 * Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 *
 * @return pointer to allocated memory or NULL if no free memory was found.
 */
void *cobra4_malloc(unsigned long size)
{
    unsigned long ptr, ptr2;
    struct heap_mem *mem, *mem2;

    if (size == 0)
        return COBRA_NULL;

    /* alignment size */
    size = COBRA_ALIGN(size, MMU_ALIGN_SIZE);

    if (size > mem_size_aligned)
    {
        return COBRA_NULL;
    }

    /* every data block must be at least MIN_SIZE_ALIGNED long */
    if (size < MIN_SIZE_ALIGNED)
        size = MIN_SIZE_ALIGNED;


    for (ptr = (uint8_t *)lfree - heap_ptr;
         ptr < mem_size_aligned - size;
         ptr = ((struct heap_mem *)&heap_ptr[ptr])->next)
    {
        mem = (struct heap_mem *)&heap_ptr[ptr];

        if ((!mem->used) && (mem->next - (ptr + SIZEOF_STRUCT_MEM)) >= size)
        {
            /* mem is not used and at least perfect fit is possible:
             * mem->next - (ptr + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem */

            if (mem->next - (ptr + SIZEOF_STRUCT_MEM) >=
                (size + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED))
            {
                /* (in addition to the above, we test if another struct heap_mem (SIZEOF_STRUCT_MEM) containing
                 * at least MIN_SIZE_ALIGNED of data also fits in the 'user data space' of 'mem')
                 * -> split large block, create empty remainder,
                 * remainder must be large enough to contain MIN_SIZE_ALIGNED data: if
                 * mem->next - (ptr + (2*SIZEOF_STRUCT_MEM)) == size,
                 * struct heap_mem would fit in but no data between mem2 and mem2->next
                 * @todo we could leave out MIN_SIZE_ALIGNED. We would create an empty
                 *       region that couldn't hold data, but when mem->next gets freed,
                 *       the 2 regions would be combined, resulting in more free memory
                 */
                ptr2 = ptr + SIZEOF_STRUCT_MEM + size;

                /* create mem2 struct */
                mem2       = (struct heap_mem *)&heap_ptr[ptr2];
                mem2->magic = HEAP_MAGIC;
                mem2->used = 0;
                mem2->next = mem->next;
                mem2->prev = ptr;

                /* and insert it between mem and mem->next */
                mem->next = ptr2;
                mem->used = 1;

                if (mem2->next != mem_size_aligned + SIZEOF_STRUCT_MEM)
                {
                    ((struct heap_mem *)&heap_ptr[mem2->next])->prev = ptr2;
                }
								
#ifdef COBRA_MEM_STATS
                used_mem += (size + SIZEOF_STRUCT_MEM);
                if (max_mem < used_mem)
                    max_mem = used_mem;
#endif
								
            }
            else
            {
                /* (a mem2 struct does no fit into the user data space of mem and mem->next will always
                 * be used at this point: if not we have 2 unused structs in a row, plug_holes should have
                 * take care of this).
                 * -> near fit or excact fit: do not split, no mem2 creation
                 * also can't move mem->next directly behind mem, since mem->next
                 * will always be used at this point!
                 */
                mem->used = 1;
            }
            /* set memory block magic */
            mem->magic = HEAP_MAGIC;
						
            if (mem == lfree)
            {
                /* Find next free block after mem and update lowest free pointer */
                while (lfree->used && lfree != heap_end)
                    lfree = (struct heap_mem *)&heap_ptr[lfree->next];
            }

            /* return the memory data except mem struct */
            return (uint8_t *)mem + SIZEOF_STRUCT_MEM;
        }
    }

    return COBRA_NULL;
}


/**
 * This function will change the previously allocated memory block.
 *
 * @param rmem pointer to memory allocated by rt_malloc
 * @param newsize the required new size
 *
 * @return the changed memory block address
 */
void *cobra4_realloc(void *rmem, unsigned long newsize)
{
    unsigned long size;
    unsigned long ptr, ptr2;
    struct heap_mem *mem, *mem2;
    void *nmem;

    /* alignment size */
    newsize = COBRA_ALIGN(newsize, MMU_ALIGN_SIZE);
    if (newsize > mem_size_aligned)
    {
        return COBRA_NULL;
    }
    else if (newsize == 0)
    {
        cobra4_free(rmem);
        return COBRA_NULL;
    }

    /* allocate a new memory block */
    if (rmem == COBRA_NULL)
        return cobra4_malloc(newsize);

    if ((uint8_t *)rmem < (uint8_t *)heap_ptr ||
        (uint8_t *)rmem >= (uint8_t *)heap_end)
    {
        /* illegal memory */
        return rmem;
    }

    mem = (struct heap_mem *)((uint8_t *)rmem - SIZEOF_STRUCT_MEM);

    ptr = (uint8_t *)mem - heap_ptr;
    size = mem->next - ptr - SIZEOF_STRUCT_MEM;
    if (size == newsize)
    {
        return rmem;
    }

    if (newsize + SIZEOF_STRUCT_MEM + MIN_SIZE < size)
    {
        /* split memory block */
			
#ifdef COBRA_MEM_STATS
        used_mem -= (size - newsize);
#endif			
        ptr2 = ptr + SIZEOF_STRUCT_MEM + newsize;
        mem2 = (struct heap_mem *)&heap_ptr[ptr2];
        mem2->magic = HEAP_MAGIC;
        mem2->used = 0;
        mem2->next = mem->next;
        mem2->prev = ptr;
        mem->next = ptr2;
        if (mem2->next != mem_size_aligned + SIZEOF_STRUCT_MEM)
        {
            ((struct heap_mem *)&heap_ptr[mem2->next])->prev = ptr2;
        }
        
        if (mem2 < lfree)
        {
            /* the splited struct is now the lowest */
            lfree = mem2;
        }

        plug_holes(mem2);
        return rmem;
    }

    /* expand memory */
    nmem = cobra4_malloc(newsize);
    if (nmem != COBRA_NULL) /* check memory */
    {
        memcpy(nmem, rmem, size < newsize ? size : newsize);
        cobra4_free(rmem);
    }

    return nmem;
}


/**
 * This function will contiguously allocate enough space for count objects
 * that are size bytes of memory each and returns a pointer to the allocated
 * memory.
 *
 * The allocated memory is filled with bytes of value zero.
 *
 * @param count number of objects to allocate
 * @param size size of the objects to allocate
 *
 * @return pointer to allocated memory / NULL pointer if there is an error
 */
void *cobra4_calloc(unsigned long count, unsigned long  size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = cobra4_malloc(count * size);

    /* zero the memory */
    if (p)
        memset(p, 0, count * size);

    return p;
}


/**
 * This function will release the previously allocated memory block by
 * rt_malloc. The released memory block is taken back to system heap.
 *
 * @param rmem the address of memory which will be released
 */
void cobra4_free(void *rmem)
{
    struct heap_mem *mem;

    if (rmem == COBRA_NULL)
        return;

    if ((uint8_t *)rmem < (uint8_t *)heap_ptr ||
        (uint8_t *)rmem >= (uint8_t *)heap_end)
    {
        return;
    }

    /* Get the corresponding struct heap_mem ... */
    mem = (struct heap_mem *)((uint8_t *)rmem - SIZEOF_STRUCT_MEM);

    /* ... which has to be in a used state ... */
    if (!mem->used || mem->magic != HEAP_MAGIC)
    {
        printf("to free a bad data block:\n");
        printf("mem: 0x%08x, used flag: %d, magic code: 0x%04x\n", mem, mem->used, mem->magic);
    }

    /* ... and is now unused. */
    mem->used  = 0;
    mem->magic = HEAP_MAGIC;

    if (mem < lfree)
    {
        /* the newly freed struct is now the lowest */
        lfree = mem;
    }
		
#ifdef COBRA_MEM_STATS
    used_mem -= (mem->next - ((uint8_t *)mem - heap_ptr));
#endif
		
    /* finally, see if prev or next are free also */
    plug_holes(mem);

}


void cobra4_test_mmu(void)
{
	  unsigned int *p[100];
	  unsigned int i;
	  unsigned int total;
	  for(i= 0; i < 20; i++)
    {
		  p[i] = cobra4_malloc(1024);
			if(p[i] == COBRA_NULL)
			{
				 total = i;
				 printf(" total = %d \n", total);
				 break;
			}
	  }	 
		
		
}

#if COBRA_MEM_STATS
void cobra4_memory_info(uint32_t *total, uint32_t *used, uint32_t *max_used)
{
    if (total != COBRA_NULL)
        *total = mem_size_aligned;
    if (used  != COBRA_NULL)
        *used = used_mem;
    if (max_used != COBRA_NULL)
        *max_used = max_mem;
}

void cobra4_list_mem(void)
{
    printf("total memory: %d\n", mem_size_aligned);
    printf("used memory : %d\n", used_mem);
    printf("maximum allocated memory: %d\n", max_mem);
}


#endif

void rdw_memory_information(int argc, char *argv[])
{
#if COBRA_MEM_STATS	
	  cobra4_list_mem();
#endif
}
#endif
