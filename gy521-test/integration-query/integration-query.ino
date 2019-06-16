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

#include <MPU6050.h>
#include <SPI.h>
#include<Wire.h>
#include <array>
#define SDA 33
#define SCL 32

MPU6050 mpu;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX;
float meterPerSecond;
float sensitivityScaleFactor = 0.49;
int iterator = 0;
int addr = 0;
float savedDatas[3000];
int isSessionFinished = 0;
unsigned long firstTime;
unsigned long currentTime;
int lenarraySize = 3000;

const char* ssid = "ssid-name";
const char* password = "wifi-password";
int counter = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //Set accelerometer range to +-16g (provide more precision??), by default at +-2g
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  Serial.print("mpu accel range : ");Serial.println(mpu.getFullScaleAccelRange());
  Serial.println();

  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting for connection...");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  isSessionFinished = checkLightSensor();
  if(isSessionFinished == 0){
    calculSpeed();
    if(isSessionFinished == 1) {
      Serial.println("isSessionFinished =  ");Serial.println(isSessionFinished);
      //When session is over send data to api
      //sendPerformancesRequest(averageArray(savedDatas, 3000)); // Get average speed of user;
      //Pour avoir la distance effectué durant l'entrainement => averageSpeed*sessionTime;
    }
  } 
}

void calculSpeed(){
  firstTime = millis();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 
  Serial.println();
  Serial.print("AcX : ");Serial.println(AcX);
  
  currentTime = (firstTime-millis());
  meterPerSecond = ((AcX * sensitivityScaleFactor)/1000)*currentTime; //Get total x in m/s
  Serial.print("meterPerSecondSquared : ");Serial.println(meterPerSecond);
  savedDatas[iterator] = meterPerSecond;
  iterator++;

  //Must be adapted when session is over, 5 is just for tests
  if(iterator == 5) {
    isSessionFinished = 1;
  }
  
  delay(1000);
}

int checkLightSensor(){
  // Check if session start or end with the sensor
  return 0;
}

float averageArray (float *array, int arraySize){
  long sum = 0; 
  for (int i = 0 ; i < arraySize ; i++){  
    sum += array [i] ;
  }
  return  ((float) sum) / arraySize;
}

void sendPerformancesRequest(float average) {
  if (WiFi.status() != WL_CONNECTED) {
    HTTPClient http;

    http.begin("localhost:27017/WaterDigitBuddy/performances/");
    http.addHeader("Content-Type", "application/json");

    //int httpCode = http.POST(average);
    String payload = http.getString();

    //Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  
  } else {
    Serial.println("Error in WiFi connection");
  }
  //Must be modified and send request only when session is over
  delay(3000);
}
