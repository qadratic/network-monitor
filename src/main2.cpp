// code only for testing, do not include in final build

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "functions.h"
#include "constants.h"

#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C // Default I2C address for 0.96" OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(115200);

  Wire.begin(4, 5); 

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  displayAnnouncement(display, "Network Monitor Starting...Network Monitor Starting...");
}

void loop()
{
    delay(1000);
}