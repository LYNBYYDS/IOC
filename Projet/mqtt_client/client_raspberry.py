import paho.mqtt.client as mqtt
import sys
sys.path.append('../librairie')
import lib_base_de_donnee

MQTT_ADDRESS = '192.168.153.159'
MQTT_PORT = 1883
MQTT_USER = 'IOC'
MQTT_PASSWORD = '1234'
MQTT_TOPIC_LUM1 = 'luminosity1'
MQTT_TOPIC_LUM2 = 'luminosity2'
MQTT_TOPIC_ALERT = 'alert'


def on_connect(client, userdata, flags, rc):
    print("Connected avec code d'erreur :" + str(rc))
    client.subscribe(MQTT_TOPIC_LUM1)
    client.subscribe(MQTT_TOPIC_LUM2)

def on_message(client, userdata, msg):
    if str(msg.topic) == MQTT_TOPIC_LUM1:
        lib_base_de_donnee.ecrire_valeur_capteur(1, int(msg.payload))
    elif str(msg.topic) == MQTT_TOPIC_LUM2:
        lib_base_de_donnee.ecrire_valeur_capteur(2, int(msg.payload))


def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, MQTT_PORT)
    mqtt_client.loop_forever()

 
if __name__ == '__main__':
    print('Debut main')
    main()
