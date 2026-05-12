#include "AN_rs485.h"



AN_rs485::AN_rs485(/* args */)
{

}

AN_rs485::~AN_rs485()
{
}

void AN_rs485::sendData(BYTE addr, BYTE cmd, BYTE *data, BYTE len)
{
    WORD crc16;
    txData[0] = addr;
    txData[1] = cmd;
    for(int i=0; i<len; i++)txData[i+2] = data[i];
    crc16 = crc.modbus(txData, len+2);
    txData[len+2] = (BYTE)crc16;
    txData[len+3] = (BYTE)(crc16>>8);
    xQueueSend(QueueRs485Out, txData, portMAX_DELAY);
}

void AN_rs485::recvData(BYTE *data, BYTE len)
{
}
