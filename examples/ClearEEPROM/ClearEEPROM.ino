#include <EEPROM.h>
void setup() {
  EEPROM.begin(32);
  int x = 0;
  EEPROM.put(0,x);
  EEPROM.commit();
}

void loop() {
  
}