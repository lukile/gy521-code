#include <SPI.h>

#include<Wire.h>
#include <array>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#define SDA 33
#define SCL 32



struct Accelerometer {
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t temperature;
  int16_t gx;
  int16_t gy;
  int16_t gz;
};

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
byte value;
int iterator = 0;
int addr = 0;
byte tracker = 0;
byte t = 0;
Accelerometer savedDatas[292];
Accelerometer getSavedDatas[292];


void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  EEPROM.begin(4096);
  
  EEPROM.get(addr, getSavedDatas);

  Serial.println();
  Serial.print(" getSavedDatas ax 1: ");Serial.println(getSavedDatas[0].ax);
  Serial.print(" getSavedDatas ay 1: ");Serial.println(getSavedDatas[0].ay);
  Serial.print(" getSavedDatas az 1: ");Serial.println(getSavedDatas[0].az);
  Serial.print(" getSavedDatas temp 1: ");Serial.println(getSavedDatas[0].temperature);
  Serial.print(" getSavedDatas gx 1: ");Serial.println(getSavedDatas[0].gx);
  Serial.print(" getSavedDatas gy 1: ");Serial.println(getSavedDatas[0].gy);
  Serial.print(" getSavedDatas gz 1: ");Serial.println(getSavedDatas[0].gz);


  
  Serial.println();
  Serial.print(" getSavedDatas ax 2: ");Serial.println(getSavedDatas[30].ax);
  Serial.print(" getSavedDatas ay 2: ");Serial.println(getSavedDatas[30].ay);
  Serial.print(" getSavedDatas az 2: ");Serial.println(getSavedDatas[30].az);
  Serial.print(" getSavedDatas temp 2: ");Serial.println(getSavedDatas[30].temperature);
  Serial.print(" getSavedDatas gx 2: ");Serial.println(getSavedDatas[30].gx);
  Serial.print(" getSavedDatas gy 2: ");Serial.println(getSavedDatas[30].gy);
  Serial.print(" getSavedDatas gz 2: ");Serial.println(getSavedDatas[30].gz);



  
  Serial.println();
  Serial.print(" getSavedDatas ax 3: ");Serial.println(getSavedDatas[60].ax);
  Serial.print(" getSavedDatas ay 3: ");Serial.println(getSavedDatas[60].ay);
  Serial.print(" getSavedDatas az 3: ");Serial.println(getSavedDatas[60].az);
  Serial.print(" getSavedDatas temp 3: ");Serial.println(getSavedDatas[60].temperature);
  Serial.print(" getSavedDatas gx 3: ");Serial.println(getSavedDatas[60].gx);
  Serial.print(" getSavedDatas gy 3: ");Serial.println(getSavedDatas[60].gy);
  Serial.print(" getSavedDatas gz 3: ");Serial.println(getSavedDatas[60].gz);
  //clear();
}
void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  
  Accelerometer acc;
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  Serial.println();
  Serial.print("AcX : ");Serial.println(AcX);
  Serial.print("AcY : ");Serial.println(AcY);
  Serial.print("AcZ : ");Serial.println(AcZ);
  Serial.print("Tmp : ");Serial.println(Tmp);
  Serial.print("GyX : ");Serial.println(GyX);
  Serial.print("GyY : ");Serial.println(GyY);
  Serial.print("GyZ : ");Serial.println(GyZ);

  acc.ax = AcX;
  acc.ay = AcY;
  acc.az = AcZ;
  acc.temperature = Tmp;
  acc.gx = GyX;
  acc.gy = GyY;
  acc.gz = GyZ;

  savedDatas[iterator] = acc;

  EEPROM.put(addr, savedDatas);
  EEPROM.commit();

  iterator++;
 
  delay(1000);
}
void clear() {
   for(int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }

  //Print "OK CLEAR" when it's done
  Serial.println();
  Serial.println("OK CLEAR");
  Serial.println();
}
