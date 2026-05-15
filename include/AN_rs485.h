#ifndef _AN_RS485_
#define _AN_RS485_
#include "define_types.h"
#include "FastCRC.h"
#include "Arduino.h"
#include "objects.h"
#include "commFuncs.h"
#include "AN_jammAddr.h"

class AN_rs485{
private:
    static AN_rs485* instance;
    AN_rs485(){};
    ~AN_rs485(){instance = nullptr;}
    AN_rs485(const AN_rs485&) = delete;
    AN_rs485& operator=(const AN_rs485&) = delete;

    BYTE txData[128];
    BYTE rxData[128];
    bool status; // master/slave    

  	BYTE localAddrEsp32;
	BYTE localAddrRm1;
	BYTE localAddrRm2;  


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

    void sendData(BYTE addr, BYTE cmd, BYTE *data, BYTE len);
    void recvData(BYTE *data, size_t len);
};



#endif