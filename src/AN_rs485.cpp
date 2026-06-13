#include "AN_rs485.h"

#define MSG_STATIC_DATA_LEN 20

AN_rs485* AN_rs485::instance = nullptr;



void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    switch(msg->cmd){
        // case CMD_GET_JAMM_LIST: msg->cmd = CMD_SEARCH_DEVICES; break;
        case CMD_SET_STATE : cCmd->loadMsgToJmrStt(msg, &G_lJmrStt);         break;
    }
    if(msg->direction == MSG_DIR_RESPONSE)cCmd->processingResponseData(msg);
    else                                  cCmd->AProcessCmd(msg);
}

void AN_rs485::prepMsg(_MSG_PACK *msg, BYTE iterNum)
{
    switch(cmdType){
        case CMD_GET_JAMM_LIST: msg->addrEsp32 = iterNum+1; 
                                msg->cmd       = CMD_SEARCH_DEVICES;
                                msg->direction = MSG_DIR_REQUEST;                          
                                break;

        case CMD_LOAD_CONFIG  : msg->cmd        = CMD_SET_STATE;                  
                                msg->direction  = MSG_DIR_REQUEST;
                                msg->addrEsp32  = G_jmrsList[iterNum].esp32Addr;
                                AN_cmd::getI()->loadJmmrStateToMsg(msg, &G_jmrsList[iterNum]); 
                                break;

    }
}
 
void AN_rs485::sendBtData(JsonDocument doc){
	
	int len = serializeJson(doc, tmpDataBuff);
	int packQty = len/128+1;

	String str = "start___";
	str.toCharArray(serialData, 9);
	std::copy(tmpDataBuff, &tmpDataBuff[0]+len, &serialData[0]+8);

	serialData[8+len+0] = 's';
	serialData[8+len+1] = 't';
	serialData[8+len+2] = 'o';
	serialData[8+len+3] = 'p';

	String("_stop").toCharArray(serialData+8+len, 6);

	ADebugLog(" --- send to BT ---");
	Serial.println(serialData);
	
	SerialBT.println(serialData);
}

void AN_rs485::sendJammListToBt(){

	JsonDocument doc;		
	doc[PARAM_CMD]    = CMD_GET_JAMM_LIST;
    doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;
    doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;

	for(size_t i=0; i<G_jmrsList.size(); i++){
		doc["jmmr_list"][i][PARAM_ADDR_ESP ] = G_jmrsList[i].esp32Addr;
		doc["jmmr_list"][i][PARAM_ADDR_RM_1 ] = G_jmrsList[i].rebMod[0].address;
		doc["jmmr_list"][i][PARAM_ADDR_RM_2 ] = G_jmrsList[i].rebMod[1].address;
		doc["jmmr_list"][i][PARAM_MOD_CODE_1] = G_jmrsList[i].rebMod[0].mc;
		doc["jmmr_list"][i][PARAM_MOD_CODE_2] = G_jmrsList[i].rebMod[1].mc;
		doc["jmmr_list"][i][PARAM_MASK_1    ] = G_jmrsList[i].rebMod[0].mask;
		doc["jmmr_list"][i][PARAM_MASK_2    ] = G_jmrsList[i].rebMod[1].mask;
		doc["jmmr_list"][i][PARAM_PWR_1     ] = G_jmrsList[i].rebMod[0].pwr;
		doc["jmmr_list"][i][PARAM_PWR_2     ] = G_jmrsList[i].rebMod[1].pwr;

        doc["jmmr_list"][i][PARAM_TXT       ] = G_jmrsList[i].info;
        doc["jmmr_list"][i][PARAM_TXT_LEN   ] = G_jmrsList[i].info.length();
	}
	sendBtData(doc);
}

void AN_rs485::sendBtResponse(BYTE cmd, uint32_t resp){
	JsonDocument doc;		
	doc[PARAM_CMD]  	= cmd;
	doc[PARAM_RESPONSE] = resp;
	sendBtData(doc);
}


void AN_rs485::sendMsgToBt(){

	Serial.println("--- q1 ---");

	switch (cmdType){
		case CMD_GET_JAMM_LIST: sendJammListToBt(); break; 
		case CMD_LOAD_CONFIG  : sendBtResponse(cmdType, 1);
	}
	cmdType = 0; 
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

    msg->pwr1       = data[18];
    msg->pwr2       = data[19];

    for(int i=0; i<msg->txtDataLen; i++)msg->txtData[i] = data[i+MSG_STATIC_DATA_LEN];
}

void AN_rs485::init()
{
     
}


void AN_rs485::resetDataPackProcess(String comment){
		waitTimer = 0;
		endOfDataPacks = 0;
		receiveDataPacks = 0;
		dataPackStr = "";
}

int AN_rs485::checkCrcJson(){
    String crcStr = dataPackStr.substring(dataPackStr.lastIndexOf("}")+1);
    dataPackStr = dataPackStr.substring(8, dataPackStr.lastIndexOf("}")+1);
    int len = dataPackStr.length();
    crcStr = crcStr.substring(1, crcStr.lastIndexOf("_"));
    int crcExpected = (int)strtol(crcStr.c_str(), NULL, 16);

    dataPackStr.toCharArray(serialData, len+1);
    int crc16 = crc.modbus((const uint8_t*) serialData, len);
    if(crc16 == crcExpected){
        return 1;
    }
    return 0;
}

void AN_rs485::processReceivedData(){
    _MSG_PACK msg;    
    AN_cmd *cCmd = AN_cmd::getI();
    if(dataPackStr.length() < 20){
        resetDataPackProcess("dataPack too little");	
    }else if(dataPackStr.startsWith("start")){
        if(dataSrc == SERIAL_SRC_485){
            if(!checkCrcJson()){
                ADebugLog("error crc rs485");
                return;
            }
        }else{
            dataPackStr = dataPackStr.substring(8, dataPackStr.lastIndexOf("}")+1);
        }
        if(AN_json::getI()->unpackData(dataPackStr, &msg)){ //there is error in JSON-data
            resetDataPackProcess("error JSON data");
        }else{
            vTaskSuspend(TaskHandle_wtDataPack);
            waitTimer = 0;
            resetDataPackProcess("Serial data - OK!");
           
            if(msg.addrEsp32 == G_lJmrStt.esp32Addr){ 
                // Serial2.onReceive(NULL);  
                AN_json *json = AN_json::getI();
                
                // for(int i=0;i<100;i++)Serial.write(json->txtData[i]);
                
                
                if(msg.direction == MSG_DIR_RESPONSE)cCmd->processingResponseData(&msg);
                else                                 cCmd->AProcessCmd(&msg);
                
            }

        }		
    }else{
        resetDataPackProcess("error dataPack");
    }
		 
}

int AN_rs485::concatMsgPacks(String str){
	static int cnt = 0;
	cnt++;	
	if(str.length()>120)dataPackStr += str.substring(0,120);
	else				dataPackStr += str;
	if(dataPackStr.length() > MAX_SERIAL_DATA_LEN){
		resetDataPackProcess("dataPack too long");
	}else if( dataPackStr.indexOf("stop") != -1){
        xQueueSend(QueueRs485Receive, dataPackStr.c_str(), portMAX_DELAY);
        // processReceivedData();
	}
	return 0;
}
    
void AN_rs485::sendMsgTo485(_MSG_PACK *msg){

    
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
