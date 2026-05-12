#include "commFuncs.h"


 
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



void haveNewRebModData(String readData){
    getDevInfo(readData); 
    unblockTasks(); 
}



void fillDevInfoList(int paramQt, String* data){
    int infoDataQty = 0;
    int devNum = cCmd.cRebMod->selDev-1;
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        jmrStt->rebMod[devNum].info[infoDataQty] = data[i];     
        infoDataQty++;  
    }  
    jmrStt->rebMod[devNum].infoDataQty = infoDataQty;
}


/**
 * @brief search parameters in "dirty" data
 * 
 * @param dataArrLen  "dirty" data array length
 * @param data        string's array with "dirty" data
 */
void fillDevParams(int dataArrLen, String *data){
    int tmpCnt = 0;
    int devNum = cCmd.cRebMod->selDev-1;
    String paramStr = "";
 
    for(int i=0; i<dataArrLen; i++){
        if(data[i].indexOf(":")== -1)continue;

        if(data[i].indexOf("MT"  )  != -1){jmrStt->rebMod[devNum].mc    = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("SP"  )  != -1){jmrStt->rebMod[devNum].mask  = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("VCPU")  != -1){jmrStt->rebMod[devNum].vcpu  = data[i].substring(5, data[i].length()).toFloat();}
        if(data[i].indexOf("TEMP")  != -1){jmrStt->rebMod[devNum].temp  = data[i].substring(5, data[i].length()).toFloat();}
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









