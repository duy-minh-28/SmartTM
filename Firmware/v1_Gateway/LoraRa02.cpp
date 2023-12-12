#include "LoraRa02.h" 
#include "MyMQTT.h"  
#include "Error_Ctrl.h"  
#include "NTP_Client.h"  

// LoRa data transmission configuration 
const byte localAddress = 0x01;        // Address of this device (Master Address). 
const byte responseCodeOK = 1;         // Confirm receiver MSG from Slave

// For messaging 
String SlaveMsgContent = "";
String Message = "";  

// Variable declaration for Millis, Timer 
unsigned long previousMillis_receivedMSG = 0;
unsigned long previousMillis_RestartLORA = 0; 
 
// Variable for checking Lora communication error 
int count_receivedMSG = 0; 
int count_receivedMSG_Total = 0; 
int ct_GreenPeriod = 0;
int ct_RedPeriod = 0;
int ct_MSG_Length_err = 0; 
int periodID=0;                   //Master checking Slave msg frequency
int publishMsgID=0;               //MQTT MSG ID 
int periodExpectedPkg;
 
 
// Counter to restart Lora Ra-02.
// int count_to_Rst_LORA = 0;
// Get the address of the slaves.
// byte slave_Address; 
// Process of receiving/sending the message has finished.
// bool finished_Receiving_Message = false;  
// bool finished_Sending_Message = false;  
// String Slave_Number = ""; 

// Variable declaration to count slaves.
// byte Slv = 0; 

/* ============================= Function ============================= */
/* Setup Pins, configs for Lora */
int LoraSetup() {
  LoRa.setPins(ss, rst, dio0); 
  Serial.println(); 
  Serial.println("Start LoRa init.....");
  if (!LoRa.begin(433E6)) { // initialize ratio at 915 or 433 MHz
    Serial.println("[LoRa init failed. Check your connections.] - 0xB1"); 
    return 0;
  }
  LoRa.setSpreadingFactor(12);
  LoRa.enableCrc();
  Serial.println("[LoRa init successfully! - 0xB0]");  
  // Finish then blink 2 time   
  for(int i=0; i<3; i++){
    blinkLedSec(LED_CheckMSG_Green, 500);
  }    
  return 1;
} 
 
/* ============================================== Endnode PACKAGE ============================================== */
// Structure: masterAddress(1)| localAddress(2)| MSG Content Length(3)| requestMode(4)| MSG Content(5)  
// MSG Content: "MSGID(1), binID(2), SR04 Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), reserved1(7), reserved2(8)"  
 
void onReceive(int packetSize) { 
  // ==============<1.PROCESSING RECEIVED MSG>==============
  if (packetSize == 0) return;          // if there's no packet, return
  blinkLedSec(LED_CheckMSG_Blue, 250);   // Blink LED to notify MSG has been received
  
  Serial.println("[------------------------- Receiving data -------------------------]");
  // ----- Read packet header -----
  byte recipient = LoRa.read();               // masterAddress(1) | Recipient
  byte sender = LoRa.read();                  // localAddress(2) | Sender/End Node address 
  byte SlaveMsgContentLength = LoRa.read();   // MSG Content Length(3) | For simple checksum
  byte requestMode = LoRa.read();             // requestMode(4) | Different commands
  Serial.println("recipient: "+String(recipient)+ "| sender: "+String(sender)+ 
                "| SlaveMsgContentLength: " +String(SlaveMsgContentLength)+ "| requestMode: "+String(requestMode));

  // - Clears SlaveMsgContent variable data.
  SlaveMsgContent = ""; 
  // - Get all SlaveMsgContent data / message.
  while (LoRa.available()) {
    SlaveMsgContent += (char)LoRa.read();
  } 
  Serial.println("SlaveMsgContent: "+SlaveMsgContent);
  
  // - Check length for error.
  if (SlaveMsgContentLength != SlaveMsgContent.length()) {
    Serial.println("SlaveMsgContentLength: "+String(SlaveMsgContentLength)+" |SlaveMsgContent.length(): "+String(SlaveMsgContent.length()));
    Serial.println("Warning! MSG length not match! Something error in transmission process"); //--> "er" = error: message length does not match length. 
    // finished_Receiving_Message = true;
    ct_MSG_Length_err++;
    return; //--> skip rest of function
  }  
  // - Check whether the SlaveMsgContent data or message for this device.
  if (recipient != localAddress) {  // localAddress -> Master address:01
    Serial.println("Warning! This message is not for me");
    // finished_Receiving_Message = true;
    return; //--> skip rest of function
  } 
  blinkLedSec(LED_CheckMSG_Blue, 250);  // + Blink LED to notify MSG is correct
  /* Send CONFORMATION MSG back to slave */ 
  // sendMessageToSlave("Confirm!Do you copy?", sender, responseCodeOK);

  // ==============<2.Print details>============== 
  // + Print MSG meta
  Serial.println("+Rc from bin: 0x" + String(sender, HEX));
  Serial.println("+Message Content: [" + SlaveMsgContent+"]");  
  Serial.println("+current Rssi: " + String(LoRa.rssi()));
  Serial.println("+packetRssi: " + String(LoRa.packetRssi())); 
  Serial.println("+packetSnr: " + String(LoRa.packetSnr()));
  
  // ==============<3.Send MSG to MQTT BROKER>============== 
  publishMsgID++; 
  // ----- [MQTT publish MSG Structure] -----
  // MSG Content: publish MQTT Msg ID(1)| SlaveMsgID(2)| SlaveMsgContent(3)| packetRssi(4)| packetSnr(5) 
  String publishMSG = String(publishMsgID)+","+ SlaveMsgContent+","+ String(LoRa.packetRssi())+","+ String(LoRa.packetSnr());
  char *char_publishMSG = &publishMSG[0];     
  char topic_insert_DB[] = "TrashbinCapstone/Slave/MSG";   
  mqttPublish(topic_insert_DB, char_publishMSG);  
   
  count_receivedMSG++;   
} 

 
// -------------------- Sending data (LoRa Ra-02) --------------------
void sendMessageToSlave(String sms, byte targetAddress, byte statusCode) { 
  // finished_Sending_Message = false; 
  Serial.println("[Send Confirmation To Slave: 0x"+String(targetAddress, HEX)+"]");
  // Serial.print("Mode   : ");
  // if (statusCode == 1) Serial.println("Getting Data"); 
  // Serial.println("Message: " + sms); 
  LoRa.beginPacket();             //--> start packet
  LoRa.write(targetAddress);      //--> add targetAddress address
  LoRa.write(localAddress);       //--> add sender address
  LoRa.write(statusCode);           //--> 
  LoRa.write(sms.length());  //--> add payload length
  LoRa.print(sms);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it  
  // finished_Sending_Message = true;
} 

// -------------------- String function to process the data received --------------------
// Referenced: https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String extractMsgContent(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1; 
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
} 

// Count the amount of time Master received SMG from slave | FOR ONE SLAVE ONLY
void checkVarSlave_receivedMSG(int intervalSec, int slaveMsgInterval, int periodPkgThreshold){
  // Check slave send SMG times over a period of time, if something wrong then give warning    
  periodExpectedPkg = (intervalSec/slaveMsgInterval);
  unsigned long currentMillis_receivedMSG = millis(); 
  if(currentMillis_receivedMSG - previousMillis_receivedMSG >= (intervalSec)*1000){
    previousMillis_receivedMSG = currentMillis_receivedMSG; 
    ++periodID;
    // delay(5);
    Serial.println("=======================================================================");    
    Serial.println("#PeriodID: "+String(periodID));
    // // Warning
    // --------------------- Enough MSG ~ GREEN ---------------------
    if(count_receivedMSG>=periodPkgThreshold){
      Serial.println("Notification: Enough MSG is received.\n"+String(count_receivedMSG)+
                    "/"+String(periodExpectedPkg)+" MSG had been received over "+String(intervalSec)+" seconds period.");                                    
      digitalWrite(LED_CheckMSG_Yellow, LOW);                    
      digitalWrite(LED_CheckMSG_Red, LOW);    
      digitalWrite(LED_CheckMSG_Green, HIGH);   
      ct_GreenPeriod++;
    } 
    // --------------------- Enough MSG ~ Yellow ---------------------
    if(0<count_receivedMSG && count_receivedMSG<periodPkgThreshold){
      Serial.println("Notification: WARNING! Not enough MSG has been received.\nOnly "+String(count_receivedMSG)+
                    "/"+String(periodExpectedPkg)+" MSG had been received over seconds "+String(intervalSec)+" period.");       
      digitalWrite(LED_CheckMSG_Green, LOW);   
      digitalWrite(LED_CheckMSG_Red, LOW);                                  
      digitalWrite(LED_CheckMSG_Yellow, HIGH);  
    }
    // --------------------- Enough MSG ~ RED ---------------------
    if(count_receivedMSG==0){      
      Serial.println("Notification: WARNING! Zero MSG has been received.\nOnly "+String(count_receivedMSG)+
                    "/"+String(periodExpectedPkg)+" MSG had been received over seconds "+String(intervalSec)+" period.");  
      digitalWrite(LED_CheckMSG_Green, LOW);   
      digitalWrite(LED_CheckMSG_Yellow, LOW);                                  
      digitalWrite(LED_CheckMSG_Red, HIGH);  
      ct_RedPeriod++;
      // Not enough MSG, Lora might be the cause -> Reset Lora 
      LoRa.end();
      LoraSetup(); 
      buzzer(2000);       
    }
    
    // ---------------------- Logging ----------------------
    // getdate();   
    Serial.println("Current date: "+getdate()); 
    Serial.println("Total Periods: "+String(periodID)+"| Green Period: "+String(ct_GreenPeriod)+"| Red Period: "+String(ct_RedPeriod));
    Serial.println("Total MSG_Length_err: "+String(ct_MSG_Length_err));
    Serial.println("Total received MSG since Master start: "+String(count_receivedMSG_Total));
    
    // ------------------------- checkSlaveToMQTT -------------------------
    // DB Struture: checkID, binID, periodID, periodDate, periodPkgCount, status
    // MSG Struture: periodID, periodPkgCount
    String publishMSG = String(periodID) + "," + String(count_receivedMSG);
    char *char_arr = &publishMSG[0];     
    char topic_CheckMSG[] = "TrashbinCapstone/Slave/CheckMSG";   
    mqttPublish(topic_CheckMSG, char_arr);  

    count_receivedMSG_Total+=count_receivedMSG;
    count_receivedMSG=0;
    Serial.println("=======================================================================");    
  } 
} 
 
// Get data from MSG String that separated by comma - ','
// Counting from zero: data1[0],data2[1],data3[2],...
String extractMsg(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}  

// -------------------- Other setup for Logging and Warning --------------------
void loggingSetup(){
  // getdate();
  Serial.println("########################################################################################################");    
  Serial.println("######################### STARTING NEW SESSION/LOG | DATE: "+getdate()+" #########################");    
  Serial.println("########################################################################################################");   
}
  