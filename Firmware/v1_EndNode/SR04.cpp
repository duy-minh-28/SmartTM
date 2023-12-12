#include "SR04.h" 

// -- Defines variables -- 
long duration;
float distance;  

void initHC_SR(int trigPin, int echoPin){
  // -- Init -- 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.println("[Init SR04 successfully! - 0xA0]");
}

// ---------------------------- Read SR04 Sensor Value and Calculate distance ----------------------------
float readSR04(int trigPin, int echoPin){  
  // -- Clears the trigPin -- 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(100); 
  // -- Sets the trigPin on HIGH state for 10 micro seconds -- 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  // --  Reads the echoPin, returns the sound wave travel time in microseconds -- 
  duration = pulseIn(echoPin, HIGH);
  // -- Calculating the distance-- 
  distance = duration * 0.034 / 2;
  // -- Prints the distance on the Serial Monitor -- 
  // Serial.print("Distance: ");
  // Serial.println(distance);
  return distance; 
} 


 
 
