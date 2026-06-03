#include "tasks.h"
 
#define MAX_JMMR_QTY 10


 
// BYTE G_waitResponse = 0;
// BYTE G_opCode       = 0;
// BYTE G_opList[16] = {0};
// BYTE G_opQty = 0;
// bool G_swtchActDev  = false;
// bool G_waitUnblockTasks[32] = {false};
// DWORD G_wait485Cnt = 0;

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

 
    initLocalAddresses();
    initObjects();
    cCmd = AN_cmd::getI();
    cCmd->init();
    RmCtrl *rm = RmCtrl::getI();
    AN_rs485::getI()->init();
 
    rm->selDev = 0;
    rm->rmSer1.rx = 34;
    rm->rmSer1.tx = 32;
    rm->rmSer2.rx = 35;
    rm->rmSer2.tx = 33;

    // vTaskResume(TaskHandle_automat);
}

void Task_RebMod_In  (void *param){
    String readData = "";
    for(;;){

        vTaskDelete(NULL);
    }
}
void Task_RebMod_Out (void *param){
    BYTE activeRebMod = 0;
    char data[128]={0};
    int cnt;
    String str;
    for(;;){
        if(xQueueReceive(RmCtrl::getI()->queueOut, &data[0], portMAX_DELAY)){
 


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
        // vTaskDelay(1);
        vTaskDelete(NULL);
    }
}

void Task_Serial_Out(void *param){
    String str;
    for(;;){


        // vTaskDelay(10);
        vTaskDelete(NULL);

    }
}

void Task_RS485_In (void *param){
    for(;;){
        
        // vTaskDelay(10);
        vTaskDelete(NULL);
    }
}
void Task_RS485_Out(void *param){
    _RS485_data outData;
    for(;;){
 
        vTaskDelete(NULL);
    }
}

void Task_wait485Resp(void *param){

    for(;;){
        if(G_wait485Cnt < 50){
            G_wait485Cnt++;
        }else{
            vTaskResume(TaskHandle_txRs485);
        }
        
        vTaskDelay(10);
    }
}




void Task_txRs485(void *param){
    int iterNum = 0;
	_MSG_PACK msg;
    AN_rs485 *rs = AN_rs485::getI();
    bool stop = 0;
	for(;;){
        if(iterNum < rs->subscribersQty){
            rs->prepMsg(&msg, iterNum);
            if(msg.addressee == G_lJmrStt.esp32Addr){
                cCmd->updateLocalData(iterNum);
            }else{
                rs->sendData(&msg);		
            }
            iterNum++;
            G_wait485Cnt = 0;
            if(!stop){
                vTaskResume(TaskHandle_wait485Resp);
                stop = 1;
            }
        }else{
            stop = 0;
            iterNum = 0;
            cCmd->sendResultToBt();
            vTaskSuspend(TaskHandle_wait485Resp);
        }            
		vTaskSuspend(NULL);
	}    
}

void Task_init(void *param){

    
 

  AN_cmd *mCmd = AN_cmd::getI();
  vTaskDelay(100);
  mCmd->getInfo();
  
  for(;;){

    vTaskDelete(NULL);
  }

}

void Task_automat(void *param)
{   
  static bool start = 0;  
  for(;;){
    for(int i=0; i<G_opQty; i++){
        start = 1;
        G_opCode = G_opList[i];

        switch(G_opList[i]){         
            case CMD_AT       : RmCtrl::getI()->At();       break;
            case CMD_GET_ATBT : RmCtrl::getI()->getAtbt();  break;
            case CMD_GET_ATC  : RmCtrl::getI()->getAtc();   break;
            case CMD_SET_ATC  : RmCtrl::getI()->setAtc();   break;
            case CMD_GET_ATI  : RmCtrl::getI()->getAti();   break;
            case CMD_ATZ      : RmCtrl::getI()->setAtz();   break;
            case CMD_SET_ATW  : RmCtrl::getI()->setAtw();   break;
            case CMD_SET_ATE0 : RmCtrl::getI()->setAte0();  break;
            case CMD_SET_ATE1 : RmCtrl::getI()->setAte1();  break;
            
        }
        G_waitUnblockTasks[SuspendTask_automat] = true;
        vTaskSuspend(NULL);    
        if(G_swtchActDev)
            RmCtrl::getI()->selDev = (RmCtrl::getI()->selDev==0) ? 1 : 0;
    }
 
    if(start){
        start = 0;
        G_opQty = 0;
        G_swtchActDev = 0;
        Serial.println("   --- DEV 1 ----------- ");
        Serial.println("MT   -> "+String(G_lJmrStt.rebMod[0].mc  ));
        Serial.println("SP   -> "+String(G_lJmrStt.rebMod[0].mask));
        Serial.println("VCPU -> "+String(G_lJmrStt.rebMod[0].vcpu));
        Serial.println("TEMP -> "+String(G_lJmrStt.rebMod[0].temp));
        Serial.println("   --- DEV 2 ----------- ");
        Serial.println("MT   -> "+String(G_lJmrStt.rebMod[1].mc  ));
        Serial.println("SP   -> "+String(G_lJmrStt.rebMod[1].mask));
        Serial.println("VCPU -> "+String(G_lJmrStt.rebMod[1].vcpu));
        Serial.println("TEMP -> "+String(G_lJmrStt.rebMod[1].temp)); 
    }



    vTaskSuspend(NULL);    
  }
}

 












