#include <ArduinoJson.h>

#include <MPU6050_tockn.h>

#include <SPI.h>

#include <time.h>

#include<Wire.h>
#include <array>

#include <HTTPClient.h>

#include <WiFiUdp.h>
#include <ETH.h>
#include <WiFiSTA.h>
#include <WiFiMulti.h>
#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFiScan.h>
#include <WiFiAP.h>
#include <WiFiGeneric.h>
#include <WiFi.h>

#define SDA 33
#define SCL 32

/*
TO BEGIN 
  session date -> compute chronos program at the beginning of session and stock result
  startTime -> compute chronos program at the beginning of session and stock result
 
GET FROM API BEFORE START : 
  lenghtType (25 or 50)
  programType (winter, spring, summer Body)
  user id

SEND TO API AT THE END (update) :
  speed (computed in this program)
  distance (computed in this program)
  endTime -> compute chronos program at the end of session and stock result 

lostWeight?

Define a session and a deviceId in app
 
*/


MPU6050 mpu6050(Wire);


int iterator = 0;

long timer = 0;
long timerStartSession = 0;
double arraySpeeds[1000];
int cpt = 0;
int endSession = 0;
double currentV = 0;
double currentX = 0;
double previousV = 0;
double previousX = 0;

double distance = 0;
double averageSpeed = 0;

float minimalHighAccX = 0.20;
float minimalLowAccX = -0.20;

String datas;

String dateTime;
String startTime;
String endTime;

const char* ssid = "under-wifi";
const char* password = "4F3EC76435781E341151852682";

int addr = 0;
float savedDatas[3000];
char jsonTest;



void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);

  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  timerStartSession = millis();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting for connection...\n");
  }
  
  Serial.println();

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  
  Serial.print("Connected, IP address: \n");
  Serial.println(WiFi.localIP()); 

  datas = getBeginningDatasFromApi(345);

  sleep(20);
}

void loop(){

  if(cpt == 0) {
    dateTime = getCurrentDate();
    startTime = getCurrentTime();
  }
  
  Serial.print("cpt : ");Serial.print(cpt);Serial.print("\n");
  Serial.print("endSession : ");Serial.print(endSession);Serial.print("\n");

  mpu6050.update();
  if(mpu6050.getAccX() > minimalHighAccX || mpu6050.getAccX() < minimalLowAccX) {
    Serial.print("accX > 0.20: "); Serial.print(mpu6050.getAccX());
    Serial.println("==================================================\n");
    timer = millis();
 
    currentV = previousV + mpu6050.getAccX()* 9.8 * 0.05;
    currentX = previousX + currentV * 0.05;
    previousV = currentV;
    previousX = currentX;
    arraySpeeds[cpt] = currentV;

  
    Serial.print("vitesse : ");Serial.print(currentV);Serial.print("m/s\n");
    cpt += 1;
  
    if(cpt >= 20) {
      averageSpeed = averageArray(arraySpeeds, cpt);
      distance = currentV * ((millis() - timerStartSession)/1000);
      
      Serial.print("\nvitesse moyenne : ");Serial.print(currentV);Serial.print("m/s\n");
      Serial.print("Distance moyenne : ");Serial.print(distance); Serial.print("m\n");

      endTime = getCurrentTime();
      sendToApi(datas, dateTime, startTime, averageSpeed, distance, endTime);
      sleep(50);
    }
  }
  delay(50);
}

double averageArray(double *array, int arraySize) {
  double sum = 0;
  for (int i = 0; i < arraySize; i++) {
    sum += array[i];
  }
  return ((double) sum) / arraySize;
}

String getCurrentUserFromApi() {
    StaticJsonDocument<600> doc;

  String userIdField = "\"user\" : ";

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://192.168.1.18:3000/users");

    

    int httpCode = http.GET();
    String payload = http.getString();

    DeserializationError error = deserializeJson(doc, payload);

    const char* lengthT = doc["lengthType"];


    String completeUserField = userIdField + payload;
    
    return completeUserField;
  }
}

String getBeginningDatasFromApi(int deviceId) {
  StaticJsonDocument<600> doc;

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://192.168.1.18:3000/performances/device/" + (String)deviceId);

    int httpCode = http.GET();
    String payload = http.getString();

    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(error.c_str());
      return "error";
    }

    String lengthType = doc["data"]["performances"][0]["lengthType"];
    String programType = doc["data"]["performances"][0]["programType"];
    double session = doc["data"]["performances"][0]["session"];
    String userId = doc["data"]["performances"][0]["user"];

    datas = "lengthType : " + lengthType + ", programType : " + programType + ", session : " + session + ", deviceId : "+ deviceId + ", userId : " + userId;

    //Serial.print("datePerf : ");  Serial.println(datas);

    return datas;
  }
}

void sendToApi(String datas, String dateTime, String startTime, double averageSpeed, double distance, String endTime) {
    HTTPClient http;

    Serial.print("datas : ");Serial.print(datas);
    Serial.println();
    Serial.print("dateTime : ");Serial.print(dateTime);
    Serial.println();
    Serial.print("startTime : ");Serial.print(startTime);
    Serial.println();
    Serial.print("averageSpeed : ");Serial.print(averageSpeed);
    Serial.println();
    Serial.print("distance : ");Serial.print(distance);
    Serial.println();
    Serial.print("endTime : ");Serial.print(endTime);
    Serial.println();



    //Change
   /* http.begin("http://192.168.1.18:3000/performances");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("{\"performance\" : {\"datePerformance\" : \"1555000960\",\"speed\" : 76,\"lengthType\" : 26,\"startTime\" : \"1555000960\",\"endTime\" : \"1555000960\",\"distance\" : 200,\"lostWeight\" : 47,\"programType\" : {\"_id\": \"5d0e7b075f1af2c6b6b1a00c\"},\"user\" : \"5d0e60621bdcd81d4236e2bc\"}}");
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();*/
  

 
  delay(1000);
}

String getCurrentDate() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String date;

  if(localTime->tm_mday < 10 && localTime->tm_mon < 10) {
    date = zero + "" + ((String)localTime->tm_mday) + " " + zero + "" + ((String)(localTime->tm_mon + 1)) + " " + ((String)(localTime->tm_year + 1900));
    return date;
  }
  if(localTime->tm_mday < 10) {
    date = zero + "" + ((String)localTime->tm_mday) + " " + ((String)(localTime->tm_mon + 1)) + " " + ((String)(localTime->tm_year + 1900));
    return date;
  } 
  if(localTime->tm_mon < 10) {
    date = ((String)localTime->tm_mday) + " " + zero + "" + ((String)(localTime->tm_mon + 1)) + " " + ((String)(localTime->tm_year + 1900));
    return date;
  }
  if(localTime->tm_mday > 10 && localTime->tm_mon > 10) {
    date = ((String)localTime->tm_mday) + " " + ((String)(localTime->tm_mon + 1)) + " " + ((String)(localTime->tm_year + 1900));
    return date;
  }
  
  Serial.print("date : ");Serial.print(date);

  Serial.println();
}

String getCurrentTime() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String tim3;

  if(localTime->tm_hour < 10 && localTime->tm_min < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec);
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_min < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec);
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec);
      return tim3;
  }
  if(localTime->tm_min < 10 && localTime->tm_sec < 10) {
     tim3 = ((String)localTime->tm_hour) + ":" + zero + "" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec);
     return tim3;
  }
  if(localTime->tm_sec < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec);
    return tim3;
  } 
  if (localTime->tm_min < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec);
    return tim3;
  } 
  if (localTime->tm_hour < 10) {
    tim3 = zero + "" + ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec);
    return tim3;
  } 
  if(localTime->tm_sec > 10 && localTime->tm_min > 10 && localTime->tm_hour > 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec);
    return tim3;
  }

  Serial.print("time : ");Serial.print(tim3);

  Serial.println();
}


struct tm* getLocalDateTime() {
  time_t currentTime;
  struct tm *localTime;
  time( &currentTime );
  currentTime -= 2*30*60;
  localTime= localtime(&currentTime);

  Serial.println(localTime);

  return localTime;

}
