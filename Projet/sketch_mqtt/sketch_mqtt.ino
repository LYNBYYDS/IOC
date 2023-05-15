#include "PubSubClient.h"
#include "WiFi.h"
#include "config.h"
#include "buzzer.h"
#include "lum.h"
#include "oled.h"

#define captor_id 1

// WiFi
const char* ssid = "li";                         // Nom du wifi
const char* password = "woshinibaba";            // Mdp wifi

// MQTT
const char* mqtt_server = "172.20.10.4";  // Adresse IP Raspberry

struct Lum lum1;
struct Oled oled1;
struct Buzzer buzzer1;
struct mailbox_lum luminosity = {.state = EMPTY};
struct mailbox_buzzer buzzer_mailbox = {.state = EMPTY};

// MQTT
const int mqtt_port = 1883;
const char* mqtt_username = "IOC";        // MQTT username
const char* mqtt_password = "1234";       // MQTT password
const char* clientID = "esp32";           // MQTT client ID
const char* topic_lum1 = "luminosity1";   // Nom des topics :
const char* topic_lum2 = "luminosity2";
const char* topic_alert = "alert";

// Initialise the WiFi and MQTT Client objects
WiFiClient espClient;

// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, mqtt_port, espClient);

// Custom function to connect to the MQTT broker via WiFi
void setup_MQTT()
{
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected())
  {
    client.connect(clientID, mqtt_username, mqtt_password);
  }
  // Subscribe to the topic to receive alerts
  client.subscribe(topic_alert);
  Serial.println("Alert topic subscribed");
}

// Callback function for MQTT messages
void callback(char* topic, byte* payload, unsigned int length)
{
  if (strcmp(topic, topic_alert) == 0)
  {
    int temps_alert = 0;
    // Convert the received payload from MQTT to an integer value (temps_alert)
    for (int i = 0; i < length; i++)
    {
      temps_alert |= (payload[i] << (8 * i));
    }
    Serial.print("Received value:");
    if (buzzer_mailbox.state == EMPTY)  // Check if the buzzer mailbox is empty
    {
      buzzer_mailbox.second = temps_alert;  // Update the duration of the buzzer sound
      Serial.println(buzzer_mailbox.second);
      buzzer_mailbox.state = FULL;  // Set the buzzer mailbox state to full
      displayAlert(temps_alert);    // Display the alert on the OLED display
    }
  }
}

// Function to publish luminosity data to MQTT
void loop_MQTT(mailbox_lum* luminosity)
{
  // Poll the mailbox
  if (luminosity->state == FULL)
  {
    // There is a new value in the mailbox
    // Publish the value to MQTT
    String lum = String(luminosity->lumPercent);
    if (captor_id == 1)
    {
      client.publish(topic_lum1, lum.c_str());
    }
    else if (captor_id == 2)
    {
      client.publish(topic_lum2, lum.c_str());
    }
    luminosity->state = EMPTY;
  }
}

void setup() {
  // Set up I2C communication on pins 4 and 15
  Wire.begin(4, 15);
  Serial.begin(9600);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ; // Don't proceed, loop forever
  }
  display.clearDisplay();   // Clear the display buffer
  setup_MQTT();             // Set up MQTT connection
  setup_Oled(&oled1, LUM_PERIOD / 2);  // Set up the OLED display with a refresh period
  setup_Lum(&lum1, LUM_PERIOD, &luminosity);  // Set up the luminosity sensor with a sampling period and mailbox reference
  setup_Buzzer(&buzzer1, 200000);  // Set up the buzzer with a duration of 0.2 seconds
  client.setCallback(callback);   // Set the MQTT callback function
}

void loop() {
  Serial.setTimeout(2000);
  loop_Lum(&lum1, &luminosity);       // Check and update the luminosity value
  loop_MQTT(&luminosity);             // Publish the luminosity value to MQTT if there is a new value
  loop_Buzzer(&buzzer1, &buzzer_mailbox);  // Control the buzzer based on the received commands
  client.loop();                     // Maintain the MQTT connection and handle incoming messages
}
