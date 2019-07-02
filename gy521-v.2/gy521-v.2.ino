#include <MPU6050.h>

#include <SPI.h>

#include<Wire.h>
#include <array>
#include <EEPROM.h>

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


MPU6050 mpu;

struct Accelerometer {
  int16_t ax;
};

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX;
float meterPerSecondSquared;
float sensitivityScaleFactor = 0.49;
int iterator = 0;
int addr = 0;
float savedDatas[3000];
char jsonTest;


const char* ssid = "ssid";
const char* password = "password";

void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //Set accelerometer range to +-16g (provide more precision??), by default at +-2g
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting for connection...");
  }
  
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 

  Serial.print("mpu accel range : ");Serial.println(mpu.getFullScaleAccelRange());
  Serial.println();
}
void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);

  /*getPCtime(); //try to get time sync from pc
  if(DateTime.available()) {
    unsigned long
  }*/
  
  Accelerometer acc;
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 
  Serial.println();
  Serial.print("AcX : ");Serial.println(AcX);

  //Get total x in m/s²
  meterPerSecondSquared = (AcX * sensitivityScaleFactor)/1000;
  Serial.print("meterPerSecondSquared : ");Serial.println(meterPerSecondSquared);

  savedDatas[iterator] = meterPerSecondSquared;


  if(iterator == 10) {
    sendToApi(meterPerSecondSquared);
    sleep(50);
  }

  iterator++;
 
  delay(1000);
}

void sendToApi(float meterPerSecond) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    //Change
    http.begin("localhost:3000/performances/");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("{\"performance\" : {\"datePerformance\" : \"1555000960\",\"speed\" : 76,\"lengthType\" : 25,\"startTime\" : \"1555000960\",\"endTime\" : \"1555000960\",\"distance\" : 200,\"lostWeight\" : 47,\"programType\" : {\"_id\": \"5d0e7b075f1af2c6b6b1a00c\"},\"user\" : \"5d0e60621bdcd81d4236e2bc\"}}");
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  
  } else {
    Serial.println("Error in WiFi connection");
  }
}
