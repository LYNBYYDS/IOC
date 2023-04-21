import paho.mqtt.client as mqtt
import sys
sys.path.append('../librairie')
import lib_base_de_donnee

MQTT_ADDRESS = ‘192.168.153.159’
MQTT_PORT = 1883
MQTT_USER = ‘IOC’
MQTT_PASSWORD = ‘1234’
MQTT_TOPIC_LUM = ‘luminosity’
const char* clientID = "RaspberryPI";     // MQTT client ID

def on_connect(client, userdata, flags, rc):
    print(‘Connected with result code ‘ + str(rc))
    client.subscribe(MQTT_TOPIC_LUM)
    #client.subscribe(MQTT_TOPIC_HUMD)

def on_message(client, userdata, msg):
    if msg.client_id == "capteur1"
        ecrire_valeur_capteur(1, int(msg.payload))
        print ('le vapteur' + 1 + 'envoie donne la valeur' + int(msg.payload))  # message de debut
    else if msg.client_id == "capteur2"
        ecrire_valeur_capteur(2, int(msg.payload))
        print ('le vapteur' + 2 + 'envoie donne la valeur' + int(msg.payload))  # message de debut


def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, MQTT_PORT)
    mqtt_client.loop_forever()

 
if __name__ == ‘__main__’:
    print(‘Debut main’)
    main()