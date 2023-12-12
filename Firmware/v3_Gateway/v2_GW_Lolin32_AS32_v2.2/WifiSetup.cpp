#include "WifiSetup.h"  
#include "Oled.h"  


// Wifi variables
const char *ssid     = "P1407-2.4G";        
const char *password = "14071407";
 
String smartSSID;                       
String smartPassword;           

// const char* ssid     = "Redmi10";        
// const char* password = "momo8383";

// const char* ssid     = "Minh";        
// const char* password = "1234567890";

// ------------ Setup Wifi ------------
// With mode and ssid/password
int WifiSetup(){ 
  // Set Wifi to STA mode 
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA); 
  delay(100);

  // Connect to Wi-Fi (STA) 
  Serial.println("------------");
  Serial.println("WIFI STA");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // The process of connecting ESP32 with WiFi Hotspot / WiFi Router. 
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      return 0; // Return 0 if can't connect to Wifi after 20 seconds
      // delay(1000);
      // ESP.restart();
    }
  }  
  Serial.print("\nWiFi connected. ");  
  Serial.print("MCU IP address : ");
  Serial.println(WiFi.localIP());  
  return 1;
} 

void WifiSmartConfig() { 
  //Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  clearOLED();
  drawReceiveFromSlave(0, 30, "Waiting for SmartConfig.");  
  
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }  
  Serial.println("\nSmartConfig received.");
  clearOLED();
  drawReceiveFromSlave(0, 30, "SmartConfig received.");  

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  clearOLED();
  drawReceiveFromSlave(0, 30, "WiFi Connected.");   
  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  smartSSID = WiFi.SSID();
  smartPassword = WiFi.psk();
  Serial.print("SSID:"); Serial.println(ssid);
  Serial.print("password:"); Serial.println(password);
  
  clearOLED();
  drawReceiveFromSlave(0, 20, "SSID: "+smartSSID); 
  drawReceiveFromSlave(0, 34, "smartPassword: "+smartPassword);    
}
