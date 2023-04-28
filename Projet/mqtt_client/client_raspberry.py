import paho.mqtt.client as mqtt
import sys
import os

sys.path.append('../librairie')
import lib_base_de_donnee

#-------------------------------------
# ------ Parametre ------
#-------------------------------------

MQTT_ADDRESS = '192.168.1.29'       # Adresse IP du raspberry (A modifier si changement de router wifi)
MQTT_PORT = 1883                  # Port du broker MQTT (ne devrait pas changer)
MQTT_USER = 'IOC'                 # Nom du user MQTT (ne devrait pas changer)
MQTT_PASSWORD = '1234'            # Mdp du user MQTT (ne devrait pas changer)

#-------------------------------------

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
    mqtt_client.loop_start()

    # Ouverture de la FIFO
    try:
        os.mkfifo('Web_to_MQTT.fifo')
    except FileExistsError:
        pass

    with open('Web_to_MQTT.fifo', 'r') as fifo:
        while True:
            # Lecture non-bloquante de la FIFO
            message = fifo.readline().strip()
            if message:
                # Publication du message en MQTT sur le topic 'alert'
                mqtt_client.publish(MQTT_TOPIC_ALERT, int(message))

    mqtt_client.loop_stop()
    mqtt_client.disconnect()


if __name__ == '__main__':
    print('Debut main')
    main()
