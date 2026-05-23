#include "AN_rs485.h"

#define MSG_STATIC_DATA_LEN 15

AN_rs485* AN_rs485::instance = nullptr;

void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    
    switch(msg->cmd){
        case CMD_GET_JAMM_LIST: msg->cmd = CMD_SEARCH_DEVICES; break;
    }

    if(msg->direction == MSG_DIR_RESPONSE)cCmd->processingResponseData(msg);
    else                                  cCmd->AProcessCmd(msg);
}

BYTE AN_rs485::dataPackaging(BYTE addrDev, _MSG_PACK *msg, _RS485_data *qData)
{
    Serial.println("addr-> "+String(G_localAddresses.esp32));
    qData->data[0] = addrDev;
    qData->data[1] = msg->cmd;
    qData->data[2] = msg->modCode1;
    qData->data[3] = msg->modCode2;
    for(int i=0; i<4; i++) qData->data[i+4] = (BYTE)(msg->mask1>>(8*i));
    for(int i=0; i<4; i++) qData->data[i+8] = (BYTE)(msg->mask2>>(8*i));
    qData->data[12] = msg->txtDataLen;
    qData->data[13] = msg->direction;
    qData->data[14] = msg->addrSender;
    if(msg->txtDataLen){
        for(int i=0; i<msg->txtDataLen-1; i++)qData->data[i+MSG_STATIC_DATA_LEN] = msg->txtData[i];
        qData->data[msg->txtDataLen-1+MSG_STATIC_DATA_LEN] = 0;        
    }

    String str = "";
    for(int i=0; i<17; i++){
        str+= String(qData->data[i], HEX)+", ";
    }
   
    Serial.println("-------- SEND DATA  --------");
    Serial.println(str);
    return 0;
}

void AN_rs485::dataUnpackaging(BYTE* data, _MSG_PACK *msg)
{
    msg->cmd        = data[1] ;
    msg->modCode1   = data[2] ;
    msg->modCode2   = data[3] ;
    msg->mask1      = ((data[7 ]<<24)|(data[6 ]<<16)|(data[5]<<8)|(data[4]));
    msg->mask2      = ((data[11]<<24)|(data[10]<<16)|(data[9]<<8)|(data[8]));
    msg->txtDataLen = data[12];
    msg->direction  = data[13];
    msg->addrSender = data[14]; 
    for(int i=0; i<msg->txtDataLen; i++)msg->txtData[i] = data[i+MSG_STATIC_DATA_LEN];
}

void AN_rs485::init()
{
    getLocalAddresses(&G_localAddresses);
}

void AN_rs485::sendData(BYTE addr, _MSG_PACK *msg, BYTE waitResp)
{
    WORD crc16;
    _RS485_data qData;
    BYTE msgLen = MSG_STATIC_DATA_LEN+msg->txtDataLen;
    msg->addrSender = G_localAddresses.esp32;
    dataPackaging(addr, msg, &qData);

    crc16 = crc.modbus(qData.data, msgLen);
    qData.data[msgLen+1] = (BYTE)crc16;
    qData.data[msgLen]   = (BYTE)(crc16>>8);
    qData.dataLen = msgLen+2;

    xQueueSend(QueueRs485Out, &qData, portMAX_DELAY);


}
 

void AN_rs485::recvData(BYTE* data, size_t len)
{
    _MSG_PACK msg;
    WORD crc16, crcExp;
    crcExp = data[len-1];
    crcExp |= (data[len-2]<<8);
    crc16 = crc.modbus(data, len-2);
    String str = "";
    if(crc16 == crcExp){
        if((data[0] == G_localAddresses.esp32)||(data[0] == BROADCAST_ADDR)||(devStsus==DEV_STATUS_MASTER)){
            ADebugLog("addr OK -> "+String(G_localAddresses.esp32));
            dataUnpackaging(data, &msg);
            processMsg(&msg);
            str = "";

        

        
        }else{
            ADebugLog("local addr-> "+String(G_localAddresses.esp32)+" : get addr-> "+String(data[0]));
        }
    }else{
        AErrorLog("CRC error !!!");
    }

    for(int i=0; i<len; i++){
        str+= String(data[i], HEX)+", ";
    }
    Serial.println("----------  RECEIVE DATA  --------");
    Serial.println(str);

 
}
