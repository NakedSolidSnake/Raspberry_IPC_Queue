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

#include <led_interface.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>



bool LED_Run(void *object, Queue_Context *queue_context, LED_Interface *led)
{
    int state_cur;
    int state_old;

    if (led->Init(object) == false)
        return false;

    if(Queue_Init(&queue_context->queue) == false)
       return false;

    while (true)
    {
        if (Queue_Receive(&queue_context->queue, &queue_context->data, queue_context->buffer_size) == false)
        {
            continue;
        }

        sscanf(queue_context->data.buffer, "state = %d", &state_cur);
        memset(queue_context->data.buffer, 0, queue_context->buffer_size);

        if (state_cur != state_old)
        {

            state_old = state_cur;
            led->Set(object, state_cur);
        }
    }    
}
