#!/usr/bin/env python

import sys
sys.path.append('../librairie')
import lib_base_de_donnee

#-------------------------
# ------ Parametre ------
#-------------------------

periode_rafraichissement = 100  # En seconde

#------------------------- 

# Recupere les dernier valeurs pour chaque capteur dans la base de donnee
derniere_valeur1 = lib_base_de_donnee.lire_derniere_valeur_capteur(1)
derniere_valeur2 = lib_base_de_donnee.lire_derniere_valeur_capteur(2)

# Creer des graphiques des derniers valeurs de capteur
lib_base_de_donnee.creer_graphique_capteur(1, 50)
lib_base_de_donnee.creer_graphique_capteur(2, 50)


html="""
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT="{}; URL=/cgi-bin/main.py">
  <link rel="stylesheet" type="text/css" href="../css/style.css">
</head>
<body>
  <div style="display: flex; flex-direction: column; align-items: center;">
    <div style="display: flex; justify-content: center; align-items: center; padding: 1em;">
      <img src="../img/LTSS_logo_reduit.png" style="width: 50%; height: auto; max-width: 400px;">
    </div>
    <div style="display: flex; justify-content: center; align-items: center;">
      <div style="flex: 1; max-width: 50%; padding: 1em;">
        <img src="../base_de_donnee/capteur_1.png" style="width: 100%; height: auto; max-width: 400px;">
      </div>
      <div style="flex: 1; max-width: 50%; padding: 1em;">
        <img src="../base_de_donnee/capteur_2.png" style="width: 100%; height: auto; max-width: 400px;">
      </div>  
    </div>
  </div>

  Dernieres valeurs :
  <br/>
  Capteur 1 : {} %
  <br/>
  Capteur 2 : {} %
  <br/>
  <br/>
  <form class="centered-form" method="POST" action="main.py">
    <label for="Rafraichissement">Rafraichissement automatique des valeurs toutes les {} secondes.</label>
    <br/>
    <input type="submit" value="Nouvelles valeurs !">
  </form>
  <br/>
  <form class="centered-form" method="POST" action="alerte.py">
    <label for="temps_alerte">Temps de l'alerte en secondes :</label>
    <select name="temps_alerte" id="temps_alerte">
      <option value="5">5</option>
      <option value="10">10</option>
      <option value="30">30</option>
      <option value="60">60</option>
    </select>
    <br/>
    <input type="submit" value="Lancer alerte !">
  </form>
</body>
""".format(periode_rafraichissement, derniere_valeur1, derniere_valeur2, periode_rafraichissement)

print(html)