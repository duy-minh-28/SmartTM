#include "Timer.h" 

int secondRTC = 0; 
int minuteRTC = 0; 
int hourRTC = 0; 
int dayRTC = 0; 
int monthRTC = 0; 
int yearRTC = 0; 

String dateRTC;
ESP32Time rtc(0);  // offset in seconds GMT+1

void rtcSetup(){
  if(bootCount==0){
    rtc.setTime(0, 0, 17, 23, 11, 2023);  // 17th Jan 2021 15:24:30
  }
  else{
    secondRTC = secondRTC+UpdateTimeSleep;
    rtc.setTime(secondRTC, minuteRTC, hourRTC, dayRTC, monthRTC, yearRTC);  // Ex: 17th Jan 2021 15:24:30 
  }  
  // Serial.println("\n\n#################################################");
  // Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S")); 
}

void saveTimeRTC(){ //save time before go to sleep 
  secondRTC = rtc.getSecond(); 
  minuteRTC = rtc.getMinute(); 
  hourRTC = rtc.getHour(true); 
  dayRTC = rtc.getDay(); 
  monthRTC = rtc.getMonth()+1; 
  yearRTC = rtc.getYear();  
  
  Serial.print("\nSecond: " +String(secondRTC));  
  Serial.print("| minute: " +String(minuteRTC));    
  Serial.print("| hour: " +String(hourRTC));     
  Serial.print("| day: " +String(dayRTC));     
  Serial.print("| month: " +String(monthRTC));    
  Serial.println("| year: " +String(yearRTC));     
}

String getDateRTC(){
  return rtc.getTime("%A, %B %d %Y %H:%M:%S");
}

//  Serial.println(rtc.getTime());          //  (String) 15:24:38
//  Serial.println(rtc.getDate());          //  (String) Sun, Jan 17 2021
//  Serial.println(rtc.getDate(true));      //  (String) Sunday, January 17 2021
//  Serial.println(rtc.getDateTime());      //  (String) Sun, Jan 17 2021 15:24:38
//  Serial.println(rtc.getDateTime(true));  //  (String) Sunday, January 17 2021 15:24:38
//  Serial.println(rtc.getTimeDate());      //  (String) 15:24:38 Sun, Jan 17 2021
//  Serial.println(rtc.getTimeDate(true));  //  (String) 15:24:38 Sunday, January 17 2021
//
//  Serial.println(rtc.getMicros());        //  (long)    723546
//  Serial.println(rtc.getMillis());        //  (long)    723
//  Serial.println(rtc.getEpoch());         //  (long)    1609459200
//  Serial.println(rtc.getSecond());        //  (int)     38    (0-59)
//  Serial.println(rtc.getMinute());        //  (int)     24    (0-59)
//  Serial.println(rtc.getHour());          //  (int)     3     (0-12)
//  Serial.println(rtc.getHour(true));      //  (int)     15    (0-23)
//  Serial.println(rtc.getAmPm());          //  (String)  pm
//  Serial.println(rtc.getAmPm(true));      //  (String)  PM
//  Serial.println(rtc.getDay());           //  (int)     17    (1-31)
//  Serial.println(rtc.getDayofWeek());     //  (int)     0     (0-6)
//  Serial.println(rtc.getDayofYear());     //  (int)     16    (0-365)
//  Serial.println(rtc.getMonth());         //  (int)     0     (0-11)
//  Serial.println(rtc.getYear());          //  (int)     2021