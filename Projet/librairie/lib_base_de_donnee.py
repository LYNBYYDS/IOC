import json
import os
import matplotlib.pyplot as plt
import time

#--------------------------------------------------
# ------ Parametres ------
#--------------------------------------------------

# Fichier base de donnee avec les valeurs de capteur
dir_fichier_bd = "base_de_donnee/capteurs_valeur.json"
dir_graph = "base_de_donnee/capteur"
dir_ecriture_fichier_bd = "../server/base_de_donnee/capteurs_valeur.json"

#--------------------------------------------------
# ------ Signature des fonctions ------
#--------------------------------------------------


def ecrire_valeur_capteur(num_capteur: int, valeur: float) -> None:
    """
    Écrit dans la base de données la valeur du capteur donné par son numéro.

    :param num_capteur: Le numéro du capteur.
    :param valeur: La valeur du capteur.
    """
    pass

def lire_derniere_valeur_capteur(num_capteur: int) -> float:
    """
    Renvoie la dernière valeur enregistrée pour le capteur donné par son numéro.

    :param num_capteur: Le numéro du capteur.
    :return: La dernière valeur enregistrée pour le capteur donné, ou None si aucune valeur n'a été enregistrée.
    """
    pass

def creer_graphique_capteur(num_capteur: int, nb_valeur: int) -> None:
    """
    Crée un graphique des nb_valeur dernières valeurs du capteur spécifié par son numéro et sauvegarde le graphique
    au format PNG dans le dossier 'img'.

    :param num_capteur: Le numéro du capteur.
    """
    pass

#--------------------------------------------------
# ------ code des fonctions ------
#--------------------------------------------------

def ecrire_valeur_capteur(num_capteur, valeur):

    # Vérifier si le fichier de base de données existe
    if not os.path.exists(dir_ecriture_fichier_bd):
        # Créer une nouvelle base de données avec une liste vide de capteurs
        capteurs_valeur = []
        with open(dir_ecriture_fichier_bd, 'w') as f:
            json.dump(capteurs_valeur, f)

    # Vérifier si le fichier de verrouillage existe
    while os.path.exists(dir_ecriture_fichier_bd + '.lock'):
        time.sleep(0.1)

    # Créer le fichier de verrouillage
    open(dir_ecriture_fichier_bd + '.lock', 'w').close()

    try:
        # Ouvrir la base de données JSON
        with open(dir_ecriture_fichier_bd, 'r') as f:
            capteurs_valeur = json.load(f)

        # Ajouter une nouvelle valeur de capteur à la base de données
        capteurs_valeur.append({"capteur": num_capteur, "valeur": valeur})

        # Limiter le nombre de valeurs de capteurs à 1000 valeurs
        if len(capteurs_valeur) > 1000:
            capteurs_valeur = capteurs_valeur[-1000:]

        # Enregistrer les modifications dans la base de données JSON
        with open(dir_ecriture_fichier_bd, 'w') as f:
            json.dump(capteurs_valeur, f)
            
    finally:
        # Supprimer le fichier de verrouillage
        os.remove(dir_ecriture_fichier_bd + '.lock')

#--------------------------------------------------

def lire_derniere_valeur_capteur(num_capteur):
    # Vérifier si le fichier de base de données existe
    if not os.path.exists(dir_fichier_bd):
        # Créer une nouvelle base de données avec une liste vide de capteurs
        capteurs_valeur = []
        with open(dir_fichier_bd, 'w') as f:
            json.dump(capteurs_valeur, f)

    # Vérifier si le fichier de verrouillage existe
    while os.path.exists(dir_fichier_bd + '.lock'):
        time.sleep(0.1)

    # Créer le fichier de verrouillage
    open(dir_fichier_bd + '.lock', 'w').close()

    try:
        # Ouvrir la base de données JSON
        with open(dir_fichier_bd, 'r') as f:
            capteurs_valeur = json.load(f)

        # Récupérer la dernière valeur du capteur dans la base de données
        derniere_valeur = None
        for valeur in reversed(capteurs_valeur):
            if valeur["capteur"] == num_capteur:
                derniere_valeur = valeur["valeur"]
                break

        return derniere_valeur

    finally:
        # Supprimer le fichier de verrouillage
        os.remove(dir_fichier_bd + '.lock')

#--------------------------------------------------

def creer_graphique_capteur(num_capteur, nb_valeur):
    # Vérifier si le fichier de base de données existe
    if not os.path.exists(dir_fichier_bd):
        # Créer une nouvelle base de données avec une liste vide de capteurs
        capteurs_valeur = []
        with open(dir_fichier_bd, 'w') as f:
            json.dump(capteurs_valeur, f)

    # Vérifier si le fichier de verrouillage existe
    while os.path.exists(dir_fichier_bd + '.lock'):
        time.sleep(0.1)

    # Créer le fichier de verrouillage
    open(dir_fichier_bd + '.lock', 'w').close()

    try:
        # Ouvrir la base de données JSON
        with open(dir_fichier_bd, 'r') as f:
            capteurs_valeur = json.load(f)

        # Récupérer les nb_valeur dernières valeurs du capteur dans la base de données
        valeurs_capteur = []
        for valeur in reversed(capteurs_valeur):
            if valeur["capteur"] == num_capteur:
                valeurs_capteur.append(valeur["valeur"])
                if len(valeurs_capteur) == nb_valeur:
                    break

        # Créer le graphique
        plt.plot(valeurs_capteur)
        plt.xlabel("<-- dernieres valeurs | anciennes valeurs --> ")
        plt.ylabel("Valeur du capteur [%]")
        plt.title(f"Graphique des {nb_valeur} dernières valeurs du capteur {num_capteur}")
        plt.ylim(0, 100)
        plt.savefig(dir_graph + f"_{num_capteur}.png")
        plt.close()

    finally:
        # Supprimer le fichier de verrouillage
        os.remove(dir_fichier_bd + '.lock')
