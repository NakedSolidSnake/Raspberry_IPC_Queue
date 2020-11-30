#ifndef __QUEUE_H
#define __QUEUE_H

#define BSIZE   4096

typedef struct queue{
  long int queueType;
  char bData[BSIZE];
}queue_st;

/**
 * @brief 
 * 
 * @param id 
 * @return int 
 */
int queue_init(int id);

/**
 * @brief 
 * 
 * @param queue_id 
 * @param data 
 * @param bSize 
 * @return int 
 */
int queue_send(int queue_id, const queue_st *data, const int bSize);

/**
 * @brief 
 * 
 * @param queue_id 
 * @param data 
 * @param bSize 
 * @return int 
 */
int queue_recv(int queue_id, queue_st *data, const int bSize);

/**
 * @brief 
 * 
 * @param queue_id 
 * @return int 
 */
int queue_destroy(int queue_id);

#endif
