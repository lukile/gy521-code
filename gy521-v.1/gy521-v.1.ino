//Allows to communicate with I2C devices
#include <Wire.h>

//I2C address of GY-521 (= MPU-6050). If AD0 pin is set to HIGH, I2C address = 0x69
const int MPU_ADDR = 0x68; 

//Variables for accelerometer raw data
int16_t accelerometer_x;
int16_t accelerometer_y;
int16_t accelerometer_z;

//Variables for gyro raw data
int16_t gyro_x;
int16_t gyro_y;
int16_t gyro_z;

//Variable for temperature data
int16_t temperature;

//Temp variable used in convert function
char tmp_str[7];

//Converts int16 to string
char* convert_int16_to_str(int16_t i) {
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
//PWR_MGMT_1 register
  Wire.write(0x6B);
//Set to 0 (wakes up MPU-6050)
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
//Starting register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.write(0x3B);
//Parameter indicates that the ESP will send a restart. As a result, the connection is kept active
  Wire.endTransmission(false);
//Request a total of 7*2=14 registers
  Wire.requestFrom(MPU_ADDR, 7*2, true);

//"Wire.read()<<8 | Wire.read();" means 2 registers are read and stored in the same variable

//Reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_x = Wire.read()<<8 | Wire.read();
//Reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read();
//Reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L) 
  accelerometer_z = Wire.read()<<8 | Wire.read();
  
//Reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  temperature = Wire.read()<<8 | Wire.read();

//Reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_x = Wire.read()<<8 | Wire.read();
//Reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read();
//Reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read();

//Print datas
  Serial.print("aX = ");
  Serial.print(convert_int16_to_str(accelerometer_x));

  Serial.print(" | aY = ");
  Serial.print(convert_int16_to_str(accelerometer_y));

  Serial.print(" | aZ = ");
  Serial.print(convert_int16_to_str(accelerometer_z));

//The following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | temperature = ");
  Serial.print(temperature/340.00+36.53);

  Serial.print(" | gX = ");
  Serial.print(convert_int16_to_str(gyro_x));

  Serial.print(" | gY = ");
  Serial.print(convert_int16_to_str(gyro_y));

  Serial.print(" | gZ = ");
  Serial.print(convert_int16_to_str(gyro_z));

  Serial.println();

  delay(1000);
}
