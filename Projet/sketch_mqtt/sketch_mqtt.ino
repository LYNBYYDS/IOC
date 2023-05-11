
#include "PubSubClient.h"
#include "WiFi.h"
#include "config.h"
#include "buzzer.h"
#include "lum.h"


//-------------------------
// ------ Parametre ------
//-------------------------

#define captor_id 1

// WiFi
const char* ssid = "ATH-Telecom";             // Nom du wifi 
const char* password = "12345678";            // Mdp wifi

// MQTT
const char* mqtt_server = "192.168.170.159";  // Adresse IP Raspberry

//------------------------- 


struct Lum lum1;
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
  // On subscribe le topic pour recevoir les alerts
  client.subscribe(topic_alert);
}

void callback(char* topic, byte* payload, unsigned int length)
{
  if (strcmp(topic, topic_alert) == 0)
  {
    int temps_alert = 0;
    // On convertie l'octet recu via MQTT payload en un entier temps_alert
    for (int i = 0; i < length; i++) {
      temps_alert |= (payload[i] << (8 * i));
    }
    if (buzzer_mailbox.state == EMPTY)           // Check if the buzzer mailbox is empty
    {
      buzzer_mailbox.second = int(temps_alert);   // Update the type of music asked
      Serial.print(buzzer_mailbox.second);
      buzzer_mailbox.state = FULL;               // Set the buzzer mailbox state to full
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
    if (captor_id == 1){
      client.publish(topic_lum1, lum.c_str());}
    else if (captor_id == 2){
      client.publish(topic_lum2, lum.c_str());}
    luminosity->state = EMPTY;
  }
}

void setup() {
  Serial.begin(9600);
  setup_MQTT();
  setup_Lum(&lum1, LUM_PERIOD, &luminosity);
  setup_Buzzer(&buzzer1, 1000000);              // 0.5s
  client.setCallback(callback);
}

void loop() {
  Serial.setTimeout(2000);
  loop_Lum(&lum1, &luminosity);
  loop_MQTT(&luminosity);
  loop_Buzzer(&buzzer1, &buzzer_mailbox);
  client.loop();
}
