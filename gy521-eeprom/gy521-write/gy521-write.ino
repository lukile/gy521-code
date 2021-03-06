/*
  EEPROM Write
  
  Store values read from analog input 0 into EEPROM.
  These values will stay in the EEPROM when the board 
  is turned off and may be retrieved later by another sketch
*/

#include <EEPROM.h>

//The current address in the EEPROM (which byte we're going to write to next)
int addr = 0;

void setup() {
  EEPROM.begin(512);
}

void loop() {
  //Need to divide by 4 because analog inputs range 
  //from 0 to 1023 and each byte of the EEPROM can 
  //only hold a value from 0 to 255
  int val = analogRead(A0)/4;

  //Write the value to the appropriate byte of the 
  //EEPROM. These values will remain there when the 
  //board is turned off.
  EEPROM.write(addr, val);

  //Advance to the next address. There are 512 bytes in
  //the EEPROM, so go back to 0 when we hit 512.
  //Save all changes to the flash.
  addr = addr + 1;
  if(addr == 512) {
    addr = 0;
    EEPROM.commit();
  }

  delay(100);
}
