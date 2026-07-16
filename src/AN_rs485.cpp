#include "AN_rs485.h"

#define MSG_STATIC_DATA_LEN 20

AN_rs485* AN_rs485::instance = nullptr; 
void AN_rs485::processMsg(_MSG_PACK *msg)
{
    AN_cmd* cCmd = AN_cmd::getI();
    switch(msg->cmd){
        // case CMD_GET_JMMR_LIST: msg->cmd = CMD_SEARCH_DEVICES; break;
        case CMD_SET_STATE : loadMsgToJmrStt(msg, &G_lJmrStt);         break;
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
                                msg->addrEsp32  = G_jmmrsList[iterNum].esp32Addr;
                                loadJmmrStateToMsg(msg, &G_jmmrsList[iterNum]); 
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

/**todo check data length parameter */
void AN_rs485::sendBtData(String data){
    int len = data.length();
	int packQty = len/128+1;
 
    String str  = "start___"+data+"_stop";
	ADebugLog(" --- send to BT ---");
	Serial.println(str);
	SerialBT.println(str);    
}

void AN_rs485::sendJammListToBt(){
    String data = AN_json::getI()->packJmmrList();
    sendBtData(data);
}

void AN_rs485::sendBtResponse(BYTE cmd, uint32_t resp){
    String data = AN_json::getI()->packResponse(cmd, resp);
    sendBtData(data);
}

void AN_rs485::sendBtJmmrData(_MSG_PACK *msg){
    String data = AN_json::getI()->packJmmrData(msg);
    sendBtData(data);
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
    
 

 