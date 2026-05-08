#include "tasks.h"
 
QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;
SemaphoreHandle_t SemaphoreTaskAutomat = NULL;
 
struct_rebModSerial rmSer1;
struct_rebModSerial rmSer2;
struct_rebModSerial rmRxTx;
 
BYTE G_waitResponse = 0;
BYTE G_opCode       = 0;
BYTE G_opList[16] = {0};
BYTE G_opQty = 0;
bool G_swtchActDev  = false;
bool G_waitUnblockTasks[32] = {false};




void initTasks(){
    rmSer1.rx = 34;
    rmSer1.tx = 32;
    rmSer2.rx = 35;
    rmSer2.tx = 33;
 
    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    SemaphoreTaskAutomat = xSemaphoreCreateBinary();
    
    cCmd.cRebMod->selDev = ACTIVE_REBMOD_1;


    // vTaskResume(TaskHandle_automat);
}

void Task_RebMod_In  (void *param){
    String readData = "";
    for(;;){
        if(Serial1.available()){
            readData = Serial1.readString();
            Serial.println(readData);
            isNewRebModData(readData);
        }
        vTaskDelay(1);
    }
}
void Task_RebMod_Out (void *param){
    BYTE activeRebMod = ACTIVE_REBMOD_1;
    char data[128]={0};
    int cnt;
    String str;
    for(;;){
        if(xQueueReceive(cCmd.cRebMod->queueOut, &data[0], portMAX_DELAY)){
            if(activeRebMod != cCmd.cRebMod->selDev){
                activeRebMod = cCmd.cRebMod->selDev;
                rmRxTx = (activeRebMod == ACTIVE_REBMOD_1) ? rmSer1 : rmSer2;
                Serial1.begin(9600, SERIAL_8N1, rmRxTx.rx, rmRxTx.tx);
            }    
            cnt = 0;
            str = String(data);
            Serial1.println(str);
            Serial.println(str);
            
            // while(data[cnt])Serial1.write(data[cnt++]);  


        }
        vTaskDelay(1); // 1000ms
    } 
}
void Task_BT_In      (void *param){
    for(;;){

        vTaskDelay(10);
    }
}
void Task_BT_Out     (void *param){
    for(;;){

        vTaskDelay(10);
    }
}
void Task_Serial_In (void *param){
    bool rm = false;
	char buff[128];
	String str;

    for(;;){
        if(Serial.available()){
            AReadSerialData(SRC_COM);
            if(cCmd.AGetJson()){ //there is error in JSON-data
                xQueueSend(cCmd.cRebMod->queueOut, G_tmpBuff, portMAX_DELAY);
            }else{
                cCmd.AProcessCmd();
            }
        }

        vTaskDelay(1);
    }
}

void Task_Serial_Out(void *param){
    String str;
    for(;;){


        vTaskDelay(10);
    }
}

void Task_RS485_In (void *param){
    for(;;){

        vTaskDelay(10);
    }
}
void Task_RS485_Out(void *param){
    for(;;){

        vTaskDelay(10);
    }
}

void Task_init(void *param){
//   int opList[] = {
//     CMD_SET_ATE  ,
//     CMD_SET_ATE  ,    
//     CMD_GET_ATI  ,
//     CMD_GET_ATI  ,
//     CMD_GET_ATC  ,
//     CMD_GET_ATC  ,
//     CMD_GET_ATBT ,
//     CMD_GET_ATBT   
//   };  
//   cCmd.cRebMod->activeDev = ACTIVE_REBMOD_1;
//   for(int i=0; i<8; i++){
//     G_opCode = opList[i];
//     // switch(opList[i]){         
//     //     case CMD_SET_ATE   : rebModFuncs_setAte  (); break; 
//     //     case CMD_GET_ATI   : rebModFuncs_getAti  (); break;
//     //     case CMD_GET_ATC   : rebModFuncs_getAtc  (); break;
//     //     case CMD_GET_ATBT  : rebModFuncs_getAtbt (); break;
//     // }
//     G_waitUnblockTasks[SuspendTask_init] = true;
//     vTaskSuspend(NULL);    
//     cCmd.cRebMod->activeDev = (cCmd.cRebMod->activeDev==ACTIVE_REBMOD_1) ? 
//                                 ACTIVE_REBMOD_2 : ACTIVE_REBMOD_1;
//   }

    
//     Serial.println("   --- DEV 1 ----------- ");
//     Serial.println("MT   -> "+String(cCmd.cRebMod->devStt[0].mc  ));
//     Serial.println("SP   -> "+String(cCmd.cRebMod->devStt[0].mask));
//     Serial.println("VCPU -> "+String(cCmd.cRebMod->devStt[0].vcpu));
//     Serial.println("TEMP -> "+String(cCmd.cRebMod->devStt[0].temp));
//     Serial.println("   --- DEV 2 ----------- ");
//     Serial.println("MT   -> "+String(cCmd.cRebMod->devStt[1].mc  ));
//     Serial.println("SP   -> "+String(cCmd.cRebMod->devStt[1].mask));
//     Serial.println("VCPU -> "+String(cCmd.cRebMod->devStt[1].vcpu));
//     Serial.println("TEMP -> "+String(cCmd.cRebMod->devStt[1].temp));


  vTaskDelete(NULL);
 
}

void Task_automat(void *param)
{   
  static bool start = 0;  
  for(;;){
    for(int i=0; i<G_opQty; i++){
        start = 1;
        G_opCode = G_opList[i];

        switch(G_opList[i]){         
            case CMD_AT       : cCmd.cRebMod->At();       break;
            case CMD_GET_ATBT : cCmd.cRebMod->getAtbt();  break;
            case CMD_GET_ATC  : cCmd.cRebMod->getAtc();   break;
            case CMD_SET_ATC  : cCmd.cRebMod->setAtc();   break;
            case CMD_GET_ATI  : cCmd.cRebMod->getAti();   break;
            case CMD_ATZ      : cCmd.cRebMod->setAtz();   break;
            case CMD_SET_ATW  : cCmd.cRebMod->setAtw();   break;
            case CMD_SET_ATE0 : cCmd.cRebMod->setAte0();  break;
            case CMD_SET_ATE1 : cCmd.cRebMod->setAte1();  break;
            
        }
        G_waitUnblockTasks[SuspendTask_automat] = true;
        vTaskSuspend(NULL);    
        if(G_swtchActDev)
            cCmd.cRebMod->selDev = 
                (cCmd.cRebMod->selDev==ACTIVE_REBMOD_1) ?
                                ACTIVE_REBMOD_2 : ACTIVE_REBMOD_1;
    }
 
    if(start){
        start = 0;
        G_opQty = 0;
        G_swtchActDev = 0;
        Serial.println("   --- DEV 1 ----------- ");
        Serial.println("MT   -> "+String(cCmd.cRebMod->devStt[0].mc  ));
        Serial.println("SP   -> "+String(cCmd.cRebMod->devStt[0].mask));
        Serial.println("VCPU -> "+String(cCmd.cRebMod->devStt[0].vcpu));
        Serial.println("TEMP -> "+String(cCmd.cRebMod->devStt[0].temp));
        Serial.println("   --- DEV 2 ----------- ");
        Serial.println("MT   -> "+String(cCmd.cRebMod->devStt[1].mc  ));
        Serial.println("SP   -> "+String(cCmd.cRebMod->devStt[1].mask));
        Serial.println("VCPU -> "+String(cCmd.cRebMod->devStt[1].vcpu));
        Serial.println("TEMP -> "+String(cCmd.cRebMod->devStt[1].temp)); 
    }



    vTaskSuspend(NULL);    
  }
}

 












