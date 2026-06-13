#include "commFuncs.h"

void AErrorLog(String error){
    Serial.println(error);
}

void ADebugLog(String msg){
    Serial.println(" ");
    Serial.println(msg);
  

}
  
void initLocalAddresses(){
    bool keyIsExist;

	preferences.begin("prefAddres", false);
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

	preferences.end();
}

 

 









