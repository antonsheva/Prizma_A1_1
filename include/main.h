#ifndef _AN_MAIN_
#define _AN_MAIN_


#include "Arduino.h"

#include <ArduinoJson.h>
#include "AN_cmd.h"
#include "define_types.h"
#include "tasks.h"
#include "commFuncs.h"
#include "define.h" 
#include "AN_json.h"

#include "RmCtrl.h"
#include "objects.h"
#include "AN_cbFuncs.h"


#define TMP_BUFF_LEN 128

#define SRC_COM 0
#define SRC_BT  1
 

extern TaskHandle_t TaskHandle_RebMod_In;           
extern TaskHandle_t TaskHandle_RebMod_Out;            
extern TaskHandle_t TaskHandle_BT_In;            
extern TaskHandle_t TaskHandle_BT_Out;           
extern TaskHandle_t TaskHandle_Serial_In;           
extern TaskHandle_t TaskHandle_eventControl;
extern TaskHandle_t TaskHandle_transmitRs485;      
extern TaskHandle_t TaskHandle_wtDataPack;      
extern TaskHandle_t TaskHandle_rebModAut; 
extern TaskHandle_t TaskHandle_init;
extern TaskHandle_t TaskHandle_connLevel_up;   
extern TaskHandle_t TaskHandle_wait485Resp; 


#endif