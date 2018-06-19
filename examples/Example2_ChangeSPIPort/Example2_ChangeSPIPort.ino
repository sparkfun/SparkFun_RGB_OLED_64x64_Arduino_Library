#include "SparkFun_RGB_OLED_64x64.h"

#define DRIVER_SPI SPI

#define CS_PIN 0
#define DC_PIN 1
#define RST_PIN 2

SSD1357 driver;

void setup() {
  Serial.begin(9600);
  DRIVER_SPI.begin();
  driver.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI);
  while(!Serial){};

}

void loop() {

}
