#!/usr/bin/env python

import sys
sys.path.append('librairie')
import lib_base_de_donnee

#-------------------------
# ------ Parametre ------
#-------------------------

periode_rafraichissement = 6  # En seconde

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
</head>
<body>
<img src= "../img/LTSS_logo_reduit.png" style="width: 30%;">
<br/>
<img src= "../base_de_donnee/capteur_1.png" style="width: 30%;">
<img src= "../base_de_donnee/capteur_2.png" style="width: 30%;">
<br/>
Dernieres valeurs :
<br/>
Capteur 1 : {} || Capteur 2 : {}
<br/><br/>
<form method="POST" action="alerte.py">
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
""".format(periode_rafraichissement, derniere_valeur1, derniere_valeur2)

print(html)