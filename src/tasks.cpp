#include "tasks.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* cCmd;
  
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

void ASetExpectedEvent(int event){
    G_eventExpected[event] = true;
}

void ASetOccurredEvent(int event){
    G_eventOccurred[event] = true;
}


void AProcessEvent(int event){
    switch (event){
        case Event_finishLoadConfig   : cCmd->setState(NULL);     break;
    }
}

void Task_eventControl(void *param){
    String str;
    for(;;){
        for(int event=0; event<8; event++){
            if(G_eventOccurred[event]){
                G_eventOccurred[event] = false;
                if(G_eventExpected[event]){
                    G_eventExpected[event] = false;
                    AProcessEvent(event);
                }
            }
        }

        vTaskDelay(10);
    }
}

void Task_transmitRs485 (void *param){
    _RS485_data qData;    
    for(;;){
        xQueueReceive(QueueRs485Out, &qData, portMAX_DELAY);
        digitalWrite(RS485_DIR_DRV,1);
        Serial2.write(qData.data, qData.dataLen);     
        Serial2.flush();
        digitalWrite(RS485_DIR_DRV,0);
        // vTaskDelay(10);
    }
}

void Task_watiDataPacks(void *param){
    for(;;){
        cCmd->waitTimer++;
        if(cCmd->endOfDataPacks){
            cCmd->waitTimer = 0;
            vTaskSuspend(NULL);
        }
        if(cCmd->waitTimer > 500){
            cCmd->resetDataPackProcess("error timeout dataPacks");
            vTaskSuspend(NULL);
        }
        vTaskDelay(10);
    }
}

void Task_wait485Resp(void *param){
    for(;;){
        if(G_wait485Cnt < 10){
            G_wait485Cnt++;
        }else{
            vTaskResume(TaskHandle_txRs485);
        }
        vTaskDelay(5);
    }
}


void Task_txRs485(void *param){
    int iterNum = 0;
	_MSG_PACK msg;
    AN_rs485 *rs = AN_rs485::getI();
    bool stop = 0;
	for(;;){
        // Serial.println("485 iterNum-> "+String(iterNum)+" sbrQty-> "+String(rs->subscribersQty));
        if(iterNum < rs->subscribersQty){
            rs->prepMsg(&msg, iterNum);
            if(msg.addressee == G_lJmrStt.esp32Addr){
                cCmd->updateLocalData(&msg);
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
            ASetOccurredEvent(Event_finishLoadConfig);
            vTaskSuspend(TaskHandle_wait485Resp);
        }
        // Serial.println("485 task suspend");
		vTaskSuspend(NULL);
        // Serial.println("485 task resume");
	}    
}


int G_rebModAut_tm = 0;
void Task_init(void *param){

  AN_cmd::getI()->getInfo();
  vTaskDelay(100);
  for(;;){
    if(G_rebModAut_tm < 300)G_rebModAut_tm++;
    else if(G_opQty){
        G_rebModAut_tm = 0;
        vTaskResume(TaskHandle_rebModAut);
    }




    vTaskDelay(10);
  }

}

void Task_rebModAut(void *param)
{   
     
  for(;;){
    while(G_opQty){
    
        G_opCode = G_opList[G_opCnt];
        Serial.println("rm G_opQty -> "+String(G_opQty)+"   G_opCnt -> "+String(G_opCnt)+" opCode -> "+String(G_opCode));

        switch(G_opList[G_opCnt]){         
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
        Serial.println("rm Stop task");
 
        G_opCnt++;  
        G_opQty--; 
        G_rebModAut_tm = 0;             
        vTaskSuspend(NULL);


        Serial.println("rm Resum task");   
        if(G_swtchActDev)
            RmCtrl::getI()->selDev = (RmCtrl::getI()->selDev==0) ? 1 : 0;
    }
    G_opCnt = 0;
  
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
  



    vTaskSuspend(NULL);    
  }
}

 












