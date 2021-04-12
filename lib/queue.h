#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct 
{
  long type;
  char buffer[BUFFER_SIZE];
} Queue_Data;

typedef struct
{
  int key;
  int id;
} Queue_t;

bool Queue_Init(Queue_t *queue);

bool Queue_Send(Queue_t *queue, const Queue_Data *data, const int buffer_size);

bool Queue_Receive(Queue_t *queue, Queue_Data *data, const int buffer_size);

bool Queue_Destroy(Queue_t *queue);

#endif /* QUEUE_H_ */
