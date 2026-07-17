#include "tasks.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* cCmd;
AN_pref* mPref;
void Task_savePreferences  (void *param){
    _MSG_PACK msg;
    AN_cmd *cCmd = AN_cmd::getI();
    int cnt = 0;
    int i = 0;
    for(;;){
        xQueueReceive(QueuePreferences, &msg, portMAX_DELAY);
        
        switch (msg.cmd){
            case CMD_SET_ADDR_ESP   :  mPref->setAddrEsp  (msg.addrEsp32);                break;
            case CMD_SET_ADDR_RM    :  mPref->setAddrRm   (msg.addrRm1, msg.addrRm2);     break;
            case CMD_SET_PWR        :  mPref->setPwr      (msg.pwr1, msg.pwr2);           break;         
            case CMD_SET_DEV_ID     :  mPref->setDevId    (msg.devId);                    break;
            case CMD_SET_GROUP_ID   :  mPref->setGroupId  (msg.groupId);                  break;
            case CMD_SET_DEV_TYPE   :  mPref->setDevType  (msg.devType);                  break;
            case CMD_SET_DEV_RANGE  :  mPref->setDevRange (msg.devRange);                 break;
            
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
        case Event_finishLoadConfig   : cCmd->setState(NULL);             break;
        // case Event_finishLoadRm       : 
        // 	        DWORD eventCode = EVENT_APPLY_CHANGES;
        //           xQueueSend(QueuePwrAut, &eventCode, portMAX_DELAY);     break;
        
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
    char serialData [1024]      = "\0"; 
    char tmpData    [1024]      = "\0";     
    WORD crc16;    
    for(;;){
        xQueueReceive(QueueRs485Send, &msg, portMAX_DELAY);       
        String data = AN_json::getI()->packRs485Data(&msg);
        int len = data.length();
        crc16 = crc.modbus((const uint8_t*) data.c_str(), len);
        String sendData = "start___"+data+"_"+String(crc16,HEX)+"_stop";
        len = sendData.length();        

        sendData.toCharArray((char*)msg485.data, 1024);
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
            xQueueSend(QueueBtTransmit, &msg, 100);
            ASetOccurredEvent(Event_finishLoadConfig);
            vTaskSuspend(TaskHandle_wait485Resp);
        }
		vTaskSuspend(NULL);
    vTaskDelay(10);
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

 

  if((aTemper < A_TEMPERATURE_ON_FAN)&&(!fanEn)){
    fanEn = 1;
    digitalWrite(PIN_FAN,1);
  } 

  if((aTemper > A_TEMPERATURE_OFF_FAN)&&(fanEn)){
    fanEn = 0;
    digitalWrite(PIN_FAN,0);
  }

  if(a24 < A24_CRITICAL_VAL){
      ledsCode[0]=5;
      if(!critPwrLatch){
          xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);           
          critPwrLatch = 1;
      }
  }else{
      G_led_ccl_5 = 0;
      critPwrLatch = 0;
      a24_d   = (A24_NORMAL_VAL - A24_CRITICAL_VAL)/5;
      a24_tmp = (BYTE)(5-(A24_NORMAL_VAL - a24)/a24_d);
      
      ledsCode[0] = 4;
      ledsCode[1] = 0;
      for(int i=0; i<a24_tmp;i++)ledsCode[1] |= (1<<i);
      ledsCode[1] &= 0x0F;               
      xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);                 
  }

  /**
   * @brief  status display of analog inputs
   * 
   */
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
  BYTE pwrOutVal = 0;
  BYTE prevPwrOutVal = 0;
  
  BYTE ledsCode[4] = {0};
  DWORD btWaitCode = EVENT_TIMEOUT_BT_CONNECT;
  vTaskDelay(100);
  for(;;){

    switch(aut){
        case 0: ledsCode[0]=1;
                xQueueSend(QueueLeds, ledsCode, 10); break;
        case 1: mPref->init();                                  break;
        case 2: cCmd->getInfo();                                break;
        case 3: cCmd->setPwrJmmr();                             break;
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
        xQueueSend(QueuePwrAut, &btWaitCode, 10);
    }

    if(!(cnt%50)){
        AAnalogMonitor();
    }

    if(!(cnt%11)){
      pwrOutVal = ~(digitalRead(PIN_CN1) | (digitalRead(PIN_CN2)<<1));
      pwrOutVal &= 0x03;
      if(pwrOutVal != prevPwrOutVal){
        prevPwrOutVal = pwrOutVal;
        ledsCode[0]=6;
        ledsCode[1]=pwrOutVal;
        xQueueSend(QueueLeds, ledsCode, 10);        

      }

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
    BYTE startStt = 0;
    BYTE signalValStt = 0;
    BYTE blinkBits = 0;
    for(;;){
        xQueueReceive(QueueLeds, code, portMAX_DELAY);
        if(!startStt){
            if(code[0] != 6)stt &= 0x30;
            switch(code[0]){
                case 0 :ASetLedState(0);                   break;
                case 1 :startStt = 1;
                        for(int i=0; i<12; i++){
                            ASetLedState(1 << (i < 6 ? i : i-6));
                            vTaskDelay(200);        
                        } 
                        startStt = 0; 
                        ASetLedState(0x01);                 break;

                case 2 :stt |= 0x03;
                        while(1){
                            ASetLedState(stt);
                            blinkBits ^= 0x02;
                            stt &= ~(1<<1);
                            stt |= blinkBits;
                            if(G_btConnect)break;
                            vTaskDelay(300); 
                        }                                   break;
                
                case 3 :stt |= 0x03;
                        ASetLedState(stt);                  break;
                        
                case 4 :stt |= (code[1] & 0x0F);             
                        ASetLedState(stt);                  break;
            
                case 5 :stt |= 0x01;
                        G_led_ccl_5 = 1;
                        while(1){
                            ASetLedState(stt);
                            blinkBits ^= 0x01;
                            stt &= ~(1<<0);
                            stt |= blinkBits;
                            if(!G_led_ccl_5)break;
                            vTaskDelay(300); 
                        }                                   break;  
                        
                case 6 :signalValStt = (code[1]<<4);  
                        stt &= 0xCF;
                        stt |= signalValStt;
                        ASetLedState(stt);                  break; 

                default:stt = code[1];             
                        ASetLedState(stt);                  break;                     
            }
       
        }
 
        vTaskDelay(1);
    }
}

void Task_rebModAut(void *param)
{
  AN_shiftDataArr shft;
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
      Serial.println("------ stt 1 ---------");        
        vTaskSuspend(NULL);
      Serial.println("------ stt 2 ---------");                    

        for(int k=0; k<RM_BUFF_LEN; k++)rmCtrl->inData[k] = 0;
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
            rmCtrl->selDev = (rmCtrl->selDev==0) ? 1 : 0;
    }
    rmCtrl->isBusy = false;
    shft.printJmmrData(&G_lJmrStt);  
    // ASetOccurredEvent(Event_finishLoadRm);   
    if(rmCtrl->cmdAfterAutFinish == CMD_RESTART_ESP)esp_restart();
  
  }
}

void Task_cmd(void *param){
  _MSG_PACK msg;
  AN_cmd* cCmd = AN_cmd::getI();
  for(;;){
    xQueueReceive(QueueCmd, &msg, portMAX_DELAY); 
    cCmd->AProcessCmd(&msg);
    vTaskDelay(1);
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
        Serial.println("------ Task_pwrAut 1 ---------");
        G_led_ccl_5 = 0;
        switch (eventCode){
            case EVENT_CODE_BTTN_ON:   
                G_pwrMode = 1; 
                if(btEnSwch)break;
                btEnSwch = 1;
                Serial.println("BT on");
                JMMR_1_OFF
                JMMR_2_OFF
                devName = "Prizma_JMR_A_"+String(G_lJmrStt.esp32Addr);
                SerialBT.begin(devName);            
                G_waitBtConnect = 3000; //x 10mSec; if don't connect to bluetooth during this time - turn off the power
                
                ledsCode[0]=2;
                xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);
                                                                        break;
            case EVENT_BT_CONNECT:   
                Serial.println("BT connect");
                G_waitBtConnect = 0;
                ledsCode[0]=3;
                xQueueSend(QueueLeds, ledsCode, portMAX_DELAY);          
                                                                        break;
            case EVENT_BT_DISCONNECT:  
                Serial.println("BT disconnect");
                G_waitBtConnect = 1000;
                ledsCode[0]=2;
                xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY);        
                                                                        break;
            case EVENT_TIMEOUT_BT_CONNECT: 

                Serial.println("EVENT_PWR_OFF_BTTN");
                ledsCode[0]=0;
                xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY);      
                while(1)digitalWrite(PIN_PWR_HOLD_DRV, 0);                  
                                                                        break;
            case EVENT_APPLY_CHANGES: ledsCode[0]=1;
            Serial.println("------ Task_pwrAut 2 ---------");
                    xQueueSend(QueueLeds, &ledsCode, portMAX_DELAY); 
                    Serial.println("------ Task_pwrAut 3 ---------");
                    SerialBT.disconnect();
                    SerialBT.end();
                    Serial.println("------ Task_pwrAut 4 ---------");
                    btEnSwch = 0;
                    G_btConnect = 0;
                    G_pwrMode = 0;
                    cCmd->setPwrJmmr();
                    Serial.println("------ Task_pwrAut 5 ---------");
                    break;
        }
        vTaskDelay(10);
    }
}

 






