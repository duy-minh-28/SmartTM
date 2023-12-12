// ESP32/ESP8266 Lora Ra-02 Slave number n
#include "SR04.h"
#include "LoraRa02.h"
#include "Timer.h"
#include "Firmware.h"
#include "WifiConfig.h"   

/* Bootcount can be used as Message ID, but using time slot schedule, 
sometimes esp wake up and it not the right slot time to for the end node to 
send data, so the msgID shouldn't increase -> MSG ID only increase when sending data*/

RTC_DATA_ATTR int MSGID = 0;
RTC_DATA_ATTR int TimeSlotID = 0;

int UpdateTimeSleep = 0;
int BetweenSlotTime = 5;
int bootCount = 0;

int sleepTimerSec = 5;
byte localAddress = 0;

// Ctrl pin to turn power off for Sensor and Lora
uint8_t readVoltagePin = 13;
uint8_t SR04_GND = 25;
uint8_t notifiLed = 15;

// uint8_t startTimeSec=0;
// uint8_t endTimeSec=30;

// ================================== MAIN PROGRAM ==================================
// For this code, ESP will only wake up -> Send MSG to Master -> then sleep again
// ==================================================================================
void setup() {
  Serial.begin(115200);
  rtcSetup();
  Serial.println("\n\n\n########################################################################################################");
  Serial.println("######################################## Boot count: " + String(bootCount) + " ########################################");
  Serial.println("########################################################################################################");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35,1); //1 = High, 0 = Low
  print_wakeup_reason();  // Print the wakeup reason for ESP32
  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  /* For Time slot Schedule (failed) */
  // slave_send_data_schedule(1, 30, localAddress);
  // UpdateTimeSleep = ( 0 +(5*(localAddress-1)) + (TIME_TO_SLEEP*(TimeSlotID)) ) - secondRTC;
  // Serial.println("Time Slot: "+String(TimeSlotID));
  // Serial.println("UpdateTimeSleep: "+String(UpdateTimeSleep));

  /* Just use random amount of time tp sleep */
  slave_send_data();

  // ============================ Start sleeping  ============================
  ++bootCount;  // Increment boot number and print it every reboot
  // int timeSleepRandom = random(5000, 10000); // microsec
  // Serial.println("timeSleepRandom (5-10s): "+String(timeSleepRandom));
 
  saveTimeRTC(); // Save time
  esp_sleep_enable_timer_wakeup(sleepTimerSec * uS_TO_S_FACTOR);  // Set time sleep  
  Serial.println("[[[[[[[[[[[[[[[ TIME TO SLEEP SECONDS: "+String(sleepTimerSec)+" SECONDS. ]]]]]]]]]]]]]]]");
  Serial.println("########################################################################################################\n\n\n\n\n");
  Serial.flush();
  esp_deep_sleep_start();
}

// void slave_send_data_schedule(int startSec, int endSec, int SlaveID){
//   Serial.println("Condition: "+String( (0+(5*(SlaveID-1))+(BetweenSlotTime*TimeSlotID)) )+"| "+String( (5+(5*(SlaveID-1))+(BetweenSlotTime*TimeSlotID))) );
//   // 0,5
//   // if(0+(5*(SlaveID-1))+(TIME_TO_SLEEP*TimeSlotID) =< secondRTC && secondRTC < 5+(5*(SlaveID-1))+(TIME_TO_SLEEP*TimeSlotID) )
//   if( 0+(5*(SlaveID-1))+(BetweenSlotTime*TimeSlotID) <= secondRTC && secondRTC < 5+(5*(SlaveID-1))+(BetweenSlotTime*TimeSlotID) ){
//     slave_send_data();
//     TimeSlotID++;
//     if(TIME_TO_SLEEP*TimeSlotID >= 60){
//       TimeSlotID = 0;
//     }
//   }
//   else{
//     Serial.println("Wrong time!");
//   }
// }

void slave_send_data() {
  // ============================ <1.SETUP> ============================
  MSGID++;
  Serial.println("[SETUP]");
  pinMode(notifiLed, OUTPUT);
  pinMode(SR04_GND, OUTPUT);
  digitalWrite(SR04_GND, 1);

  // ============================ <2.Read value and sending msg to Master Node ============================
  // -- SR04 --
  // Turn on power for SR04
  Serial.println("[1. SR04]");
  digitalWrite(SR04_GND, 0);
  delay(40);
  // Run
  initHC_SR(trigPin, echoPin);
  int SR04Val_toMaster = readSR04(trigPin, echoPin);  //return SR04 Value
  if (SR04Val_toMaster > 500) {
    SR04Val_toMaster = 1;
  }
  // Turn off power for SR04
  digitalWrite(SR04_GND, 1);
  Serial.println("Value: " + String(SR04Val_toMaster));

  // -- Read Voltage --
  Serial.println("[2. Read Voltage]");
  int vol = analogRead(readVoltagePin);
  float calcVol = ((((float)vol * 3.3) / 4096) * 2) + 0.3;
  Serial.println("Vol: " + String(calcVol));


  // -- Mac Address --
  String MACaddress = getMyMacAddress();
  // -- EndNode ID --
  localAddress = getEndNodeAddress();

  // -- Lora --
  // Turn on power for Lora
  Serial.println("[3. Lora]");
  LoraSetup();

  // ======================= MSG PACKAGE =======================
  // MSG Content: "MSGID(1), binID(2), SR04 Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), Sleep Timer(7), reserved2(8)"
  String MsgToMaster = String(MSGID) + "," + String(localAddress) + "," + String(SR04Val_toMaster) + "," + String(calcVol) + ","
                       + MACaddress + "," + FirmWareVersion + "," + String(sleepTimerSec) + "," /*future3*/;

  // Blink Led multiple times, notify slave sent data
  for (int i = 0; i < 3; i++) {
    digitalWrite(notifiLed, HIGH);
    delay(100);
    digitalWrite(notifiLed, LOW);
    delay(100);
  }
  prepareMessageToMaster(MsgToMaster);
  delay(50);
  // onReceive(LoRa.parsePacket());
  // delay(1000);
  LoRa.end();  // Include sleep mode
  Serial.println("Finishing LORA.");
}

/*Print the reason by which ESP32 has been awaken from sleep*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: 
      Serial.println("Wakeup caused by external signal using RTC_IO"); 
      WifiConfig(); 
      break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void button_wake_up() {
  while(1){
    if (Serial.available()){
      String command = Serial.readStringUntil('/'); 
      if (command == "timer:" ){break;}   
    }   
  } 
}

 
void loop() {}
// END