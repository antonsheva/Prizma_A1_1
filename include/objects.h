#ifndef _OBJECTS_
#define _OBJECTS_

#include "Preferences.h"
#include "JammerState.h"

 


#define SERIAL_BUFF_LEN 128

typedef struct{
    BYTE dataLen = 0;
    BYTE data[128];
    WORD marker = 0; 
}_RS485_data;
 
 
typedef struct{
    BYTE  cmd         = 0;
    BYTE  modCode     = 0;
    BYTE  modCode1    = 0;
    BYTE  modCode2    = 0;
    BYTE  addrEsp32   = 0;
    BYTE  addrRm      = 0;
    BYTE  addrRm1     = 0;
    BYTE  addrRm2     = 0;
    BYTE  devNum      = 0;
    BYTE  txtDataLen  = 0;
    DWORD mask        = 0;
    DWORD mask1       = 0;
    DWORD mask2       = 0;
    char  txtData[64] = {'\0'};
}_MSG_PACK;


extern JammerState *jmrStt;
extern Preferences preferences;

extern QueueHandle_t QueueSerialOut;   
extern QueueHandle_t QueueBtOut    ;
extern QueueHandle_t QueueRs485Out ;
extern QueueHandle_t QueueRs485In  ;

extern SemaphoreHandle_t SemaphoreTaskAutomat;

void initObjects();




#endif

