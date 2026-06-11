#include "AN_cmd.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* AN_cmd::instance = nullptr;
RmCtrl* cRebMod;

AN_rs485* rs485; 

void AN_cmd::init()
{
	cRebMod = RmCtrl::getI();
	cRebMod->init();
	rs485 = AN_rs485::getI();
	getAddresses();
}

int AN_cmd::AGetJson(String data, _MSG_PACK *msg)
{
    JsonDocument jsonObj;
    
    DeserializationError error = deserializeJson(jsonObj, data);
    Serial.println(data);

    if(error)return -1;
    
    msg->cmd         	= jsonObj[PARAM_CMD       		];
	msg->modCode     	= jsonObj[PARAM_MOD_CODE  		];
    msg->mask        	= jsonObj[PARAM_MASK      		];
	msg->rmNum      	= jsonObj[PARAM_RM_NUM    		];
	msg->jmmrListLen	= jsonObj[PARAM_JMMR_LIST_LEN	];
	msg->addressee 		= jsonObj[PARAM_ADDRESSEE 		];  
	msg->addrRm1   		= jsonObj[PARAM_ADDR_RM_1 		];  
	msg->addrRm2   		= jsonObj[PARAM_ADDR_RM_2 		];   
    msg->modCode1    	= jsonObj[PARAM_MOD_CODE_1		];
    msg->modCode2    	= jsonObj[PARAM_MOD_CODE_2		];
	msg->mask1       	= jsonObj[PARAM_MASK_1    		];
    msg->mask2       	= jsonObj[PARAM_MASK_2    		];
	msg->pwr1       	= jsonObj[PARAM_PWR_1    		];
	msg->pwr2       	= jsonObj[PARAM_PWR_2    		];
	


	if((msg->rmNum == 1)||(msg->rmNum == 2))cRebMod->selDev = msg->rmNum-1;

	if(msg->jmmrListLen>0){
		rs485->subscribersQty = msg->jmmrListLen;
		G_jmrsList.clear();
		for(int i=0; i<msg->jmmrListLen; i++){
			JammerState jmmr; 
			jmmr.esp32Addr         = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDRESSEE	];
			jmmr.rebMod[0].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_1	];			
			jmmr.rebMod[0].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_1	];			
			jmmr.rebMod[0].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_1		];		
			jmmr.rebMod[1].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_2	];			
			jmmr.rebMod[1].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_2	];		
			jmmr.rebMod[1].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_2		];		
			jmmr.rebMod[0].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_1		];		
			jmmr.rebMod[1].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_2		];		
			
			
			G_jmrsList.push_back(jmmr);
		}
		printJmmrList();
	}
    return 0;
}

void AN_cmd::AProcessCmd(_MSG_PACK *msg)
{
	switch (msg->cmd){
		case CMD_AT        :  AT();    	    break; 
		case CMD_GET_ATBT  :  getATBT();    break; 
		case CMD_GET_ATC   :  getATC(); 	break; 
		case CMD_SET_ATC   :  setATC(msg); 	break; 
		case CMD_GET_ATI   :  getATI(); 	break; 
		case CMD_ATZ       :  ATZ(); 		break; 
		case CMD_SET_ATW   :  setATW(); 	break; 
		case CMD_GET_STATE :  getState(); 	break;
		case CMD_SET_STATE :  setState(msg); 	break;
		case CMD_GET_INFO  :  getInfo();    break;

		case CMD_GET_ADDRESSES : getAddresses(); 				break;
		case CMD_SET_ADDRESSES : setAddresses(msg); 			break;
		case CMD_SET_ADDRESSEE : setAddrEsp32(msg->addressee);	break;
		case CMD_SET_ADDR_RM_1 : setAddrRm1(msg->addrRm1);   	break; 
		case CMD_SET_ADDR_RM_2 : setAddrRm2(msg->addrRm2);   	break;
		case CMD_GET_JAMM_LIST : getJammList();  				break; 
		case CMD_SEARCH_DEVICES: searchDevices(msg->sender);  	break;
		case CMD_GEN_TEST_DATA : generateTestData();			break;
		case CMD_LOAD_CONFIG   : loadConfig();					break;					
	}
}

void AN_cmd::loadConfig(){
	rs485->cmdType = CMD_LOAD_CONFIG;
	vTaskResume(TaskHandle_txRs485);
}

void AN_cmd::loadMsgToJmrStt(_MSG_PACK *msg, JammerState *jmmr)
{
    jmmr->rebMod[0].mc      = msg->modCode1;
    jmmr->rebMod[1].mc      = msg->modCode2;
    jmmr->rebMod[0].mask    = msg->mask1;
    jmmr->rebMod[1].mask    = msg->mask2;
	jmmr->rebMod[0].pwr     = msg->pwr1;
	jmmr->rebMod[1].pwr     = msg->pwr2;
	
    if(msg->addrRm1 && msg->addrRm1 < 128)jmmr->rebMod[0].address = msg->addrRm1;
    if(msg->addrRm2 && msg->addrRm2 < 128)jmmr->rebMod[1].address = msg->addrRm2;
}

void AN_cmd::loadJmmrStateToMsg(_MSG_PACK *msg, JammerState *jmmrStt){
	msg->addrRm1   = jmmrStt->rebMod[0].address;
	msg->addrRm2   = jmmrStt->rebMod[1].address;
    msg->modCode1  = jmmrStt->rebMod[0].mc;
    msg->modCode2  = jmmrStt->rebMod[1].mc;	
	msg->mask1     = jmmrStt->rebMod[0].mask;
	msg->mask2     = jmmrStt->rebMod[1].mask;
	msg->pwr1      = jmmrStt->rebMod[0].pwr;
	msg->pwr2      = jmmrStt->rebMod[1].pwr;
}

void AN_cmd::updateLocalData(_MSG_PACK *msg)
{
	ADebugLog("Change local data");
    printJmmrList(1);
	if(rs485->cmdType == CMD_GET_JAMM_LIST){
		_MSG_PACK msg;
		msg.sender = G_lJmrStt.esp32Addr;
		loadJmmrStateToMsg(&msg, &G_lJmrStt);
        addJmmr(&msg);
	}
	if(rs485->cmdType == CMD_LOAD_CONFIG){
		loadMsgToJmrStt(msg, &G_lJmrStt);       
		ASetExpectedEvent(Event_finishLoadConfig);
	}
}

void AN_cmd::generateTestData()
{
	JammerState jmr;
	for(int i=0; i<8; i++){
		jmr.esp32Addr = i+1;

		jmr.rebMod[0].address = i+1;
		jmr.rebMod[1].address = i+11;

		jmr.rebMod[0].mc = 1+i;
		jmr.rebMod[1].mc = 1+i;

		jmr.rebMod[0].mask = 0x0F<<i+1;
		jmr.rebMod[1].mask = 0xF0<<i+1;		
		
		G_jmrsList.push_back(jmr);
	}
	rs485->subscribersQty = 8;
	printJmmrList();
}

void AN_cmd::printJmmrList(int src)
{
	if(src){
		Serial.println("----------------");
		Serial.println("ESP addr-> "+String(G_lJmrStt.esp32Addr));
		Serial.println("addrRm1->  "+String(G_lJmrStt.rebMod[0].address));
		Serial.println("addrRm2->  "+String(G_lJmrStt.rebMod[1].address));
		Serial.println("mc1    ->  "+String(G_lJmrStt.rebMod[0].mc));
		Serial.println("mc2    ->  "+String(G_lJmrStt.rebMod[1].mc));
		Serial.println("mask1  ->  "+String(G_lJmrStt.rebMod[0].mask));
		Serial.println("mask2  ->  "+String(G_lJmrStt.rebMod[1].mask));	
		Serial.println("pwr1   ->  "+String(G_lJmrStt.rebMod[0].pwr));	
		Serial.println("pwr2   ->  "+String(G_lJmrStt.rebMod[1].pwr));
	}else{
		for(int i=0; i<rs485->subscribersQty; i++){
			Serial.println("----------------");
			Serial.println("ESP addr-> "+String(G_jmrsList[i].esp32Addr));
			Serial.println("addrRm1-> "+String(G_jmrsList[i].rebMod[0].address));
			Serial.println("addrRm2-> "+String(G_jmrsList[i].rebMod[1].address));
			Serial.println("mc1    -> "+String(G_jmrsList[i].rebMod[0].mc));
			Serial.println("mc2    -> "+String(G_jmrsList[i].rebMod[1].mc));
			Serial.println("mask1  -> "+String(G_jmrsList[i].rebMod[0].mask));
			Serial.println("mask2  -> "+String(G_jmrsList[i].rebMod[1].mask));	
			Serial.println("pwr1   -> "+String(G_jmrsList[i].rebMod[0].pwr));	
			Serial.println("pwr2   -> "+String(G_jmrsList[i].rebMod[1].pwr));			
		}
	}

}

void AN_cmd::APrintMsg(_MSG_PACK *msg)
{
	Serial.println("ESP addr-> "+String(msg->sender));
	
	Serial.println("mc1  -> "+String(msg->modCode1));
	Serial.println("mc2  -> "+String(msg->modCode2));
	Serial.println("mask1-> "+String(msg->mask1));
	Serial.println("mask2-> "+String(msg->mask2));
}

void AN_cmd::sendBtData(JsonDocument doc){
	int len = serializeJson(doc, G_tmpDataBuff);
	int packQty = len/128+1;
	String str = "start___";
	str.toCharArray(G_serialData, 9);
	std::copy(G_tmpDataBuff, &G_tmpDataBuff[0]+len, &G_serialData[0]+8);
	
	G_serialData[8+len+0] = 's';
	G_serialData[8+len+1] = 't';
	G_serialData[8+len+2] = 'o';
	G_serialData[8+len+3] = 'p';

	String("_stop").toCharArray(G_serialData+8+len, 6);
	Serial.println("json len -> "+String(len));
	Serial.println(G_serialData);
	Serial.println("--- end ---");
	
	SerialBT.println(G_serialData);
}

void AN_cmd::sendJammListToBt(){

	JsonDocument doc;		
	doc[PARAM_CMD] = CMD_GET_JAMM_LIST;
	for(size_t i=0; i<G_jmrsList.size(); i++){
		doc["jmmr_list"][i][PARAM_ADDRESSEE ] = G_jmrsList[i].esp32Addr;
		doc["jmmr_list"][i][PARAM_ADDR_RM_1 ] = G_jmrsList[i].rebMod[0].address;
		doc["jmmr_list"][i][PARAM_ADDR_RM_2 ] = G_jmrsList[i].rebMod[1].address;
		doc["jmmr_list"][i][PARAM_MOD_CODE_1] = G_jmrsList[i].rebMod[0].mc;
		doc["jmmr_list"][i][PARAM_MOD_CODE_2] = G_jmrsList[i].rebMod[1].mc;
		doc["jmmr_list"][i][PARAM_MASK_1    ] = G_jmrsList[i].rebMod[0].mask;
		doc["jmmr_list"][i][PARAM_MASK_2    ] = G_jmrsList[i].rebMod[1].mask;
		doc["jmmr_list"][i][PARAM_PWR_1     ] = G_jmrsList[i].rebMod[0].pwr;
		doc["jmmr_list"][i][PARAM_PWR_2     ] = G_jmrsList[i].rebMod[1].pwr;
	}
	sendBtData(doc);
}

void AN_cmd::sendBtResponse(BYTE cmd, uint32_t resp){
	JsonDocument doc;		
	doc[PARAM_CMD]  	= cmd;
	doc[PARAM_RESPONSE] = resp;
	sendBtData(doc);
}

void AN_cmd::sendResultToBt(){

	Serial.println("--- q1 ---");

	switch (lastCmd){
		case CMD_GET_JAMM_LIST: sendJammListToBt(); break; 
		case CMD_LOAD_CONFIG  : sendBtResponse(lastCmd, 1);
	}
	lastCmd = 0; 
}

void AN_cmd::addJmmr(_MSG_PACK *msg){
	JammerState jmmr; 
	jmmr.esp32Addr         = msg->sender;

	loadMsgToJmrStt(msg, &jmmr);

	// jmmr.rebMod[0].mc      = msg->modCode1;
	// jmmr.rebMod[1].mc      = msg->modCode2;
	// jmmr.rebMod[0].mask    = msg->mask1;
	// jmmr.rebMod[1].mask    = msg->mask2;
	// jmmr.rebMod[0].pwr     = msg->pwr1;
	// jmmr.rebMod[1].pwr     = msg->pwr2;

	// jmmr.rebMod[0].address = msg->addrRm1;
	// jmmr.rebMod[1].address = msg->addrRm2;
	

	G_jmrsList.push_back(jmmr);
	rs485->foundSubscribersQty++;
	vTaskResume(TaskHandle_txRs485);
}

int AN_cmd::processingResponseData(_MSG_PACK *msg){
	
    APrintMsg(msg);
    switch (msg->cmd){
		case CMD_SEARCH_DEVICES: addJmmr(msg); break;
	}
	 
	return 0 ;
}

int AN_cmd::searchDevices(BYTE addrSeneder){
	Serial.println(" -> Search device cmd");
	_MSG_PACK msg;
	msg.cmd       = CMD_SEARCH_DEVICES;
	msg.direction = MSG_DIR_RESPONSE;
	msg.addressee = addrSeneder;
	msg.sender    = G_lJmrStt.esp32Addr;
	loadJmmrStateToMsg(&msg, &G_lJmrStt); 
 	rs485->sendData(&msg);
	return 0;   
} 

int AN_cmd::getJammList(){
	G_rs485IterNum = 0;
	rs485->foundSubscribersQty = 0;
	rs485->subscribersQty = MAX_DEVICE_QTY;
	G_jmrsList.clear();
	rs485->cmdType = CMD_GET_JAMM_LIST;
	vTaskResume(TaskHandle_txRs485);
	return 0;
}

void AN_cmd::resetDataPackProcess(String comment){
		ADebugLog(comment);
		waitTimer = 0;
		endOfDataPacks = 0;
		receiveDataPacks = 0;
		dataPackStr = "";
}

int AN_cmd::concatMsgPacks(String str){
	static int cnt = 0;
	cnt++;
	Serial.println("--");
	Serial.println(str);
	_MSG_PACK msg;
	if(str.length()>120)dataPackStr += str.substring(0,120);
	else				dataPackStr += str;
Serial.println(dataPackStr);
	if(dataPackStr.length() > MAX_SERIAL_DATA_LEN){
		resetDataPackProcess("dataPack too long");
	}else if( dataPackStr.indexOf("stop") != -1){
		Serial.println(" -- data ---"+String(cnt));
		if(dataPackStr.length() < 20){
			resetDataPackProcess("dataPack too little");
			cnt = 0;	
		}else if(dataPackStr.startsWith("start")){
			dataPackStr = dataPackStr.substring(8, dataPackStr.lastIndexOf("}")+1);
			Serial.println("--- OUT STRING ---");
			Serial.println(dataPackStr);

			if(AGetJson(dataPackStr, &msg)){ //there is error in JSON-data
				resetDataPackProcess("error JSON data");
			}else{
				lastCmd = msg.cmd;
				vTaskSuspend(TaskHandle_wtDataPack);
				waitTimer = 0;
				resetDataPackProcess("error dataPack");
				AProcessCmd(&msg);
			}		
		}else{
			resetDataPackProcess("error dataPack");
		}
		cnt = 0;
	}
	return 0;
}

int AN_cmd::getAddresses(){
	JsonDocument doc;
	char serialData[128] = "\0";
	 
	doc[PARAM_ADDRESSEE]  = G_lJmrStt.esp32Addr        ; 
	doc[PARAM_ADDR_RM_1 ] = G_lJmrStt.rebMod[0].address;
	doc[PARAM_ADDR_RM_2 ] = G_lJmrStt.rebMod[1].address;
	
	serializeJson(doc, serialData);
	xQueueSend(QueueBtOut, serialData, portMAX_DELAY);

	return 0;
}
 
int AN_cmd::setAddresses(_MSG_PACK *msg){

	if(msg->addressee)setAddrEsp32	(msg->addressee,0);
	if(msg->addrRm1)setAddrRm1		(msg->addrRm1,0);
	if(msg->addrRm2)setAddrRm2		(msg->addrRm2,0);

	getAddresses();
	return 0;
}

int AN_cmd::setAddrEsp32(BYTE addr, bool needRead)
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDRESSEE, addr);
	preferences.end();
	initLocalAddresses();
	if(needRead) getAddresses();

	return 0;
}

int AN_cmd::setAddrRm1(BYTE addr, bool needRead) 
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_1, addr);
	preferences.end();
	initLocalAddresses();
	if(needRead) getAddresses();

	return 0;
}

int AN_cmd::setAddrRm2(BYTE addr, bool needRead) 
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_2, addr);
	preferences.end();
	initLocalAddresses();
	if(needRead) getAddresses();

	return 0;
}

int AN_cmd::AT()
{
	cRebMod->At();
  	return 0;
}

int AN_cmd::getATBT()
{
	cRebMod->getAtbt();
	return 0;
}

int AN_cmd::getATC(){
	cRebMod->getAtc();
	return 0;
}

int AN_cmd::setATC(_MSG_PACK *msg){
 
    G_lJmrStt.rebMod[cRebMod->selDev].mc   = msg->modCode;
    G_lJmrStt.rebMod[cRebMod->selDev].mask = msg->mask;

	for(int i=0; i<16; i++)G_opList[i] = 0;
    G_opList[0] = CMD_SET_ATC ;
    G_opList[1] = CMD_SET_ATW ;
	G_opList[2] = CMD_ATZ ;
	
	G_opQty = 3;
	G_opCnt = 0;
	
 	vTaskResume(TaskHandle_rebModAut);
	return 0;
}

int AN_cmd::setState(_MSG_PACK *msg){
	if(msg != NULL) loadMsgToJmrStt(msg, &G_lJmrStt);
	for(int i=0; i<16; i++)G_opList[i] = 0;
	
	G_opList[0] = CMD_SET_ATC ;
	G_opList[1] = CMD_SET_ATC ;
    G_opList[2] = CMD_SET_ATW ;
	G_opList[3] = CMD_SET_ATW ;
	G_opList[4] = CMD_ATZ     ;
	G_opList[5] = CMD_ATZ     ;
	
	G_opQty = 6;
	G_opCnt = 0;

    G_swtchActDev = true;	
    vTaskResume(TaskHandle_rebModAut);
   
	return 0;
}

int AN_cmd::getATI()
{
	cRebMod->getAti();
  	return 0;
}

int AN_cmd::setATE()
{
 
    return 0;
}

int AN_cmd::ATZ()
{
	cRebMod->setAtz();
	return 0;
}

int AN_cmd::setATW()
{	
	cRebMod->setAtw();
	return 0;
}

int AN_cmd::getInfo()
{
	for(int i=0; i<16; i++)G_opList[i] = 0;
 
    G_opList[0] = CMD_GET_ATI ;
    G_opList[1] = CMD_GET_ATI ;
    G_opList[2] = CMD_GET_ATC ;
    G_opList[3] = CMD_GET_ATC ;
    G_opList[4] = CMD_GET_ATBT;
    G_opList[5] = CMD_GET_ATBT;

    G_opQty = 6;
	G_opCnt = 0;

    G_swtchActDev = true;	
    vTaskResume(TaskHandle_rebModAut);
    return 0;
}

int AN_cmd::getState(){

	return 0;
}






