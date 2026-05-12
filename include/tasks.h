#ifndef _TASKS_
#define _TASKS_

#include "main.h"

#define ACTIVE_REBMOD_1 1
#define ACTIVE_REBMOD_2 2

#define AUT_CODE_GET_STT_1   1
#define AUT_CODE_GET_STT_2   2
#define AUT_CODE_GET_ATC_1   3
#define AUT_CODE_GET_ATC_2   4
#define AUT_CODE_SET_ATC_1   5
#define AUT_CODE_SET_ATC_2   6
#define AUT_CODE_GET_ATBT_1  7
#define AUT_CODE_GET_ATBT_2  8
#define AUT_CODE_ATZ_1       9
#define AUT_CODE_ATZ_2       10
#define AUT_CODE_GET_ATW_1   11
#define AUT_CODE_GET_ATW_2   12
#define AUT_CODE_SET_ATW_1   13
#define AUT_CODE_SET_ATW_2   14
#define AUT_CODE_SET_ATE_1   15
#define AUT_CODE_SET_ATE_2   16

// extern QueueHandle_t QueueRebModIn ;   
// extern QueueHandle_t QueueRebModOut;   



extern BYTE G_waitResponse;
extern BYTE G_opCode     ;

extern bool G_waitUnblockTasks[32];
extern BYTE G_opList[16];
extern BYTE G_opQty;


enum {
    SuspendTask_init      ,
    SuspendTask_RebMod_In ,
    SuspendTask_RebMod_Out,
    SuspendTask_BT_In     ,
    SuspendTask_BT_Out    ,
    SuspendTask_Serial_In ,
    SuspendTask_Serial_Out,
    SuspendTask_RS485_In  ,
    SuspendTask_RS485_Out ,
    SuspendTask_automat      
};

void unblockTasks();
void initTasks();

void Task_RebMod_In(void *param);
void Task_RebMod_Out(void *param);
void Task_BT_In(void *param);
void Task_BT_Out(void *param);
void Task_Serial_In(void *param);
void Task_Serial_Out(void *param);
void Task_RS485_In(void *param);
void Task_RS485_Out(void *param);
void Task_init(void *param);
void Task_automat(void *param);




typedef struct {
    BYTE rx;
    BYTE tx;
}struct_rebModSerial;

typedef struct{
    BYTE   devNum;
    BYTE   mc;
    DWORD  mask;
    BYTE   address;
    BYTE   echo;
    float  vcpu;
    float  temp;
    String info[32]; 
}struct_devState;

// extern struct_devState devStt1;
// extern struct_devState devStt2;
// extern struct_devState devStt[2];



extern struct_rebModSerial rmSer1;
extern struct_rebModSerial rmSer2;
extern struct_rebModSerial rmRxTx;

extern bool G_swtchActDev;

#endif


