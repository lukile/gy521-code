/*
  EEPROM clear

  Sets all of the bytes of the EEPROM to 0
*/

#include <EEPROM.h>

void setup() {
  EEPROM.begin(512);

  //Write a 0 to all 512 bytes of the EEPROM
  for(int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }

  //Print "OK CLEAR" when it's done
  Serial.print();
  Serial.print("OK CLEAR");
  Serial.print();
}

void loop() {

}
