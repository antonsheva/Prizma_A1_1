#ifndef _REBMODE_CNTRL_
#define _REBMODE_CNTRL_

#include "Arduino.h" 
#include "define_types.h"
#include "objects.h"


class RebModCntrl{
private:

    typedef struct {
        BYTE rx;
        BYTE tx;
    }struct_rebModSerial;

    // typedef struct{
    //     BYTE   devNum;
    //     BYTE   mc;
    //     DWORD  mask;
    //     BYTE   address;
    //     BYTE   echo;
    //     float  vcpu;
    //     float  temp;
    //     String info[32];
    //     BYTE   infoDataQty; 
    // }struct_devState;


public:
    BYTE selDev = 0;
    // struct_devState devStt[2];
    // struct_rebModSerial sreRxTx, ser1, ser2;

    QueueHandle_t queueIn   = NULL;   
    QueueHandle_t queueOut  = NULL;   

    RebModCntrl();
    ~RebModCntrl();




    void At();
    void getAti();
    void getAtc();
    void setAtc();
    void getAte();
    void setAte0();
    void setAte1();
    void getAtbt();
    void getAtw();
    void setAtw();
    void setAtz();
    void sendCmd(String str);
};
 

#endif


