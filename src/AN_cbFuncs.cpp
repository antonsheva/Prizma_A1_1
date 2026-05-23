#include "AN_cbFuncs.h"

AN_cbFuncs* AN_cbFuncs::instance = nullptr;

void AN_cbFuncs::uart0Rx()
{
    RebModCntrl* rm = RebModCntrl::getI();
    AN_cmd* cCmd = AN_cmd::getI();
    AReadSerialData(SRC_COM);
    if(cCmd->AGetJson()){ //there is error in JSON-data
        AErrorLog("Error JSON data");
        xQueueSend(rm->queueOut, G_tmpBuff, portMAX_DELAY); 
    }else{
        cCmd->AProcessCmd(&G_serial_msg);
    }
}

void AN_cbFuncs::uart1Rx()
{
    String readData;
    readData = Serial1.readString();
    Serial.println("---------------");
    Serial.println(readData);
    haveNewRebModData(readData);
}

void AN_cbFuncs::uart2Rx()
{
    BYTE data[SERIAL_BUFF_LEN];
    AN_rs485* rs485 = AN_rs485::getI();
    int len = Serial2.read(data, SERIAL_BUFF_LEN);
    rs485->recvData(data, len);

}
