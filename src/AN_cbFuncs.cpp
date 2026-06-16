#include "AN_cbFuncs.h"

AN_cbFuncs* AN_cbFuncs::instance = nullptr;


AN_cmd* cbFuncs_cCmd = AN_cmd::getI();
 
AN_json*  json = AN_json::getI();
void AN_cbFuncs::processingSerialData(char *data, int src)
{
    AN_rs485* rs485 = AN_rs485::getI();
    _MSG_PACK msg;

    
    String strData = data;
    rs485->dataSrc = src;
    Serial.println("---processingSerialData---");
    Serial.println(data);

    if(strData.startsWith("start")){
        rs485->receiveDataPacks = 1;  
        vTaskResume(TaskHandle_wtDataPack);
    } 
    if(rs485->receiveDataPacks){
        rs485->concatMsgPacks(strData);
    }else{
        if(json->unpackData(strData, &msg)){ //there is error in JSON-data
            AErrorLog("Error JSON data");
        }else{
            // ADebugLog("JSON data - OK");
            cbFuncs_cCmd->AProcessCmd(&msg);
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
        if(!G_opQty)processingSerialData(data, SERIAL_SRC_BT);      
    }
}

void AN_cbFuncs::uartUsb()
{
  
    _MSG_PACK msg;
    int cnt = 0;
    char data[TMP_BUFF_LEN];
    for(int i=0; i<TMP_BUFF_LEN;i++)data[i]=0;
    while (Serial.available())data[cnt++] = Serial.read();
    processingSerialData(data, SERIAL_SRC_USB);      
}

void AN_cbFuncs::uartRm()
{
    static BYTE swch = 0;
    if(!swch){
        swch = 1;
        return;
    }
    String readData = Serial1.readString();        
    RmCtrl::getI()->receiveData(readData);
    vTaskResume(TaskHandle_rebModAut);
}

void AN_cbFuncs::uart485()
{
    char tmp;
    char data[SERIAL_BUFF_LEN];
    int cnt = 0;

    if( AN_rs485::getI()->isBusy){
    Serial.println("AN_rs485 -> isBusy");

        while(Serial2.available()){
            tmp = Serial2.read();
        }        
        return;
    }
    while(Serial2.available()){
        data[cnt] = Serial2.read();
        cnt++;
    }

    Serial.println("len -> "+String (cnt));
    Serial.write(data, cnt);
    Serial.println("\n\r");


    // if(!G_opQty){
    //     processingSerialData(data, SERIAL_SRC_485);      
    // }else{
    //     Serial.println(" --G_opQty -- ");
    // }
}






