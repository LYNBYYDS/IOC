#!/bin/bash

echo "Démarrage du projet..."

# change permission of python files in cgi-bin folder
echo "Changement des permissions des fichiers Python dans le dossier cgi-bin..."
chmod u+x ./server/cgi-bin/*.py

# start MQTT broker
echo "Démarrage du broker MQTT..."
sudo systemctl start mosquitto

# start MQTT client
echo "Démarrage du client MQTT..."
cd ./mqtt_client
python client_raspberry.py &
cd ..

# start server
echo "Démarrage du serveur..."
cd ./server
python server.py &
cd ..

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
