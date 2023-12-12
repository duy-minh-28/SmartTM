#include "MyMQTT.h"  
#include "LoraRa02.h" 
#include "Error_Ctrl.h"  
 
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
  // Serial.println("============= MQTT Callback =============");
  // Serial.print("Message arrived on topic: "); Serial.println(topic);
  Serial.print("MQTT Callback-Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(); 
  if(String(topic)=="XAMPP/command"){ 
  }   
}
 
void mqttPublish(char *topic, char *toPublish){ 
  Serial.println("Publish message: " + String(toPublish));  
  client.publish(topic, toPublish);  
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT reconnection...");
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