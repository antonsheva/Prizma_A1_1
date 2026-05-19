#include "AN_rs485.h"



AN_rs485* AN_rs485::instance = nullptr;

void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    // cCmd->cmd = msg->cmd;
    // cCmd-> = msg->;
    // cCmd-> = msg->;
    // cCmd-> = msg->;
    // cCmd-> = msg->;
    // cCmd-> = msg->;
    // cCmd-> = msg->;
    

 
}

BYTE AN_rs485::dataPackaging(BYTE addrDev, _MSG_PACK *msg, _RS485_data *qData)
{
    qData->data[0] = addrDev;
    qData->data[1] = msg->cmd;
    qData->data[2] = msg->modCode1;
    qData->data[3] = msg->modCode2;
    for(int i=3; i>=0; i--) qData->data[i+4] = (BYTE)(msg->mask1>>(8*i));
    for(int i=3; i>=0; i--) qData->data[i+8] = (BYTE)(msg->mask2>>(8*i));
    qData->data[12] = (BYTE)(msg->txtDataLen);

    for(int i=0; i<msg->txtDataLen-1; i++)qData->data[i+13] = msg->txtData[i];
    qData->data[msg->txtDataLen-1+13] = 0;

    return 0;
}

void AN_rs485::dataUnpackaging(BYTE* data, _MSG_PACK *msg)
{
    msg->cmd        = data[1] ;
    msg->modCode1   = data[2] ;
    msg->modCode2   = data[3] ;
    msg->mask1      = ((data[4]<<24)|(data[5]<<16)|(data[6 ]<<8)|(data[7 ]));
    msg->mask2      = ((data[8]<<24)|(data[9]<<16)|(data[10]<<8)|(data[11]));
    
    msg->txtDataLen = data[12];
    for(int i=0; i<msg->txtDataLen; i++)msg->txtData[i] = data[i+13];
}

void AN_rs485::init()
{
    getLocalAddresses(&addr);
}

void AN_rs485::sendData(BYTE addr, _MSG_PACK *msg)
{
    WORD crc16;
    _RS485_data qData;
    BYTE msgLen = 13+msg->txtDataLen;
     
    dataPackaging(addr, msg, &qData);

    crc16 = crc.modbus(qData.data, msgLen);
    qData.data[msgLen+1] = (BYTE)crc16;
    qData.data[msgLen]   = (BYTE)(crc16>>8);
    qData.dataLen = msgLen+2;
    xQueueSend(QueueRs485Out, &qData, portMAX_DELAY);

    Serial.println("crcOut -> "+String(crc16, HEX));
    String str = "";
    for(int i=0; i<qData.dataLen; i++){
        str+= String(qData.data[i], HEX)+", ";
    }
   
    Serial.println("-------- SEND DATA  --------");
    Serial.println(str);
}

//  AN_rs485::

void AN_rs485::recvData(BYTE* data, size_t len)
{
    _MSG_PACK msg;
    WORD crc16, crcExp;
    crcExp = data[len-1];
    crcExp |= (data[len-2]<<8);
    crc16 = crc.modbus(data, len-2);

    if(crc16 == crcExp){
        if((data[0] == addr.esp32)||(data[0] == BROADCAST_ADDR)){
            ADebugLog("addr OK -> "+String(addr.esp32));
            dataUnpackaging(data, &msg);
            processMsg(&msg);
            String str = "";
            for(int i=0; i<len; i++){
                str+= String(data[i], HEX)+", ";
            }
        
            Serial.println("----------  RECEIVE DATA  --------");
            Serial.println(str);

            Serial.println("crcExp -> "+String(crcExp, HEX));
            Serial.println("crc16 -> "+String(crc16, HEX));
        
        }else{
            ADebugLog("other addr -> "+String(addr.esp32)+" : "+String(data[0]));
        }
    }else{
        AErrorLog("CRC error !!!");
    }



 
}
