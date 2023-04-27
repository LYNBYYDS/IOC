#!/bin/bash

echo "Arrêt du projet..."

# stop server
echo "Arrêt du serveur..."
pkill -f server.py

# stop MQTT client
echo "Arrêt du client MQTT..."
pkill -f client_raspberry.py

# stop MQTT broker
echo "Arrêt du broker MQTT..."
sudo systemctl stop mosquitto

echo "Le projet a été arrêté avec succès !"
