import json
import random
import time
import os

# Vérifier si le fichier de base de données existe
if not os.path.exists('capteurs_valeur.json'):
    # Créer une nouvelle base de données avec une liste vide de capteurs
    capteurs = []
    with open('capteurs_valeur.json', 'w') as f:
        json.dump(capteurs, f)

# Boucle principale
while True:
    # Vérifier si le fichier de verrouillage existe
    while os.path.exists('capteurs_valeur.lock'):
        time.sleep(1)

    # Créer le fichier de verrouillage
    open('capteurs_valeur.lock', 'w').close()

    try:
        # Ouvrir la base de données JSON
        with open('capteurs_valeur.json', 'r') as f:
            capteurs = json.load(f)

        # Ajouter une nouvelle valeur de capteur à la base de données
        nouvelle_valeur_1 = random.randint(0, 100)
        nouvelle_valeur_2 = random.randint(0, 100)
        capteurs.append({"capteur": 1, "valeur": nouvelle_valeur_1})
        capteurs.append({"capteur": 2, "valeur": nouvelle_valeur_2})

        # Limiter le nombre de valeurs de capteurs à 1000 valeurs
        if len(capteurs) > 1000:
            capteurs = capteurs[-1000:]

        # Enregistrer les modifications dans la base de données JSON
        with open('capteurs_valeur.json', 'w') as f:
            json.dump(capteurs, f)

    finally:
        # Supprimer le fichier de verrouillage
        os.remove('capteurs_valeur.lock')

    # Attendre une seconde avant d'enregistrer la prochaine valeur de capteur
    time.sleep(1)
