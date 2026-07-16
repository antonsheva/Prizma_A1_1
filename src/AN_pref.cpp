#include "AN_pref.h"
  
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
		G_lJmrStt.devId = preferences.getULong64(PARAM_DEV_ID);
	}else{
		G_lJmrStt.devId = 0;
		preferences.getULong64(PARAM_DEV_ID, G_lJmrStt.devId);		
	}

	keyIsExist = preferences.isKey(PARAM_DEV_TYPE);
	if(keyIsExist){
		G_lJmrStt.devType = preferences.getUChar(PARAM_DEV_TYPE);
	}else{
		G_lJmrStt.devType = 0;
		preferences.getUChar(PARAM_DEV_TYPE, G_lJmrStt.devType);		
	}	
	
	keyIsExist = preferences.isKey(PARAM_DEV_RANGE);
	if(keyIsExist){
		G_lJmrStt.devRange = preferences.getUChar(PARAM_DEV_RANGE);
	}else{
		G_lJmrStt.devRange = 0;
		preferences.getUChar(PARAM_DEV_RANGE, G_lJmrStt.devRange);		
	}	


	keyIsExist = preferences.isKey(PARAM_GROUP_ID);
	if(keyIsExist){
		G_lJmrStt.groupId = preferences.getUChar(PARAM_GROUP_ID);
	}else{
		G_lJmrStt.groupId = 0;
		preferences.getUChar(PARAM_GROUP_ID, G_lJmrStt.groupId);		
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


void AN_pref::setParam(String param, uint64_t val){
    taskENTER_CRITICAL(&spinlockPref);
    preferences.begin("prefData", false);
    preferences.putULong64(param.c_str(), val);
    preferences.end();
    taskEXIT_CRITICAL(&spinlockPref);            
}

void AN_pref::setGroupId(BYTE groupId){
    if((groupId > 0) && (groupId < 127)){
    	G_lJmrStt.groupId = groupId;
		setParam(PARAM_GROUP_ID, groupId);
    }
}

void AN_pref::setDevId(uint64_t id){
    if((id > 0) && (id < 0x7FFFFFFFFFFFFFFF)){
			G_lJmrStt.devId = id;
      setParam(PARAM_DEV_ID, id);
    }
}

void AN_pref::setDevType(BYTE type){
    BYTE tp = (type == DEV_TYPE_B) ? DEV_TYPE_B : DEV_TYPE_A;
	G_lJmrStt.devType = type;
    setParam(PARAM_DEV_TYPE, tp);
}

void AN_pref::setDevRange(BYTE range){
    if((range > 0) &&(range < 13)){
			G_lJmrStt.devRange = range;
			setParam(PARAM_DEV_RANGE, range);
		}
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


