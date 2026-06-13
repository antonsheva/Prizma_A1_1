#include "AN_rs485.h"

#define MSG_STATIC_DATA_LEN 20

AN_rs485* AN_rs485::instance = nullptr;



void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    switch(msg->cmd){
        // case CMD_GET_JMMR_LIST: msg->cmd = CMD_SEARCH_DEVICES; break;
        case CMD_SET_STATE : cCmd->loadMsgToJmrStt(msg, &G_lJmrStt);         break;
    }
    if(msg->direction == MSG_DIR_RESPONSE)cCmd->processingResponseData(msg);
    else                                  cCmd->AProcessCmd(msg);
}

void AN_rs485::prepMsg(_MSG_PACK *msg, BYTE iterNum)
{
    switch(cmdType){
        case CMD_GET_JMMR_LIST: msg->addrEsp32 = iterNum+1; 
                                msg->cmd       = CMD_SEARCH_DEVICES;
                                msg->direction = MSG_DIR_REQUEST;
                                msg->response  = RESP_GET_JMMR_LIST;                                     
                                break;
        case CMD_SET_JMMR_LIST: msg->cmd        = CMD_SET_STATE;                  
                                msg->direction  = MSG_DIR_REQUEST;
                                msg->addrEsp32  = G_jmrsList[iterNum].esp32Addr;
                                AN_cmd::getI()->loadJmmrStateToMsg(msg, &G_jmrsList[iterNum]); 
                                break;                                
        case CMD_GET_JMMR_DATA: msg->cmd       = CMD_SEARCH_DEVICES;
                                msg->direction = MSG_DIR_REQUEST;   
                                msg->response  = RESP_GET_JMMR_DATA;                             
                                break;
        case CMD_SET_JMMR_DATA: msg->cmd        = CMD_SET_STATE;                  
                                msg->direction  = MSG_DIR_REQUEST;
                                msg->response   = RESP_SET_JMMR_DATA;   
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
	doc[PARAM_CMD]    = CMD_GET_JMMR_LIST;
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

void AN_rs485::sendBtJmmrData(_MSG_PACK *msg){
	JsonDocument doc;	
    	
	doc[PARAM_CMD]    = CMD_GET_JMMR_LIST;
    doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;
    doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;
 
    doc["jmmr_data"][PARAM_ADDR_ESP  ] = msg->addrEsp32; 
    doc["jmmr_data"][PARAM_ADDR_RM_1 ] = msg->addrRm1; 
    doc["jmmr_data"][PARAM_ADDR_RM_2 ] = msg->addrRm2; 
    doc["jmmr_data"][PARAM_MOD_CODE_1] = msg->modCode1; 
    doc["jmmr_data"][PARAM_MOD_CODE_2] = msg->modCode2; 
    doc["jmmr_data"][PARAM_MASK_1    ] = msg->mask1; 
    doc["jmmr_data"][PARAM_MASK_2    ] = msg->mask2; 
    doc["jmmr_data"][PARAM_PWR_1     ] = msg->pwr1; 
    doc["jmmr_data"][PARAM_PWR_2     ] = msg->pwr2; 
    doc["jmmr_data"][PARAM_TXT       ] = G_msgTxtData;
    doc["jmmr_data"][PARAM_TXT_LEN   ] = G_msgTxtDataLen;
    sendBtData(doc);    
}

void AN_rs485::sendMsgToBt(_MSG_PACK *msg){
	Serial.println("--- q1 ---");
	switch (cmdType){
		case CMD_GET_JMMR_LIST: sendJammListToBt();         break; 
		case CMD_SET_JMMR_LIST: sendBtResponse(cmdType, 1); break;
        case CMD_GET_JMMR_DATA: sendBtJmmrData(msg);        break;
	}
	cmdType = 0; 
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
                AN_json *json = AN_json::getI();     
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
    
 

 