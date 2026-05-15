#ifndef _REBMODE_CNTRL_
#define _REBMODE_CNTRL_

#include "Arduino.h" 
#include "define_types.h"
#include "objects.h"


class RebModCntrl{
private:
    static RebModCntrl* instance;
	RebModCntrl(){};
    ~RebModCntrl(){instance = nullptr;}
    RebModCntrl(const RebModCntrl&) = delete;
    RebModCntrl& operator=(const RebModCntrl&) = delete;
 

    typedef struct {
        BYTE rx;
        BYTE tx;
    }struct_rebModSerial;


public:
    static RebModCntrl* getI(){
        if(instance == nullptr){
            instance = new RebModCntrl();
        }
        return instance;
    }

    BYTE selDev = 0;
 

    QueueHandle_t queueIn   = NULL;   
    QueueHandle_t queueOut  = NULL;   


 

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
    
    void init();

};
 

#endif


