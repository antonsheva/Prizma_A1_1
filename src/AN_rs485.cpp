#include "AN_rs485.h"



AN_rs485* AN_rs485::instance = nullptr;
void AN_rs485::init(){
    getLocalAddresses(&addr);
}

void AN_rs485::sendData(BYTE addr, BYTE cmd, BYTE *data, BYTE len)
{
    WORD crc16;
 
    _RS485_data qData;
    qData.data[0] = addr;
    qData.data[1] = len+5;
    qData.data[2] = cmd;
    for(int i=0; i<len; i++)qData.data[i+3] = data[i];
    crc16 = crc.modbus(qData.data, len+3);
    qData.data[len+4] = (BYTE)crc16;
    qData.data[len+3] = (BYTE)(crc16>>8);
    qData.dataLen = len+5;
    Serial.println("crcOut -> "+String(crc16, HEX));
    xQueueSend(QueueRs485Out, &qData, portMAX_DELAY);
}

//  AN_rs485::

void AN_rs485::recvData(BYTE* data, size_t len)
{
    WORD crc16, crcExp;
    crcExp = data[len-1];
    crcExp |= (data[len-2]<<8);
    crc16 = crc.modbus(data, len-2);

    if(crc16 == crcExp){
        if(data[0] == addr.esp32){
            ADebugLog("addr OK -> "+String(addr.esp32));
        }{
            ADebugLog("other addr -> "+String(addr.esp32)+" : "+String(data[0]));
        }
    }else{
        AErrorLog("CRC error !!!");
    }


    // String str = "";
    // for(int i=0; i<len; i++){
    //     str+= String(data[i], HEX)+", ";
    // }
   
    // Serial.println("----------===========--------");
    // Serial.println(str);

    // Serial.println("crcExp -> "+String(crcExp, HEX));
    // Serial.println("crc16 -> "+String(crc16, HEX));
 
 
}
