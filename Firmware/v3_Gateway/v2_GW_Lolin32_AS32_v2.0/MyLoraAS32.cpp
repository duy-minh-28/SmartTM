#include "MyLoraAS32.h" 
#include "MyMQTT.h"  
#include "Error_Ctrl.h"  
#include "NTP_Client.h"  

EspSoftwareSerial::UART Serial3; 

// Counter to restart Lora Ra-02.
int count_to_Rst_LORA = 0;
int count_receivedMSG = 0; 
int count_receivedMSG_Total = 0;

int ct_GreenPeriod = 0;
int ct_RedPeriod = 0;

// -------------------- ERROR --------------------
int ct_MSG_Length_err = 0; 
int periodID=0; //Master checking Slave msg frequency
int publishMsgID=0; //MQTT MSG ID 
int periodExpectedPkg;
  
// -------------------- Variable declaration for Millis, Timer --------------------
unsigned long previousMillis_receivedMSG = 0;
unsigned long previousMillis_RestartLORA = 0;


void AS32Setup(){ 
  pinMode(MD0, OUTPUT);
  pinMode(MD1, OUTPUT);
  digitalWrite(MD0, 0);   
  digitalWrite(MD1, 0);  
 
  Serial3.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  if (!Serial3) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
   }
  }else{
    Serial.println("OK EspSoftwareSerial");     
  }
}

void AS32SendMessage(String msgContent){ 
  Serial3.print(msgContent); 
}
 
// ============================================= MSG PACKAGE =============================================
// Structure: (0)"*,"| (1)masterAddress| (2)localAddress| (3)msgID| (4)msg Content Length| (5)msg Content
// msg Content: "binID, SR04 value, Voltage"
void AS32Receive(){ 
  if(Serial3.available()>0){
    String SlaveMsgContent = Serial3.readStringUntil('#');     
    Serial.println("Massage: "+SlaveMsgContent);
 
    // int recipient = atoi(extractMsg(SlaveMsgContent, ',', 1));
    // int SlaveMsgID = atoi(extractMsg(SlaveMsgContent, ',', 2));
    // int sender = atoi(extractMsg(SlaveMsgContent, ',', 3));
    // int SlaveMsgContentLength = atoi(extractMsg(SlaveMsgContent, ',', 4)); 
    // String msgContent = getMsgContentFromIndex(SlaveMsgContent, ',', 5);


    String recipient = extractMsg(SlaveMsgContent, ',', 1);
    String SlaveMsgID = extractMsg(SlaveMsgContent, ',', 2);
    String sender = extractMsg(SlaveMsgContent, ',', 3);
    String SlaveMsgContentLength = extractMsg(SlaveMsgContent, ',', 4); 
    String msgContent = getMsgContentFromIndex(SlaveMsgContent, ',', 5);
 
    Serial.println("recipient: "+recipient+ "| SlaveMsgID: "+SlaveMsgID+ "| Sender: "+sender+ 
                  "| SlaveMsgContentLength: " +SlaveMsgContentLength+ "| msgContent: "+msgContent);
                  
    int recipient_int = atoi(recipient.c_str()); 
    // int SlaveMsgID_int = atoi(SlaveMsgID.c_str()); 
    // int sender_int = atoi(sender.c_str()); 
    int SlaveMsgContentLength_int = atoi(SlaveMsgContentLength.c_str()); 
    // Serial.println("recipient_int: "+String(recipient_int)+ "| SlaveMsgID_int: "+String(SlaveMsgID_int)+ "| sender_int: "+String(sender_int)+ 
    //               "| SlaveMsgContentLength_int: " +String(SlaveMsgContentLength_int));    
 
    // - Check length for error.
    if (SlaveMsgContentLength_int != msgContent.length()) {
      // Serial.println("SlaveMsgContentLength: "SlaveMsgContentLength);
      Serial.println("Warning! MSG length not match! Something error in transmission process"); //--> "er" = error: message length does not match length.  
      ct_MSG_Length_err++;
      return; //--> skip rest of function
    }  
    
    // // - Check whether the SlaveMsgContent data or message for this device.
    if (recipient_int != localAddress) {  // localAddress -> Master address:01
      Serial.println("Warning! This message is not for me"); 
      return; //--> skip rest of function
    } 

  // ==============<3.Send MSG to MQTT BROKER>============== 
  publishMsgID++; 
  // - [MQTT publish MSG Structure]: publish MQTT Msg ID| SlaveMsgID| SlaveMsgContent| packetRssi| packetSnr 
  String publishMSG = String(publishMsgID) +","+ SlaveMsgID +","+ msgContent;
  //+","+ String(LoRa.packetRssi()) +","+ String(LoRa.packetSnr()); 
  char *topic_insert_DB = "TrashbinCapstone/Slave/MSG";   
  mqttPublish(topic_insert_DB, publishMSG.c_str());  
  } 
}


void AS32Sleep(){  
  digitalWrite(MD0, 1);   
  digitalWrite(MD1, 1);  
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
      // LoRa.end();
      // LoraSetup(); 
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
    char *topic_checkMSG = "TrashbinCapstone/Slave/CheckMSG";   
    mqttPublish(topic_checkMSG, char_arr);  

    count_receivedMSG_Total+=count_receivedMSG;
    count_receivedMSG=0;
    Serial.println("=======================================================================");    
  }
} 
 

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


String getMsgContentFromIndex(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length();
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1; 
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], maxIndex) : "";
} 