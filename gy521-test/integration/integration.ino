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
float meterPerSecond;
float sensitivityScaleFactor = 0.49;
int iterator = 0;
int addr = 0;
float savedDatas[3000];
int isSessionFinished = 0;
unsigned long firstTime;
unsigned long currentTime;

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
  isSessionFinished = checkLightSensor();
  if(isSessionFinished == 0){
    calculSpeed();
  }

  //float averageSpeed = averageArray(savedDatas, 3000); // Get average speed of user;
  //Pour avoir la distance effectué durant l'entrainement => averageSpeed*sessionTime;
}

void calculSpeed(){
  firstTime = millis();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  
  Accelerometer acc;
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 
  Serial.println();
  Serial.print("AcX : ");Serial.println(AcX);
  
  currentTime = (firstTime-millis());
  meterPerSecond = ((AcX * sensitivityScaleFactor)/1000)*currentTime; //Get total x in m/s
  Serial.print("meterPerSecondSquared : ");Serial.println(meterPerSecondSquared);
  savedDatas[iterator] = meterPerSecond;
  iterator++;
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
  return  ((float) sum) / lenarraySize;
}
