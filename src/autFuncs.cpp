#include "autFuncs.h"

void autFuncs_getState(int devNum)
{
    G_activeRebMod = devNum;
    sendCmdToRM_WithoutParam("ATI\n\r"); 
}

void autFuncs_getAtc(int devNum)
{
    G_activeRebMod = devNum;
    sendCmdToRM_WithoutParam("ATC\n\r");     
}

void autFuncs_setAtc(int devNum)
{
}

void autFuncs_getAtbt(int devNum)
{
}

void autFuncs_getAtw(int devNum)
{
}

void autFuncs_setAtw(int devNum)
{
}
