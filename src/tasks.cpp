#include "tasks.h"
 



QueueHandle_t QueueRebModIn   = NULL;   
QueueHandle_t QueueRebModOut  = NULL;   
QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;

SemaphoreHandle_t SemaphoreTaskAutomat = NULL;

BYTE G_activeRebMod = ACTIVE_REBMOD_1;
BYTE G_waitResponse = 0;
BYTE G_autCode      = 0;

struct_rebModSerial rmSer1;
struct_rebModSerial rmSer2;
struct_rebModSerial rmRxTx;

struct_devState devStt1;
struct_devState devStt2;




void initTasks(){
 
    rmSer1.rx = 34;
    rmSer1.tx = 32;
    rmSer2.rx = 35;
    rmSer2.tx = 33;

    QueueRebModIn        = xQueueCreate(2, 128);
    QueueRebModOut       = xQueueCreate(2, 128);
    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    SemaphoreTaskAutomat = xSemaphoreCreateBinary();
}

void Task_RebMod_In  (void *param){
    String readData = "";
    for(;;){
        if(Serial1.available()){
            readData = Serial1.readString();
            if(G_waitResponse){
                switch(G_autCode){
                    case AUT_CODE_GET_STT_1: getDevInfo(1, readData);
                                             G_autCode = AUT_CODE_GET_STT_2;  
                        break;
                    case AUT_CODE_GET_STT_2: getDevInfo(2, readData); break;
                    
                }  
                G_waitResponse--;  
                if(G_waitResponse)xSemaphoreGive(SemaphoreTaskAutomat);
            }
        }

        vTaskDelay(10);
    }
}
void Task_RebMod_Out (void *param){
    BYTE activeRebMod = ACTIVE_REBMOD_1;
    char data[128]={0};
    int cnt;
  
    for(;;){
        if(xQueueReceive(QueueRebModOut, &data[0], portMAX_DELAY)){
            if(activeRebMod != G_activeRebMod){
                activeRebMod = G_activeRebMod;
                rmRxTx = (activeRebMod == ACTIVE_REBMOD_1) ? rmSer1 : rmSer2;
                Serial1.begin(9600, SERIAL_8N1, rmRxTx.rx, rmRxTx.tx);
            }    
            cnt = 0;
            
            while(data[cnt])Serial1.write(data[cnt++]);  
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 1000ms
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
            str = Serial.readString();
            str.toCharArray(buff,128);
            xQueueSend(QueueRebModOut, buff, portMAX_DELAY);
            if(G_activeRebMod==ACTIVE_REBMOD_1)G_activeRebMod = ACTIVE_REBMOD_2;
            else                               G_activeRebMod = ACTIVE_REBMOD_1;
        }

        vTaskDelay(10);
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

void Task_automat(void *param)
{   
    int cnt = 0;
    char buff[128];
    String str = "";
    for(;;){
        if(xSemaphoreTake(SemaphoreTaskAutomat, portMAX_DELAY)){
            switch (G_autCode){
                case AUT_CODE_GET_STT_1: 
                    G_activeRebMod = ACTIVE_REBMOD_1;
                    sendCmdToRM_WithoutParam("ATI\n\r");  break;
                case AUT_CODE_GET_STT_2: 
                    G_activeRebMod = ACTIVE_REBMOD_2;
                    sendCmdToRM_WithoutParam("ATI\n\r");  break;
                
            }
        }    
        vTaskDelay(10);
    }
}

 












