#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

long timer = 0;
long timerStartSession = 0;
double arraySpeeds[1000];
int cpt = 0;
int endSession = 0;
double currentV = 0;
double currentX = 0;

double previousV = 0;
double previousX = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  timerStartSession = millis();
}

void loop() {
  Serial.print("cpt : ");Serial.print(cpt);Serial.print("\n");
  Serial.print("endSession : ");Serial.print(endSession);Serial.print("\n");

  if(endSession == 0){
    mpu6050.update();
    Serial.print("accX : ");Serial.print(mpu6050.getAccX());
    Serial.println("=======================================================\n");
    timer = millis();
    
    currentV = previousV + mpu6050.getAccX()*9.8*0.05;
    currentX = previousX + currentV * 0.05;
    previousV = currentV;
    previousX = currentX;
    arraySpeeds[cpt] = currentV;
    Serial.print("vitesse : ");Serial.print(currentV); Serial.print("m/s");
    cpt += 1;
    if(cpt >= 30){
      endSession == 1;
    }
    delay(50);
  } else {
    double averageSpeed = averageArray(arraySpeeds, cpt);
    Serial.print("vitesse moyenne : ");Serial.print(currentV); Serial.print("m/s");
    Serial.print("Distance moyenne : ");Serial.print(currentV*((millis()-timerStartSession)/1000)); Serial.print("m");
  }
  
}

double averageArray (double *array, int arraySize){
  double sum = 0; 
  for (int i = 0 ; i < arraySize ; i++){  
    sum += array [i] ;
  }
  return  ((double) sum) / arraySize;
}
