#include "SendMsg.h"   
 
void print_wakeup_reason();

void setup() {
  Serial.begin(115200);
  Serial.println("=======================================================================================");
  Serial.println("==================================== Begin Program ====================================");
  Serial.println("=======================================================================================");
  print_wakeup_reason(); // Print to Serial the reason ESP32 wake up 
    
  setup_EN_Wifi();

  // -- Data -- 
  int binID = 6;  
  int sensorValue = random(1, 50);  
  String token = "eHZKWBUK|khoalnv83|n9f0RLQ";
  sendMessage_EN_Wifi(binID, sensorValue, token);

  sleep_EN_Wifi();
  
}
 
void loop() {}

/*Print the reason why ESP32 wake up from sleep: from external signal, timer, etc.*/
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
