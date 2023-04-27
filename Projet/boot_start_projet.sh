#!/bin/bash

# Ce script est censé etre execute automatiquement au demarrage

echo "Démarrage du projet..."

# change permission of python files in cgi-bin folder
echo "Changement des permissions des fichiers Python dans le dossier cgi-bin..."
chmod u+x /home/authier/Desktop/Projet/server/cgi-bin/*.py

# start MQTT broker
echo "Démarrage du broker MQTT..."
sudo systemctl start mosquitto

# attendre que le broker MQTT soit lancé
sleep 2

# start server
echo "Démarrage du serveur Web..."
cd /home/authier/Desktop/Projet/server
python server.py &

# attendre que le serveur Web soit lancé
while ! pgrep -f "python server.py" > /dev/null; do
    sleep 1
done


# start MQTT client
echo "Démarrage du client MQTT..."
cd /home/authier/Desktop/Projet/mqtt_client
python client_raspberry.py &

# attendre que le client MQTT soit lancé
while ! pgrep -f "python client_raspberry.py" > /dev/null; do
    sleep 1
done

echo "Le projet a été démarré avec succès !"
