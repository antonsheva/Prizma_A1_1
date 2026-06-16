#ifndef _TASKS_
#define _TASKS_

#include "main.h"
#include "AN_rs485.h"
#include "AN_cbFuncs.h"

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




enum {
    SuspendTask_init      ,
    SuspendTask_RebMod_In ,
    SuspendTask_RebMod_Out,
    SuspendTask_BT_In     ,
    SuspendTask_BT_Out    ,
    SuspendTask_Serial_In ,
    SuspendTask_eventControl,
    SuspendTask_RS485_In  ,
    SuspendTask_watiDataPacks ,
    SuspendTask_rebModAut      
};
 
enum {
    Event_empty,
    Event_finishLoadConfig
};
 
void initTasks();

void Task_savePreferences(void *param);
void Task_RebMod_Out(void *param);
void Task_rs485_send(void *param);
void Task_txBt(void *param);
void Task_rxRs485(void *param);
void ASetExpectedEvent(int event);
void ASetOccurredEvent(int event);
void AProcessEvent(int event);
void Task_eventControl(void *param);
void Task_txRs485(void *param);
void Task_watiDataPacks(void *param);
void Task_wait485Resp(void *param);
void Task_pollRs485(void *param);
void Task_monitor(void *param);
void Task_rebModAut(void *param);




typedef struct {
    BYTE rx;
    BYTE tx;
}struct_rebModSerial;

typedef struct{
    BYTE   rmNum;
    BYTE   mc;
    DWORD  mask;
    BYTE   address;
    BYTE   echo;
    float  vcpu;
    float  temp;
    String info[32]; 
}struct_devState;

 
 

#endif


