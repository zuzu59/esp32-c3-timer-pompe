// ESP32-c3 super mini utilisé pour faire juste office de timer pour une pompe de fontaine pour enfants.
// On réveil l'esp32-c3 au moyen du RESET, un timer démarre pour faire fonctionner la pompe pendant une 
// certaine durée. A la fin du timer, on part en mode dsleep infini. 
// Si on appuie sur le RESET pendant que le timer tourne encore, on part directement en mode dsleep infini.
//
// ATTENTION, ce code a été testé sur un esp32-c3 super mini. Pas testé sur les autres boards !
//
#define zVERSION        "zf240810.1519"
#define zHOST           "tmr_pmp_1"             // ATTENTION, tout en minuscule
#define zDSLEEP         1                       // 0 ou 1 !
#define TIME_TO_SLEEP   60                      // dSleep en secondes 
int zDelay1Interval =   5000;                   // Délais en mili secondes pour la boucle loop

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




#define DEBUG true
// #undef DEBUG



// General
const int ledPin = 8;             // the number of the LED pin
const int buttonPin = 9;          // the number of the pushbutton pin


// Sonar Pulse
#include "zSonarpulse.h"


// // WIFI
// #include "zWifi.h"


// // OTA WEB server
// #include "otaWebServer.h"


// // MQTT
// #include "zMqtt.h"

float sensorValue1 = 0;  // variable to store the value coming from the sensor 1
float sensorValue2 = 0;  // variable to store the value coming from the sensor 2
float sensorValue3 = 0;  // variable to store the value coming from the sensor 3
float sensorValue4 = 0;  // variable to store the value coming from the sensor 4
float sensorValue5 = 0;  // variable to store the value coming from the sensor 5



// LittleFS
#include "zlittlefs.h"


// Temperature sensor
#include "zTemperature.h"

#if zDSLEEP == 1
  // Deep Sleep
  #define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
  // #define TIME_TO_SLEEP  300      /* Time ESP32 will go to sleep (in seconds) */
  RTC_DATA_ATTR int bootCount = 0;
#endif


void setup() {
  // Il faut lire la température tout de suite au début avant que le MCU ne puisse chauffer !
  readSensor();

  // Pulse deux fois pour dire que l'on démarre
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  delay(zSonarPulseWait);

  // Start serial console
  USBSerial.begin(19200);
  USBSerial.setDebugOutput(true);       //pour voir les messages de debug des libs sur la console série !
  delay(3000);                          //le temps de passer sur la Serial Monitor ;-)
  USBSerial.println("\n\n\n\n**************************************\nCa commence !"); USBSerial.println(zHOST ", " zVERSION);

  #if zDSLEEP == 1
    //Increment boot number and print it every reboot
    ++bootCount;
    sensorValue4 = bootCount;
    USBSerial.println("Boot number: " + String(bootCount));
    // Configuration du dsleep
    // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // USBSerial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  #endif


  // Lit la config
    USBSerial.println("\n\nLit la config\n");

    #ifdef DEBUG
      mountFS();
      listDir(LittleFS, "/", 0);              // List the directories up to one level beginning at the root directory
      readFile(LittleFS, "/config.json");     // Read the complete file
    #endif

    // saveConfig();
    readConfig();


  // Start WIFI
  // zStartWifi();
  // sensorValue3 = WiFi.RSSI();

  // // Start OTA server
  // otaWebServer();

  // // Connexion au MQTT
  // USBSerial.println("\n\nConnect MQTT !\n");
  // ConnectMQTT();

  // go go go
  USBSerial.println("\nC'est parti !\n");

  // Envoie toute la sauce !
  zEnvoieTouteLaSauce();
  USBSerial.println("\nC'est envoyé !\n");

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
  // Envoie toute la sauce !
  // zEnvoieTouteLaSauce();

  // Délais non bloquant pour le sonarpulse et l'OTA
  zDelay1(zDelay1Interval);
}


// Envoie toute la sauce !
void zEnvoieTouteLaSauce(){

  USBSerial.print("bootCount:");
  USBSerial.print(bootCount);



  // Lit les températures
  // readSensor();

  // Envoie les mesures au MQTT
  // sendSensorMqtt();

  // Graphe sur l'Arduino IDE les courbes des mesures
  // USBSerial.print("sensor1:");
  // USBSerial.print(sensorValue1);
  // USBSerial.print(",tempInternal1:");
  // USBSerial.print(tempInternal1);
  // USBSerial.print(",tempInternal2:");
  // USBSerial.print(tempInternal2);

  // USBSerial.print(",temp_HTU21D:");
  // USBSerial.print(sensorValue5);
  // USBSerial.print(",hum_HTU21D:");
  // USBSerial.print(sensorValue2);

  // USBSerial.print(",sensor3:");
  // USBSerial.print(sensorValue3);
  // USBSerial.print(",sensor4:");
  // USBSerial.print(sensorValue4);
  USBSerial.println("");
}


// Délais non bloquant pour le sonarpulse et l'OTA
void zDelay1(long zDelayMili){
  long zDelay1NextMillis = zDelayMili + millis(); 
  while(millis() < zDelay1NextMillis ){
    // OTA loop
    // server.handleClient();
    // Un petit coup sonar pulse sur la LED pour dire que tout fonctionne bien
    sonarPulse();
  }
}

