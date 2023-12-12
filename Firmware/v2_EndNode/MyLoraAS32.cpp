#include "MyLoraAS32.h" 
#include "internalRTC.h" 

EspSoftwareSerial::UART Serial3; 
  
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
 
// ============================================= Endnode PACKAGE =============================================
// Structure: (0)"*,"| (1)masterAddress| (2)localAddress| (3)msgID| (4)msg Content Length| (5)msg Content
// MSG Content: "MSGID(1), binID(2), VL53L0X Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), reserved1(7), reserved2(8)"  

void AS32SendMessage(byte master_Address, byte local_Address, String msgContent){ 
  String toMaster = "*," +String(master_Address)+"," +String(local_Address)+"," +String(msgContent.length())+"," +String(msgContent)+"#";
  Serial3.print(toMaster); 
}   

void AS32Sleep(){  
  digitalWrite(MD0, 1);   
  digitalWrite(MD1, 1);  
}