#include "tasks.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* cCmd;
 
int G_rebModAut_tm = 0; 
void initTasks(){
    initPreferencesData();
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

static portMUX_TYPE spinlockPref = portMUX_INITIALIZER_UNLOCKED;
void setPref(String param, BYTE val){
    taskENTER_CRITICAL(&spinlockPref);
    preferences.begin("prefData", false);
    preferences.putUChar(param.c_str(), val);
    preferences.end();
    taskEXIT_CRITICAL(&spinlockPref);            
}


void Task_savePreferences  (void *param){
    String readData = "";
    _MSG_PACK msg;
    AN_cmd *cCmd = AN_cmd::getI();
    int cnt = 0;

    for(;;){
        xQueueReceive(QueuePreferences, &msg, portMAX_DELAY);
        Serial.println(" addr -> "+String(msg.addrEsp32));
        
        for(int i=0; i < 5; i++){
            if(i==0){
                if((msg.addrEsp32 > 0) && (msg.addrEsp32 < 127) && (msg.cmd == CMD_SET_ADDR_ESP)){
                        setPref(PARAM_ADDR_ESP, msg.addrEsp32);          
                        G_lJmrStt.esp32Addr = msg.addrEsp32;
                        cCmd->getAddresses();
                }
            }
            if(i==1){                
                if((msg.addrRm1 > 0) && (msg.addrRm1 < 127) && (msg.cmd == CMD_SET_ADDR_RM_1)){
                    setPref(PARAM_ADDR_RM_1, msg.addrRm1);
                    G_lJmrStt.rebMod[0].address = msg.addrRm1;
                    cCmd->getAddresses();
                }
            }
            if(i==2){
                if((msg.addrRm2 > 0) && (msg.addrRm2 < 127) && (msg.cmd == CMD_SET_ADDR_RM_2)){
                    setPref(PARAM_ADDR_RM_2, msg.addrRm2);
                    G_lJmrStt.rebMod[1].address = msg.addrRm2;
                    cCmd->getAddresses();
                }
            }
            if(i==3){
                if((msg.pwr1 == PWR_ON) || (msg.pwr1 == PWR_OFF)){
                    setPref(PARAM_PWR_1, msg.pwr1);          
                    G_lJmrStt.rebMod[0].pwr = msg.pwr1;
                } 
            }
            if(i==4){
                if((msg.pwr2 == PWR_ON) || (msg.pwr2 == PWR_OFF)){
                    setPref(PARAM_PWR_2, msg.pwr2);          
                    G_lJmrStt.rebMod[1].pwr = msg.pwr2;
                } 
            }
            vTaskDelay(10);
        }               
        vTaskDelay(100);
    }
}

void Task_RebMod_Out (void *param){
    BYTE activeRebMod = 0;
    char data[128]={0};
    int cnt;
    String str;
    for(;;){
        // if(xQueueReceive(RmCtrl::getI()->queueOut, &data[0], portMAX_DELAY)){
 


        // }
        vTaskDelete(NULL); // 1000ms
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

void Task_txBt     (void *param){
    _MSG_PACK msg;
    AN_rs485 *rs485 = AN_rs485::getI();
    for(;;){
        if(xQueueReceive(QueueBtTransmit, &msg, portMAX_DELAY)){
            Serial.println("  ---  Response  ----");
            rs485->sendMsgToBt(&msg);
        }
        vTaskDelay(10);
    }
}

void Task_watiDataPacks(void *param){
    AN_rs485 *rs485 = AN_rs485::getI();
    for(;;){
        rs485->waitTimer++;
        if(rs485->endOfDataPacks){
            rs485->waitTimer = 0;
            vTaskSuspend(NULL);
        }
        if(rs485->waitTimer > 500){
            rs485->resetDataPackProcess("error timeout dataPacks");
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
            vTaskResume(TaskHandle_pollRs485);
        }
        vTaskDelay(20);
    }
}

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
void Task_txRs485 (void *param){
    _RS485_data qData; 
    int packCnt = 0;   
    int len;
    for(;;){
        xQueueReceive(QueueRs485Transmit, &qData, portMAX_DELAY);
        Serial.println((char*)qData.data);
        for(int i=0; i < qData.packQty; i++){
            len = (i < qData.packQty-1) ? 120 : qData.lastPackLen;
            taskENTER_CRITICAL(&my_spinlock);
            digitalWrite(RS485_DIR_DRV,1);
            Serial2.write(&qData.data[i*120], len);     
            Serial2.flush();
            digitalWrite(RS485_DIR_DRV,0);  
            taskEXIT_CRITICAL(&my_spinlock);
            vTaskDelay(50);
        }
    }
}
 
void Task_rxRs485 (void *param){
    char str[1024];
    AN_rs485 *rs485 = AN_rs485::getI();
    for(;;){
        xQueueReceive(QueueRs485Receive, str, portMAX_DELAY);
        rs485->dataPackStr = String(str);
        rs485->processReceivedData();
        
        vTaskDelay(1);
    
    }
}

void Task_rs485_send(void *param){
    FastCRC16 crc;
    _MSG_PACK msg;
    _RS485_data msg485;
    JsonDocument doc;    
    char serialData [1024]      = "\0"; 
    char tmpData    [1024]      = "\0";     
    WORD crc16;    
    for(;;){
        xQueueReceive(QueueRs485Send, &msg, portMAX_DELAY);

        doc[PARAM_SENDER    ]   = G_lJmrStt.esp32Addr;        
        doc[PARAM_CMD       ]   = msg.cmd;
        doc[PARAM_ADDR_ESP  ]   = msg.addrEsp32;  
        doc[PARAM_MSG_DIR   ]   = msg.direction;
        doc[PARAM_RESPONSE  ]   = msg.response;      
        
        if(msg.direction == MSG_DIR_REQUEST){
            switch (msg.cmd){
                case CMD_SEARCH_DEVICES:

                break;
                case CMD_SET_STATE:                
                    doc[PARAM_MOD_CODE_1]   = msg.modCode1;
                    doc[PARAM_MOD_CODE_2]   = msg.modCode2;
                    doc[PARAM_MASK_1    ]   = msg.mask1;
                    doc[PARAM_MASK_2    ]   = msg.mask2;
                    doc[PARAM_PWR_1     ]   = msg.pwr1;
                    doc[PARAM_PWR_2     ]   = msg.pwr2;   
                    doc[PARAM_RESPONSE  ]   = msg.pwr2;            
                break;

                case CMD_GET_JMMR_DATA:

                break;

            }
        }else{
            switch (msg.response){
                case RESP_GET_JMMR_LIST:
                    doc[PARAM_MOD_CODE_1]   = msg.modCode1;
                    doc[PARAM_MOD_CODE_2]   = msg.modCode2;
                    doc[PARAM_MASK_1    ]   = msg.mask1;
                    doc[PARAM_MASK_2    ]   = msg.mask2;
                    doc[PARAM_PWR_1     ]   = msg.pwr1;
                    doc[PARAM_PWR_2     ]   = msg.pwr2;  
                break;
                case RESP_GET_JMMR_DATA:
                    doc[PARAM_MOD_CODE_1]   = msg.modCode1;
                    doc[PARAM_MOD_CODE_2]   = msg.modCode2;
                    doc[PARAM_MASK_1    ]   = msg.mask1;
                    doc[PARAM_MASK_2    ]   = msg.mask2;
                    doc[PARAM_PWR_1     ]   = msg.pwr1;
                    doc[PARAM_PWR_2     ]   = msg.pwr2;
                    doc[PARAM_TXT       ]   = G_lJmrStt.info;
                    doc[PARAM_TXT_LEN   ]   = G_lJmrStt.info.length();   
                break;                

            }
        }

        if(msg.addrRm1 && msg.addrRm1 < 128)doc[PARAM_ADDR_RM_1 ] = msg.addrRm1;
        if(msg.addrRm2 && msg.addrRm2 < 128)doc[PARAM_ADDR_RM_2 ] = msg.addrRm2;   
 
        int len = serializeJson(doc, serialData); 
    
        doc[PARAM_MOD_CODE_1]   = NULL;
        doc[PARAM_MOD_CODE_2]   = NULL;
        doc[PARAM_MASK_1    ]   = NULL;
        doc[PARAM_MASK_2    ]   = NULL;
        doc[PARAM_PWR_1     ]   = NULL;
        doc[PARAM_PWR_2     ]   = NULL;
        doc[PARAM_TXT       ]   = NULL;
        doc[PARAM_TXT_LEN   ]   = NULL; 
 
        crc16 = crc.modbus((const uint8_t*) serialData, len);
        
        len += 18;
        sprintf(tmpData, "start___%s_%X_stop",serialData, crc16);
        for(int i=0; i< 1024; i++)msg485.data[i] = 0;
        for(int i=0; i< len; i++)msg485.data[i] = tmpData[i];

        msg485.dataLen     = len;
        msg485.packQty     = len/120+1;
        msg485.lastPackLen = len%120;
        
        xQueueSend(QueueRs485Transmit, &msg485, portMAX_DELAY);

        vTaskDelay(10);
    }
}

void Task_pollRs485(void *param){
    int iterNum = 0;
	_MSG_PACK msg;
    AN_rs485 *rs = AN_rs485::getI();
    BYTE data = 0;
    bool stop = 0;
	for(;;){
        if(iterNum < rs->subscribersQty){
            rs->prepMsg(&msg, iterNum);
            if(msg.addrEsp32 == G_lJmrStt.esp32Addr){
                cCmd->updateLocalData(&msg);
            }else{
                xQueueSend(QueueRs485Send, &msg, portMAX_DELAY);	
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
            // rs->sendMsgToBt();
            xQueueSend(QueueBtTransmit, &msg, portMAX_DELAY);
            ASetOccurredEvent(Event_finishLoadConfig);
            vTaskSuspend(TaskHandle_wait485Resp);
        }
		vTaskSuspend(NULL);
	}    
}

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
        G_opCnt++;  
        G_opQty--; 
        G_rebModAut_tm = 0;             
        vTaskSuspend(NULL);

        if(G_swtchActDev)
            RmCtrl::getI()->selDev = (RmCtrl::getI()->selDev==0) ? 1 : 0;
    }
    G_opCnt = 0;
  
    G_swtchActDev = 0;
    Serial.println("   --- DEV 1 ----------- ");
    Serial.println("ModCodeT -> "+String(G_lJmrStt.rebMod[0].mc  ));
    Serial.println("Mask     -> "+String(G_lJmrStt.rebMod[0].mask));
    Serial.println("Pwr      -> "+String(G_lJmrStt.rebMod[0].pwr));
    Serial.println("VCPU     -> "+String(G_lJmrStt.rebMod[0].vcpu));
    Serial.println("TEMP     -> "+String(G_lJmrStt.rebMod[0].temp));

    Serial.println("   --- DEV 2 ----------- ");
    Serial.println("ModCodeT -> "+String(G_lJmrStt.rebMod[1].mc  ));
    Serial.println("Mask     -> "+String(G_lJmrStt.rebMod[1].mask));
    Serial.println("Pwr      -> "+String(G_lJmrStt.rebMod[1].pwr));
    Serial.println("VCPU     -> "+String(G_lJmrStt.rebMod[1].vcpu));
    Serial.println("TEMP     -> "+String(G_lJmrStt.rebMod[1].temp));
  
    vTaskSuspend(NULL);    
  }
}

 












