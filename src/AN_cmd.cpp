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
	G_serial_msg.addrEsp32 			= jsonObj[PARAM_ADDR_ESP32];  
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

		case CMD_GET_ADDRESSES : getAddresses(); break;
		case CMD_SET_ADDRESSES : setAddresses(); break;
		case CMD_SET_ADDR_ESP32: setAddrEsp32(); break;
		case CMD_SET_ADDR_RM_1 : setAddrRm1();   break; 
		case CMD_SET_ADDR_RM_2 : setAddrRm2();   break;
		case CMD_GET_JAMM_LIST : getJammList();  break; 
		case CMD_SEARCH_DEVICES: searchDevices(msg->addrSender);     break;
		 
	}
}


void AN_cmd::APrintMsg(_MSG_PACK *msg){
	Serial.println("ESP addr-> "+String(msg->addrEsp32));
	
	Serial.println("mc1-> "+String(msg->modCode1));
	Serial.println("mc2-> "+String(msg->modCode2));
	Serial.println("mask1-> "+String(msg->mask1));
	Serial.println("mask2-> "+String(msg->mask2));
}

void AN_cmd::addJrmr(_MSG_PACK *msg){
	JammerState jmr;// = new JammerState();
	jmr.esp32Addr         = msg->addrEsp32;
	jmr.rebMod[0].address = msg->addrRm1;
	jmr.rebMod[0].mc      = msg->modCode1 ;
	jmr.rebMod[0].mask    = msg->mask1 ;
	jmr.rebMod[1].address = msg->addrRm2;
	jmr.rebMod[1].mc      = msg->modCode2;
	jmr.rebMod[1].mask    = msg->mask2;
	
	G_connJmrs[0].push_back(jmr);
	vTaskResume(TaskHandle_getJammList);
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
	msg.addrEsp32 = G_localAddresses.esp32;
	msg.addrRm1   = G_localAddresses.rm1;
	msg.addrRm2   = G_localAddresses.rm2;
    msg.modCode1  = localJmrStt->rebMod[0].mc;
	msg.mask1     = localJmrStt->rebMod[0].mask;
    msg.modCode2  = localJmrStt->rebMod[1].mc;	
	msg.mask2     = localJmrStt->rebMod[1].mask;
 
 
 	rs485->sendData(addrSeneder, &msg);
	return 0;   
} 

int AN_cmd::getJammList(){
	vTaskResume(TaskHandle_getJammList);
	return 0;
}

int AN_cmd::getAddresses(){
	JsonDocument doc;
	char serialData[128] = "\0";
	 
	getLocalAddresses(&G_localAddresses);

	doc[PARAM_ADDR_ESP32] = G_localAddresses.esp32;
	doc[PARAM_ADDR_RM_1 ] = G_localAddresses.rm1;
	doc[PARAM_ADDR_RM_2 ] = G_localAddresses.rm2;
	
	serializeJson(doc, serialData);
	xQueueSend(QueueBtOut, serialData, portMAX_DELAY);
	return 0;
}
 
int AN_cmd::setAddresses(){
	setAddrEsp32();
	setAddrRm1();
	setAddrRm2();

	getAddresses();
	return 0;
}

int AN_cmd::setAddrEsp32()
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_ESP32, G_serial_msg.addrEsp32);
	preferences.end();
	return 0;
}

int AN_cmd::setAddrRm1() 
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_1, G_serial_msg.addrRm1);
	preferences.end();
	return 0;
}

int AN_cmd::setAddrRm2() 
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_2, G_serial_msg.addrRm2);
	preferences.end();
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
 
	localJmrStt->rebMod[cRebMod->selDev].mc   = G_serial_msg.modCode;
    localJmrStt->rebMod[cRebMod->selDev].mask = G_serial_msg.mask;

	for(int i=0; i<16; i++)G_opList[i] = 0;
    G_opList[0] = CMD_SET_ATC ;
    G_opList[1] = CMD_SET_ATW ;
	G_opList[2] = CMD_ATZ ;
	
	G_opQty = 3;
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

int AN_cmd::setState(){
	
	
	return 0;
}

 
