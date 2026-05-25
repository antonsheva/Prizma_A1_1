#include "commFuncs.h"

void AErrorLog(String error){
    Serial.println(error);
}

void ADebugLog(String msg){
    Serial.println(msg);
}
 
void AReadSerialData(int src){
    int cnt = 0;
    for(int i=0; i<TMP_BUFF_LEN;i++)
        G_tmpBuff[i]=0;

    if(src == SRC_COM)
        while (Serial.available())
        G_tmpBuff[cnt++] = Serial.read();
    
    if(src == SRC_BT)    
        while (SerialBT.available())
        G_tmpBuff[cnt++] = SerialBT.read();
}

void getLocalAddresses(_ADDRESSESS *addr){
    bool keyIsExist;

	preferences.begin("prefAddres", false);
	keyIsExist = preferences.isKey(PARAM_ADDRESSEE);
	if(keyIsExist){
		addr->esp32 = preferences.getUChar(PARAM_ADDRESSEE);
	}else{
		addr->esp32 = 0;
		preferences.putUChar(PARAM_ADDRESSEE, addr->esp32);		
	}

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_1);
	if(keyIsExist){
		addr->rm1 = preferences.getUChar(PARAM_ADDR_RM_1);
	}else{
		addr->rm1 = 0;
		preferences.putUChar(PARAM_ADDR_RM_1, addr->rm1);		
	}

	keyIsExist = preferences.isKey(PARAM_ADDR_RM_2);
	if(keyIsExist){
		addr->rm2 = preferences.getUChar(PARAM_ADDR_RM_2);
	}else{
		addr->rm2 = 0;
		preferences.putUChar(PARAM_ADDR_RM_2, addr->rm2);		
	}

	preferences.end();


}

void haveNewRebModData(String readData){
    getDevInfo(readData); 
    unblockTasks(); 
}



void fillDevInfoList(int paramQt, String* data){
    int infoDataQty = 0;
    int rmNum = RebModCntrl::getI()->selDev;
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        G_lJmrStt.rebMod[rmNum].info[infoDataQty] = data[i];     
        infoDataQty++;  
    }  
    G_lJmrStt.rebMod[rmNum].infoDataQty = infoDataQty;
}


/**
 * @brief search parameters in "dirty" data
 * 
 * @param dataArrLen  "dirty" data array length
 * @param data        string's array with "dirty" data
 */
void fillDevParams(int dataArrLen, String *data){
    int tmpCnt = 0;
    int rmNum = RebModCntrl::getI()->selDev;
    String paramStr = "";
 
    for(int i=0; i<dataArrLen; i++){
        if(data[i].indexOf(":")== -1)continue;

        if(data[i].indexOf("MT"  )  != -1){G_lJmrStt.rebMod[rmNum].mc    = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("SP"  )  != -1){G_lJmrStt.rebMod[rmNum].mask  = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("VCPU")  != -1){G_lJmrStt.rebMod[rmNum].vcpu  = data[i].substring(5, data[i].length()).toFloat();}
        if(data[i].indexOf("TEMP")  != -1){G_lJmrStt.rebMod[rmNum].temp  = data[i].substring(5, data[i].length()).toFloat();}
    }      
}


void getDevInfo(String data){
    String strArr[24] = {""};
    String delimiter = "\r\n";
    int start = 0;
    int end = data.indexOf(delimiter);
    int strCnt = 0;
 
    String tmpStr = "";

    while (end != -1) {
        strArr[strCnt] = data.substring(start, end);
        start = end + delimiter.length();
        end = data.indexOf(delimiter, start);
        strCnt++;
    }

    if(G_opCode == CMD_GET_ATI) fillDevInfoList(strCnt, strArr);
    else                        fillDevParams  (strCnt, strArr); 

}









