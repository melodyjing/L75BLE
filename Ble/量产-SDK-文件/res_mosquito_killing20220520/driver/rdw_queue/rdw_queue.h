#ifndef __RDW_QUEUE_H__
#define __RDW_QUEUE_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_in(uint16_t *Front,uint16_t *Rear,uint8_t *PBase,uint16_t Len,uint8_t *PData);
extern uint8_t queue_out(uint16_t *Front,uint16_t *Rear,uint8_t *PBase,uint16_t Len,uint8_t *PData);

//queue init.
#define QueueInit(q)         queue_init((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue full?
#define QueueFull(q)         queue_full((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue empty?
#define QueueEmpty(q)        queue_empty((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//put pdata in queue
#define QueueIn(q, pdata)    queue_in((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)
//get data from queue
#define QueueOut(q, pdata)   queue_out((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)




//64 Bytes Queue Struct
typedef struct queue64 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[256+1];
} QUEUE64_TYPE;



#endif
