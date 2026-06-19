#ifndef _OBJECTS_
#define _OBJECTS_

#include <vector> 
#include <BluetoothSerial.h>

#include "Preferences.h"
#include "JammerState.h"

 
#define SERIAL_BUFF_LEN 128



typedef struct{
    BYTE dataLen = 0;
    BYTE data[1024];
    WORD marker = 0; 
    int  packQty;
    int  lastPackLen;
}_RS485_data;
 
 
typedef struct{
    BYTE    addrEsp32     = 0;
    BYTE    rmNum         = 0;
    BYTE    direction     = 0;
    BYTE    cmd           = 0;
    BYTE    response      = 0; 
    BYTE    modCode       = 0;  
    BYTE    modCode1      = 0;
    BYTE    modCode2      = 0;
    BYTE    sender        = 0;
    // BYTE    addressee     = 0;
    BYTE    addrRm1       = 0;
    BYTE    addrRm2       = 0;
    BYTE    pwr1          = 0;
    BYTE    pwr2          = 0;  
    BYTE    jmmrListLen   = 0;
    WORD    txtDataLen    = 0;
    DWORD   mask1         = 0;
    DWORD   mask2         = 0;
    DWORD   mask          = 0;
    String  txtData       = "";
}_MSG_PACK;

typedef struct{
    BYTE esp32;
    BYTE rm1;
    BYTE rm2;
}_ADDRESSESS;

typedef struct{
    BYTE opCodeList [16];
    BYTE opCodeQty   = 0;
    bool swtchActDev = 0;
}_RM_AUT;

extern BluetoothSerial SerialBT; 

extern std::vector<JammerState> G_jmrsList;
 
extern _MSG_PACK G_rm_msg;
extern _MSG_PACK G_485_msg;
 

// extern _ADDRESSESS G_localAddresses;
extern JammerState G_lJmrStt;
extern Preferences preferences;

extern QueueHandle_t QueueSerialOut     ;   
extern QueueHandle_t QueueBtTransmit    ;
extern QueueHandle_t QueueRs485Transmit ;
extern QueueHandle_t QueueRs485Send     ;
extern QueueHandle_t QueueRs485Receive  ;
extern QueueHandle_t QueuePreferences   ;
extern QueueHandle_t QueuePwrAut        ;
extern QueueHandle_t QueueRebModAut     ;
extern QueueHandle_t QueueLeds          ;
  
 




extern bool G_updatePref; 
extern bool G_led_ccl_5;
extern bool G_btConnect;
extern bool G_pwrMode;

extern BYTE G_waitResponse;
extern DWORD G_wait485Cnt;
extern DWORD G_pauseRmDataCnt;   
extern DWORD G_waitBtConnect;

extern int G_rebModAut_tm;
extern int G_rs485IterNum;
extern int G_msgTxtDataLen;

extern String G_msgTxtData;
 
extern bool G_eventExpected [32];
extern bool G_eventOccurred[32];


void initObjects();




#endif

