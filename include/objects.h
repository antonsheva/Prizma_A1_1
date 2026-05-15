#ifndef _OBJECTS_
#define _OBJECTS_

#include "Preferences.h"
#include "JammerState.h"
#include "AN_rs485.h"

#define SERIAL_BUFF_LEN 128

typedef struct{
    BYTE dataLen = 0;
    BYTE data[128];
    WORD marker = 0; 
}_RS485_data;
 



extern JammerState *jmrStt;
extern Preferences preferences;

extern QueueHandle_t QueueSerialOut;   
extern QueueHandle_t QueueBtOut    ;
extern QueueHandle_t QueueRs485Out ;
extern QueueHandle_t QueueRs485In  ;

extern SemaphoreHandle_t SemaphoreTaskAutomat;

 

void initObjects();




#endif

