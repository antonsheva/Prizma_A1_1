#ifndef _AN_PREFERENCE_
#define _AN_PREFERENCE_

#include "main.h"

class AN_pref
{
private:
    static AN_pref* instance;
	AN_pref(){};
    ~AN_pref(){instance = nullptr;}
    AN_pref(const AN_pref&) = delete;
    AN_pref& operator=(const AN_pref&) = delete; 



    void setParam(String param, BYTE val);
    void setParam(String param, DWORD val);


public:
    static AN_pref* getI(){
        if(instance == nullptr){
            instance = new AN_pref();
        }
        return instance;
    }

    void init();
    void setGroupId(BYTE groupId);
    void setDevId(DWORD id);
    void setDevType(BYTE type);
    void setAddrEsp(BYTE addr);
    void setAddrRm(BYTE addrRm1, BYTE addrRm2);
    void setPwr(BYTE pwr1, BYTE pwr2);

    DWORD getParam(BYTE param);
};

#endif


