#!/bin/bash

echo "Démarrage du projet..."

# change permission of python files in cgi-bin folder
echo "Changement des permissions des fichiers Python dans le dossier cgi-bin..."
chmod u+x ./server/cgi-bin/*.py

# start MQTT broker
echo "Démarrage du broker MQTT..."
sudo systemctl start mosquitto

# attendre que le broker MQTT soit lancé
echo "Attente de démarrage du broker MQTT..."
while ! nc -z localhost 1883; do
  sleep 1
done

# start MQTT client
echo "Démarrage du client MQTT..."
cd ./mqtt_client
python client_raspberry.py &
cd ..

# attendre que le client MQTT soit lancé
echo "Attente de démarrage du client MQTT..."
while ! pgrep -f client_raspberry.py > /dev/null; do
  sleep 1
done

# start server
echo "Démarrage du serveur..."
cd ./server
python server.py &
cd ..

# attendre que le serveur Web soit lancé
echo "Attente de démarrage du serveur Web..."
while ! nc -z localhost 8000; do
  sleep 1
done

# demande à l'utilisateur s'il veut lancer la page Web des données
read -p "Voulez-vous ouvrir la page Web des données ? (y/n)" choice
case "$choice" in 
  y|Y ) 
    echo "Ouverture de la page Web des données..."
    xdg-open "http://localhost:8000"
    ;;
  n|N ) 
    echo "Vous avez choisi de ne pas ouvrir la page Web des données."
    ;;
  * ) 
    echo "Choix invalide. La page Web des données ne sera pas ouverte."
    ;;
esac

echo "Le projet a été démarré avec succès !"
