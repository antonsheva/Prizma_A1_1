#ifndef _AN_RS485_
#define _AN_RS485_
#include "define_types.h"
#include "FastCRC.h"
#include "Arduino.h"
#include "objects.h"
class AN_rs485{
private:
    BYTE txData[128];
    BYTE rxData[128];
public:
    FastCRC16 crc;
    AN_rs485(/* args */);
    ~AN_rs485();

    void sendData(BYTE addr, BYTE cmd, BYTE* data, BYTE len);
    void recvData(BYTE* data, BYTE len);

};

#endif