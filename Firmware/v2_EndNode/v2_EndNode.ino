// ESP32 Lora E32 End Node
#include "VL53L0X.h" 
#include "MyLoraAS32.h"  
#include "internalRTC.h" 
#include "Firmware.h"   


// Firmware version 
int bootCount = 0;

// Ctrl pin to turn power off for Sensor  
uint8_t readVoltagePin = 13;
uint8_t notifiLed = 25;
const byte masterAddress = 0x03;      // Master Address
// const byte localAddress = 0x08;       // End Node Address 

// uint8_t startTimeSec=0;
// uint8_t endTimeSec=30;

/*Print the reason by which ESP32 has been awaken from sleep*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason; 
  wakeup_reason = esp_sleep_get_wakeup_cause(); 
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
  
// ================================== MAIN PROGRAM ==================================
// For this code, ESP will only wake up -> Send MSG to Master -> then sleep again
// ==================================================================================  

void setup(){ 
  Serial.begin(115200); 
  ++bootCount; // Increment boot number and print it every reboot
  rtcSetup();
  Serial.println("########################################################################################################");    
  Serial.println("############################ MSG ID: "+String(bootCount)+"| "+getDateRTC()+" ############################");    
  Serial.println("########################################################################################################");     
  print_wakeup_reason(); // Print the wakeup reason for ESP32 
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Set time sleep
  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
 
  // slave_send_data_schedule(1, 30, localAddress);
  slave_send_data();
  
  // ============================ Start sleeping  ============================
  // saveTimeRTC();
  Serial.println("Going to sleep now");
  Serial.println("########################################################################################################\n\n\n");     
  Serial.flush(); 
  esp_deep_sleep_start();  
} 

// void slave_send_data_schedule(int startSec, int endSec, int SlaveID){
//   Serial.println("Condition: "+String(startSec*(localAddress-1))+"| "+String(endSec*(localAddress-1)) );
  
//   if(startSec*(localAddress-1) < secondRTC && secondRTC < endSec*(localAddress-1)){
//     slave_send_data();
//   }
// }
  
void slave_send_data(){      
  Serial.println("---- slave_send_data ----");   
  pinMode(notifiLed, OUTPUT); 
  /* ============================ <Read value and sending msg to Master Node> ============================ */
  // -- Distance Sensor --  
  VL53L0X_Setup();  
  int VL53Val = readVL53L0X();  
  Serial.println("1.Distance Sensor Value: "+String(VL53Val));
  
  // -- Read Voltage --  
  int vol = analogRead(readVoltagePin);
  float calcVol = ((((float)vol*3.3)/4096)*2)+0.3;
  Serial.println("2.Voltage Value: "+String(calcVol));

  // -- Mac Address -- 
  String MACaddress = getMyMacAddress();
  // -- EndNode ID --
  byte localAddress = getEndNodeAddress();
   
  // -- 3. Lora -- 
  // Turn on power for Lora
  Serial.println("[3. Lora]");   
  AS32Setup(); 
  delay(20); 
  // Blink Led multiple times, notify slave sent data
  for(int i=0; i<3; i++){
    digitalWrite(notifiLed, HIGH);   
    delay(100); 
    digitalWrite(notifiLed, LOW);   +
    delay(100); 
  }     
  // ============================================== Endnode PACKAGE ============================================== 
  // MSG Content: "MSGID(1), binID(2), VL53L0X Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), reserved1(7), reserved2(8)"  
  String MsgToMaster = String(bootCount)+"," +String(localAddress)+"," +String(VL53Val)+"," +String(calcVol)+","
                          +MACaddress+"," +FirmWareVersion+"," +/*future2*/"," /*future3*/;  
  AS32SendMessage(masterAddress, localAddress, MsgToMaster); 
  delay(1000);  
  AS32Sleep();
  Serial.println("Lora Sleep.");   
  Serial.println("MsgToMaster: "+MsgToMaster);  
  
}

void loop(){}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 