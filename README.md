# esp32-c3-timer-pompe
Petit timer pour une pompe de fonctaine pour enfants

ESP32-c3 super mini utilisé pour faire juste office de timer pour une pompe de fontaine pour enfants.

On réveil l'esp32-c3 au moyen du RESET, un timer démarre pour faire fonctionner la pompe pendant une certaine durée. A la fin du timer, on part en mode dsleep infini. 

Si on appuie sur le RESET pendant que le timer tourne encore, on part directement en mode dsleep infini.
