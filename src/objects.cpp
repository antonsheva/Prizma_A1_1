#include "objects.h"

JammerState G_lJmrStt;
Preferences preferences;  
BluetoothSerial SerialBT; 

QueueHandle_t QueueSerialOut  = NULL;   
QueueHandle_t QueueBtOut      = NULL;

QueueHandle_t QueueRs485Out = NULL;
QueueHandle_t QueueRs485In  = NULL;


SemaphoreHandle_t SemaphoreTaskAutomat = NULL;

std::vector<JammerState> G_jmrsList;

_RS485_data RS485_data;


_MSG_PACK G_rm_msg;
_MSG_PACK G_485_msg;
 
BYTE G_waitResponse = 0;
BYTE G_opCode       = 0;
BYTE G_opList[16] = {0};
BYTE G_opQty = 0;
bool G_swtchActDev  = false;
bool G_waitUnblockTasks[32] = {false};
DWORD G_wait485Cnt = 0;
char G_serialData [2048] = "\0"; 
char G_tmpDataBuff[2048] = "\0"; 


void initObjects()
{
   
    String devName = "Prizma_JMR_A_"+String(G_lJmrStt.esp32Addr);
    SerialBT.begin(devName);
    QueueSerialOut       = xQueueCreate(2, 128);
    QueueBtOut           = xQueueCreate(2, 128);
    QueueRs485Out        = xQueueCreate(2, sizeof(_RS485_data));
    QueueRs485In         = xQueueCreate(2, sizeof(_RS485_data));


SemaphoreTaskAutomat = xSemaphoreCreateBinary();

}


