#ifndef _TASKS_
#define _TASKS_

#include "main.h"

#define ACTIVE_REBMOD_1 1
#define ACTIVE_REBMOD_2 2

#define AUT_CODE_GET_STT_1   1
#define AUT_CODE_GET_STT_2   2
#define AUT_CODE_GET_ATC_1   3
#define AUT_CODE_GET_ATC_2   4
#define AUT_CODE_ATC_1       5
#define AUT_CODE_ATC_2       6
#define AUT_CODE_GET_ATBT_1  7
#define AUT_CODE_GET_ATBT_2  8
#define AUT_CODE_ATZ_1       9
#define AUT_CODE_ATZ_2       10
#define AUT_CODE_GET_ATW_1   11
#define AUT_CODE_GET_ATW_2   12
#define AUT_CODE_ATW_1       13
#define AUT_CODE_ATW_2       14




void initTasks();

void Task_RebMod_In(void *param);
void Task_RebMod_Out(void *param);
void Task_BT_In(void *param);
void Task_BT_Out(void *param);
void Task_Serial_In(void *param);
void Task_Serial_Out(void *param);
void Task_RS485_In(void *param);
void Task_RS485_Out(void *param);
void Task_automat(void *param);


extern QueueHandle_t QueueRebModIn ;   
extern QueueHandle_t QueueRebModOut;   
extern QueueHandle_t QueueSerialOut;   
extern QueueHandle_t QueueBtOut    ;

extern SemaphoreHandle_t SemaphoreTaskAutomat;


extern BYTE G_waitResponse;
extern BYTE G_autCode     ;


typedef struct {
    BYTE rx;
    BYTE tx;
}struct_rebModSerial;

typedef struct{
    BYTE   devNum;
    BYTE   modulationCode;
    DWORD  mask;
    BYTE   address;
    String info[32]; 
}struct_devState;

// extern struct_devState devStt1;
// extern struct_devState devStt2;
extern struct_devState devStt[2];



extern struct_rebModSerial rmSer1;
extern struct_rebModSerial rmSer2;
extern struct_rebModSerial rmRxTx;

extern BYTE G_activeRebMod;

#endif


