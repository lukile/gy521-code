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
int addressGet = 0;
byte tracker = 0;
byte t = 0;
Accelerometer savedDatas[3];
Accelerometer acc;

void setup(){
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  EEPROM.begin(512);

    Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
 
  acc.ax = AcX;
  acc.ay = AcY;
  acc.az = AcZ;
  acc.temperature = Tmp/340.00+36.53;
  acc.gx = GyX;
  acc.gy = GyY;
  acc.gz = GyZ;
  /*Serial.print("AcX = "); Serial.print(acc.ax);
  Serial.print(" | AcY = "); Serial.print(acc.ay);
  Serial.print(" | AcZ = "); Serial.print(acc.az);
  Serial.print(" | Tmp = "); Serial.print(acc.temperature);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(acc.gx);
  Serial.print(" | GyY = "); Serial.print(acc.gy);
  Serial.print(" | GyZ = "); Serial.println(acc.gz);*/

  
  //Serial.print(" acc.ax : ");Serial.println(acc.ax);
  //Serial.print(" acc.ay : ");Serial.println(acc.ay);


  //addr += sizeof(float);
  //EEPROM.put(addr, acc);
  //EEPROM.commit();

  //Serial.print(" ax : ");Serial.println(acc.ax);
  //Serial.print(" ay : ");Serial.println(acc.ay);



  //clear();


  Accelerometer acc2;
  EEPROM.get(addr, acc2);

  Serial.print(" ax : ");Serial.println(acc2.ax);
  Serial.print(" ay : ");Serial.println(acc2.ay);
  

  //clear();
}
void loop(){
  


  
  /*Serial.print("eeprom write anything : ");Serial.print(tracker,DEC);
  Serial.println();
  Serial.print("ax value : ");Serial.print(acc.ax,DEC);
  Serial.println();
  acc.ax = 0;
  Serial.println(acc.ax,DEC);

  //t = EEPROM_readAnything(0,acc);
  Serial.println(t,DEC);
  
  
  

  //EEPROM.write(addr, acc.ax);
  //addr = addr + 1;

  //if(addr == 512) {erial.print(addr);
  //Serial.println();
  //Serial.print(" ax value : "); Serial.print(acc.ax);
  //Serial.println();
   // addr = 0;
    //EEPROM.commit();
  //}
  //Serial.print("Ax : ");Serial.println(savedDatas[iterator].ax);
  //value = EEPROM.read(addr);
  //Serial.print("address = ");
  //Serial.print(" value saved : ");Serial.print(value);
  //Serial.println();

  delay(1000);
 // clear();*/
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
