#include "AN_rebMod.h"

 

AN_rebMod::AN_rebMod(){
    queueIn  = xQueueCreate(2, 128);
    queueOut = xQueueCreate(2, 128);  
    selDev = 1;
}

AN_rebMod::~AN_rebMod()
{
}

void AN_rebMod::sendCmd(String str){
    char buff[128];
    str.toCharArray(buff,128);
    xQueueSend(queueOut, buff, portMAX_DELAY);
}

void AN_rebMod::At(){
    sendCmd("AT \n\r");
}

void AN_rebMod::getAti()
{
    sendCmd("ATI\n\r"); 
}
void AN_rebMod::getAtc()
{
    sendCmd("ATC\n\r");     
}
void AN_rebMod::setAtc()
{
    String str = "ATC="+String(devStt[selDev-1].mc)+
                 ","+String(devStt[selDev-1].mask)+"\n\r";
    sendCmd(str);
}
void AN_rebMod::getAte()
{
    sendCmd("ATE?\n\r");     
}
void AN_rebMod::setAte0()
{
    sendCmd("ATE0\n\r");
}
void AN_rebMod::setAte1()
{
    sendCmd("ATE1\n\r");
}


void AN_rebMod::getAtbt()
{
    sendCmd("ATBT\n\r");     
}
void AN_rebMod::getAtw()
{

}
void AN_rebMod::setAtw()
{
    sendCmd("AT&W\n\r");
}

void AN_rebMod::setAtz(){
    sendCmd("ATZ\n\r");
}
