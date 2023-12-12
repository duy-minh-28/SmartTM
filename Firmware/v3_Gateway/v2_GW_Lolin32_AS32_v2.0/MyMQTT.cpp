#include "MyMQTT.h"  
#include "LoraRa02.h" 
#include "Error_Ctrl.h"  

// Wifi variables
const char *ssid     = "P1407-2.4G";        
const char *password = "14071407";

// const char* ssid     = "Redmi10";        
// const char* password = "momo8383";

// const char* ssid     = "Minh";        
// const char* password = "1234567890";

// MQTT variables
// const char* mqtt_server = "192.168.1.15";   
// const char* mqtt_server = "192.168.166.31";  

const char* mqtt_server = "14.225.207.85";  
const int mqtt_port = 1883;   

// const char* mqtt_server = "54.244.173.190";  //broker.emqx.io
// ws://broker.emqx.io:8083/mqtt

const int MSG_BUFFER_SIZE	= 30;
char msg[MSG_BUFFER_SIZE]; 
int value = 0;
long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient); 

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

// ===================================================== MQTT ===========================================================
void MQTTSetup(){ 
  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  // Subscribe
  client.subscribe("TrashbinCapstone/Slave/MSG-PUBACK");
}
 
// Callback - get message from MQTT broker
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println("============= MQTT Callback =============");
  Serial.print("Message arrived on topic: "); Serial.println(topic);
  Serial.print("Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(); 
  if(String(topic)=="XAMPP/command"){ 
  }   
}
 
void mqttPublish(const char* topic, const char* toPublish){ 
  Serial.println("Publish message: " + String(toPublish));  
  client.publish(topic, toPublish);  
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "Khoa83-ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      // Subscribe
      client.subscribe("TrashbinCapstone/Slave/MSG-PUBACK");
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait before retrying
      buzzer(1000);
      blinkLedSec(LED_CheckMSG_Red, 1000);
      delay(2000);
    }
  }
}

void mqttClientRefresh(){ 
  if (!client.connected()) { 
    reconnect();
  }
  // else{
  //   Serial.println("client.connected()!");     
  // }
  client.loop(); 
}