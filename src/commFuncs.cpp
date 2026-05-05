#include "commFuncs.h"


void checkWaitResponse(String readData){
    switch(G_autCode){
        case AUT_CODE_GET_STT_1:getDevInfo(1, readData);
                                G_autCode = AUT_CODE_GET_STT_2;  
            break;
        case AUT_CODE_GET_STT_2:getDevInfo(2, readData); 
                                G_autCode = AUT_CODE_GET_ATC_1;
            break;
        case AUT_CODE_GET_ATC_1:getDevInfo(1, readData); 
                                G_autCode = AUT_CODE_GET_ATC_2;
            break;
        case AUT_CODE_GET_ATC_2:getDevInfo(2, readData); 
                                // G_autCode = AUT_CODE_GET_ATC_1;
            break;
            
    }  
    G_waitResponse--;  
    if(G_waitResponse)xSemaphoreGive(SemaphoreTaskAutomat);
}

void sendCmdToRM_WithoutParam(String str){
    char buff[128];
    str.toCharArray(buff,128);
    xQueueSend(QueueRebModOut, buff, portMAX_DELAY);
}

void fillDevInfoList(int devNum, int paramQt, String* data){
    int tmpCnt = 0;
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        devStt[devNum-1].info[tmpCnt] = data[i];     
        // if(devNum==1)devStt1.info[tmpCnt] = data[i];     
        // if(devNum==2)devStt2.info[tmpCnt] = data[i];     
        tmpCnt++;  
    }  
    
    if(devNum==2){
        Serial.println("");     
        Serial.println("-------------------------");         
        for(int k=0;k<2;k++){
            for(int i=0; i<tmpCnt; i++){
                Serial.println(devStt[k].info[i]);
            }   
            Serial.println("");     
            Serial.println("-------------------------");                        
        }
 
    }
}

void fillDevAtc(int devNum, int paramQt, String* data){
    int tmpCnt = 0;
    String paramStr = "";
 
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        if(data[i].indexOf("MT") != -1){
            devStt[devNum-1].modulationCode = data[i].substring(3, data[i].length()).toInt();
        }
        if(data[i].indexOf("SP") != -1){
            devStt[devNum-1].mask  = data[i].substring(3, data[i].length()).toInt();
        }
    }   
   

        Serial.println("");     
        Serial.println("-------------------------");  
        Serial.println("SP -> "+String(devStt[devNum-1].mask));
        Serial.println("MT -> "+String(devStt[devNum-1].modulationCode));
        
}

void getDevInfo(int devNum, String data){
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

    switch(G_autCode){
        case AUT_CODE_GET_STT_1: fillDevInfoList(ACTIVE_REBMOD_1, strCnt, strArr); break;
        case AUT_CODE_GET_STT_2: fillDevInfoList(ACTIVE_REBMOD_2, strCnt, strArr); break;
        case AUT_CODE_GET_ATC_1: fillDevAtc     (ACTIVE_REBMOD_1, strCnt, strArr); break;     
        case AUT_CODE_GET_ATC_2: fillDevAtc     (ACTIVE_REBMOD_2, strCnt, strArr); break;     


    }
    


      
}









