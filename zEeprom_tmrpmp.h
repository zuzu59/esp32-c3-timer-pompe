#define zVERSION        "zf240811.1131"
#define zHOST           "eeprom_save_1"             // ATTENTION, tout en minuscule

#include <EEPROM.h>

#define EEPROM_SIZE 64

int bootCount = 0;
float tempMoy = 0.0;


void saveVariables() {
  EEPROM.put(0, bootCount);
  EEPROM.put(sizeof(int), tempMoy);
  EEPROM.commit();
}

void loadVariables() {
  EEPROM.get(0, bootCount);
  EEPROM.get(sizeof(int), tempMoy);
}


float calculateAverageTemperature() {
  // Implémentez votre propre logique pour calculer la température moyenne
  return 25.0; // Exemple de valeur
}


void setup() {
// Start serial console
  USBSerial.begin(19200);
  USBSerial.setDebugOutput(true);       //pour voir les messages de debug des libs sur la console série !
  delay(300);                          //le temps de passer sur la Serial Monitor ;-)
  USBSerial.println("\n\n\n\n**************************************\nCa commence !"); USBSerial.println(zHOST ", " zVERSION);


  if (!EEPROM.begin(EEPROM_SIZE)) {
    USBSerial.println("Failed to initialise EEPROM");
    while (1);
  }


  loadVariables();
  bootCount++;
  tempMoy = calculateAverageTemperature(); // Fonction hypothétique pour calculer la température moyenne
  saveVariables();

  USBSerial.print("Boot Count: ");
  USBSerial.println(bootCount);
  USBSerial.print("Average Temperature: ");
  USBSerial.println(tempMoy);

  // delay(3000);                          //le temps de passer sur la Serial Monitor ;-)

}

void loop() {
 
  delay(50000); // Attendre 5 secondes avant de sauvegarder à nouveau
}

