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
TaskHandle_t TaskHandle_rs485_send;            
TaskHandle_t TaskHandle_txBt;           
TaskHandle_t TaskHandle_rxRs485;           
TaskHandle_t TaskHandle_eventControl;
TaskHandle_t TaskHandle_txRs485;      
TaskHandle_t TaskHandle_wtDataPack;      
TaskHandle_t TaskHandle_rebModAut; 
TaskHandle_t TaskHandle_init; 
TaskHandle_t TaskHandle_wait485Resp; 
TaskHandle_t TaskHandle_pollRs485; 


 

void setup() {
  AN_cbFuncs* cbFuncs = AN_cbFuncs::getI();
  String device_name = "Prizma JMR";
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17, true);
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

  xTaskCreatePinnedToCore(Task_savePreferences,         "t1", 2048, NULL, 1, &TaskHandle_RebMod_In    , 1);
  xTaskCreatePinnedToCore(Task_RebMod_Out     ,         "t1", 2048, NULL, 1, &TaskHandle_RebMod_Out   , 1);
  xTaskCreatePinnedToCore(Task_rs485_send     ,         "t1", 8192, NULL, 1, &TaskHandle_rs485_send   , 1);
  xTaskCreatePinnedToCore(Task_txBt           ,         "t1", 8192, NULL, 1, &TaskHandle_txBt         , 1);
  xTaskCreatePinnedToCore(Task_rxRs485        ,         "t1", 8192, NULL, 1, &TaskHandle_rxRs485      , 1);
  xTaskCreatePinnedToCore(Task_eventControl   ,         "t1", 1024, NULL, 1, &TaskHandle_eventControl , 1);
  xTaskCreatePinnedToCore(Task_txRs485        ,         "t1", 2048, NULL, 1, &TaskHandle_txRs485      , 1);
  xTaskCreatePinnedToCore(Task_rebModAut      ,         "t1", 2048, NULL, 1, &TaskHandle_rebModAut    , 1);
  xTaskCreatePinnedToCore(Task_wait485Resp    ,         "t1", 1024, NULL, 1, &TaskHandle_wait485Resp  , 1);
  xTaskCreatePinnedToCore(Task_pollRs485      ,         "t1", 8192, NULL, 1, &TaskHandle_pollRs485    , 1);
  xTaskCreatePinnedToCore(Task_watiDataPacks  ,         "t1", 1024, NULL, 1, &TaskHandle_wtDataPack   , 1);
  xTaskCreatePinnedToCore(Task_init           ,         "t1", 2048, NULL, 1, &TaskHandle_init         , 1);

  
  // vTaskSuspend(TaskHandle_txBt); 
  // vTaskSuspend(TaskHandle_txRs485);  
  // vTaskSuspend(TaskHandle_rxRs485);
  vTaskSuspend(TaskHandle_pollRs485);
  vTaskSuspend(TaskHandle_RebMod_Out);
  vTaskSuspend(TaskHandle_wtDataPack);
  vTaskSuspend(TaskHandle_rebModAut);

}

void loop() {

  
  vTaskDelay(300);   
}
 