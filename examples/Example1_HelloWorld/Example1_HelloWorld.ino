#include "SparkFun_RGB_OLED_64x64.h"

#define CS_PIN 0
#define DC_PIN 1
#define RST_PIN 2

SSD1357 driver;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  driver.begin(DC_PIN, RST_PIN, CS_PIN, SPI);

  Serial.println("Hello command line!");
  driver.println("Hello world!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
