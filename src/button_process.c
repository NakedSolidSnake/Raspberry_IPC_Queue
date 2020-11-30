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

static Button_t button7 = {
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
    if(Button_init(&button7))
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
    if(!Button_read(&button7)){
        usleep(_1MS * 40);
        while(!Button_read(&button7));
        usleep(_1MS * 40);
        state ^= 0x01;

        memset(queue.bData, 0, sizeof(queue.bData));
        snprintf(queue.bData, 32, "state = %d\n", state);
        queue_send(queue_id, &queue, strlen(queue.bData));        
    }
}
