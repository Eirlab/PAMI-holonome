#include <Adafruit_NeoPixel.h>
#include "io.hpp"
#include "main.hpp"
#include <DRV89xx.h>
#include "PCF8575.h"
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

PCF8575 pcf8575(0x20);

DRV89xx motor_driver(CS, 0, 0);

PAMI leH;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du robot...");

  // Set pinMode to OUTPUT
	pcf8575.pinMode(7, OUTPUT);
  pcf8575.pinMode(11, OUTPUT);

	pcf8575.begin();

  motor_driver.configMotor(0, 3, 4, 0, 0);  // set up motor 0 to use half bridges 1,5 and pwm channel 0
  motor_driver.begin(); // init motor driver

  // spi_dev.begin();

  // uint8_t buffer[32];

  // buffer[0] = 0x8F; // we'll reuse the same buffer
  // // spi_dev.write_then_read(buffer, 1, buffer, 2, false);
  // spi_dev.write(buffer, 1);

  // Initialisation des LEDs
  // leH.animateLeds();
  leH.bluetoothLeds();

  // Initialisation de la musique
  leH.playMelody();

  // Serial.println("Adafruit VL53L0X test");
  // if (!lox.begin()) {
  //   Serial.println(F("Failed to boot VL53L0X"));
  //   while(1);
  // }

  // Initialisation de la communication série
  Serial.println("Pret !");
}

void loop() {

  pcf8575.digitalWrite(7, HIGH);

  pcf8575.digitalWrite(11, HIGH);
  pcf8575.digitalWrite(1, HIGH);
  pcf8575.digitalWrite(2, HIGH);
  pcf8575.digitalWrite(3, HIGH);
  pcf8575.digitalWrite(4, HIGH);
  pcf8575.digitalWrite(5, HIGH);
  pcf8575.digitalWrite(6, HIGH);

  Serial.println("Looping...");

  // VL53L0X_RangingMeasurementData_t measure;
    
  // Serial.print("Reading a measurement... ");
  // lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  // if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  //   Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  // } else {
  //   Serial.println(" out of range ");
  // }
  // vTaskDelay(100 / portTICK_PERIOD_MS); // Délai pour éviter une boucle trop rapide

  // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  for (byte i=0;i<255;i++) {
    motor_driver.setMotor(0, i, DRV89xx_FORWARD);   // ramp up speed to full, then wrap around
    motor_driver.updateConfig();  // writes new motor values to driver
    // Serial.println(i);
    vTaskDelay(25);                    // wait for a 25ms
  }

  // apply brakes for 1sec
  motor_driver.setMotor(0, 0, DRV89xx_BRAKE);   
  motor_driver.updateConfig();  // writes new motor values to driver
  vTaskDelay(500);                    // wait for 1sec

  pcf8575.digitalWrite(7, LOW);
  
  for (byte i=0;i<255;i++) {
    motor_driver.setMotor(0, i, DRV89xx_REVERSE);   // ramp up speed to full, then wrap around
    motor_driver.updateConfig();  // writes new motor values to driver
    delay(25);                    // wait for a 25ms
  }
  //motor_driver.debugConfig();

  // Disable motors and Sleep for 1sec
  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
  motor_driver.disableMotor(0);   // disconnect bridge
  motor_driver.updateConfig();  // writes new motor values to driver
  vTaskDelay(500);                    // wait for a 25ms
}
