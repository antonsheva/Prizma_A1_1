#ifndef _AN_JSON_
#define _AN_JSON_
#include "Arduino.h"
#include <ArduinoJson.h>
#include "FastCRC.h"
#include "objects.h"
#include "define.h"

class AN_json{
private:
    static AN_json* instance;
    AN_json(){};
    ~AN_json(){instance = nullptr;}
    AN_json(const AN_json&) = delete;
    AN_json& operator=(const AN_json&) = delete;
   
   
   const char* txtData; 
   JsonDocument jsonObj;
   FastCRC16 crc;
public:
    static AN_json* getI(){
        if(instance == nullptr){
            instance = new AN_json();
        }
        return instance;
    }
    

 

    void printJmmrList(int src);
    int unpackData(String data, _MSG_PACK *msg);
    int packData(_MSG_PACK *msg, char *data);
};

#endif





