#include "smarttmMQTT.h"  

const char* mqtt_server = "14.225.207.85";  
const int mqtt_port = 1883;   

WiFiClient espClient;
PubSubClient client(espClient); 

void MQTTSetup(){ 
  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  // Subscribe
  client.subscribe("TrashbinCapstone/Slave/MSG-PUBACK");
}

void mqttPublish(char *topic, char *toPublish){ 
  Serial.println("Publish message: " + String(toPublish));  
  client.publish(topic, toPublish);  
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: "); 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection..."); 
    // Attempt to connect
    if (client.connect("smartTM_ESP32Client")) {
      Serial.println("Connected to MQTT broker!");
      // Subscribe
      // client.subscribe("TrashbinCapstone/Slave/MSG-PUBACK");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Serial.println("mqtt_server: ");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttClientRefresh(){ 
  if (!client.connected()) { 
    reconnect();
  } 
  client.loop(); 
}