#include "RebModCntrl.h"

RebModCntrl* RebModCntrl::instance = nullptr;

void RebModCntrl::init(){
    queueIn  = xQueueCreate(2, 128);
    queueOut = xQueueCreate(2, 128);  
    selDev = 0;
}

 
void RebModCntrl::sendCmd(String str){
    char buff[128];
    str.toCharArray(buff,128);
    xQueueSend(queueOut, buff, portMAX_DELAY);
}

void RebModCntrl::At(){
    sendCmd("AT \n\r");
}

void RebModCntrl::getAti()
{
    sendCmd("ATI\n\r"); 
}
void RebModCntrl::getAtc()
{
    sendCmd("ATC\n\r");     
}
void RebModCntrl::setAtc()
{
    String str = "ATC="+String(G_lJmrStt.rebMod[selDev].mc)+
                 ","+String(G_lJmrStt.rebMod[selDev].mask)+"\n\r";
    sendCmd(str);
}
void RebModCntrl::getAte()
{
    sendCmd("ATE?\n\r");     
}
void RebModCntrl::setAte0()
{
    sendCmd("ATE0\n\r");
}
void RebModCntrl::setAte1()
{
    sendCmd("ATE1\n\r");
}


void RebModCntrl::getAtbt()
{
    sendCmd("ATBT\n\r");     
}
void RebModCntrl::getAtw()
{

}
void RebModCntrl::setAtw()
{
    sendCmd("AT&W\n\r");
}

void RebModCntrl::setAtz(){
    sendCmd("ATZ\n\r");
}
