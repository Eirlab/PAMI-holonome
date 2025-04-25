#include "pami.hpp"
#include <Adafruit_NeoPixel.h>
#include "io.hpp"
#include "main.hpp"
#include "music.hpp"
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

Adafruit_NeoPixel strip(NUM_LEDS, LED, NEO_GRB + NEO_KHZ800);

void bluetoothInterface();

PAMI::PAMI(){
    pinMode(BUZZER, OUTPUT);
    xTaskCreate(
        [](void* pvParameters) { static_cast<PAMI*>(pvParameters)->bluetoothInterface(); },  // Tâche Bluetooth
        "Bluetooth",
        4096,
        NULL,
        1,
        NULL
    );
}

float alpha = 1; // Coefficient d'atténuation pour la luminosité des LEDs

void PAMI::bluetoothInterface() {
    // Initialisation de la communication série
    SerialBT.begin("PAMIH");
    SerialBT.println("Initialisation de la communication série...");
    SerialBT.println("Prêt !");

    while (true) {
        if(SerialBT.available()) {
            String command = SerialBT.readStringUntil('\n');
            Serial.println(command);
            alpha = command.toFloat();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Délai pour éviter une boucle trop rapide
    }
}

// === Tâche 2 : Animation LED fluide ===
void animateLedsTask(void *pvParameters) {
    strip.begin();
    strip.show();
  
    int hue = 0;
  
    while (true) {
      for (int i = 0; i < NUM_LEDS; i++) {
        int a = alpha * map(analogRead(POT), 0, 4095, 0, 255);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, a*a/255));
      }
      strip.show();
      hue += 256; // petit pas de teinte
      if (hue >= 65536) hue = 0;
      vTaskDelay(30 / portTICK_PERIOD_MS); // délai fluide
    }
  }

  unsigned char PAMI::readPot(){
    return map(analogRead(POT), 0, 4095, 0, 255);
  }

  void PAMI::animateLeds(){
    xTaskCreate(
        animateLedsTask,      // Tâche LEDs
        "LEDs",
        2048,
        NULL,
        1,
        NULL
    );
}

// === Fonction utilitaire pour générer des ondes carrées ===
void playTone(int freq, int duration) {
    long period = 1000000L / freq;
    long cycles = (duration * 1000L) / period;
  
    for (long i = 0; i < cycles; i++) {
      digitalWrite(BUZZER, HIGH);
      delayMicroseconds(period / 2);
      digitalWrite(BUZZER, LOW);
      delayMicroseconds(period / 2);
    }
  }
  
  // === Tâche 1 : Lecture de musique ===
  void playMelodyTask(void *pvParameters) {
    pinMode(BUZZER, OUTPUT);
  
    // there are two values per note (pitch and duration), so for each note there are four bytes
    int notes=sizeof(melody)/sizeof(melody[0])/2; 
    // this calculates the duration of a whole note in ms (60s/tempo)*4 beats
    int wholenote = (60000 * 4) / tempo;
    int divider = 0, noteDuration = 0;
  
    while (true) {
      while(!digitalRead(BUT1)); // Attendre que le bouton soit pressé
      while(digitalRead(BUT1)); // Attendre que le bouton soit relâché
  
      for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
        
        if(digitalRead(BUT1)) {
          while(digitalRead(BUT1)); // Attendre que le bouton soit relâché
          break; // Sortir de la boucle si le bouton est relâché
        }
  
        // calculates the duration of each note
        divider = melody[thisNote + 1];
        if (divider > 0) {
          // regular note, just proceed
          noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
          // dotted notes are represented with negative durations!!
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5; // increases the duration in half for dotted notes
        }
    
        // we only play the note for 90% of the duration, leaving 10% as a pause
        playTone(melody[thisNote], noteDuration*0.9);
    
        // Wait for the specief duration before playing the next note.
        vTaskDelay(noteDuration);
        
        // stop the waveform generation before the next note.
        // noTone(BUZZER_PIN);
      }
  
      // for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
      //   int freq = melody[i];
      //   int duration = noteDurations[i];
      //   playTone(freq, duration);
      //   vTaskDelay(50 / portTICK_PERIOD_MS);
      // }
      // vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }

void PAMI::playMelody(){
    xTaskCreatePinnedToCore(
        playMelodyTask,       // Tâche musique
        "Musique",
        2048,
        NULL,
        1,
        NULL,
        0 // cœur 0
    );
}