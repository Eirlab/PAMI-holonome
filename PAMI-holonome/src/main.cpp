#include <Adafruit_NeoPixel.h>
#include "io.hpp"
#include "main.hpp"
#include <DRV89xx.h>
#include "PCF8575.h"
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

PCF8575 pcf8575(0x20);

// DRV89xx motor_driver(CS, 0, 0);

PAMI leH;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du robot...");

  // Set pinMode to OUTPUT
  pcf8575.pinMode(7, OUTPUT);
  pcf8575.pinMode(11, OUTPUT);

  pcf8575.pinMode(1, OUTPUT);
  pcf8575.pinMode(2, OUTPUT);
  pcf8575.pinMode(3, OUTPUT);
  pcf8575.pinMode(4, OUTPUT);
  pcf8575.pinMode(5, OUTPUT);
  pcf8575.pinMode(6, OUTPUT);

  pcf8575.pinMode(16, OUTPUT);
  pcf8575.pinMode(15, OUTPUT);
  pcf8575.pinMode(14, OUTPUT);
  pcf8575.pinMode(13, OUTPUT);
  pcf8575.pinMode(0, OUTPUT);

  pcf8575.begin();

  pinMode(IO13, OUTPUT);
  pinMode(IO2, OUTPUT);
  pinMode(IO26, OUTPUT);

  digitalWrite(IO13, LOW);
  digitalWrite(IO2, LOW);
  digitalWrite(IO26, LOW);
	

  // motor_driver.configMotor(0, 3, 4, 0, 0);  // set up motor 0 to use half bridges 1,5 and pwm channel 0
  // motor_driver.begin(); // init motor driver

  // spi_dev.begin();

  // uint8_t buffer[32];

  // buffer[0] = 0x8F; // we'll reuse the same buffer
  // // spi_dev.write_then_read(buffer, 1, buffer, 2, false);
  // spi_dev.write(buffer, 1);

  // Initialisation des LEDs
  leH.animateLeds();
  // leH.bluetoothLeds();

  // Initialisation de la musique
  leH.playMelody();

  // leH.navigation();

  // Serial.println("Adafruit VL53L0X test");
  lox.begin();

  // Initialisation de la communication série
  Serial.println("Pret !");
}

void mot1(int speed, char direction) {
  analogWrite(IO13, speed); 
  if (direction == 'F') {
    pcf8575.digitalWrite(14, HIGH);
    pcf8575.digitalWrite(13, LOW);
  } else if (direction == 'R') {
    pcf8575.digitalWrite(14, LOW);
    pcf8575.digitalWrite(13, HIGH);
  }
}

void mot2(int speed, char direction) {
  analogWrite(IO2, speed); 
  if (direction == 'F') {
    pcf8575.digitalWrite(7, HIGH);
    pcf8575.digitalWrite(0, LOW);
  } else if (direction == 'R') {
    pcf8575.digitalWrite(7, LOW);
    pcf8575.digitalWrite(0, HIGH);
  }
}

void mot3(int speed, char direction) {
  analogWrite(IO26, speed); 
  if (direction == 'F') {
    pcf8575.digitalWrite(5, HIGH);
    pcf8575.digitalWrite(6, LOW);
  } else if (direction == 'R') {
    pcf8575.digitalWrite(5, LOW);
    pcf8575.digitalWrite(6, HIGH);
  }
}

void loop() {

  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect dat
    if (measure.RangeMilliMeter < 100) {
      mot1(0, 'F');
      mot2(0, 'F');
      mot3(0, 'F');
    } else {

      float angleRad = angle * PI / 180.0;
  
    // Direction du vecteur de translation
    float vx = -cos(angleRad);
    float vy = sin(angleRad);
  
    // Vecteurs de direction des moteurs (angles des roulettes)
    float m1Angle = -PI / 6;  // -30° = 330°
    float m2Angle = PI / 6;   // 30°
    float m3Angle = -PI / 2;  // -90° = 270°
  
    // Projeter le vecteur (vx, vy) sur les axes moteurs
    float m1 = vx * cos(m1Angle) + vy * sin(m1Angle);
    float m2 = vx * cos(m2Angle) + vy * sin(m2Angle);
    float m3 = vx * cos(m3Angle) + vy * sin(m3Angle);
  
    // Normalisation
    float maxVal = max(max(abs(m1), abs(m2)), abs(m3));
    if (maxVal > 1.0) {
      m1 /= maxVal;
      m2 /= maxVal;
      m3 /= maxVal;
    }
  
    // Appliquer alpha
    int v1 = int(abs(m1) * alpha);
    int v2 = int(abs(m2) * alpha);
    int v3 = int(abs(m3) * alpha);
  
    char d1 = m1 >= 0 ? 'F' : 'R';
    char d2 = m2 >= 0 ? 'F' : 'R';
    char d3 = m3 >= 0 ? 'F' : 'R';
  
    // Commande des moteurs
    mot1(v1, d1);
    mot2(v2, d2);
    mot3(v3, d3);
  }
}

    vTaskDelay(50);
  

  // Serial.print("Angle : ");
  //   Serial.println(angle);
  //   Serial.print("Alpha : ");
  //   Serial.println(alpha);
  //   // Convertir l'angle en radians
  //   float rad = angle * 3.14 / 180.0;

  //   // Calculer les composantes X et Y du vecteur de mouvement
  //   float vx = cos(rad);
  //   float vy = sin(rad);

  //   // Projections du vecteur de vitesse sur les axes des moteurs
  //   // Les moteurs sont espacés de 120°, donc leurs angles sont 0°, 120°, et 240°
  //   float m1 = vx * cos(0) + vy * sin(0);               // Moteur 1
  //   float m2 = vx * cos(2*PI/3) + vy * sin(2*PI/3);     // Moteur 2
  //   float m3 = vx * cos(4*PI/3) + vy * sin(4*PI/3);     // Moteur 3

  //   // Normalisation pour que le max des valeurs soit dans [-1, 1]
  //   float maxVal = max(max(abs(m1), abs(m2)), abs(m3));
  //   if (maxVal > 1.0) {
  //     m1 /= maxVal;
  //     m2 /= maxVal;
  //     m3 /= maxVal;
  //   }

  //   // Appliquer la vitesse alpha
  //   int v1 = int(abs(m1) * alpha);
  //   int v2 = int(abs(m2) * alpha);
  //   int v3 = int(abs(m3) * alpha);

  //   // Déterminer les directions
  //   char d1 = m1 >= 0 ? 'F' : 'R';
  //   char d2 = m2 >= 0 ? 'F' : 'R';
  //   char d3 = m3 >= 0 ? 'F' : 'R';

  //   // Contrôle des moteurs
  //   mot1(v1, d1);
  //   mot2(v2, d2);
  //   mot3(v3, d3);

  //   Serial.print("M1 : ");
  //   Serial.print(v1);
  //   Serial.print(" ");
  //   Serial.println(d1);
  //   Serial.print(" M2 : ");
  //   Serial.print(v2);
  //   Serial.print(" ");
  //   Serial.println(d2);
  //   Serial.print(" M3 : ");
  //   Serial.print(v3);
  //   Serial.print(" ");
  //   Serial.println(d3);

  //   vTaskDelay(50);

  // Serial.println("Arret");

  // mot1(0, 'F');
  // mot2(0, 'F');
  // mot3(0, 'F');

  // digitalWrite(IO13, LOW); // 1
  // digitalWrite(IO2, LOW); // 2
  // digitalWrite(IO26, LOW); // 3

  // pcf8575.digitalWrite(16, HIGH);
  // pcf8575.digitalWrite(15, LOW);

  // pcf8575.digitalWrite(14, HIGH);
  // pcf8575.digitalWrite(13, LOW);

  // pcf8575.digitalWrite(7, HIGH);
  // pcf8575.digitalWrite(0, LOW);

  // vTaskDelay(1000);                    // wait for 1sec

  // Serial.println("1");

  // mot1(255, 'F');

  // vTaskDelay(500);

  // mot1(255, 'R');

  // vTaskDelay(500);

  // mot1(0, 'F');

  // digitalWrite(IO13, HIGH); // 1

  // pcf8575.digitalWrite(16, HIGH);
  // pcf8575.digitalWrite(15, LOW);

  // pcf8575.digitalWrite(14, LOW);
  // pcf8575.digitalWrite(13, LOW);

  // pcf8575.digitalWrite(7, LOW);
  // pcf8575.digitalWrite(0, LOW);

  // vTaskDelay(1000);                    // wait for 1sec

  // digitalWrite(IO13, LOW); // 1

  // Serial.println("2");

  // mot2(255, 'F');

  // vTaskDelay(500);

  // mot2(255, 'R');

  // vTaskDelay(500);

  // mot2(0, 'F');

  // digitalWrite(IO2, HIGH); // 2

  // pcf8575.digitalWrite(16, LOW);
  // pcf8575.digitalWrite(15, LOW);

  // pcf8575.digitalWrite(14, HIGH);
  // pcf8575.digitalWrite(13, LOW);

  // pcf8575.digitalWrite(7, LOW);
  // pcf8575.digitalWrite(0, LOW);

  // vTaskDelay(1000);                    // wait for 1sec
  
  // digitalWrite(IO2, LOW); // 2

  // Serial.println("3");

  // mot3(255, 'F');

  // vTaskDelay(500);

  // mot3(255, 'R');

  // vTaskDelay(500);

  // mot3(0, 'F');

  // digitalWrite(IO26, HIGH); // 3

  // pcf8575.digitalWrite(16, LOW);
  // pcf8575.digitalWrite(15, LOW);

  // pcf8575.digitalWrite(14, LOW);
  // pcf8575.digitalWrite(13, LOW);

  // pcf8575.digitalWrite(7, HIGH);
  // pcf8575.digitalWrite(0, LOW);

  // vTaskDelay(1000);                    // wait for 1sec

  // digitalWrite(IO26, LOW); // 3

  // pcf8575.digitalWrite(11, HIGH);
  // pcf8575.digitalWrite(1, HIGH);
  // pcf8575.digitalWrite(2, LOW);
  // pcf8575.digitalWrite(3, HIGH);
  // pcf8575.digitalWrite(4, HIGH);
  // pcf8575.digitalWrite(5, HIGH);
  // pcf8575.digitalWrite(6, HIGH);

  // Serial.println("Looping...");



  // // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  // for (byte i=0;i<255;i++) {
  //   motor_driver.setMotor(0, i, DRV89xx_FORWARD);   // ramp up speed to full, then wrap around
  //   motor_driver.updateConfig();  // writes new motor values to driver
  //   // Serial.println(i);
  //   vTaskDelay(25);                    // wait for a 25ms
  // }

  // // apply brakes for 1sec
  // motor_driver.setMotor(0, 0, DRV89xx_BRAKE);   
  // motor_driver.updateConfig();  // writes new motor values to driver
  // vTaskDelay(1000);                    // wait for 1sec

  // pcf8575.digitalWrite(7, LOW);

  // pcf8575.digitalWrite(11, LOW);
  // pcf8575.digitalWrite(1, LOW);
  // pcf8575.digitalWrite(2, LOW);
  // pcf8575.digitalWrite(3, LOW);
  // pcf8575.digitalWrite(4, LOW);
  // pcf8575.digitalWrite(5, LOW);
  // pcf8575.digitalWrite(6, LOW);
  // digitalWrite(IO13, LOW);

  // vTaskDelay(1000);                    // wait for 1sec

  // pcf8575.digitalWrite(7, LOW);
  
  // for (byte i=0;i<255;i++) {
  //   motor_driver.setMotor(0, i, DRV89xx_REVERSE);   // ramp up speed to full, then wrap around
  //   motor_driver.updateConfig();  // writes new motor values to driver
  //   delay(25);                    // wait for a 25ms
  // }
  // //motor_driver.debugConfig();

  // // Disable motors and Sleep for 1sec
  // // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
  // motor_driver.disableMotor(0);   // disconnect bridge
  // motor_driver.updateConfig();  // writes new motor values to driver
  // vTaskDelay(500);                    // wait for a 25ms
}
