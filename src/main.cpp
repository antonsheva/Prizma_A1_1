#include <Arduino.h>
#include "main.h"

TaskHandle_t TaskHandle_RebMod_In;           
TaskHandle_t TaskHandle_RebMod_Out;            
TaskHandle_t TaskHandle_BT_In;            
TaskHandle_t TaskHandle_BT_Out;           
TaskHandle_t TaskHandle_Serial_In;           
TaskHandle_t TaskHandle_Serial_Out;
TaskHandle_t TaskHandle_RS485_In;      
TaskHandle_t TaskHandle_RS485_Out;      
TaskHandle_t TaskHandle_automat; 
TaskHandle_t TaskHandle_init; 
TaskHandle_t TaskHandle_wait485Resp; 
TaskHandle_t TaskHandle_getJammList; 

BluetoothSerial SerialBT; 

 


char G_tmpBuff[TMP_BUFF_LEN];

 



void setup() {
  AN_cbFuncs* cbFuncs = AN_cbFuncs::getI();
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial1.begin(9600, SERIAL_8N1, 34, 32);


  Serial.onReceive(cbFuncs ->uart0Rx);
  Serial1.onReceive(cbFuncs->uart1Rx);
  Serial2.onReceive(cbFuncs->uart2Rx);
  
  initObjects();

  
  localJmrStt = new JammerState();

  pinMode(15, OUTPUT);

 

  initTasks(); 
   
  xTaskCreatePinnedToCore(Task_init       ,      "t1", 2048, NULL, 1, &TaskHandle_init         , 1);
  xTaskCreatePinnedToCore(Task_RebMod_In  ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_In    , 1);
  xTaskCreatePinnedToCore(Task_RebMod_Out ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_Out   , 1);
  xTaskCreatePinnedToCore(Task_BT_In      ,      "t1", 2048, NULL, 1, &TaskHandle_BT_In        , 1);
  xTaskCreatePinnedToCore(Task_BT_Out     ,      "t1", 2048, NULL, 1, &TaskHandle_BT_Out       , 1);
  xTaskCreatePinnedToCore(Task_Serial_In  ,      "t1", 2048, NULL, 1, &TaskHandle_Serial_In    , 1);
  xTaskCreatePinnedToCore(Task_Serial_Out ,      "t1", 2048, NULL, 1, &TaskHandle_Serial_Out   , 1);
  xTaskCreatePinnedToCore(Task_RS485_In   ,      "t1", 2048, NULL, 1, &TaskHandle_RS485_In     , 1);
  xTaskCreatePinnedToCore(Task_RS485_Out  ,      "t1", 2048, NULL, 1, &TaskHandle_RS485_Out    , 1);
  xTaskCreatePinnedToCore(Task_automat    ,      "t1", 2048, NULL, 1, &TaskHandle_automat      , 1);
  xTaskCreatePinnedToCore(Task_wait485Resp,      "t1", 2048, NULL, 1, &TaskHandle_wait485Resp  , 1);
  xTaskCreatePinnedToCore(Task_getJammList,      "t1", 2048, NULL, 1, &TaskHandle_getJammList  , 1);


  vTaskSuspend(TaskHandle_getJammList);
  vTaskSuspend(TaskHandle_wait485Resp);
}

void loop() {

  // vTaskDelay(1);   
}
 