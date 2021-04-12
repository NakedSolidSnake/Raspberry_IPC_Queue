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

#include <button_interface.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define _1ms    1000

bool Button_Run(void *object, Queue_Context *queue_context, Button_Interface *button)
{
    int state = 0;

    if(button->Init(object) == false)
        return false;

    if(Queue_Init(&queue_context->queue) == false)
       return false;

    while(true)
    {
        while(true)
        {
            if(!button->Read(object)){
                usleep(_1ms * 100);
                break;
            }else{
                usleep( _1ms );
            }
        }

        state ^= 0x01;
        memset(queue_context->data.buffer, 0, queue_context->buffer_size);
        snprintf(queue_context->data.buffer, queue_context->buffer_size, "state = %d\n", state);
        Queue_Send(&queue_context->queue, &queue_context->data, queue_context->buffer_size);
    }

    Queue_Destroy(&queue_context->queue);

    return false;
}
