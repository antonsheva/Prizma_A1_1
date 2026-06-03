#include "commFuncs.h"

void AErrorLog(String error){
    Serial.println(error);
}

void ADebugLog(String msg){
    Serial.println(" ");
    Serial.println("----- DEBUG INFO  ----------");
    Serial.println(msg);
    Serial.println(" ");

}
  
void initLocalAddresses(){
    bool keyIsExist;

	preferences.begin("prefAddres", false);
	keyIsExist = preferences.isKey(PARAM_ADDRESSEE);
	if(keyIsExist){
		G_lJmrStt.esp32Addr = preferences.getUChar(PARAM_ADDRESSEE);
	}else{
		G_lJmrStt.esp32Addr = 0;
		preferences.putUChar(PARAM_ADDRESSEE, G_lJmrStt.esp32Addr);		
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

void haveNewRebModData(String readData){
    Serial.println ();
    Serial.println("------ REBMOD DATA ---------");
    Serial.println(readData);
    Serial.println("----------------------------");
    Serial.println ();

    getDevInfo(readData); 
    unblockTasks(); 
}

// void fillDevInfoList(int paramQt, String* data){
//     int infoDataQty = 0;
//     int rmNum = RmCtrl::getI()->selDev;
//     for(int i=0; i<paramQt; i++){
//         if(data[i].indexOf(":")== -1)continue;
//         G_lJmrStt.rebMod[rmNum].info[infoDataQty] = data[i];     
//         infoDataQty++;  
//     }  
//     G_lJmrStt.rebMod[rmNum].infoDataQty = infoDataQty;
// }

// /**
//  * @brief search parameters in "dirty" data
//  * 
//  * @param dataArrLen  "dirty" data array length
//  * @param data        string's array with "dirty" data
//  */
// void fillDevParams(int dataArrLen, String *data){
//     int tmpCnt = 0;
//     int rmNum = RmCtrl::getI()->selDev;
//     String paramStr = "";
 
//     for(int i=0; i<dataArrLen; i++){
//         if(data[i].indexOf(":")== -1)continue;

//         if(data[i].indexOf("MT"  )  != -1){G_lJmrStt.rebMod[rmNum].mc    = data[i].substring(3, data[i].length()).toInt();  }
//         if(data[i].indexOf("SP"  )  != -1){G_lJmrStt.rebMod[rmNum].mask  = data[i].substring(3, data[i].length()).toInt();  }
//         if(data[i].indexOf("VCPU")  != -1){G_lJmrStt.rebMod[rmNum].vcpu  = data[i].substring(5, data[i].length()).toFloat();}
//         if(data[i].indexOf("TEMP")  != -1){G_lJmrStt.rebMod[rmNum].temp  = data[i].substring(5, data[i].length()).toFloat();}
//     }      
// }

// void getDevInfo(String data){
//     String strArr[24] = {""};
//     String delimiter = "\r\n";
//     int start = 0;
//     int end = data.indexOf(delimiter);
//     int strCnt = 0;
 
//     String tmpStr = "";

//     while (end != -1) {
//         strArr[strCnt] = data.substring(start, end);
//         start = end + delimiter.length();
//         end = data.indexOf(delimiter, start);
//         strCnt++;
//     }

//     if(G_opCode == CMD_GET_ATI) fillDevInfoList(strCnt, strArr);
//     else                        fillDevParams  (strCnt, strArr); 

// }









