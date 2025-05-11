#include "pami.hpp"
#include <Adafruit_NeoPixel.h>
#include "io.hpp"
#include "main.hpp"
#include "music.hpp"
#include "BluetoothSerial.h"
#include "PCF8575.h"

// PCF8575 pcf8575(0x20);

BluetoothSerial SerialBT;

Adafruit_NeoPixel strip(NUM_LEDS, LED, NEO_GRB + NEO_KHZ800);

int hue = 0; // Variable pour la teinte des LEDs
int angle = 0; // Variable pour l'angle de rotation des servos
unsigned char alpha = 0; // luminosité des LEDs

void bluetoothInterface();

PAMI::PAMI(){
    pinMode(BUZZER, OUTPUT);
    // pcf8575.pinMode(7, OUTPUT);
    // pcf8575.pinMode(11, OUTPUT);

    // pcf8575.pinMode(1, OUTPUT);
    // pcf8575.pinMode(2, OUTPUT);
    // pcf8575.pinMode(3, OUTPUT);
    // pcf8575.pinMode(4, OUTPUT);
    // pcf8575.pinMode(5, OUTPUT);
    // pcf8575.pinMode(6, OUTPUT);

    // pcf8575.pinMode(16, OUTPUT);
    // pcf8575.pinMode(15, OUTPUT);
    // pcf8575.pinMode(14, OUTPUT);
    // pcf8575.pinMode(13, OUTPUT);
    // pcf8575.pinMode(0, OUTPUT);

    // pcf8575.begin();

    // pinMode(IO13, OUTPUT);
    // pinMode(IO2, OUTPUT);
    // pinMode(IO26, OUTPUT);
    xTaskCreate(
        [](void* pvParameters) { static_cast<PAMI*>(pvParameters)->bluetoothInterface(); },  // Tâche Bluetooth
        "Bluetooth",
        4096,
        NULL,
        1,
        NULL
    );
}

void PAMI::bluetoothInterface() {
    // Initialisation de la communication série
    SerialBT.begin("PAMIH");
    SerialBT.println("Initialisation de la communication série...");
    SerialBT.println("Prêt !");

    while (true) {
        if(SerialBT.available()) {
            String command = SerialBT.readStringUntil('\n');
            // Serial.print("Commande reçue : ");
            // Serial.println(command);
            int separatorIndex = command.indexOf(':');
            String id;
            
            if (separatorIndex != -1) {
              id = command.substring(0, separatorIndex);  // Extraire l'ID
              id.trim();  // Nettoyer les espaces autour de l'ID
              // Serial.print("id : ");
              // Serial.println(id);
              command = command.substring(separatorIndex + 1);  // Extraire la valeur
              // Serial.print("command : ");
              // Serial.println(command);
            }
            else {
              id = command;
            }

            if (id.startsWith("help")) {
              //
            }
            else if (id.startsWith("JOY")) {
                separatorIndex = command.indexOf(',');
                hue = command.substring(0, separatorIndex).toInt()*65536/360;
                angle = command.substring(0, separatorIndex).toInt();
                // Serial.print("hue : ");
                // Serial.println(hue);
                alpha = command.substring(separatorIndex + 1).toInt();
                // Serial.print("alpha : ");
                // Serial.println(alpha);
            }
            else if (id.startsWith("deploy")) {
                // avancerCanettes();
                // delay(value);
                // arreterCanettes();
            }
            else if (id.startsWith("retract")) {
                // reculerCanettes();
                // delay(value);
                // arreterCanettes();
            }
            else Serial.println("Commande inconnue");
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); // Délai pour éviter une boucle trop rapide
    }
}

// === Tâche 2 : Animation LED fluide ===
void animateLedsTask(void *pvParameters) {
    strip.begin();
    strip.show();
  
    hue = 0;
  
    while (true) {
      for (int i = 0; i < NUM_LEDS; i++) {
        int a = map(analogRead(POT), 0, 4095, 0, 255);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, a*a/255));
      }
      strip.show();
      hue += 256; // petit pas de teinte
      if (hue >= 65536) hue = 0;
      vTaskDelay(30 / portTICK_PERIOD_MS); // délai fluide
    }
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

unsigned char PAMI::readPot(){
  return map(analogRead(POT), 0, 4095, 0, 255);
}

// === Tâche 3 : Animation LED Bluetooth ===
void bluetoothLedsTask(void *pvParameters) {
  strip.begin();
  strip.show();

  hue = 0;

  while (true) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.ColorHSV(hue, 255, alpha*alpha/255));
    }
    strip.show();
    vTaskDelay(30 / portTICK_PERIOD_MS); // délai fluide
  }
}

void PAMI::bluetoothLeds(){
  xTaskCreate(
    bluetoothLedsTask,      // Tâche LEDs
      "BLEDs",
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
      while(!digitalRead(BUT1)) vTaskDelay(100); // Attendre que le bouton soit pressé
      while(digitalRead(BUT1)) vTaskDelay(100); // Attendre que le bouton soit relâché
  
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

// void mot1(int speed, char direction) {
//   analogWrite(IO13, speed); 
//   if (direction == 'F') {
//     pcf8575.digitalWrite(14, HIGH);
//     pcf8575.digitalWrite(13, LOW);
//   } else if (direction == 'R') {
//     pcf8575.digitalWrite(14, LOW);
//     pcf8575.digitalWrite(13, HIGH);
//   }
// }

// void mot2(int speed, char direction) {
//   analogWrite(IO2, speed); 
//   if (direction == 'F') {
//     pcf8575.digitalWrite(7, HIGH);
//     pcf8575.digitalWrite(0, LOW);
//   } else if (direction == 'R') {
//     pcf8575.digitalWrite(7, LOW);
//     pcf8575.digitalWrite(0, HIGH);
//   }
// }

// void mot3(int speed, char direction) {
//   analogWrite(IO26, speed); 
//   if (direction == 'F') {
//     pcf8575.digitalWrite(5, HIGH);
//     pcf8575.digitalWrite(6, LOW);
//   } else if (direction == 'R') {
//     pcf8575.digitalWrite(5, LOW);
//     pcf8575.digitalWrite(6, HIGH);
//   }
// }

// void navigationTask(void *pvParameters) {

//   while (true){
//     Serial.print("Angle : ");
//     Serial.println(angle);
//     Serial.print("Alpha : ");
//     Serial.println(alpha);
//     // Convertir l'angle en radians
//     float rad = angle * 3.14 / 180.0;

//     // Calculer les composantes X et Y du vecteur de mouvement
//     float vx = cos(rad);
//     float vy = sin(rad);

//     // Projections du vecteur de vitesse sur les axes des moteurs
//     // Les moteurs sont espacés de 120°, donc leurs angles sont 0°, 120°, et 240°
//     float m1 = vx * cos(0) + vy * sin(0);               // Moteur 1
//     float m2 = vx * cos(2*PI/3) + vy * sin(2*PI/3);     // Moteur 2
//     float m3 = vx * cos(4*PI/3) + vy * sin(4*PI/3);     // Moteur 3

//     // Normalisation pour que le max des valeurs soit dans [-1, 1]
//     float maxVal = max(max(abs(m1), abs(m2)), abs(m3));
//     if (maxVal > 1.0) {
//       m1 /= maxVal;
//       m2 /= maxVal;
//       m3 /= maxVal;
//     }

//     // Appliquer la vitesse alpha
//     int v1 = int(abs(m1) * alpha);
//     int v2 = int(abs(m2) * alpha);
//     int v3 = int(abs(m3) * alpha);

//     // Déterminer les directions
//     char d1 = m1 >= 0 ? 'F' : 'R';
//     char d2 = m2 >= 0 ? 'F' : 'R';
//     char d3 = m3 >= 0 ? 'F' : 'R';

//     // Contrôle des moteurs
//     mot1(v1, d1);
//     mot2(v2, d2);
//     mot3(v3, d3);

//     Serial.print("M1 : ");
//     Serial.print(v1);
//     Serial.print(" ");
//     Serial.println(d1);
//     Serial.print(" M2 : ");
//     Serial.print(v2);
//     Serial.print(" ");
//     Serial.println(d2);
//     Serial.print(" M3 : ");
//     Serial.print(v3);
//     Serial.print(" ");
//     Serial.println(d3);

//     vTaskDelay(50);
//   }
// }


// void PAMI::navigation(){
//     xTaskCreatePinnedToCore(
//         navigationTask,  // Tâche navigation
//         "Navigation",
//         2048,
//         NULL,
//         1,
//         NULL,
//         0 // cœur 0
//     );
// }