/*
  EEPROM clear

  Sets all of the bytes of the EEPROM to 0
*/

#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  EEPROM.begin(4096);

  //Write a 0 to all length of the EEPROM
 for(int i = 0; i < 4096; i++) {
    EEPROM.write(i, 0);
  }

  //Print "OK CLEAR" when it's done
  Serial.println();
  Serial.println("OK CLEAR");
  Serial.println();
}

void loop() {

}
