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
TaskHandle_t TaskHandle_monitor; 
TaskHandle_t TaskHandle_wait485Resp; 
TaskHandle_t TaskHandle_pollRs485; 
TaskHandle_t TaskHandle_pwrButton;
TaskHandle_t TaskHandle_pwrAut;
TaskHandle_t TaskHandle_leds;
 

void init(){
    // initPreferencesData();
    
    initObjects();
    AN_pref::getI()->init();
    AN_cmd::getI()->init();
    RmCtrl *rm = RmCtrl::getI();
    AN_rs485::getI()->init();

 
    rm->selDev = 0;
    rm->rmSer1.rx = 34;
    rm->rmSer1.tx = 32;
    rm->rmSer2.rx = 35;
    rm->rmSer2.tx = 33;
} 

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

  pinMode(PIN_PWR_HOLD_DRV, OUTPUT);
  pinMode(PIN_PWR_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RS485_DIR_DRV, OUTPUT);
  pinMode(PIN_FAN, OUTPUT); 

  pinMode(PIN_CN1, INPUT);
  pinMode(PIN_CN2, INPUT);
  
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
   
  pinMode(PIN_JMMR_ON_DRV_1, OUTPUT);
  pinMode(PIN_JMMR_ON_DRV_2, OUTPUT);
  
  digitalWrite(PIN_PWR_HOLD_DRV , 1);   

 
  init(); 

  xTaskCreatePinnedToCore(Task_pwrAut         ,         "t1", 8192, NULL, 1, &TaskHandle_pwrAut       , 1);
  xTaskCreatePinnedToCore(Task_leds           ,         "t1", 1024, NULL, 1, &TaskHandle_leds         , 1);
  xTaskCreatePinnedToCore(Task_pwrButton      ,         "t1", 1024, NULL, 1, &TaskHandle_pwrButton    , 1);

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
  xTaskCreatePinnedToCore(Task_monitor        ,         "t1", 2048, NULL, 1, &TaskHandle_monitor      , 1);

  
  // vTaskSuspend(TaskHandle_txBt); 
  // vTaskSuspend(TaskHandle_txRs485);  
  // vTaskSuspend(TaskHandle_rxRs485);
  vTaskSuspend(TaskHandle_pollRs485);
  vTaskSuspend(TaskHandle_RebMod_Out);
  vTaskSuspend(TaskHandle_wtDataPack);
  // vTaskSuspend(TaskHandle_rebModAut);

}

void loop() {

  
  vTaskDelay(300);   
}
 