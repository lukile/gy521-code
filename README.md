# gy521-code
Accelerometer GY-521 (MPU-6050) + LOLIN D32 code base

Important to keep in mind !
EEPROM memory can only save byte datas. 
MPU6050 (= GY-521) retrieve accelerometer datas (x,y,z) and rotations datas (= gyro x,y,z) and even temperature, in int16_t.

As a reminder : int16_t = 16bit = 2 bytes. If we want to keep the precision, we cannot cast to int8_t.
So we have to save datas on 2 "slots" on ESP.
Thereby, we can save datas as bytes on ESP and retrieve them on int16_t;
