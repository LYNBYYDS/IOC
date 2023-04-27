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

# ferme la page Web des données si elle est ouverte
if pgrep -f "python -m http.server 8000" > /dev/null; then
  echo "Fermeture de la page Web des données..."
  xdotool search --onlyvisible --class "Navigator" windowactivate --sync key --clearmodifiers "Ctrl+l" "Ctrl+w"
fi

echo "Le projet a été arrêté avec succès !"
