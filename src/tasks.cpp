#include "tasks.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* cCmd;
 



static portMUX_TYPE spinlockPref = portMUX_INITIALIZER_UNLOCKED;
void setPref(String param, BYTE val){
    taskENTER_CRITICAL(&spinlockPref);
    preferences.begin("prefData", false);
    preferences.putUChar(param.c_str(), val);
    preferences.end();
    taskEXIT_CRITICAL(&spinlockPref);            
}


void Task_savePreferences  (void *param){
    _MSG_PACK msg;
    AN_cmd *cCmd = AN_cmd::getI();
    int cnt = 0;
    int i = 0;
    for(;;){
        xQueueReceive(QueuePreferences, &msg, portMAX_DELAY);
    
         
                if((msg.addrEsp32 > 0) && (msg.addrEsp32 < 127) && (msg.cmd == CMD_SET_ADDR_ESP)){
                    Serial.println(" t -> 1");
                        setPref(PARAM_ADDR_ESP, msg.addrEsp32);          
                        G_lJmrStt.esp32Addr = msg.addrEsp32;
                        cCmd->getAddresses();
                }
          
              
                if((msg.addrRm1 > 0) && (msg.addrRm1 < 127) && (msg.cmd == CMD_SET_ADDR_RM_1)){
                    Serial.println(" t -> 2");

                    setPref(PARAM_ADDR_RM_1, msg.addrRm1);
                    G_lJmrStt.rebMod[0].address = msg.addrRm1;
                    cCmd->getAddresses();
                }
          
         
                if((msg.addrRm2 > 0) && (msg.addrRm2 < 127) && (msg.cmd == CMD_SET_ADDR_RM_2)){
                    Serial.println(" t -> 3");
                    setPref(PARAM_ADDR_RM_2, msg.addrRm2);
                    G_lJmrStt.rebMod[1].address = msg.addrRm2;
                    cCmd->getAddresses();
                }
    
                if (msg.cmd == CMD_SET_PWR){
                    preferences.begin("prefData", false);
                    if((msg.pwr1 == PWR_ON) || (msg.pwr1 == PWR_OFF))
                            preferences.putUChar(PARAM_PWR_1,  G_lJmrStt.rebMod[0].pwr);
                    if((msg.pwr2 == PWR_ON) || (msg.pwr2 == PWR_OFF))
                            preferences.putUChar(PARAM_PWR_2,  G_lJmrStt.rebMod[1].pwr);                    
                    preferences.end();       
                }            
        vTaskDelay(10);
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
            digitalWrite(PIN_RS485_DIR_DRV,1);
            Serial2.write(&qData.data[i*120], len);     
            Serial2.flush();
            digitalWrite(PIN_RS485_DIR_DRV,0);  
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
            xQueueSend(QueueBtTransmit, &msg, portMAX_DELAY);
            ASetOccurredEvent(Event_finishLoadConfig);
            vTaskSuspend(TaskHandle_wait485Resp);
        }
		vTaskSuspend(NULL);
	}    
}

void AAnalogMonitor(){
  static bool critPwrLatch = 0;
  static bool fanEn = 0;
  BYTE ledsCode[4] = {0};
  int a24, aTemper;
  int a24_d;
  BYTE a24_tmp;
  BYTE a24_leds;

  if(G_pwrMode)return; //the BT connection mode is enable

  a24     = analogReadMilliVolts(36);
  aTemper = analogRead(39);

 

  if((aTemper < 2580)&&(!fanEn)){
    fanEn = 1;
    digitalWrite(PIN_FAN,1);
  } 

  if((aTemper > 2620)&&(fanEn)){
    fanEn = 0;
    digitalWrite(PIN_FAN,0);
  }

  if(a24 < 1900){
      ledsCode[0]=5;
      if(!critPwrLatch){
          xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);           
          critPwrLatch = 1;
      }
  }else{
      G_led_ccl_5 = 0;
      critPwrLatch = 0;
      a24_d = (2400 - A24_CRITICAL_VAL)/5;
      a24_tmp = (BYTE)(5-(2400 - a24)/a24_d);
      
      ledsCode[0] = 4;
      ledsCode[1] = 0;
      for(int i=0; i<a24_tmp;i++)ledsCode[1] |= (1<<i);
      ledsCode[1] &= 0x0F;               
      xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);                 
  }
  
  // Serial.println("a24 -> "+String(a24)+" a24_tmp -> "+
  //                 String(a24_tmp)+"; aTemp -> "+String(aTemper)+
  //                "; fanEn -> "+String(fanEn));
} 


void Task_monitor(void *param){
  _MSG_PACK msg;
  AN_cmd *cCmd = AN_cmd::getI();
  RmCtrl *rmCtrl = RmCtrl::getI();
  int aut = 0;
  int cnt = 0;
  DWORD btWaitCode = EVENT_TIMEOUT_BT_CONNECT;
  vTaskDelay(100);
  for(;;){

    switch(aut){
        // case 1: AinitTasks();       break;
        case 2: cCmd->getInfo();    break;
        case 3: cCmd->setPwrJmmr(); break;
    }
    if(aut < 4) aut++;

    if(G_updatePref){
        cCmd->setPwr();
        G_updatePref = false;
    }
    if(G_rebModAut_tm < 300)G_rebModAut_tm++;
    else if(rmCtrl->isBusy){
        G_rebModAut_tm = 0;
        vTaskResume(TaskHandle_rebModAut);
    }

    if(G_pauseRmDataCnt)G_pauseRmDataCnt--;
    if(G_pauseRmDataCnt == 2){
        vTaskResume(TaskHandle_rebModAut);

    }

    if(G_waitBtConnect)G_waitBtConnect--;
    if(G_waitBtConnect == 2){
        Serial.println("G_waitBtConnect---- "); 
        xQueueSend(QueuePwrAut, &btWaitCode, portMAX_DELAY);
    }

    if(!(cnt%50)){
        AAnalogMonitor();
    }

    if(!(cnt%11)){
        digitalWrite(LED_5, digitalRead(PIN_CN1));
        digitalWrite(LED_6, digitalRead(PIN_CN2));
    }    

    cnt++;
    vTaskDelay(10);
  }
}


void ASetLedState(BYTE stt){
    BYTE ledsArr[6] = {LED_1, LED_2, LED_3, LED_4, LED_5, LED_6};
    for(int i=0; i<6; i++){
        digitalWrite(ledsArr[i], (stt & 0x01));
        stt = stt >> 1;
    }
}


void Task_leds(void *param){
    BYTE code[4] = {0};
    BYTE stt = 0;
    for(;;){
        xQueueReceive(QueueLeds, code, portMAX_DELAY);

        switch(code[0]){
            case 0 :ASetLedState(0);                   break;
            case 1 :for(int i=0; i<8; i++){
                        ASetLedState(1 << (i & 0x03));
                        vTaskDelay(300);        
                    }  
                    ASetLedState(0x01);                 break;

            case 2 :stt = 0x03;
                    while(1){
                        ASetLedState(stt);
                        stt ^= 0x02;
                        if(G_btConnect)break;
                        vTaskDelay(300); 
                    }                                   break;
            
            case 3 :stt = 0x03;
                    ASetLedState(stt);                  break;
                     
            case 4 :stt = code[1];             
                    ASetLedState(stt);                  break;
        
            case 5 :stt   = 0x01;
                    G_led_ccl_5 = 1;
                    while(1){
                        ASetLedState(stt);
                        stt ^= 0x01;
                        if(!G_led_ccl_5)break;
                        vTaskDelay(300); 
                    }                                   break;  
                    
            
            default:stt = code[1];             
                    ASetLedState(stt);                  break;                     
        }
        vTaskDelay(10);
    }
}


void Task_rebModAut(void *param)
{
  _RM_AUT rmAut;  
  BYTE opCode;
  RmCtrl *rmCtrl = RmCtrl::getI();   
  for(;;){
    xQueueReceive(QueueRebModAut, &rmAut, portMAX_DELAY);
    rmCtrl->isBusy = true;
    for(int i=0; i<rmAut.opCodeQty; i++){
      
        switch(rmAut.opCodeList[i]){       
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
        G_rebModAut_tm = 0;             
        vTaskSuspend(NULL);
        for(int i=0; i<RM_BUFF_LEN; i++)rmCtrl->inData[i] = 0;
        int len = Serial1.available();
        Serial1.read(rmCtrl->inData, len);
        Serial1.flush();

        rmCtrl->getDevInfo(String(rmCtrl->inData)); 
        Serial.println ();
        Serial.println("------ REBMOD DATA ---------");
        Serial.println(String(rmCtrl->inData));
        Serial.println("----------------------------");
        Serial.println ();
        if(rmAut.swtchActDev)
            RmCtrl::getI()->selDev = (RmCtrl::getI()->selDev==0) ? 1 : 0;
    }
    rmCtrl->isBusy = false;
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

void Task_pwrButton(void *param){
    DWORD   bttnState       = 0;
    bool    bttnSwch        = 0;
    int     bttnPressCnt    = 0;
    int     bttnUnpressCnt  = 0;
    int     bttn            = 0;
    int     longBttnPress   = 0;
    for(;;){
        bttn = digitalRead(PIN_PWR_BUTTON);
        if(!bttnSwch){
            if(bttn)bttnUnpressCnt++;
            else    bttnUnpressCnt = 0;
            if(bttnUnpressCnt>100){
                bttnSwch = 1;             
            }
        }else{
            if(!bttn)bttnPressCnt++;
            else     bttnPressCnt = 0;
            if(bttnPressCnt > 50){
                bttnSwch = 0;
                bttnState = EVENT_CODE_BTTN_ON;
                xQueueSend(QueuePwrAut, &bttnState, portMAX_DELAY);   
                Serial.println("stt 2");             
            }
        }  
        vTaskDelay(10);
    }
} 

void Task_pwrAut(void *param){
    AN_cbFuncs *cbFuncs = AN_cbFuncs::getI();
    String devName;
    DWORD eventCode;
 
    BYTE ledsCode[4] = {0};
    bool btEnSwch = 0;
    for(;;){
        xQueueReceive(QueuePwrAut, &eventCode, portMAX_DELAY); 
        G_led_ccl_5 = 0;
        switch (eventCode){
            case 1:  //EVENT_CODE_BTTN_ON
                G_pwrMode = 1; 
                if(btEnSwch)break;
                btEnSwch = 1;
                Serial.println("BT on");
                JMMR_1_OFF
                JMMR_2_OFF
                devName = "Prizma_JMR_A_"+String(G_lJmrStt.esp32Addr);
                SerialBT.begin(devName);            
                G_waitBtConnect = 1000; //x 10mSec; if don't connect to bluetooth during this time - turn off the power
                
                ledsCode[0]=2;
                xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);
                                                                        break;
            case 2:  //EVENT_BT_CONNECT
                Serial.println("BT connect");
                G_waitBtConnect = 0;
                ledsCode[0]=3;
                xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);          
                                                                        break;
            case 3:  //EVENT_BT_DISCONNECT
                Serial.println("BT disconnect");
                G_waitBtConnect = 1000;
                ledsCode[0]=2;
                xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY);        
                                                                        break;
            case 4:// EVENT_PWR_OFF_BTTN 

                Serial.println("EVENT_PWR_OFF_BTTN");
                ledsCode[0]=0;
                xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY);      
                while(1)digitalWrite(PIN_PWR_HOLD_DRV, 0);                  
                                                                        break;
            case 5: ledsCode[0]=1;
                    xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY); 
                    SerialBT.end();
                    btEnSwch = 0;
                    G_btConnect = 0;
                    G_pwrMode = 0;
                    cCmd->setPwrJmmr();                                  break;
        }
        vTaskDelay(10);
    }
}











