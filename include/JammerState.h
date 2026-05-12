#ifndef _REMOMOD_STATE_
#define _REMOMOD_STATE_
#include "define_types.h"
#include "arduino.h"

class JammerState{
private:
    typedef struct{
        BYTE   devNum;
        BYTE   mc;
        DWORD  mask;
        BYTE   address;
        BYTE   echo;
        float  vcpu;
        float  temp;
        String info[16];
        BYTE   infoDataQty; 
    }struct_rebMod;



public:
    JammerState(/* args */);
    ~JammerState();
    BYTE esp32Addr;

    struct_rebMod rebMod[2];
};











#endif
