#ifndef _OBJECTS_
#define _OBJECTS_

#include "Preferences.h"
#include "JammerState.h"

extern JammerState *jmrStt;
extern Preferences preferences;

extern QueueHandle_t QueueSerialOut;   
extern QueueHandle_t QueueBtOut    ;
extern QueueHandle_t QueueRs485Out ;
extern QueueHandle_t QueueRs485In  ;

extern SemaphoreHandle_t SemaphoreTaskAutomat;





#endif