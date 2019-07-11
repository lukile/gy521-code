#include <TimeLib.h>
#include <Time.h>

#include <WiFi.h>
#include <time.h>

const char* ssid = "under-wifi";
const char* password = "4F3EC76435781E341151852682";


void setup() {
  Serial.begin(9600);

   WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
 /*Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }*/
  Serial.println(""); 
  
}

void loop() {
    /*Serial.print(getCurrentDate());
    Serial.println();
    Serial.print(getCurrentTime());
    Serial.println();*/

    String finalString = getCurrentDate() + getCurrentTime(); 

    Serial.print(finalString);
    Serial.println();

    
  delay(1000);
}

String getCurrentDate() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String date;

  if(localTime->tm_mon < 10 && localTime->tm_mday < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + zero + "" + ((String)(localTime->tm_mon + 1)) + "-" + zero + "" + ((String)localTime->tm_mday) + "T");
    return date;
  }
  if(localTime->tm_mon < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + zero + "" + ((String)(localTime->tm_mon + 1)) + "-" + ((String)localTime->tm_mday) + "T");
    return date;
  }
  if(localTime->tm_mday < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + ((String)(localTime->tm_mon + 1)) + "-" + zero + "" + ((String)localTime->tm_mday) + "T");
    return date;
  } 
  
  if(localTime->tm_mon > 10 && localTime->tm_mday > 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + ((String)(localTime->tm_mon + 1)) + "-" + ((String)localTime->tm_mday) + "T");
    return date;
  }
}

String getCurrentTime() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String tim3;

  if(localTime->tm_hour < 10 && localTime->tm_min < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_min < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_min < 10 && localTime->tm_sec < 10) {
     tim3 = ((String)localTime->tm_hour) + ":" + zero + "" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
     return tim3;
  }
  if(localTime->tm_sec < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if (localTime->tm_min < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if (localTime->tm_hour < 10) {
    tim3 = zero + "" + ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if(localTime->tm_sec > 10 && localTime->tm_min > 10 && localTime->tm_hour > 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  }
}

struct tm* getLocalDateTime() {
  struct timespec ts;
  
  time_t currentTime;
  struct tm *localTime;
  struct tm *nanoLocalTime;
  
  time( &currentTime );
  currentTime -= 2*30*60;
  localTime= localtime(&currentTime);


  return localTime;
}
