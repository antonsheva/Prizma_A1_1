#include "AN_pref.h"

AN_pref* AN_pref::instance = nullptr;

void AN_pref::init(){
    bool keyIsExist;
    preferences.begin("prefData", false);
	
    keyIsExist = preferences.isKey(PARAM_ADDR_ESP);
	if(keyIsExist){
		G_lJmrStt.esp32Addr = preferences.getUChar(PARAM_ADDR_ESP);
	}else{
		G_lJmrStt.esp32Addr = 0;
		preferences.putUChar(PARAM_ADDR_ESP, G_lJmrStt.esp32Addr);		
	}	

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_1);
	if(keyIsExist){
		G_lJmrStt.rebMod[0].address = preferences.getUChar(PARAM_ADDR_RM_1);
	}else{
		G_lJmrStt.rebMod[0].address = 0;
		preferences.putUChar(PARAM_ADDR_RM_1, G_lJmrStt.rebMod[0].address);		
	}

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_2);
	if(keyIsExist){
		G_lJmrStt.rebMod[1].address = preferences.getUChar(PARAM_ADDR_RM_2);
	}else{
		G_lJmrStt.rebMod[1].address = 0;
		preferences.putUChar(PARAM_ADDR_RM_2,G_lJmrStt.rebMod[1].address);		
	}


	keyIsExist = preferences.isKey(PARAM_PWR_1);
	if(keyIsExist){
		G_lJmrStt.rebMod[0].pwr = preferences.getUChar(PARAM_PWR_1);
	}else{
		G_lJmrStt.rebMod[0].pwr = 0;
		preferences.putUChar(PARAM_PWR_1, G_lJmrStt.rebMod[0].pwr);		
	}

	keyIsExist = preferences.isKey(PARAM_PWR_2);
	if(keyIsExist){
		G_lJmrStt.rebMod[1].pwr = preferences.getUChar(PARAM_PWR_2);
	}else{
		G_lJmrStt.rebMod[1].pwr = 0;
		preferences.putUChar(PARAM_PWR_2, G_lJmrStt.rebMod[1].pwr);		
	}	

	keyIsExist = preferences.isKey(PARAM_DEV_ID);
	if(keyIsExist){
		G_lJmrStt.devId = preferences.getUInt(PARAM_PWR_2);
	}else{
		G_lJmrStt.devId = 0;
		preferences.getUInt(PARAM_PWR_2, G_lJmrStt.devId);		
	}
 
	preferences.end();    
}
 
static portMUX_TYPE spinlockPref = portMUX_INITIALIZER_UNLOCKED;
void AN_pref::setParam(String param, BYTE val){
    taskENTER_CRITICAL(&spinlockPref);
    preferences.begin("prefData", false);
    preferences.putUChar(param.c_str(), val);
    preferences.end();
    taskEXIT_CRITICAL(&spinlockPref);            
}

void AN_pref::setParam(String param, DWORD val){
    taskENTER_CRITICAL(&spinlockPref);
    preferences.begin("prefData", false);
    preferences.putUInt(param.c_str(), val);
    preferences.end();
    taskEXIT_CRITICAL(&spinlockPref);            
}


void AN_pref::setGroupId(BYTE groupId){
    if((groupId > 0) && (groupId < 127)){
    	G_lJmrStt.groupId = groupId;
		setParam(PARAM_GROUP_ID, groupId);
    }
}

void AN_pref::setDevId(DWORD id){
    if((id > 0) && (id < 0x7FFFFFFF)){
		G_lJmrStt.devId = id;
        setParam(PARAM_DEV_ID, id);
    }
}

void AN_pref::setDevType(BYTE type){
    BYTE tp = (type == DEV_TYPE_B) ? DEV_TYPE_B : DEV_TYPE_A;
	G_lJmrStt.devType = type;
    setParam(PARAM_DEV_TYPE, tp);
}

void AN_pref::setAddrEsp(BYTE addr){
    if((addr > 0) && (addr < 127)){
        G_lJmrStt.esp32Addr = addr;
        setParam(PARAM_ADDR_ESP, addr);
    }
}

void AN_pref::setAddrRm(BYTE addrRm1, BYTE addrRm2){
    if((addrRm1 > 0) && (addrRm1 < 127)){
        G_lJmrStt.rebMod[0].address = addrRm1;
        setParam(PARAM_ADDR_RM_1, addrRm1);
    }
    if((addrRm2 > 0) && (addrRm2 < 127)){
        G_lJmrStt.rebMod[1].address = addrRm2;
        setParam(PARAM_ADDR_RM_2, addrRm2);  
    } 
}

void AN_pref::setPwr(BYTE pwr1, BYTE pwr2){
    BYTE p1 = pwr1 == PWR_OFF ? PWR_OFF : PWR_ON;
    BYTE p2 = pwr2 == PWR_OFF ? PWR_OFF : PWR_ON;
    G_lJmrStt.rebMod[0].pwr = p1;
    G_lJmrStt.rebMod[1].pwr = p2;
    
    setParam(PARAM_PWR_1, p1);
    setParam(PARAM_PWR_2, p2);
}


