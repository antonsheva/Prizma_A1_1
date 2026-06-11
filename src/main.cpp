#include <Arduino.h>
#include "main.h"

 // Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif



TaskHandle_t TaskHandle_RebMod_In;           
TaskHandle_t TaskHandle_RebMod_Out;            
TaskHandle_t TaskHandle_BT_In;            
TaskHandle_t TaskHandle_BT_Out;           
TaskHandle_t TaskHandle_Serial_In;           
TaskHandle_t TaskHandle_eventControl;
TaskHandle_t TaskHandle_transmitRs485;      
TaskHandle_t TaskHandle_wtDataPack;      
TaskHandle_t TaskHandle_rebModAut; 
TaskHandle_t TaskHandle_init; 
TaskHandle_t TaskHandle_wait485Resp; 
TaskHandle_t TaskHandle_txRs485; 


 

void setup() {
  AN_cbFuncs* cbFuncs = AN_cbFuncs::getI();
  String device_name = "Prizma JMR";
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial1.begin(9600, SERIAL_8N1, 34, 32);
  

  Serial.onReceive(cbFuncs ->uartUsb);
  Serial1.onReceive(cbFuncs->uartRm);
  Serial2.onReceive(cbFuncs->uart485);
  SerialBT.register_callback(cbFuncs->uartBt);

  pinMode(22, OUTPUT);
  pinMode(19, INPUT_PULLUP);
  pinMode(RS485_DIR_DRV, OUTPUT);
  
  digitalWrite(22, 1);



    

 

  initTasks(); 

  xTaskCreatePinnedToCore(Task_init       ,      "t1", 2048, NULL, 1, &TaskHandle_init         , 1);
  xTaskCreatePinnedToCore(Task_RebMod_In  ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_In    , 1);
  xTaskCreatePinnedToCore(Task_RebMod_Out ,      "t1", 2048, NULL, 1, &TaskHandle_RebMod_Out   , 1);
  xTaskCreatePinnedToCore(Task_BT_In      ,      "t1", 2048, NULL, 1, &TaskHandle_BT_In        , 1);
  xTaskCreatePinnedToCore(Task_BT_Out     ,      "t1", 2048, NULL, 1, &TaskHandle_BT_Out       , 1);
  xTaskCreatePinnedToCore(Task_Serial_In  ,      "t1", 2048, NULL, 1, &TaskHandle_Serial_In    , 1);
  xTaskCreatePinnedToCore(Task_eventControl ,    "t1", 2048, NULL, 1, &TaskHandle_eventControl , 1);
  xTaskCreatePinnedToCore(Task_transmitRs485   , "t1", 2048, NULL, 1, &TaskHandle_transmitRs485, 1);
  xTaskCreatePinnedToCore(Task_rebModAut     ,   "t1", 2048, NULL, 1, &TaskHandle_rebModAut    , 1);
  xTaskCreatePinnedToCore(Task_wait485Resp,      "t1", 2048, NULL, 1, &TaskHandle_wait485Resp  , 1);
  xTaskCreatePinnedToCore(Task_txRs485,          "t1", 2048, NULL, 1, &TaskHandle_txRs485      , 1);
  xTaskCreatePinnedToCore(Task_watiDataPacks  ,  "t1", 2048, NULL, 1, &TaskHandle_wtDataPack   , 1);


  vTaskSuspend(TaskHandle_txRs485);
  vTaskSuspend(TaskHandle_wait485Resp);
  vTaskSuspend(TaskHandle_wtDataPack);
  vTaskSuspend(TaskHandle_rebModAut);

}

void loop() {

  
  vTaskDelay(300);   
}
 