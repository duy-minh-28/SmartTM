#include <ArduinoJson.h>

#include "SendMsg.h" 
#include "Sensor.h" 
#include "Firmware.h"   
#include "WifiConfig.h"   
#include "smarttmMQTT.h" 
  
int bootCount = 0; 
int sleepTimerSec = 5;

int ledD4 = 12;
int ledD5 = 13;
int ledD_Green = 2;

int MSGID = 0;
int publishMsgID = 0;
int readVoltagePin = 8;

/*
*
*
*
*/

void setup_EN_Wifi(){
  bootCount++;
  esp_sleep_enable_timer_wakeup(sleepTimerSec * uS_TO_S_FACTOR); // Set time sleep
  Serial.println("Sleep time in senconds: "+String(sleepTimerSec));
  
  pinMode(ledD4, OUTPUT);
  pinMode(ledD5, OUTPUT);
  pinMode(ledD_Green, OUTPUT);
  
  WifiConfig(); 
  MQTTSetup();   
}

void sleep_EN_Wifi(){
  Serial.println("Go to sleep\n\n\n");
  Serial.flush(); 
  esp_deep_sleep_start();  
}

void sendMessage_EN_Wifi(int binID, int sensorValue, String token){
  mqttClientRefresh();
  // JSON
  StaticJsonDocument<250> JsonDoc;
   
  
  /* ============================ <Read value and sending msg to Master Node> ============================ */
  // -- Distance Sensor --  
  // VL53L0X_Setup();  
  // int VL53Val = readVL53L0X();  
  // Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  // Serial.println("[1.Distance Sensor Value: "+String(VL53Val)+"]");
  // Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  
  // -- Read Voltage -- 
  // Serial.println("[2. Read Voltage]");
  // int vol = analogRead(readVoltagePin);
  // float calcVol = ((((float)vol*1.1)/4095)*2.034*2)+0.7; 
  // Serial.println("Vol: "+String(calcVol));

  // -- Mac Address --
  String MACaddress = getMyMacAddress(); 
  Serial.print("MACaddress: ");
  Serial.println(MACaddress);
  
  // -- Send data --   
  int calcVol = random(3, 6);  
  int packetRssi = random(-130, -90);  
  int packetSnr = random(1, 20);  
  
  // ============================================== MSG PACKAGE ==============================================
  // MSG Content: "MSGID(1), binID(2), Sensor Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), Sleep Timer(7), reserved2(8)"
  MSGID++;
  publishMsgID++;
  
  // ---------------- Method 1: String msg ----------------
  // String MsgToMaster = String(bootCount)+"," +String(binID)+"," +String(sensorValue)+"," +String(calcVol)+","
  //                     +MACaddress+"," +FirmWareVersion+"," +String(sleepTimerSec)+"," /*future3*/; 
  // // [MQTT publish MSG Structure]: publish MQTT Msg ID(0)| SlaveMsgContent(1-8)| packetRssi(9)| packetSnr(10)  
  // String publishMSG = String(bootCount)+"," +MsgToMaster+"," +String(packetRssi)+"," +String(packetSnr);
  // Publish MQTT message
  // Serial.print("Publish message: ");
  // Serial.println(publishMSG);
  // char *char_arr = &publishMSG[0];     
  // char topic_insert_DB[] = "TrashbinCapstone/Slave/MSG_JSON";    
  // mqttPublish(topic_insert_DB, char_arr);  


  // ---------------- Method 2: JSON msg ----------------
  JsonDoc["msgID"] = String(bootCount);
  JsonDoc["binID"] = String(binID);
  JsonDoc["sensorValue"] = String(sensorValue);
  JsonDoc["voltageValue"] = String(calcVol);
  JsonDoc["MacAddress"] = MACaddress;
  JsonDoc["firmWareVersion"] = String(FirmWareVersion);
  JsonDoc["sleepPeriod"] = String(sleepTimerSec);
  JsonDoc["pckRSSI"] = String(packetRssi);
  JsonDoc["pckSNR"] = String(packetSnr);
  JsonDoc["token"] = String(token);
 
  char msg_buffer[250];
  int msg_bytes = serializeJson(JsonDoc, msg_buffer);
  Serial.print("msg_bytes = ");
  Serial.println(msg_bytes, DEC);
   
  char topic_insert_DB[] = "TrashbinCapstone/Slave/MSG_JSON";    
  mqttPublish(topic_insert_DB, msg_buffer);  


  // Blink n times
  for (int i=0; i<3; i++) {
    digitalWrite(2, HIGH);   
    delay(200);                       
    digitalWrite(2, LOW);   
    delay(200);  
  }  
  delay(200); 
}