#include <MPU6050.h>

#include <SPI.h>

#include<Wire.h>
#include <array>
#include <EEPROM.h>
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
}
void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  
  Accelerometer acc;
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 
  Serial.println();
  Serial.print("AcX : ");Serial.println(AcX);

  //Get total x in m/s²
  meterPerSecondSquared = (AcX * sensitivityScaleFactor)/1000;
  Serial.print("meterPerSecondSquared : ");Serial.println(meterPerSecondSquared);

  savedDatas[iterator] = meterPerSecondSquared;

  iterator++;
 
  delay(1000);
}
