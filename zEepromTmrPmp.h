// zf240811.1339

#include <EEPROM.h>

#define EEPROM_SIZE 64


void saveVariables() {
  EEPROM.put(0, bootCount);
  // EEPROM.put(sizeof(int), tempMoy);
  EEPROM.commit();
}


void loadVariables() {
  EEPROM.get(0, bootCount);
  // EEPROM.get(sizeof(int), tempMoy);
}


// float calculateAverageTemperature() {
//   // Implémentez votre propre logique pour calculer la température moyenne
//   return 25.0; // Exemple de valeur
// }


void zStartEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    USBSerial.println("Failed to initialise EEPROM");
    while (1);
  }
}

