// ESP32-c3 super mini utilisé pour faire juste office de timer pour une pompe de fontaine pour enfants.
// On réveil l'esp32-c3 au moyen du RESET, un timer démarre pour faire fonctionner la pompe pendant une 
// certaine durée. A la fin du timer, on part en mode dsleep infini. 
// Si on appuie sur le RESET pendant que le timer tourne encore, on part directement en mode dsleep infini.
//
// ATTENTION, ce code a été testé sur un esp32-c3 super mini. Pas testé sur les autres boards !
//
#define zVERSION        "zf240811.1405"
#define zHOST           "tmr_pmp_1"             // ATTENTION, tout en minuscule
#define zDSLEEP         0                       // 0 ou 1 !
#define TIME_TO_SLEEP   60                      // dSleep en secondes 
int zDelay1Interval =   1000;                   // Délais en mili secondes pour la boucle loop

/*
Utilisation:

Astuce:

plus valable ici, docu va arriver prochainement
Afin de simplifier au maximum le câblage, j'utilise les deux pins adjacentes 
pour la masse et l'alimentation pour le HTU21D. Il faudra donc configurer 
ces pins en conséquence !
Mais les pins par défaut pour l'I2C sur l'esp32-c3 super mini sont les 8 et 9, 
elles ne sont pas adjacentes aux pins utilisées pour l'alimentation du capteur HTU21D, 
il faut donc les reprogrammer en utilisant l'astuce du Wire.begin(SdaPin,SclPin) AVANT le htu.begin() !


Installation:

Pour les esp32-c3 super mini, il faut:
 * choisir comme board ESP32C3 Dev Module
 * disabled USB CDC On Boot et utiliser USBSerial. au lieu de Serial. pour la console !
 * changer le schéma de la partition à Minimal SPIFFS (1.9MB APP with OTA/190kB SPIFFS)

Pour le WiFiManager, il faut installer cette lib depuis le lib manager sur Arduino:
https://github.com/tzapu/WiFiManager

Pour MQTT, il faut installer la lib (home-assistant-integration):
https://github.com/dawidchyrzynski/arduino-home-assistant

Pour JSON, il faut installer cette lib:
https://github.com/bblanchon/ArduinoJson


Sources:

https://grabcad.com/library/esp32-c3-supermini-1
https://forum.fritzing.org/t/need-esp32-c3-super-mini-board-model/20561
https://chat.mistral.ai/    pour toute la partie API REST et wifiAuto ᕗ
*/




// #define DEBUG true
// #undef DEBUG



// General
const int ledPin = 8;             // the number of the LED pin
const int buttonPin = 9;          // the number of the pushbutton pin

int bootCount = 0;
int zTimeToSleep = 0;


// Sonar Pulse
#include "zSonarpulse.h"


// zEeprom_tmrpmp
#include "zEepromTmrPmp.h"


void setup() {
  // Pulse deux fois pour dire que l'on démarre
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  delay(zSonarPulseWait);

  // Start serial console
  USBSerial.begin(19200);
  USBSerial.setDebugOutput(true);       //pour voir les messages de debug des libs sur la console série !
  delay(300);                          //le temps de passer sur la Serial Monitor ;-)
  USBSerial.println("\n\n\n\n**************************************\nCa commence !"); USBSerial.println(zHOST ", " zVERSION);

  // #if zDSLEEP == 1
  //   //Increment boot number and print it every reboot
  //   ++bootCount;
  //   sensorValue4 = bootCount;
  //   USBSerial.println("Boot number: " + String(bootCount));
  //   // Configuration du dsleep
  //   // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //   // USBSerial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  // #endif

  // Start EEPROM
  zStartEEPROM();

  loadVariables();
  bootCount++;
  saveVariables();

  USBSerial.print("Boot Count: ");
  USBSerial.println(bootCount);




  // go go go
  USBSerial.println("\nC'est parti !\n");



  #if zDSLEEP == 1
    // Partie dsleep. On va dormir !
    USBSerial.println("Going to sleep now");
    delay(200);
    USBSerial.flush(); 
    esp_deep_sleep_start();
    USBSerial.println("This will never be printed");
  #endif

}

void loop() {
  zTimeToSleep++;
  USBSerial.print("zTimeToSleep:");
  USBSerial.println(zTimeToSleep);



  // Délais non bloquant pour le sonarpulse et l'OTA
  zDelay1(zDelay1Interval);
}


// Délais non bloquant pour le sonarpulse et l'OTA
void zDelay1(long zDelayMili){
  long zDelay1NextMillis = zDelayMili + millis(); 
  while(millis() < zDelay1NextMillis ){
    // Un petit coup sonar pulse sur la LED pour dire que tout fonctionne bien
    sonarPulse();
  }
}

