#include "objects.h"

JammerState *jmrStt;
Preferences preferences;  

QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;

QueueHandle_t QueueRs485Out = NULL;
QueueHandle_t QueueRs485In  = NULL;


SemaphoreHandle_t SemaphoreTaskAutomat = NULL;


_RS485_data RS485_data;


  



void initObjects()
{
   

    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    QueueRs485Out        = xQueueCreate(2, sizeof(_RS485_data));
    QueueRs485In         = xQueueCreate(2, sizeof(_RS485_data));


    SemaphoreTaskAutomat = xSemaphoreCreateBinary();

}


