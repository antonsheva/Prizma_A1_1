#include "AN_cmd.h"
 

AN_cmd cCmd;

AN_cmd::AN_cmd(/* args */)
{
	cmd       = 0;
	modCode1  = 0;
	modCode1  = 0;
	mask1     = 0;
	mask2     = 0;
	devNum1   = 0;
	devNum2   = 0;

	cRebMod = new RebModCntrl();
}

AN_cmd::~AN_cmd(void){

}
 

int AN_cmd::AGetJson(){
    int err = 0;
    JsonDocument jsonObj;
    DeserializationError error = deserializeJson(jsonObj, G_tmpBuff);
    
    Serial.println(G_tmpBuff);

    if(error){    
        err = -1;
    
    }
    
    cmd         = jsonObj[PARAM_CMD       ];
	modCode     = jsonObj[PARAM_MOD_CODE  ];
    modCode1    = jsonObj[PARAM_MOD_CODE_1];
    modCode2    = jsonObj[PARAM_MOD_CODE_2];
    mask        = jsonObj[PARAM_MASK      ];
	mask1       = jsonObj[PARAM_MASK_1    ];
    mask2       = jsonObj[PARAM_MASK_2    ];
	devNum      = jsonObj[PARAM_DEV_NUM   ];
	addrEsp32   = jsonObj[PARAM_ADDR_ESP32];  
	addrRm1     = jsonObj[PARAM_ADDR_RM_1 ];  
	addrRm2     = jsonObj[PARAM_ADDR_RM_2 ];   

	if((devNum == 1)||(devNum == 2))cRebMod->selDev = devNum;
    return err;
}
 
void AN_cmd::AProcessCmd()
{
	switch (this->cmd){
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

		case CMD_GET_ADDRESSES: getAddresses(); break;
		case CMD_SET_ADDR_ESP32: setAddrEsp32(); break;
		case CMD_SET_ADDR_RM_1 : setAddrRm1();   break; 
		case CMD_SET_ADDR_RM_2 : setAddrRm2();   break;
	}
}

int AN_cmd::getAddresses()
{
	JsonDocument doc;
	bool keyIsExist;
	char serialData[128] = "\0";
	preferences.begin("prefAddres", false);
	keyIsExist = preferences.isKey(PARAM_ADDR_ESP32);
	if(keyIsExist){
		addrEsp32 = preferences.getUChar(PARAM_ADDR_ESP32);
	}else{
		addrEsp32 = 0;
		preferences.putUChar(PARAM_ADDR_ESP32, addrEsp32);		
	}

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_1);
	if(keyIsExist){
		addrRm1 = preferences.getUChar(PARAM_ADDR_RM_1);
	}else{
		addrRm1 = 0;
		preferences.putUChar(PARAM_ADDR_RM_1, addrRm1);		
	}

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_2);
	if(keyIsExist){
		addrRm2 = preferences.getUChar(PARAM_ADDR_RM_2);
	}else{
		addrRm2 = 0;
		preferences.putUChar(PARAM_ADDR_RM_2, addrRm2);		
	}

	preferences.end();
	doc[PARAM_ADDR_ESP32] = addrEsp32;
	doc[PARAM_ADDR_RM_1 ] = addrEsp32;
	doc[PARAM_ADDR_RM_2 ] = addrEsp32;
	
	serializeJson(doc, serialData);
	xQueueSend(QueueBtOut, serialData, portMAX_DELAY);
	return 0;
}
 

int AN_cmd::setAddrEsp32()
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_ESP32, addrEsp32);
	preferences.end();
	return 0;
}

int AN_cmd::setAddrRm1() 
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_1, addrRm1);
	preferences.end();
	return 0;
}

int AN_cmd::setAddrRm2() 
{
	preferences.begin("prefAddres", false);
	preferences.putUChar(PARAM_ADDR_RM_2, addrRm2);
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
	int devNum = cRebMod->selDev-1;
	jmrStt->rebMod[devNum].mc   = modCode;
    jmrStt->rebMod[devNum].mask = mask;

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

 
