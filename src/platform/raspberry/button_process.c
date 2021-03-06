#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <button.h>
#include <button_interface.h>

#define QUEUE_KEY 200

static bool Init(void *object);
static bool Read(void *object);

int main(int argc, char *argv[])
{

    Button_t button = 
    {
        .gpio.pin = 7,
        .gpio.eMode = eModeInput,
        .ePullMode = ePullModePullUp,
        .eIntEdge = eIntEdgeFalling,
        .cb = NULL
    };

    Button_Interface button_interface = 
    {
        .Init = Init,
        .Read = Read
    };

    Queue_Context queue_context = 
    {
        .data = {.type = 1},
        .queue = {.key = QUEUE_KEY},
        .buffer_size = BUFFER_SIZE
    };

    Button_Run(&button, &queue_context, &button_interface);
        
    return 0;
}

static bool Init(void *object)
{
    Button_t *button = (Button_t *)object;
    return Button_init(button) == EXIT_SUCCESS ? true : false;
}

static bool Read(void *object)
{
    Button_t *button = (Button_t *)object;
    return (bool)Button_read(button);
}