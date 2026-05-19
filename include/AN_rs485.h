#ifndef _AN_RS485_
#define _AN_RS485_
#include "define_types.h"
#include "FastCRC.h"
#include "Arduino.h"
#include "objects.h"
#include "commFuncs.h"
#include "AN_jammAddr.h"

#define BROADCAST_ADDR 0xFF

#define CMD_RS485_SET_ATC      1
#define CMD_RS485_SET_ADDR_RM1 2
#define CMD_RS485_SET_ADDR_RM2 3
#define CMD_RS485_RESET        4
#define CMD_RS485_GET_ATC      5
#define CMD_RS485_GET_ATI      6




class AN_rs485{
private:
    static AN_rs485* instance;
    AN_rs485(){};
    ~AN_rs485(){instance = nullptr;}
    AN_rs485(const AN_rs485&) = delete;
    AN_rs485& operator=(const AN_rs485&) = delete;

    bool status; // master/slave    
 

     
    void processMsg(_MSG_PACK *msg);
    BYTE dataPackaging(BYTE addrDev, _MSG_PACK *msg, _RS485_data *qData);

    void dataUnpackaging(BYTE *data, _MSG_PACK *msg);

public:
    AN_jammAddr addr;
    
    static AN_rs485* getI(){
        if(instance == nullptr){
            instance = new AN_rs485();
        }
        return instance;
    }




    FastCRC16 crc;

    void init();
    void sendData(BYTE addr, _MSG_PACK *msg);
    void recvData(BYTE *data, size_t len);
};



#endif