#ifndef QUEUE_CONTEXT_H_
#define QUEUE_CONTEXT_H_

#include <queue.h>

typedef struct 
{
    Queue_t queue;
    Queue_Data data;
    int buffer_size;
} Queue_Context;

#endif /* QUEUE_CONTEXT_H_ */
