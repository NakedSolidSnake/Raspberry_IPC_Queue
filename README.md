<p align="center">
  <img src="https://www.tutorialspoint.com/inter_process_communication/images/message_queue.jpg">
</p>

# Queue
## Introdução
## Implementação
### Biblioteca
#### queue.h
```c
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

```
#### queue.c
```c
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

```
### launch_processes.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid_button, pid_led;
    int button_status, led_status;

    pid_button = fork();

    if(pid_button == 0)
    {
        //start button process
        char *args[] = {"./button_process", NULL};
        button_status = execvp(args[0], args);
        printf("Error to start button process, status = %d\n", button_status);
        abort();
    }   

    pid_led = fork();

    if(pid_led == 0)
    {
        //Start led process
        char *args[] = {"./led_process", NULL};
        led_status = execvp(args[0], args);
        printf("Error to start led process, status = %d\n", led_status);
        abort();
    }

    return EXIT_SUCCESS;
}
```
### button_process.c
```c
/**
 * @file button_process.c
 * @author Cristiano Silva de Souza (cristianosstec@gmail.com)
 * @brief 
 * @version 0.1.0
 * @date 2020-02-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <button.h>
#include <queue.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define _1MS    1000
#define BUFFER_SIZE     1024

#define QUEUE_ID    100

static void inputHandler(void);

static Button_t button = {
        .gpio.pin = 7,
        .gpio.eMode = eModeInput,
        .ePullMode = ePullModePullUp,
        .eIntEdge = eIntEdgeFalling,
        .cb = inputHandler
    };

static int queue_id = -1;
static queue_st queue = {
    .queueType = 1
};

int main(int argc, char const *argv[])
{
    if(Button_init(&button))
        return EXIT_FAILURE;

    if((queue_id = queue_init(QUEUE_ID))  < 0)
        return EXIT_FAILURE;

    while(1)
        usleep(_1MS);

    queue_destroy(queue_id);

    return 0;
}

static void inputHandler(void)
{
    static int state = 0;
    if(!Button_read(&button)){
        usleep(_1MS * 40);
        while(!Button_read(&button));
        usleep(_1MS * 40);
        state ^= 0x01;

        memset(queue.bData, 0, sizeof(queue.bData));
        snprintf(queue.bData, 32, "state = %d\n", state);
        queue_send(queue_id, &queue, strlen(queue.bData));        
    }
}

```
### led_process.c
```c
/**
 * @file led_process.c
 * @author Cristiano Silva de Souza (cristianosstec@gmail.com)
 * @brief 
 * @version 0.1.0
 * @date 2020-02-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <led.h>
#include <queue.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define QUEUE_ID 100

LED_t led =
    {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

static int queue_id = -1;

static queue_st queue =
    {
    .queueType = 1
    };

int main(int argc, char const *argv[])
{
    int state_cur;
    int state_old;

    if (LED_init(&led))
        return EXIT_FAILURE;

    if((queue_id = queue_init(QUEUE_ID))  < 0)
       return EXIT_FAILURE;

    while (1)
    {

        if (queue_recv(queue_id, &queue, sizeof(queue.bData)) < 0)
        {
            continue;
        }

        sscanf(queue.bData, "state = %d", &state_cur);
        memset(queue.bData, 0, sizeof(queue.bData));

        if (state_cur != state_old)
        {

            state_old = state_cur;
            LED_set(&led, (eState_t)state_cur);
        }
        usleep(1);
    }    

    return EXIT_SUCCESS;
}

```
## Conclusão
