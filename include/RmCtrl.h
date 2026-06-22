#ifndef _REBMODE_CNTRL_
#define _REBMODE_CNTRL_

#include "Arduino.h" 
#include "define_types.h"
#include "objects.h"
#include "define.h"


class RmCtrl{
private:
    static RmCtrl* instance;
	RmCtrl(){};
    ~RmCtrl(){instance = nullptr;}
    RmCtrl(const RmCtrl&) = delete;
    RmCtrl& operator=(const RmCtrl&) = delete;
 

    typedef struct {
        BYTE rx;
        BYTE tx;
    }struct_rebModSerial;



public:
    static RmCtrl* getI(){
        if(instance == nullptr){
            instance = new RmCtrl();
        }
        return instance;
    }
  
    bool isBusy = false;
    BYTE selDev = 0;
    BYTE cmdAfterAutFinish = 0;

    char inData[RM_BUFF_LEN] = {0};

    struct_rebModSerial rmSer1;
    struct_rebModSerial rmSer2;
    struct_rebModSerial rmRxTx; 

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
    void receiveData(String readData);
    void sendCmd(String str);

    void init();

    void fillDevInfoList(int paramQt, String *data);
    void fillDevParams(int dataArrLen, String *data);
    void getDevInfo(String data);
};

#endif


