#include "PubSubClient.h"
#include "WiFi.h"
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

// Define a structure to hold the data for the luminosity mailbox
struct mailbox_lum 
{
  int state;                                                // Whether the mailbox is empty or full
  int lumPercent;                                           // The percentage of luminosity (0-100)
};

// Define a structure to hold the data for the luminosity sensor
struct Lum 
{
  int timer;                                                // The timer index for the luminosity sensor
  unsigned long period;                                     // The period for reading the luminosity sensor (in microseconds)
};

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

// Function to sets up the Lum structure with a specified period
void setup_Lum(struct Lum * lum, unsigned long period, mailbox_lum * luminosity)
{
  lum->timer = 0;                                           // set the timer number to 0
  lum->period = period;                                     // set the period of the lum sensor
}

// Function to read the value of the photoresistor and convert it to a percentage
int getLuminosity() 
{
  int lumValue = analogRead(LUM_PIN);                       // Read the analog value from the photoresistor
  int lumPercent = map(lumValue, 4095, 800, 0, 100);        // Convert the value to a percentage
  lumPercent = lumPercent > 99 ? 99 : lumPercent;           // Limit the maximum percentage to 99
  return lumPercent;                                        // Return the calculated percentage
}

// Function to update the luminosity mailboxe
void loop_Lum(struct Lum * lum, mailbox_lum * luminosity) 
{
  if (!(waitFor(lum->timer,lum->period))) return;           // Wait for the specified period
  int lumPercent = getLuminosity();                         // Get the current luminosity percentage
  if (luminosity->state == EMPTY)                           // Check if the luminosity mailbox is empty
  {
    luminosity->lumPercent = lumPercent;                    // Update the current luminosity percentage
    luminosity->state = FULL; // Set the luminosity mailbox state to full
  }
}

// WiFi
const char* ssid = "SSID";                
const char* password = "PASSWORD";

// MQTT
const char* mqtt_server = "127.0.0.1"; 
const int mqtt_port = 8000;
const char* mqtt_username = "LA";                           // MQTT username
const char* mqtt_password = "LAthebest";                    // MQTT password
const char* clientID = "ESP32LA";                           // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient espClient;

// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, espClient);

 

// Custom function to connect to the MQTT broker via WiFi
void setup_MQTT(){
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    client.connect(clientID, mqtt_username, mqtt_password);
  }
}

void loop_MQTT(mailbox_lum * luminosity)
{
  // Poll the mailbox
  if (luminosity->state == FULL) {
    // There is a new value in the mailbox
    // Publish the value to MQTT
    String lum = String(luminosity->lumPercent);
    client.publish("luminosity", lum.c_str());
  }
}

struct Lum lum1;
struct mailbox_lum luminosity = {.state = EMPTY};

void setup() {
  Serial.begin(9600);
  setup_MQTT();
  setup_Lum(&lum1, LUM_PERIOD, &luminosity);
}

void loop() {
  Serial.setTimeout(2000);
  loop_Lum(&lum1, &luminosity);
   loop_MQTT(&luminosity);
}
