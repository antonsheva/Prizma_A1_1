#ifndef _AN_RS485_
#define _AN_RS485_
#include "define_types.h"
#include "FastCRC.h"
#include "Arduino.h"
#include "objects.h"
#include "commFuncs.h"
#include "AN_jammAddr.h"
#include "AN_shiftDataArr.h"

#define BROADCAST_ADDR 0xFF
#define DEV_STATUS_MASTER 1
#define DEV_STATUS_SLAVE  0



#define CMD_RS485_SET_ATC      1
#define CMD_RS485_SET_ADDR_RM1 2
#define CMD_RS485_SET_ADDR_RM2 3
#define CMD_RS485_RESET        4
#define CMD_RS485_GET_ATC      5
#define CMD_RS485_GET_ATI      6

#define RS485_TMP_BUFF_SIZE 512


class AN_rs485 : public AN_shiftDataArr{
private:
    static AN_rs485* instance;
    AN_rs485(){};
    ~AN_rs485(){instance = nullptr;}
    AN_rs485(const AN_rs485&) = delete;
    AN_rs485& operator=(const AN_rs485&) = delete;

    bool status; // master/slave
    char tmpDataBuff[1024]; 
    _RS485_data msg485;
    JsonDocument doc;
 

    void processMsg(_MSG_PACK *msg);
    void sendJammListToBt();
 
    void sendBtData(String data);
    void sendBtResponse(BYTE cmd, uint32_t resp);
    void sendBtJmmrData(_MSG_PACK *msg);
    char tmpBuff[RS485_TMP_BUFF_SIZE];

public:
    
        
    static AN_rs485* getI(){
        if(instance == nullptr){
            instance = new AN_rs485();
        }
        return instance;
    }

    BYTE subscribersQty         = 0;
    BYTE foundSubscribersQty    = 0;
    BYTE cmdType                = 0;
    BYTE devStsus               = 0;             // master - 1; slave 0 
    int  receiveDataPacks       = 0; 
    int  waitTimer              = 0;
    int  endOfDataPacks         = 0;
    int  dataSrc                = 0;
    char serialData [1024]      = "\0"; 

    String dataPackStr = "";
    FastCRC16 crc;
 
    void sendMsgToBt(_MSG_PACK *msg); 
    void prepMsg(_MSG_PACK *msg, BYTE iterNum);



    void init();
    void resetDataPackProcess(String comment);
    void processReceivedData();
    int  checkCrcJson();
    int  concatMsgPacks(String str);
};



#endif