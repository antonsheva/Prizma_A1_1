#include "objects.h"

JammerState *jmrStt;
Preferences preferences;  

QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;

QueueHandle_t QueueRs485Out = NULL;
QueueHandle_t QueueRs485In  = NULL;


SemaphoreHandle_t SemaphoreTaskAutomat = NULL;

void initObjects()
{
    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    QueueRs485Out        = xQueueCreate(2, 128);
    QueueRs485In         = xQueueCreate(2, 128);


    SemaphoreTaskAutomat = xSemaphoreCreateBinary();

}


