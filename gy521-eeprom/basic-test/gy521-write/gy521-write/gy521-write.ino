#include <EEPROM.h>

#define EEPROM_SIZE 1

void setup() {
  EEPROM.begin(EEPROM_SIZE);

}

void loop() {

  //Write 9 at address 0
  EEPROM.write(0,9);

  EEPROM.commit();

}
