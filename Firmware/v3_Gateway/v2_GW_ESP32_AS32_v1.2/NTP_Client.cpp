#include "NTP_Client.h"  

// Define NTP Client to get real time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String currentDate;

void dateSetup(){
  // Initialize a NTPClient to get time
  timeClient.begin();  
  timeClient.setTimeOffset(25200);  
}

String getdate(){
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  time_t epochTime = timeClient.getEpochTime(); 
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime);  
  int monthDay = ptm->tm_mday; 
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
 
  // int currentHour = timeClient.getHours();
  // int currentMin = timeClient.getMinutes();
  // int currentSec = timeClient.getSeconds();;

  //Print complete date:
  currentDate = String(currentYear) + "/" + String(currentMonth) + "/" + String(monthDay) + "-" + String(formattedTime);
  return currentDate;
}