#ifndef _REMOMOD_STATE_
#define _REMOMOD_STATE_
#include "define_types.h"
#include "arduino.h"

class JammerState{
private:
    typedef struct{
        BYTE   mc;
        DWORD  mask;
        BYTE   address;
        BYTE   echo;
        BYTE   pwr;
        float  vcpu;
        float  temp;
        String info;
        BYTE   infoDataQty; 
    }struct_rebMod;
public:
    JammerState(/* args */);
    ~JammerState();
    DDWORD  devId;      
    BYTE    groupId;
    BYTE    devType;   // 1 - A; 2 - B
    BYTE    devRange;
    BYTE    esp32Addr;
    String  info;

    struct_rebMod rebMod[2];
};











#endif
