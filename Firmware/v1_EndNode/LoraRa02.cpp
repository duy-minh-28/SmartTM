#include "LoraRa02.h"
#include "SR04.h"   
#include "Timer.h" 

// LORA Config
byte TxPower = 20;

// String variable for LoRa
String receivedMsg = "";
String msgContent = ""; 

// Notification 
uint8_t sleepFlag = 0;

// Variable declarations for sensor value
int SR04Val=0; 

long previousMillis = 0;
uint8_t print = 0; 

// ============================= Function ============================= 
void LoraSetup() {
  LoRa.setPins(ss, rst, dio0);   
  // Serial.println("... Start LoRa init ...");  
  if (!LoRa.begin(433E6)) { // initialize ratio at 915 or 433 MHz
    Serial.println("[Init LoRa failed. Check your connections.] - 0xB1"); 
    while (true); // if failed, do nothing
  }
  // Set output power  
  LoRa.setTxPower(TxPower);
  Serial.println("setTxPower: "+String(TxPower));  
  LoRa.setSpreadingFactor(12);
  LoRa.enableCrc();
  // LoRa.setSignalBandwidth(250000);
  // LoRa.setCodingRate4(8);  
 
  // register the receive callback
  //LoRa.onReceive(onReceive);
  // // register the channel activity dectection callback
  // LoRa.onCadDone(onCadDone);
  // // put the radio into CAD mode
  // LoRa.channelActivityDetection();  
  Serial.println("[Init LoRa successfully! - 0xB0]");
}  

// ============================================== MSG PACKAGE ==============================================
// Structure: masterAddress| localAddress| msgID| MSG Content Length| requestMode| MSG Content
// MSG Content: "MSGID(1), binID(2), SR04 Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), reserved1(7), reserved2(8)"  

// Sending data (LoRa Ra-02)  
// Lora.write (1 byte)
void sendMessage(String msgContent, int requestMode){ 
  LoRa.beginPacket();                     //--> start packet
  LoRa.write(masterAddress);              //--> destination address
  LoRa.write(localAddress);               //--> sender address 
  LoRa.write(msgContent.length());  	    //--> payload length
  LoRa.write(requestMode);                //--> Request mode
  LoRa.print(msgContent);                 //--> payload
  LoRa.endPacket();                       //--> finish packet and send it
}  

//Process the data to be sent, after that it sends a message to the Master.
void prepareMessageToMaster(String msgContent){          
  Serial.println("( ~ prepareMessageToMaster ~ )");
  Serial.println("Transmit to Master! Msg Content:"+msgContent);
  // Send a message to Master.
  sendMessage(msgContent, SendDataMode);  
  // put the radio into receive mode
  // LoRa.receive();
}  












// // ---------------------- For two communication between Master and Slave ---------------------- 
void onReceive(int packetSize){
  // ==============<A.PROCESSING RECEIVED MSG>==============
  if (packetSize == 0) return;           // if there's no packet, return

  // - Read packet header 
  int receivedAddress = LoRa.read();     //--> received address
  byte senderAddress = LoRa.read();      //--> sender address
  byte statusCode = LoRa.read();         //--> status code
  
  // - Check if Master Address correct
  if (senderAddress != masterAddress){ 
    Serial.println("[Received Error: Not from my Master] - 0xB2"); //Not from Master, Ignore.  
    return; 
  }  

  // ==============<B.Checks if slave address is correct or not, if true send CONFORMATION>==============
  if(statusCode==SyncTimeMode){
    secondRTC = LoRa.read();    
    minuteRTC = LoRa.read();   
    hourRTC = LoRa.read();   
    dayRTC = LoRa.read();   
    monthRTC = LoRa.read();     
    yearRTC = LoRa.read();     
  }
  if(receivedAddress != localAddress){
    Serial.print("!receivedAddress: "); Serial.println(receivedAddress); 
    Serial.println("This message is not for me.");
    return;  
  } 
  // else {
  //   byte receivedMsgLength = LoRa.read();  //--> receivedMsg msg length
  //   // - Clears SlaveMsgContent variable data.
  //   receivedMsg = ""; 
  //   // - Get all receivedMsg data.
  //   while (LoRa.available()) {
  //     receivedMsg += (char)LoRa.read();
  //   }    
  //   // - Check length for error.
  //   if (receivedMsgLength != receivedMsg.length()){
  //     Serial.println();
  //     Serial.println("[Received Error: Message length does not match] - 0xB3"); //--> "er" = error: message length does not match length. 
  //     return; 
  //   }  
  // }  
} 
 
void askTimeSync(){ 
} 

void loopInterval(unsigned long interval){
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  while(currentMillis - previousMillis <= interval){
    previousMillis = currentMillis; 

      if(sleepFlag==1){break;}
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

/*
- 
  while(){

  }

  if sleepFlap==1 ? (break;) : ()



*/ 

// // String function to process the data received
// // Reference (Extract Lora Msg Function): https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
// String GetValue(String data, char separator, int index){
//   int found = 0;
//   int strIndex[] = { 0, -1 };
//   int maxIndex = data.length() - 1;
  
//   for (int i = 0; i <= maxIndex && found <= index; i++) {
//     if (data.charAt(i) == separator || i == maxIndex) {
//       found++;
//       strIndex[0] = strIndex[1] + 1;
//       strIndex[1] = (i == maxIndex) ? i+1 : i;
//     }
//   }
//   return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
// } 

// ------------------------ Checking Sensor data ------------------------ 
// // Get SR04 Value
// void getSR04Val(){
//   SR04Val = readSR04(trigPin, echoPin); //return SR04 Value  
//   if(SR04Val>500){
//     SR04Val=1;  
//   } 
// }

// void printSR04Val(){
//   if (Serial.available()){
//     String command = Serial.readStringUntil('/'); 
//     if (command == "start print" ){print = 1; Serial.println("Start print");}  
//     if (command == "stop print" ){print = 0; Serial.println("Stop print");}  
//   }
//   unsigned long currentMillis = millis();
//   if(currentMillis-previousMillis>500 && print==1) { 
//     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");   
//     Serial.print("SR04Val: "); Serial.println(SR04Val); 
//     previousMillis = currentMillis;    
//   } 
// }
