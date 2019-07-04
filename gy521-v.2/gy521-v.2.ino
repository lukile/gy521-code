#include <ArduinoJson.h>

#include <MPU6050_tockn.h>

#include <SPI.h>

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
 
*/


MPU6050 mpu6050(Wire);


int iterator = 0;
char jsonTest;

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

String user;
String lengthType;
String programType;

const char* ssid = "ssid";  
const char* password = "pass";

void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);

  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  timerStartSession = millis();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting for connection...\n");
  }
  
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 

  user = getCurrentUserFromApi();
  programType = getprogramTypeFromApi();
  lengthType = getLengthTypeFromApi();

 // Serial.print("user : ");Serial.print(user);Serial.println();
 // Serial.print("programType : ");Serial.print(programType);Serial.println();
  //Serial.print("lengthType : ");Serial.print(lengthType);Serial.println(); 
  
  sleep(50);
}

void loop(){
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
  
    if(cpt >= 100 && currentV > 0) {
      averageSpeed = averageArray(arraySpeeds, cpt);
      distance = currentV * ((millis() - timerStartSession)/1000);
      
      Serial.print("\nvitesse moyenne : ");Serial.print(currentV);Serial.print("m/s\n");
      Serial.print("Distance moyenne : ");Serial.print(distance); Serial.print("m\n");
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

String getLengthTypeFromApi() {
  StaticJsonDocument<600> doc;

  String lengthTypefield = "\"lengthType\" : ";

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://192.168.1.18:3000/performances");

    int httpCode = http.GET();
    String payload = http.getString();

    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(error.c_str());
      return "error";
    }

    String datas = doc["data"]["performances"][0]["lengthType"];

    Serial.print("lengthType : ");  Serial.println(datas);

    return datas;
  }
}

String getprogramTypeFromApi() {
    String programTypeField = "\"programType\" : {\"_id\": ";

    if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    //Adapt address (heroku?)
    http.begin("http://192.168.1.18:3000/programs");

    int httpCode = http.GET();
    String payload = http.getString();

    String completeProgramTypeField = programTypeField + payload;
    
    return completeProgramTypeField;
  }
}

void sendToApi() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    //Change
    http.begin("http://192.168.1.18:3000/performances");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("{\"performance\" : {\"datePerformance\" : \"1555000960\",\"speed\" : 76,\"lengthType\" : 26,\"startTime\" : \"1555000960\",\"endTime\" : \"1555000960\",\"distance\" : 200,\"lostWeight\" : 47,\"programType\" : {\"_id\": \"5d0e7b075f1af2c6b6b1a00c\"},\"user\" : \"5d0e60621bdcd81d4236e2bc\"}}");
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  
  } else {
    Serial.println("Error in WiFi connection");
  }
}
