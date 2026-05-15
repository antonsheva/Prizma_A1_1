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



    
public:
    static AN_cbFuncs* getI(){
        if(instance == nullptr){
            instance = new AN_cbFuncs();
        }
        return instance;
    }
 
   static void uart0Rx();
   static void uart1Rx();
   static void uart2Rx();

};






#endif