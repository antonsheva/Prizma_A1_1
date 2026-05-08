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

void unblockTasks(){
    if(G_waitUnblockTasks[SuspendTask_init      ])vTaskResume(TaskHandle_init      );
    if(G_waitUnblockTasks[SuspendTask_RebMod_In ])vTaskResume(TaskHandle_RebMod_In );
    if(G_waitUnblockTasks[SuspendTask_RebMod_Out])vTaskResume(TaskHandle_RebMod_Out);
    if(G_waitUnblockTasks[SuspendTask_BT_In     ])vTaskResume(TaskHandle_BT_In     );
    if(G_waitUnblockTasks[SuspendTask_BT_Out    ])vTaskResume(TaskHandle_BT_Out    );
    if(G_waitUnblockTasks[SuspendTask_Serial_In ])vTaskResume(TaskHandle_Serial_In );
    if(G_waitUnblockTasks[SuspendTask_Serial_Out])vTaskResume(TaskHandle_Serial_Out);
    if(G_waitUnblockTasks[SuspendTask_RS485_In  ])vTaskResume(TaskHandle_RS485_In  );
    if(G_waitUnblockTasks[SuspendTask_RS485_Out ])vTaskResume(TaskHandle_RS485_Out );
    if(G_waitUnblockTasks[SuspendTask_automat   ])vTaskResume(TaskHandle_automat   );  
    for(int i=0; i<10; i++)G_waitUnblockTasks[i] = false;  
}

void isNewRebModData(String readData){
    getDevInfo(readData); 
    unblockTasks(); 
}



void fillDevInfoList(int paramQt, String* data){
    int tmpCnt = 0;
    int devNum = cCmd.cRebMod->selDev-1;
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        cCmd.cRebMod->devStt[devNum].info[tmpCnt] = data[i];      
        tmpCnt++;  
    }  
}


/**
 * @brief search parameters in "dirty" data
 * 
 * @param devNum      RebMod device number in jammer (1 or 2)
 * @param dataArrLen  "dirty" data array length
 * @param data        string's array with "dirty" data
 */
void fillDevParams(int dataArrLen, String *data){
    int tmpCnt = 0;
    int devNum = cCmd.cRebMod->selDev-1;
    String paramStr = "";
 
    for(int i=0; i<dataArrLen; i++){
        if(data[i].indexOf(":")== -1)continue;

        if(data[i].indexOf("MT"  )  != -1){cCmd.cRebMod->devStt[devNum].mc    = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("SP"  )  != -1){cCmd.cRebMod->devStt[devNum].mask  = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("VCPU")  != -1){cCmd.cRebMod->devStt[devNum].vcpu  = data[i].substring(5, data[i].length()).toFloat();}
        if(data[i].indexOf("TEMP")  != -1){cCmd.cRebMod->devStt[devNum].temp  = data[i].substring(5, data[i].length()).toFloat();}
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









