#!/usr/bin/env python

import sys
sys.path.append('librairie')
import lib_base_de_donnee

#-------------------------
# ------ Parametre ------
#-------------------------

periode_rafraichissement = 5  # En seconde

#------------------------- 

# Recupere les dernier valeurs pour chaque capteur dans la base de donnee
derniere_valeur1 = lib_base_de_donnee.lire_derniere_valeur_capteur(1)
derniere_valeur2 = lib_base_de_donnee.lire_derniere_valeur_capteur(2)

# Creer des graphiques des derniers valeurs de capteur
lib_base_de_donnee.creer_graphique_capteur(1)
lib_base_de_donnee.creer_graphique_capteur(2)


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
  <input name="temps_alerte" cols="20"</input>
  <input type="submit" value="Lancer alerte !">
</form>
<br/>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val1" cols="20"</input>
  <input name="val2" cols="20"</input>
  <input type="submit" value="Entrer">
</form>
</body>
""".format(periode_rafraichissement, derniere_valeur1, derniere_valeur2)

print(html)