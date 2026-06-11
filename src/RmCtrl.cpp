#include "RmCtrl.h"

RmCtrl* RmCtrl::instance = nullptr;

void RmCtrl::init(){
    queueIn  = xQueueCreate(2, 128);
    queueOut = xQueueCreate(2, 128);  
    selDev = 0;
}


void RmCtrl::fillDevInfoList(int paramQt, String* data){
    int infoDataQty = 0;
    int rmNum = RmCtrl::getI()->selDev;
    for(int i=0; i<paramQt; i++){
        if(data[i].indexOf(":")== -1)continue;
        // G_lJmrStt.rebMod[rmNum].info[infoDataQty] = data[i];     
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
void RmCtrl::fillDevParams(int dataArrLen, String *data){
    int tmpCnt = 0;
    int rmNum = RmCtrl::getI()->selDev;
    String paramStr = "";
 
    for(int i=0; i<dataArrLen; i++){
        if(data[i].indexOf(":")== -1)continue;

        if(data[i].indexOf("MT"  )  != -1){G_lJmrStt.rebMod[rmNum].mc    = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("SP"  )  != -1){G_lJmrStt.rebMod[rmNum].mask  = data[i].substring(3, data[i].length()).toInt();  }
        if(data[i].indexOf("VCPU")  != -1){G_lJmrStt.rebMod[rmNum].vcpu  = data[i].substring(5, data[i].length()).toFloat();}
        if(data[i].indexOf("TEMP")  != -1){G_lJmrStt.rebMod[rmNum].temp  = data[i].substring(5, data[i].length()).toFloat();}
    }      
}

void RmCtrl::getDevInfo(String data){
    int rmNum = RmCtrl::getI()->selDev;
    String strArr[24] = {""};
    String delimiter = "\r\n";
    int start = 0;
    int end = data.indexOf(delimiter);
    int strCnt = 0;
 
    String tmpStr = "";
    // Serial.println("getDevInfo 52");
    while (end != -1) {
        strArr[strCnt] = data.substring(start, end);
        start = end + delimiter.length();
        end = data.indexOf(delimiter, start);
        strCnt++;
    }
    // Serial.println("getDevInfo 60");
    if(G_opCode == CMD_GET_ATI) G_lJmrStt.rebMod[rmNum].info = data;
    else                        fillDevParams  (strCnt, strArr); 
    // Serial.println("getDevInfo 63");
}


 
void RmCtrl::receiveData(String readData){
    Serial.println ();
    Serial.println("------ REBMOD DATA ---------");
    Serial.println(readData);
    Serial.println("----------------------------");
    Serial.println ();

    getDevInfo(readData); 
 
}

void RmCtrl::sendCmd(String str){
    // char buff[128];
    // str.toCharArray(buff,128);
    // xQueueSend(queueOut, buff, portMAX_DELAY);
    
    static BYTE activeRebMod = 0; 
    if(activeRebMod != RmCtrl::getI()->selDev){
        activeRebMod = RmCtrl::getI()->selDev;
        rmRxTx = (activeRebMod == 0) ? rmSer1 : rmSer2;
        Serial1.begin(9600, SERIAL_8N1, rmRxTx.rx, rmRxTx.tx);
    }    
 
    Serial1.println(str);
    Serial. println(str);
}

void RmCtrl::At(){
    sendCmd("AT \n\r");
}

void RmCtrl::getAti()
{
    sendCmd("ATI\n\r"); 
}
void RmCtrl::getAtc()
{
    sendCmd("ATC\n\r");     
}
void RmCtrl::setAtc()
{
    String str = "ATC="+String(G_lJmrStt.rebMod[selDev].mc)+
                 ","+String(G_lJmrStt.rebMod[selDev].mask)+"\n\r";
    sendCmd(str);
}
void RmCtrl::getAte()
{
    sendCmd("ATE?\n\r");     
}
void RmCtrl::setAte0()
{
    sendCmd("ATE0\n\r");
}
void RmCtrl::setAte1()
{
    sendCmd("ATE1\n\r");
}


void RmCtrl::getAtbt()
{
    sendCmd("ATBT\n\r");     
}
void RmCtrl::getAtw()
{

}
void RmCtrl::setAtw()
{
    sendCmd("AT&W\n\r");
}

void RmCtrl::setAtz(){
    sendCmd("ATZ\n\r");
}
