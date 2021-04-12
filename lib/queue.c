#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue.h>

bool Queue_Init(Queue_t *queue)
{
  bool status = false;
  
  do 
  {
    if (!queue)
      break;

    queue->id = msgget((key_t)queue->key, 0666 | IPC_CREAT);
    if (queue->id == -1)
      break;

    status = true;
  } while(false);
  
  return status;
}

bool Queue_Send(Queue_t *queue, const Queue_Data *data, const int buffer_size)
{
  bool status = true;
  if (msgsnd(queue->id, (void *)data, buffer_size, 0) == -1)
    status = false;
  return status;
}

bool Queue_Receive(Queue_t *queue, Queue_Data *data, const int buffer_size)
{
  bool status = true;
  if(msgrcv(queue->id, (void *)data, buffer_size, data->type, 0) == -1)
    status = false;

  return status;
}

bool Queue_Destroy(Queue_t *queue)
{
  bool status = true;
  if( msgctl(queue->id, IPC_RMID, 0) == -1)
    status = false;
  
  return status;
}
