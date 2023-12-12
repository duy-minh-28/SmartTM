#include "WifiConfig.h"    
#include "EEPROM.h"
#include "Error_Ctrl.h"   
#include "Oled.h"  

#define LENGTH(x) (strlen(x) + 1)  
#define EEPROM_SIZE 200             
#define WIFI_RESET 0        
 
void setUpAPMode(); 
void getWifiConfigs();
void setUpConfigEEPROM();
void save_Wifi_EEPROM();
void write_flash(const char* toStore, int startAddr);
String read_flash(int startAddr);
 
unsigned long millis_RESET;

// Set these to your desired credentials.
// const char *host = "SmartTMfu";
// const char *ssidAP = "SmartTMfu.com-GateWay2";
// const char *passwordAP = "12345678";
 
String host = "SmartTMfu"; 
String ssidAP = "SmartTMfu-GateWay2";
String passwordAP = "12345678";

/*
 * Login page
 */
const char* loginIndex =
"<style>"
        "#loginForm {"
            "width: 40%;"
            "margin: 0 auto;"
            "padding: 20px;"
            "background-color: #A0DAA9;"
            "border-radius: 10px;"
            "box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);"
        "}"
        "#loginForm h2 {"
            "text-align: center;"
            "color: #006400;"
        "}"
        "#loginForm table {"
            "width: 90%;"
            "border: none;"
        "}"
        "#loginForm input[type='text'],"
        "#loginForm input[type='password'] {"
            "width: 100%;"
            "padding: 10px;"
            "margin: 5px 0;"
            "border: 1px solid #006400;"
            "border-radius: 5px;"
        "}"
        "#loginForm input[type='submit'] {"
            "width: 60%;"
            "padding: 10px;"
            "background-color: #006400;"
            "color: #FFFFFF;"
            "border: none;"
            "border-radius: 5px;"
            "cursor: pointer;"
        "}"
        "@media screen and (max-width: 768px) {"
            "#loginForm {"
                "width: 60%;"
            "}"
        "}"
    "</style>"
 "<form id='loginForm' name='loginForm'>"
    "<h2>SmartTmfu.com setting Wifi Page</h2>"
    "<table>"
        "<tr>"
             "<td>Wifi SSID:</td>"
             "<td><input type='text' name='userid'><br></td>"
        "</tr>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' name='pwd'><br></td>"
        "</tr>"
        "<tr>"
            "<td>Token key:</td>"
            "<td><input type='text' name='tokenkey'><br></td>"
        "</tr>"
        "<tr>"
            "<td colspan='2'><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Send data successfully!')/*displays error message*/"
    "}"
    "}"
"</script>";
  
WiFiServer server(80);

String userid = "";
String pwd = "";
String token = ""; 
int index_check = 0; 
int wifiConnected = 0;
int getWifiConfig = 0;

/* ----------------------------------------- Get Wifi Flow ----------------------------------------- 
// 1. Get Config from EPPROM first
// 2. If can not connect using this config -> turn on Wifi AP Mode to set up web server to get new Config
// Both LED turn on while webserver is on
*/
void WifiConfig() {
  Serial.println("================= Wifi Setup =================");
  /* ------------------ Get EEPROM ------------------ */  
  setUpConfigEEPROM(); // checking for 10 seconds
  
  // If no wifi connection
  if (WiFi.status() != WL_CONNECTED){
    /* ------------------ Get to STA Mode ------------------ */ 
    setUpAPMode();
    millis_RESET = millis(); 
    while(wifiConnected != 1){ 
      // Get wifi configs from web server on ESP     
      // digitalWrite(LED_CheckMSG_Red, HIGH);   
      delay(500);              
      while(getWifiConfig == 0){
        getWifiConfigs(); // return getWifiConfig = 1 after get Config from webserver
        // Reset if take too long, 180s
        if (millis() - millis_RESET >= 180000){ 
          Serial.println("\n\n\n\n\n\nRestarting the ESP"); 
          ESP.restart();            
        } 
      }  
      // digitalWrite(LED_CheckMSG_Red, LOW);     
  
      // Set Wifi to STA mode 
      Serial.println("WIFI mode : STA");
      WiFi.mode(WIFI_STA);  
      Serial.print("Connecting to ");
      Serial.println(userid); 
      WiFi.begin(userid, pwd); 

      
      // Try to connect for 20s
      for (int i=0; i<30; i++) { 
        Serial.print(".");
        delay(1000); 
        if(WiFi.status() == WL_CONNECTED){ 
          Serial.println("\nWiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          // Save Wifi to eeprom
          save_Wifi_EEPROM();
          // Print to OLED   
          clearOLED();
          drawReceiveFromSlave(0, 10, "Finish Setup Wifi.");    
          drawReceiveFromSlave(0, 22, "Connected to: "+userid); 
          return;
        }
      }  
      // If Wifi not connected, Config is incorrect, keep run  
      if (WiFi.status() != WL_CONNECTED) { 
        setUpAPMode();
        getWifiConfig = 0; // open webserver again
      } 	                
    }
  }  
}

void setUpAPMode(){
  /* ------------------------------------------ Get to AP Mode ------------------------------------------ */
  delay(10);
  WiFi.mode(WIFI_AP);
  Serial.println("### Setting AP (Access Point) ###");  
  // You can remove the password parameter if you want the AP to be open
  // A valid password must have more than 7 characters
  if (!WiFi.softAP(ssidAP, passwordAP)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  // Use MDNS for host name resolution 
  if (!MDNS.begin(host)) { //http://smarttmfu.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started!"); 
  Serial.println("Server started!");      

  clearOLED();
  drawReceiveFromSlave(0, 10, "#Setting AP(Access Point)#"); 
  drawReceiveFromSlave(0, 22, "Wifi: "+ssidAP); 
  drawReceiveFromSlave(0, 34, "Password: "+passwordAP); 
  drawReceiveFromSlave(0, 46, "Web: http://"+host+".local"); 
}

void getWifiConfigs(){ 
  /* ------------------------------------------ Get Configs ------------------------------------------ */
  WiFiClient client = server.available();   // listen for incoming clients 
  if (client) {                             // if you get a client,
    Serial.println("\n\n------------------------");     
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
 
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        // Serial.println("c: ");    
        Serial.write(c);                    // print it out the serial monitor
        
        if (c == '\n') {                    // if the byte is a newline character 
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            //server.send(200, "text/html", loginIndex);
              
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println(loginIndex);
            // the content of the HTTP response follows the header:
            /*
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            // The HTTP response ends with another blank line:
            client.println();
              */            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } 
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }  
        if (currentLine.startsWith("GET /?userid")  && currentLine.endsWith("HTTP/1.1")) { 
          // MSG: GET /?userid=_&pwd=_&tokenkey=_ HTTP/1.1
          // Reset Config  
          index_check = 0;
          userid = "";
          pwd = "";
          token = "";   
          // Extract Configs     
          for (int i = 0; i < currentLine.length(); i++) { 
            if (currentLine[i] == '&') {   
              index_check++;     
            }
            
            if (index_check==1){
              userid += currentLine[i];     
            }
            if (index_check==3){
              pwd += currentLine[i];     
            }
            if (index_check==5){
              token += currentLine[i];      
              if (currentLine[i] == ' ') {   
                index_check++;     
              }
            }
            if (currentLine[i] == '=') {   
              index_check++;     
            }
          }      
          // close the connection: 
          Serial.println("UserID: "+userid);
          Serial.println("pwd: "+pwd);
          Serial.println("token: "+token);     
          getWifiConfig = 1;
          return;        
        }
      }
    }
            
    client.stop();
    Serial.println("Client Disconnected.");    
  } 
}

// ------------------------------------------------- EEPROM ------------------------------------------------- 
void setUpConfigEEPROM(){
  pinMode(WIFI_RESET, INPUT); 
  Serial.println("###  Get Configs EEPROM ###");
  clearOLED();
  drawReceiveFromSlave(0, 10, "# Get Configs EEPROM #"); 
  
  if (!EEPROM.begin(EEPROM_SIZE)) { 
    Serial.println("Failed to init EEPROM");
    delay(1000);
  }
  else{
    userid = read_flash(0); 
    Serial.print("SSID = ");
    Serial.println(userid);
    pwd = read_flash(40); 
    Serial.print("Password = ");
    Serial.println(pwd);
  }
  WiFi.mode(WIFI_STA);  
  WiFi.begin(userid.c_str(), pwd.c_str());
  // Try to connect for 30s
  for (int i=0; i<30; i++) { 
    delay(1000);  
    Serial.print(".");
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("Connecting to: "+userid+"| Password: "+pwd);  
      Serial.println("");
      clearOLED();
      drawReceiveFromSlave(0, 10, "Connected to: "+userid); 
      return;
    }
  }
  Serial.println("");
}

void save_Wifi_EEPROM(){
  userid = WiFi.SSID();
  pwd = WiFi.psk();
  Serial.print("userid:");
  Serial.println(userid);
  Serial.print("pwd:");
  Serial.println(pwd);
  Serial.println("Store SSID & password in Flash");
  write_flash(userid.c_str(), 0); 
  write_flash(pwd.c_str(), 40); 
}
 
void write_flash(const char* toStore, int startAddr) {
  int i = 0;
  for (; i < LENGTH(toStore); i++) {
    EEPROM.write(startAddr + i, toStore[i]);
  }
  EEPROM.write(startAddr + i, '\0');
  EEPROM.commit();
}

String read_flash(int startAddr) {
  char in[128]; 
  int i = 0;
  for (; i < 128; i++) {
    in[i] = EEPROM.read(startAddr + i);
  }
  return String(in);
}