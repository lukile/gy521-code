#include <EEPROM.h>

#define EEPROM_SIZE 1

int address = 0;

byte value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  EEPROM.begin(EEPROM_SIZE);

}

void loop() {
  value = EEPROM.read(address);
  Serial.print(address);
   Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  delay(500);
}
