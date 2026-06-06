#include "JammerState.h"

JammerState::JammerState(/* args */)
{
    for(int rm=0; rm<2; rm++){
        rebMod[rm].mc = 0;
        rebMod[rm].mask = 0;
        rebMod[rm].address = 0;
        rebMod[rm].echo = 0;
        rebMod[rm].vcpu = 0;
        rebMod[rm].temp = 0;
        rebMod[rm].infoDataQty = 0;
        rebMod[rm].info = "";        
    }
}

JammerState::~JammerState()
{
}




