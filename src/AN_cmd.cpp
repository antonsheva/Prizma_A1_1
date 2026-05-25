#include "AN_cmd.h"
 
#define MAX_JMMR_QTY 10

AN_cmd* AN_cmd::instance = nullptr;
RebModCntrl* cRebMod;

AN_rs485* rs485; 

void AN_cmd::init()
{
	cRebMod = RebModCntrl::getI();
	cRebMod->init();
	rs485 = AN_rs485::getI();
	getAddresses();
}

int AN_cmd::AGetJson()
{
    int err = 0;
    JsonDocument jsonObj;
    DeserializationError error = deserializeJson(jsonObj, G_tmpBuff);
    
    Serial.println(G_tmpBuff);

    if(error){    
        err = -1;
    }
    
    G_serial_msg.cmd         		= jsonObj[PARAM_CMD       ];
	G_serial_msg.modCode     		= jsonObj[PARAM_MOD_CODE  ];
    G_serial_msg.modCode1    		= jsonObj[PARAM_MOD_CODE_1];
    G_serial_msg.modCode2    		= jsonObj[PARAM_MOD_CODE_2];
    G_serial_msg.mask        		= jsonObj[PARAM_MASK      ];
	G_serial_msg.mask1       		= jsonObj[PARAM_MASK_1    ];
    G_serial_msg.mask2       		= jsonObj[PARAM_MASK_2    ];
	G_serial_msg.addressee 			= jsonObj[PARAM_ADDRESSEE ];  
	G_serial_msg.rmNum      		= jsonObj[PARAM_RM_NUM    ];
	G_serial_msg.addrRm1   			= jsonObj[PARAM_ADDR_RM_1 ];  
	G_serial_msg.addrRm2   			= jsonObj[PARAM_ADDR_RM_2 ];   

	if((G_serial_msg.rmNum == 1)||(G_serial_msg.rmNum == 2))cRebMod->selDev = G_serial_msg.rmNum-1;
    return err;
}

void AN_cmd::AProcessCmd(_MSG_PACK *msg)
{
	switch (msg->cmd){
		case CMD_AT        :  AT();    	    break; 
		case CMD_GET_ATBT  :  getATBT();    break; 
		case CMD_GET_ATC   :  getATC(); 	break; 
		case CMD_SET_ATC   :  setATC(); 	break; 
		case CMD_GET_ATI   :  getATI(); 	break; 
		case CMD_ATZ       :  ATZ(); 		break; 
		case CMD_SET_ATW   :  setATW(); 	break; 
		case CMD_GET_STATE :  getState(); 	break;
		case CMD_SET_STATE :  setState(); 	break;
		case CMD_GET_INFO  :  getInfo();    break;

		case CMD_GET_ADDRESSES : getAddresses(); 				break;
		case CMD_SET_ADDRESSES : setAddresses(msg); 			break;
		case CMD_SET_ADDRESSEE : setAddrEsp32(msg->addressee);  break;
		case CMD_SET_ADDR_RM_1 : setAddrRm1(msg->addrRm1);   	break; 
		case CMD_SET_ADDR_RM_2 : setAddrRm2(msg->addrRm2);   	break;
		case CMD_GET_JAMM_LIST : getJammList();  				break; 
		case CMD_SEARCH_DEVICES: searchDevices(msg->sender);   	break;
		case CMD_GEN_TEST_DATA : generateTestData();			break;
		case CMD_LOAD_CONFIG   : loadConfig();					break;					
	}
}

void AN_cmd::loadConfig(){
	rs485->subscribersQty = 8;
	rs485->cmdType = CMD_LOAD_CONFIG;
	vTaskResume(TaskHandle_txRs485);
}

void AN_cmd::updateLocalData(int i)
{
	if(rs485->cmdType == CMD_GET_JAMM_LIST)return;
	if(G_jmrsList.size() < (i+1)){
		ADebugLog("G_jmrsList error");
		return;
	}

	G_lJmrStt.rebMod[0].address = G_jmrsList[i].rebMod[0].address;
	G_lJmrStt.rebMod[1].address = G_jmrsList[i].rebMod[1].address;
	
	G_lJmrStt.rebMod[0].mc 		= G_jmrsList[i].rebMod[0].mc;
	G_lJmrStt.rebMod[1].mc 		= G_jmrsList[i].rebMod[1].mc;
	
    G_lJmrStt.rebMod[0].mask 	= G_jmrsList[i].rebMod[0].mask;
    G_lJmrStt.rebMod[1].mask 	= G_jmrsList[i].rebMod[1].mask;
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

void AN_cmd::printJmmrList()
{
	for(int i=0; i<rs485->subscribersQty; i++){
		Serial.println("----------------");

		Serial.println("ESP addr-> "+String(G_jmrsList[i].esp32Addr));

		Serial.println("addrRm1-> "+String(G_jmrsList[i].rebMod[0].address));
		Serial.println("addrRm2-> "+String(G_jmrsList[i].rebMod[1].address));
		Serial.println("mc1->     "+String(G_jmrsList[i].rebMod[0].mc));
		Serial.println("mc2->     "+String(G_jmrsList[i].rebMod[1].mc));
		Serial.println("mask1->   "+String(G_jmrsList[i].rebMod[0].mask));
		Serial.println("mask2->   "+String(G_jmrsList[i].rebMod[1].mask));	
	}
}

void AN_cmd::APrintMsg(_MSG_PACK *msg)
{
	Serial.println("ESP addr-> "+String(msg->addressee));
	
	Serial.println("mc1  -> "+String(msg->modCode1));
	Serial.println("mc2  -> "+String(msg->modCode2));
	Serial.println("mask1-> "+String(msg->mask1));
	Serial.println("mask2-> "+String(msg->mask2));
}

void AN_cmd::addJrmr(_MSG_PACK *msg){
	JammerState jmr;// = new JammerState();
	jmr.esp32Addr         = msg->addressee;
	jmr.rebMod[0].address = msg->addrRm1;
	jmr.rebMod[0].mc      = msg->modCode1;
	jmr.rebMod[0].mask    = msg->mask1;
	jmr.rebMod[1].address = msg->addrRm2;
	jmr.rebMod[1].mc      = msg->modCode2;
	jmr.rebMod[1].mask    = msg->mask2;
	
	G_jmrsList.push_back(jmr);
	vTaskResume(TaskHandle_txRs485);
}

int AN_cmd::processingResponseData(_MSG_PACK *msg){
	
    APrintMsg(msg);
    switch (msg->cmd){
		case CMD_SEARCH_DEVICES: addJrmr(msg); break;
	

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
	msg.addrRm1   = G_lJmrStt.rebMod[0].address;
	msg.addrRm2   = G_lJmrStt.rebMod[1].address;
    msg.modCode1  = G_lJmrStt.rebMod[0].mc;
    msg.modCode2  = G_lJmrStt.rebMod[1].mc;	
	msg.mask1     = G_lJmrStt.rebMod[0].mask;
	msg.mask2     = G_lJmrStt.rebMod[1].mask;
 
 
 	rs485->sendData(&msg);
	return 0;   
} 

int AN_cmd::getJammList(){
	rs485->cmdType = CMD_GET_JAMM_LIST;
	vTaskResume(TaskHandle_txRs485);
	return 0;
}

int AN_cmd::getAddresses(){
	JsonDocument doc;
	_ADDRESSESS addrs;
	char serialData[128] = "\0";
	 
	getLocalAddresses(&addrs);

	doc[PARAM_ADDRESSEE] =  addrs.esp32;
	doc[PARAM_ADDR_RM_1 ] = addrs.rm1;
	doc[PARAM_ADDR_RM_2 ] = addrs.rm2;
	
	serializeJson(doc, serialData);

	G_lJmrStt.esp32Addr         = addrs.esp32;
	G_lJmrStt.rebMod[0].address = addrs.rm1;
	G_lJmrStt.rebMod[1].address = addrs.rm2;
	// G_localAddresses
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
	if(needRead) getAddresses();

	return 0;
}

int AN_cmd::setAddrRm1(BYTE addr, bool needRead) 
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_1, addr);
	preferences.end();
	if(needRead) getAddresses();

	return 0;
}

int AN_cmd::setAddrRm2(BYTE addr, bool needRead) 
{
	if(!addr || addr>127)return -1;
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_2, addr);
	preferences.end();
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

int AN_cmd::setATC(){
 
    G_lJmrStt.rebMod[cRebMod->selDev].mc   = G_serial_msg.modCode;
    G_lJmrStt.rebMod[cRebMod->selDev].mask = G_serial_msg.mask;

	for(int i=0; i<16; i++)G_opList[i] = 0;
    G_opList[0] = CMD_SET_ATC ;
    G_opList[1] = CMD_SET_ATW ;
	G_opList[2] = CMD_ATZ ;
	
	G_opQty = 3;
 	vTaskResume(TaskHandle_automat);
	return 0;
}

int AN_cmd::setState(){
	for(int i=0; i<16; i++)G_opList[i] = 0;
	
	G_opList[0] = CMD_SET_ATC ;
	G_opList[1] = CMD_SET_ATC ;
    G_opList[2] = CMD_SET_ATW ;
	G_opList[3] = CMD_SET_ATW ;
	G_opList[4] = CMD_ATZ     ;
	G_opList[5] = CMD_ATZ     ;
	
	G_opQty = 6;
    G_swtchActDev = true;	
    vTaskResume(TaskHandle_automat);
   
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
    G_swtchActDev = true;	
    vTaskResume(TaskHandle_automat);
    return 0;
}

int AN_cmd::getState(){

	return 0;
}






