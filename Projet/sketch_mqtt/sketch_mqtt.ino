#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "buzzer.h"
#include "lum.h"


struct Lum lum1;
struct Buzzer buzzer1;
struct mailbox_lum luminosity = {.state = EMPTY};
struct mailbox_buzzer buzzer = {.state = EMPTY};

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

void callback(const char[] topic, byte* payload, unsigned int length)
{
  if (strcmp(topic, "music") == 0)
  {
    if (buzzer->state == EMPTY)           // Check if the buzzer mailbox is empty
    {
      buzzer->typemusic = 1;              // Update the type of music asked
      buzzer->state = FULL;               // Set the buzzer mailbox state to full
    }
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
