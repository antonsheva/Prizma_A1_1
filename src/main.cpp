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



void init(){
  G_autCode = AUT_CODE_GET_STT_1;
  initTasks();  
  G_waitResponse = 2;
  xSemaphoreGive(SemaphoreTaskAutomat);

}


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial1.begin(9600, SERIAL_8N1, 34, 32);
  pinMode(15, OUTPUT);

  analogWrite(15, 100);
   

  xTaskCreatePinnedToCore(Task_RebMod_In  ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_In   , 1);
  xTaskCreatePinnedToCore(Task_RebMod_Out ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_Out  , 1);
  xTaskCreatePinnedToCore(Task_BT_In      ,      "t1", 2048, NULL, 1, &TaskHandle_BT_In        , 1);
  xTaskCreatePinnedToCore(Task_BT_Out     ,      "t1", 2048, NULL, 1, &TaskHandle_BT_Out       , 1);
  xTaskCreatePinnedToCore(Task_Serial_In  ,      "t1", 2048, NULL, 1, &TaskHandle_Serial_In    , 1);
  xTaskCreatePinnedToCore(Task_Serial_Out ,      "t1", 2048, NULL, 1, &TaskHandle_Serial_Out   , 1);
  xTaskCreatePinnedToCore(Task_RS485_In   ,      "t1", 2048, NULL, 1, &TaskHandle_RS485_In     , 1);
  xTaskCreatePinnedToCore(Task_RS485_Out  ,      "t1", 2048, NULL, 1, &TaskHandle_RS485_Out    , 1);
  xTaskCreatePinnedToCore(Task_automat    ,      "t1", 2048, NULL, 1, &TaskHandle_automat      , 1);
  
  init();

}

void loop() {

  vTaskDelay(10000);   
}
 