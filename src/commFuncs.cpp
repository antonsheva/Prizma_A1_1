#include "commFuncs.h"

void sendCmdToRM_WithoutParam(String str){
    char buff[128];
    str.toCharArray(buff,128);
    xQueueSend(QueueRebModOut, buff, portMAX_DELAY);
}

void getDevInfo(int devNum, String data){
    String strArr[24] = {""};
    String delimiter = "\r\n";
    int start = 0;
    int end = data.indexOf(delimiter);
    int strCnt = 0;
    int tmpCnt = 0;
    String tmpStr = "";

    while (end != -1) {
        strArr[strCnt] = data.substring(start, end);
        start = end + delimiter.length();
        end = data.indexOf(delimiter, start);
        strCnt++;
  
    }

    for(int i=0; i<strCnt; i++){
        

        if(strArr[i].indexOf(":")== -1)continue;
        if(devNum==1)devStt1.info[tmpCnt] = strArr[i];     
        if(devNum==2)devStt2.info[tmpCnt] = strArr[i];     
        tmpCnt++; 
    }

    if(devNum==2){
       
        for(int i=0; i<tmpCnt; i++){
            Serial.println(devStt1.info[i]);
        }
        Serial.println("-------------------------");
        Serial.println("");   
        for(int i=0; i<tmpCnt; i++){
            Serial.println(devStt2.info[i]);
        }
        Serial.println("-------------------------");
        Serial.println("");  
    }

      
}









