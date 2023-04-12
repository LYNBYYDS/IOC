#!/usr/bin/env python
import cgi, os, time, sys

form = cgi.FieldStorage()
temps_alerte = form.getvalue('temps_alerte')

html="""
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT=2; URL=/cgi-bin/main.py">
</head>
<body>
<img src= "../img/LTSS_logo_reduit.png" style="width: 30%;">
<br/>
<img src= "../base_de_donnee/capteur_1.png" style="width: 30%;">
<img src= "../base_de_donnee/capteur_2.png" style="width: 30%;">

<br/><br/>
<body>
	<h1>L'alerte a ete lancee</h1>
</body>
</body>
"""
print(html)


