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
#include "AN_macro.h"
#include "AN_pref.h"
#include "RmCtrl.h"
#include "objects.h"
#include "AN_cbFuncs.h"


#define TMP_BUFF_LEN 128

#define SRC_COM 0
#define SRC_BT  1
 

extern TaskHandle_t TaskHandle_RebMod_In;           
extern TaskHandle_t TaskHandle_RebMod_Out;            
extern TaskHandle_t TaskHandle_rs485_send;            
extern TaskHandle_t TaskHandle_txBt;           
extern TaskHandle_t TaskHandle_rxRs485;           
extern TaskHandle_t TaskHandle_eventControl;
extern TaskHandle_t TaskHandle_txRs485;      
extern TaskHandle_t TaskHandle_wtDataPack;      
extern TaskHandle_t TaskHandle_rebModAut; 
extern TaskHandle_t TaskHandle_monitor;
extern TaskHandle_t TaskHandle_pollRs485;   
extern TaskHandle_t TaskHandle_wait485Resp; 
extern TaskHandle_t TaskHandle_pwrButton;
extern TaskHandle_t TaskHandle_pwrAut;
extern TaskHandle_t TaskHandle_leds;

#endif