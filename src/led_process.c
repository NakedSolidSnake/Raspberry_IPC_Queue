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
