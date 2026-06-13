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



/// @brief 
/// @param msg 
void AN_cmd::AProcessCmd(_MSG_PACK *msg)
{
	AN_cbFuncs* cbFuncs = AN_cbFuncs::getI();
	if((msg->rmNum == 1)||(msg->rmNum == 2))cRebMod->selDev = msg->rmNum-1;
	
	if(rs485->dataSrc == SERIAL_SRC_BT)
		rs485->subscribersQty = msg->jmmrListLen;

Serial.println("subscribersQty -> "+String(rs485->subscribersQty));

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
		case CMD_SET_ADDR_RM_1 : setAddrRm1(msg->addrRm1);   	break; 
		case CMD_SET_ADDR_RM_2 : setAddrRm2(msg->addrRm2);   	break;
		case CMD_GET_JAMM_LIST : getJammList();  				break; 
		case CMD_SEARCH_DEVICES: searchDevices(msg->sender);  	break;
		case CMD_GEN_TEST_DATA : generateTestData();			break;
		case CMD_LOAD_CONFIG   : loadConfig();					break;	
		case CMD_TEST		   : testFunc(); 					break;
	}
	Serial2.onReceive(cbFuncs->uart485);
}

void AN_cmd::testFunc(){
	_MSG_PACK msg;
	loadJmmrStateToMsg(&msg, &G_lJmrStt);
	xQueueSend(QueueRs485Send, &msg, portMAX_DELAY);
	// rs485->sendMsgTo485(&msg);
}

void AN_cmd::loadConfig(){
	rs485->cmdType = CMD_LOAD_CONFIG;
	vTaskResume(TaskHandle_connLevel_up);
}

void AN_cmd::loadMsgToJmrStt(_MSG_PACK *msg, JammerState *jmmr)
{
    jmmr->rebMod[0].mc      = msg->modCode1;
    jmmr->rebMod[1].mc      = msg->modCode2;
    jmmr->rebMod[0].mask    = msg->mask1;
    jmmr->rebMod[1].mask    = msg->mask2;
	jmmr->rebMod[0].pwr     = msg->pwr1;
	jmmr->rebMod[1].pwr     = msg->pwr2;
	jmmr->info              = msg->txtData;
	
    if(msg->addrRm1 && msg->addrRm1 < 128)jmmr->rebMod[0].address = msg->addrRm1;
    if(msg->addrRm2 && msg->addrRm2 < 128)jmmr->rebMod[1].address = msg->addrRm2;
}

void AN_cmd::loadJmmrStateToMsg(_MSG_PACK *msg, JammerState *jmmrStt){
	msg->addrRm1   	= jmmrStt->rebMod[0].address;
	msg->addrRm2   	= jmmrStt->rebMod[1].address;
    msg->modCode1  	= jmmrStt->rebMod[0].mc;
    msg->modCode2  	= jmmrStt->rebMod[1].mc;	
	msg->mask1     	= jmmrStt->rebMod[0].mask;
	msg->mask2     	= jmmrStt->rebMod[1].mask;
	msg->pwr1      	= jmmrStt->rebMod[0].pwr;
	msg->pwr2      	= jmmrStt->rebMod[1].pwr;
	msg->txtData	= jmmrStt->info;
}

void AN_cmd::updateLocalData(_MSG_PACK *msg)
{
	if(rs485->cmdType == CMD_GET_JAMM_LIST){
		// _MSG_PACK msg;
		// msg.sender = G_lJmrStt.esp32Addr;
		// loadJmmrStateToMsg(&msg, &G_lJmrStt);
		Serial.println("----updateLocalData----");
        addJmmr(&G_lJmrStt);
	}
	if(rs485->cmdType == CMD_LOAD_CONFIG){
		loadMsgToJmrStt(msg, &G_lJmrStt);       
		ASetExpectedEvent(Event_finishLoadConfig);
		// setState(NULL);
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
    Serial.println("--------printJmmrList--------");

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
	Serial.println(" ---APrintMsg --- ");
	Serial.println("ESP addr-> "+String(msg->sender));
	Serial.println("mc1  -> "+String(msg->modCode1));
	Serial.println("mc2  -> "+String(msg->modCode2));
	Serial.println("mask1-> "+String(msg->mask1));
	Serial.println("mask2-> "+String(msg->mask2));
}

void AN_cmd::addJmmr(_MSG_PACK *msg){
	JammerState jmmr; 
	jmmr.esp32Addr = msg->sender;
	loadMsgToJmrStt(msg, &jmmr);
	G_jmrsList.push_back(jmmr);
	rs485->foundSubscribersQty++;
	vTaskResume(TaskHandle_connLevel_up);
}

void AN_cmd::copyJmmr(JammerState *jmmr1, JammerState *jmmr2){
	jmmr1->esp32Addr = jmmr2->esp32Addr;
	jmmr1->info = jmmr2->info;
    jmmr1->rebMod[0].mc          = jmmr2->rebMod[0].mc         ;    			
    jmmr1->rebMod[0].mask        = jmmr2->rebMod[0].mask       ;      				
    jmmr1->rebMod[0].address     = jmmr2->rebMod[0].address    ;         				
    jmmr1->rebMod[0].echo        = jmmr2->rebMod[0].echo       ;      				
    jmmr1->rebMod[0].pwr         = jmmr2->rebMod[0].pwr        ;     				
    jmmr1->rebMod[0].vcpu        = jmmr2->rebMod[0].vcpu       ;      					
    jmmr1->rebMod[0].temp        = jmmr2->rebMod[0].temp       ;      					
    jmmr1->rebMod[0].info        = jmmr2->rebMod[0].info       ;      					
    jmmr1->rebMod[0].infoDataQty = jmmr2->rebMod[0].infoDataQty; 
	
    jmmr1->rebMod[1].mc          = jmmr2->rebMod[1].mc         ;    			
    jmmr1->rebMod[1].mask        = jmmr2->rebMod[1].mask       ;      				
    jmmr1->rebMod[1].address     = jmmr2->rebMod[1].address    ;         				
    jmmr1->rebMod[1].echo        = jmmr2->rebMod[1].echo       ;      				
    jmmr1->rebMod[1].pwr         = jmmr2->rebMod[1].pwr        ;     				
    jmmr1->rebMod[1].vcpu        = jmmr2->rebMod[1].vcpu       ;      					
    jmmr1->rebMod[1].temp        = jmmr2->rebMod[1].temp       ;      					
    jmmr1->rebMod[1].info        = jmmr2->rebMod[1].info       ;      					
    jmmr1->rebMod[1].infoDataQty = jmmr2->rebMod[1].infoDataQty; 
}
void AN_cmd::addJmmr(JammerState *jmmr){
	JammerState j; 
	copyJmmr(&j, jmmr); 
	G_jmrsList.push_back(j);
	rs485->foundSubscribersQty++;
	vTaskResume(TaskHandle_connLevel_up);
}

int AN_cmd::processingResponseData(_MSG_PACK *msg){
	
    APrintMsg(msg);
    switch (msg->response){
		case RESP_SEARCH_DEVICES: 
        	Serial.println("---processingResponseData---");
			addJmmr(msg); break;
	}
	return 0 ;
}

int AN_cmd::searchDevices(BYTE addrSeneder){
	Serial.println(" -> Search device cmd");
	_MSG_PACK msg;
	msg.response   = RESP_SEARCH_DEVICES;
	msg.direction  = MSG_DIR_RESPONSE;
	msg.addrEsp32  = addrSeneder;
	msg.txtData    = G_lJmrStt.info;
	msg.txtDataLen = msg.txtData.length();

	loadJmmrStateToMsg(&msg, &G_lJmrStt);
    Serial.println("t- 1");

	xQueueSend(QueueRs485Send, &msg, portMAX_DELAY);
 	// rs485->sendMsgTo485(&msg);
	return 0;   
} 

int AN_cmd::getJammList(){
	G_rs485IterNum = 0;
	rs485->foundSubscribersQty = 0;
	rs485->subscribersQty = MAX_DEVICE_QTY;
	G_jmrsList.clear();
	rs485->cmdType = CMD_GET_JAMM_LIST;
	vTaskResume(TaskHandle_connLevel_up);
	return 0;
}

int AN_cmd::getAddresses(){
	JsonDocument doc;
	char serialData[128] = "\0";
	 
	doc[PARAM_ADDR_ESP]  = G_lJmrStt.esp32Addr        ; 
	doc[PARAM_ADDR_RM_1 ] = G_lJmrStt.rebMod[0].address;
	doc[PARAM_ADDR_RM_2 ] = G_lJmrStt.rebMod[1].address;
	
	serializeJson(doc, serialData);
	xQueueSend(QueueBtOut, serialData, portMAX_DELAY);

	return 0;
}
 
int AN_cmd::setAddresses(_MSG_PACK *msg){

	if(msg->addrEsp32)setAddrEsp32	(msg->addrEsp32,0);
	if(msg->addrRm1)setAddrRm1		(msg->addrRm1,0);
	if(msg->addrRm2)setAddrRm2		(msg->addrRm2,0);

	getAddresses();
	return 0;
}

int AN_cmd::setAddrEsp32(BYTE addr, bool needRead)
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_ESP, addr);
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






