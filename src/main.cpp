#include <Adafruit_NeoPixel.h>
#include "io.hpp"
#include "main.hpp"

PAMI leH;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du robot...");

  // Initialisation des LEDs
  leH.animateLeds();

  // Initialisation de la musique
  leH.playMelody();

  // Initialisation de la communication série
  Serial.println("Initialisation de la communication série...");
  Serial.println("Prêt !");
}

void loop() {
  // Ne fait rien, tout est en FreeRTOS
}
