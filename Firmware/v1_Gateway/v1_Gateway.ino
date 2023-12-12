// Include Library
#include "Error_Ctrl.h"  
#include "NTP_Client.h" 
// #include "WifiSetup.h"   
#include "WifiConfig.h"  
#include "LoraRa02.h" 
#include "MyMQTT.h"  

void setup() { 
  Serial.begin(115200);
  LEDSetup(); // Led setup
  delay(500); 
  
  /* Protocol Setup */
  // while(WifiSetup()!=1){
  //   blinkLedSec(LED_CheckMSG_Red, 1000);
  // } 
  // WifiSmartConfig();
  WifiConfig(); // Finish then blink 2 time 
  delay(3000); 
  MQTTSetup(); 
  LoraSetup(); // Finish then blink 3 time
  delay(3000); 

  /* Time and Logging Setup  */
  dateSetup();  
  loggingSetup();

  /* Finish Setup, blink green 5 times */
  for(int i=0; i<5; i++){
    blinkLedSec(LED_CheckMSG_Green, 500);
  }    
}  
 
void loop() { 
  // Waiting for msg from slave
  onReceive(LoRa.parsePacket());   
  
  // Check if Slave send enough SMG over a period of time
  checkVarSlave_receivedMSG(1800, 7, 7);
    
  // Refresh MQTT client connection
  if (runEvery(5000)) {
    mqttClientRefresh();
  }  
} 

boolean runEvery(unsigned long interval){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 