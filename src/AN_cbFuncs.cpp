#include "AN_cbFuncs.h"

AN_cbFuncs* AN_cbFuncs::instance = nullptr;



 

void AN_cbFuncs::processingSerialData(char *data)
{
    AN_cmd* cCmd = AN_cmd::getI();
    _MSG_PACK msg;
    String strData = String(data);

    Serial.println("receiveDataPacks -> "+String(cCmd->receiveDataPacks));
    if(strData.startsWith("start")){
        cCmd->receiveDataPacks = 1;
        vTaskResume(TaskHandle_wtDataPack);
    }
    if(cCmd->receiveDataPacks){
        cCmd->concatMsgPacks(strData);
    }else{
        if(cCmd->AGetJson(strData, &msg)){ //there is error in JSON-data
        AErrorLog("Error JSON data");
   
        }else{
            cCmd->lastCmd = msg.cmd;
            cCmd->AProcessCmd(&msg);
        }
    }
}

void AN_cbFuncs::uartBt(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    
    if(event == ESP_SPP_DATA_IND_EVT){
        int cnt = 0;
        char data[TMP_BUFF_LEN];
        while (SerialBT.available())
        data[cnt++] = SerialBT.read();   
        if(!G_opQty)processingSerialData(data);      
    }
}

void AN_cbFuncs::uartUsb()
{
    int cnt = 0;
    char data[TMP_BUFF_LEN];
    for(int i=0; i<TMP_BUFF_LEN;i++)data[i]=0;
    while (Serial.available())data[cnt++] = Serial.read();
    processingSerialData(data);      
}

void AN_cbFuncs::uartRm()
{
    String readData = Serial1.readString();        
    RmCtrl::getI()->receiveData(readData);
    vTaskResume(TaskHandle_rebModAut);
}

void AN_cbFuncs::uart485()
{
    if(G_opQty)return;
    BYTE data[SERIAL_BUFF_LEN];
    AN_rs485* rs485 = AN_rs485::getI();
    int len = Serial2.read(data, SERIAL_BUFF_LEN);
    if(!G_opQty) rs485->recvData(data, len);
}






