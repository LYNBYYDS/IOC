#ifndef CONFIG_H
#define CONFIG_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128                                    // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                    // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    16                                    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the analog input pin for the photo-resistor
#define LUM_PIN 36

// Define the period for reading the photo-resistor (0.5 seconds)
#define LUM_PERIOD 500000

// Define the period for capture the button (0.02 seconds)
#define BUTTON_PERIOD 20000

// Max timer numbers
#define MAX_WAIT_FOR_TIMER 1

// Pin number of the peripheral
#define BUZZER_PIN 17
#define BUTTON_PIN 23

enum {EMPTY, FULL};

// This function waits for a specified period of time
unsigned int waitFor(int timer, unsigned long period)
{
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];    // Timer array
  unsigned long newTime = micros() / period;                // Calculate current time
  int delta = newTime - waitForTimer[timer];                // Calculate time since last call
  if ( delta < 0 ) delta = 1 + newTime;                     // Handle overflow
  if ( delta ) waitForTimer[timer] = newTime;               // Update timer array
  return delta;
}

#endif
