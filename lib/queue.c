/**
 * @file queue.c
 * @author Cristiano Silva de Souza (cristianosstec@gmail.com)
 * @brief 
 * @version 0.1.0
 * @date 2020-02-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue.h>

int queue_init(int id)
{
  return msgget((key_t)id, 0666 | IPC_CREAT);
}

int queue_send(int queue_id, const queue_st *data, const int bSize)
{
  if(bSize <= 0)
    return -1;

  return msgsnd(queue_id, (void *)data, bSize, 0);
}

int queue_recv(int queue_id, queue_st *data, const int bSize)
{
  if(!data)
    return -1;

  return msgrcv(queue_id, (void *)data, bSize, data->queueType, 0);
}

int queue_destroy(int queue_id)
{
  return msgctl(queue_id, IPC_RMID, 0);
}
