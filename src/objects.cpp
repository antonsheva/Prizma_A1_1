#include "objects.h"

JammerState G_lJmrStt;
Preferences preferences;  
BluetoothSerial SerialBT; 

QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtTransmit = NULL;

QueueHandle_t QueueRs485Transmit = NULL;
QueueHandle_t QueueRs485Send     = NULL;
QueueHandle_t QueueRs485Receive  = NULL;
QueueHandle_t QueuePreferences   = NULL;
QueueHandle_t QueueRebModAut     = NULL;
QueueHandle_t QueueLeds          = NULL;
QueueHandle_t QueuePwrAut        = NULL;   

std::vector<JammerState> G_jmrsList;

_RS485_data RS485_data;


_MSG_PACK G_rm_msg;
_MSG_PACK G_485_msg;
 
BYTE G_waitResponse = 0;

DWORD G_pauseRmDataCnt = 0;
DWORD G_wait485Cnt  = 0;
DWORD G_waitBtConnect  = 0;
 
bool G_updatePref   = false;
bool G_led_ccl_5    = false;
bool G_btConnect    = false;
bool G_pwrMode      = false;

int G_rs485IterNum = 0;
int G_rebModAut_tm = 0;  
int G_msgTxtDataLen;

String G_msgTxtData;
 
bool G_eventExpected[32]    = {false};
bool G_eventOccurred[32]    = {false};   


void initObjects()
{
   
    // String devName = "Prizma_JMR_A_"+String(G_lJmrStt.esp32Addr);
    // SerialBT.begin(devName);
    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtTransmit      = xQueueCreate(2, sizeof(_MSG_PACK));
    QueueRs485Transmit   = xQueueCreate(2, sizeof(_RS485_data));
    QueueRs485Send       = xQueueCreate(2, sizeof(_MSG_PACK));
    QueueRs485Receive    = xQueueCreate(2, 1024);
    QueuePreferences     = xQueueCreate(2, sizeof(_MSG_PACK));
    QueueRebModAut          = xQueueCreate(2, sizeof(_RM_AUT));
    QueuePwrAut             = xQueueCreate(2, 4);
    QueueLeds               = xQueueCreate(2, 4);
 

}


