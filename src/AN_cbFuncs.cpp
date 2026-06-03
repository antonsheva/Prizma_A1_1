#include "AN_cbFuncs.h"

AN_cbFuncs* AN_cbFuncs::instance = nullptr;

void AN_cbFuncs::processingSerialData(char *data)
{
    AN_cmd* cCmd = AN_cmd::getI();
    _MSG_PACK msg;
    RmCtrl* rm = RmCtrl::getI();
    if(cCmd->AGetJson(data, &msg)){ //there is error in JSON-data
        AErrorLog("Error JSON data");
        // xQueueSend(rm->queueOut, data, portMAX_DELAY); 
        rm->sendCmd(data);
    }else{
        cCmd->lastCmd = msg.cmd;
        cCmd->AProcessCmd(&msg);
    }
}

void AN_cbFuncs::btRx(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if(event == ESP_SPP_DATA_IND_EVT){
        int cnt = 0;
        char data[TMP_BUFF_LEN];
        while (SerialBT.available())
        data[cnt++] = SerialBT.read();   
        processingSerialData(data);      
    }
}

void AN_cbFuncs::uart0Rx()
{
    int cnt = 0;
    char data[TMP_BUFF_LEN];
    for(int i=0; i<TMP_BUFF_LEN;i++)data[i]=0;
    while (Serial.available())data[cnt++] = Serial.read();
    processingSerialData(data);      
}

// char G_dt[1024] = {0};
void AN_cbFuncs::uart1Rx()
{
    String readData = Serial1.readString();        
    RmCtrl::getI()->receiveData(readData);
    unblockTasks();
}

void AN_cbFuncs::uart2Rx()
{
    BYTE data[SERIAL_BUFF_LEN];
    AN_rs485* rs485 = AN_rs485::getI();
    int len = Serial2.read(data, SERIAL_BUFF_LEN);
    rs485->recvData(data, len);

}
