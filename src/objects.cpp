#include "objects.h"

JammerState *localJmrStt;
Preferences preferences;  

QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;

QueueHandle_t QueueRs485Out = NULL;
QueueHandle_t QueueRs485In  = NULL;


SemaphoreHandle_t SemaphoreTaskAutomat = NULL;

std::list<JammerState> G_connJmrs[10];

_RS485_data RS485_data;

_MSG_PACK G_serial_msg;
_MSG_PACK G_rm_msg;
_MSG_PACK G_485_msg;
 
_ADDRESSESS G_localAddresses;

 

void initObjects()
{
   

    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    QueueRs485Out        = xQueueCreate(2, sizeof(_RS485_data));
    QueueRs485In         = xQueueCreate(2, sizeof(_RS485_data));


    SemaphoreTaskAutomat = xSemaphoreCreateBinary();

}


