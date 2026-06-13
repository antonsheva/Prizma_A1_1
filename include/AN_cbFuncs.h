#ifndef _AN_CB_FUNCS_
#define _AN_CB_FUNCS_
#include "Arduino.h"
#include "commFuncs.h"
#include "AN_cmd.h"
#include "AN_rs485.h"
 

class AN_cbFuncs{
private:
    static AN_cbFuncs* instance;
	AN_cbFuncs(){};
    ~AN_cbFuncs(){instance = nullptr;}
    AN_cbFuncs(const AN_cbFuncs&) = delete;
    AN_cbFuncs& operator=(const AN_cbFuncs&) = delete;
 

  
    static void processingSerialData(char *data, int src);
    
public:
    static AN_cbFuncs* getI(){
        if(instance == nullptr){
            instance = new AN_cbFuncs();
        }
        return instance;
    }



    static void uartBt(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    static void uartUsb();
    static void uartRm();
    static void uart485();

};






#endif