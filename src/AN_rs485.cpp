#include "AN_rs485.h"

#define MSG_STATIC_DATA_LEN 18

AN_rs485* AN_rs485::instance = nullptr;

void AN_rs485::loadDataToJmrStt(_MSG_PACK *msg)
{
    G_lJmrStt.rebMod[0].mc      = msg->modCode1;
    G_lJmrStt.rebMod[1].mc      = msg->modCode2;
    G_lJmrStt.rebMod[0].mask    = msg->mask1;
    G_lJmrStt.rebMod[1].mask    = msg->mask2;

    if(msg->addrRm1 && msg->addrRm1 < 128)G_lJmrStt.rebMod[0].address = msg->addrRm1;
    if(msg->addrRm2 && msg->addrRm2 < 128)G_lJmrStt.rebMod[1].address = msg->addrRm2;
}

void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    switch(msg->cmd){
        // case CMD_GET_JAMM_LIST: msg->cmd = CMD_SEARCH_DEVICES; break;
        case CMD_SET_STATE    : loadDataToJmrStt(msg);         break;
    }
    if(msg->direction == MSG_DIR_RESPONSE)cCmd->processingResponseData(msg);
    else                                  cCmd->AProcessCmd(msg);
}

void AN_rs485::prepMsg(_MSG_PACK *msg, BYTE iterNum)
{
    switch(cmdType){
        case CMD_GET_JAMM_LIST: msg->addressee = iterNum+1; 
                                msg->cmd = CMD_SEARCH_DEVICES;                           break;

        case CMD_LOAD_CONFIG  : msg->cmd        = CMD_SET_STATE;                  
                                msg->direction  = MSG_DIR_REQUEST;
                                msg->addressee  = G_jmrsList[iterNum].esp32Addr;
                                msg->addrRm1    = G_jmrsList[iterNum].rebMod[0].address;
                                msg->addrRm2    = G_jmrsList[iterNum].rebMod[1].address;
                                msg->modCode1   = G_jmrsList[iterNum].rebMod[0].mc;
                                msg->modCode2   = G_jmrsList[iterNum].rebMod[1].mc;
                                msg->mask1      = G_jmrsList[iterNum].rebMod[0].mask;
                                msg->mask2      = G_jmrsList[iterNum].rebMod[1].mask;   break;

    }
}

BYTE AN_rs485::dataPackaging(_MSG_PACK *msg, _RS485_data *qData)
{
    Serial.println("addr-> "+String(G_lJmrStt.esp32Addr));
    qData->data[0] = msg->addressee;
    qData->data[1] = msg->cmd;

    qData->data[2] = msg->sender;
    qData->data[3] = msg->addrRm1;
    qData->data[4] = msg->addrRm2;
    
    qData->data[5] = msg->modCode1;
    qData->data[6] = msg->modCode2;
    for(int i=0; i<4; i++) qData->data[i+ 7] = (BYTE)(msg->mask1>>(8*i));
    for(int i=0; i<4; i++) qData->data[i+11] = (BYTE)(msg->mask2>>(8*i));
    qData->data[15] = msg->txtDataLen;
    qData->data[16] = msg->direction;
    qData->data[17] = msg->sender;
    if(msg->txtDataLen){
        for(int i=0; i<msg->txtDataLen-1; i++)qData->data[i+MSG_STATIC_DATA_LEN] = msg->txtData[i];
        qData->data[msg->txtDataLen-1+MSG_STATIC_DATA_LEN] = 0;        
    }

    String str = "";
    for(int i=0; i<20; i++){
        str+= String(qData->data[i], HEX)+", ";
    }
   
    Serial.println("-------- SEND DATA  --------");
    Serial.println(str);
    return 0;
}

void AN_rs485::dataUnpackaging(BYTE* data, _MSG_PACK *msg)
{
    msg->cmd        = data[1] ;
    
    msg->sender     = data[2];
    msg->addrRm1    = data[3];
    msg->addrRm2    = data[4];

    msg->modCode1   = data[5] ;
    msg->modCode2   = data[6] ;


    msg->mask1      = ((data[10]<<24)|(data[9 ]<<16)|(data[8 ]<<8)|(data[7 ]));
    msg->mask2      = ((data[14]<<24)|(data[13]<<16)|(data[12]<<8)|(data[11]));
    msg->txtDataLen = data[15];
    msg->direction  = data[16];
    msg->sender     = data[17]; 
    for(int i=0; i<msg->txtDataLen; i++)msg->txtData[i] = data[i+MSG_STATIC_DATA_LEN];
}

void AN_rs485::init()
{
     
}

void AN_rs485::sendData(_MSG_PACK *msg, BYTE waitResp)
{
    WORD crc16;
    _RS485_data qData;
    BYTE msgLen = MSG_STATIC_DATA_LEN+msg->txtDataLen;
    msg->sender = G_lJmrStt.esp32Addr;
    dataPackaging(msg, &qData);

    crc16 = crc.modbus(qData.data, msgLen);
    qData.data[msgLen+1] = (BYTE)crc16;
    qData.data[msgLen]   = (BYTE)(crc16>>8);
    qData.dataLen = msgLen+2;

    Serial2.write(qData.data, qData.dataLen);       
    // xQueueSend(QueueRs485Out, &qData, portMAX_DELAY);


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
        if((data[0] == G_lJmrStt.esp32Addr)||(data[0] == BROADCAST_ADDR)||(devStsus==DEV_STATUS_MASTER)){
            ADebugLog("addr OK -> "+String(G_lJmrStt.esp32Addr));
            dataUnpackaging(data, &msg);
            processMsg(&msg);
            str = "";        
        }else{
            ADebugLog("local addr-> "+String(G_lJmrStt.esp32Addr)+" : get addr-> "+String(data[0]));
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
