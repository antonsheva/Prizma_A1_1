#include "tasks.h"
 
 

struct_rebModSerial rmSer1;
struct_rebModSerial rmSer2;
struct_rebModSerial rmRxTx;
 
BYTE G_waitResponse = 0;
BYTE G_opCode       = 0;
BYTE G_opList[16] = {0};
BYTE G_opQty = 0;
bool G_swtchActDev  = false;
bool G_waitUnblockTasks[32] = {false};

AN_cmd* cCmd;

void unblockTasks(){
    if(G_waitUnblockTasks[SuspendTask_init      ])vTaskResume(TaskHandle_init      );
    if(G_waitUnblockTasks[SuspendTask_RebMod_In ])vTaskResume(TaskHandle_RebMod_In );
    if(G_waitUnblockTasks[SuspendTask_RebMod_Out])vTaskResume(TaskHandle_RebMod_Out);
    if(G_waitUnblockTasks[SuspendTask_BT_In     ])vTaskResume(TaskHandle_BT_In     );
    if(G_waitUnblockTasks[SuspendTask_BT_Out    ])vTaskResume(TaskHandle_BT_Out    );
    if(G_waitUnblockTasks[SuspendTask_Serial_In ])vTaskResume(TaskHandle_Serial_In );
    if(G_waitUnblockTasks[SuspendTask_Serial_Out])vTaskResume(TaskHandle_Serial_Out);
    if(G_waitUnblockTasks[SuspendTask_RS485_In  ])vTaskResume(TaskHandle_RS485_In  );
    if(G_waitUnblockTasks[SuspendTask_RS485_Out ])vTaskResume(TaskHandle_RS485_Out );
    if(G_waitUnblockTasks[SuspendTask_automat   ])vTaskResume(TaskHandle_automat   );  
    for(int i=0; i<10; i++)G_waitUnblockTasks[i] = false;  
}

void initTasks(){
    rmSer1.rx = 34;
    rmSer1.tx = 32;
    rmSer2.rx = 35;
    rmSer2.tx = 33;
 
    cCmd = AN_cmd::getI();
    cCmd->init();
    RebModCntrl::getI()->selDev = ACTIVE_REBMOD_1;
     
    AN_rs485::getI()->init();
     


    // vTaskResume(TaskHandle_automat);
}

void Task_RebMod_In  (void *param){
    String readData = "";
    for(;;){
        // if(Serial1.available()){
        //     readData = Serial1.readString();
        //     Serial.println(readData);
        //     haveNewRebModData(readData);
        // }
        vTaskDelay(100);
    }
}
void Task_RebMod_Out (void *param){
    BYTE activeRebMod = ACTIVE_REBMOD_1;
    char data[128]={0};
    int cnt;
    String str;
    for(;;){
        if(xQueueReceive(RebModCntrl::getI()->queueOut, &data[0], portMAX_DELAY)){
            if(activeRebMod != RebModCntrl::getI()->selDev){
                activeRebMod = RebModCntrl::getI()->selDev;
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
    char data[128]={0};
    for(;;){
        if(xQueueReceive(QueueBtOut, &data[0], portMAX_DELAY)){

            Serial.println("  ---  Response  ----");
            
            Serial.println(String(data));
        }
        vTaskDelay(10);
    }
}
void Task_Serial_In (void *param){
    bool rm = false;
	char buff[128];
	String str;

    for(;;){
        vTaskDelay(1000);
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
    _RS485_data outData;
    for(;;){
        if(xQueueReceive(QueueRs485Out, &outData, portMAX_DELAY)){ 
            Serial2.write(outData.data, outData.dataLen);       
        }        
        vTaskDelay(1);
    }
}

void Task_init(void *param){
    AN_rs485* rs485 = AN_rs485::getI();
    String str = "text field"; 
    _MSG_PACK msg;
    msg.cmd       = 0x11;
    msg.modCode1  = 0x22;
    msg.modCode1  = 0x33;
    msg.mask1     = 0x44444444;
    msg.mask2     = 0x55555555;

    msg.txtDataLen = str.length()+1;
    str.toCharArray(msg.txtData, str.length()+1);  

    for(;;){
        rs485->sendData(1, &msg);    
        vTaskDelay(500);        
    }


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
            case CMD_AT       : RebModCntrl::getI()->At();       break;
            case CMD_GET_ATBT : RebModCntrl::getI()->getAtbt();  break;
            case CMD_GET_ATC  : RebModCntrl::getI()->getAtc();   break;
            case CMD_SET_ATC  : RebModCntrl::getI()->setAtc();   break;
            case CMD_GET_ATI  : RebModCntrl::getI()->getAti();   break;
            case CMD_ATZ      : RebModCntrl::getI()->setAtz();   break;
            case CMD_SET_ATW  : RebModCntrl::getI()->setAtw();   break;
            case CMD_SET_ATE0 : RebModCntrl::getI()->setAte0();  break;
            case CMD_SET_ATE1 : RebModCntrl::getI()->setAte1();  break;
            
        }
        G_waitUnblockTasks[SuspendTask_automat] = true;
        vTaskSuspend(NULL);    
        if(G_swtchActDev)
            RebModCntrl::getI()->selDev = 
                (RebModCntrl::getI()->selDev==ACTIVE_REBMOD_1) ?
                                ACTIVE_REBMOD_2 : ACTIVE_REBMOD_1;
    }
 
    if(start){
        start = 0;
        G_opQty = 0;
        G_swtchActDev = 0;
        Serial.println("   --- DEV 1 ----------- ");
        Serial.println("MT   -> "+String(jmrStt->rebMod[0].mc  ));
        Serial.println("SP   -> "+String(jmrStt->rebMod[0].mask));
        Serial.println("VCPU -> "+String(jmrStt->rebMod[0].vcpu));
        Serial.println("TEMP -> "+String(jmrStt->rebMod[0].temp));
        Serial.println("   --- DEV 2 ----------- ");
        Serial.println("MT   -> "+String(jmrStt->rebMod[1].mc  ));
        Serial.println("SP   -> "+String(jmrStt->rebMod[1].mask));
        Serial.println("VCPU -> "+String(jmrStt->rebMod[1].vcpu));
        Serial.println("TEMP -> "+String(jmrStt->rebMod[1].temp)); 
    }



    vTaskSuspend(NULL);    
  }
}

 












