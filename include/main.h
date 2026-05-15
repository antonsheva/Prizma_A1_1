#ifndef _AN_MAIN_
#define _AN_MAIN_


#include "Arduino.h"
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include "AN_cmd.h"
#include "define_types.h"
#include "tasks.h"
#include "commFuncs.h"
 
#include "RebModCntrl.h"
#include "objects.h"
#include "AN_cbFuncs.h"


#define TMP_BUFF_LEN 128

#define SRC_COM 0
#define SRC_BT  1

extern char G_tmpBuff[TMP_BUFF_LEN];
extern BluetoothSerial SerialBT; 
extern TaskHandle_t TaskHandle_RebMod_In;           
extern TaskHandle_t TaskHandle_RebMod_Out;            
extern TaskHandle_t TaskHandle_BT_In;            
extern TaskHandle_t TaskHandle_BT_Out;           
extern TaskHandle_t TaskHandle_Serial_In;           
extern TaskHandle_t TaskHandle_Serial_Out;
extern TaskHandle_t TaskHandle_RS485_In;      
extern TaskHandle_t TaskHandle_RS485_Out;      
extern TaskHandle_t TaskHandle_automat; 
extern TaskHandle_t TaskHandle_init; 
 


#endif