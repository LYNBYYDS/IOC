#!/usr/bin/env python
import cgi, os, time, sys

form = cgi.FieldStorage()
temps_alerte = form.getvalue('temps_alerte')

html="""
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT="{}; URL=/cgi-bin/main.py">
  <link rel="stylesheet" type="text/css" href="../css/style.css">
  <script>
    var compteur = {};
    function decrement() {{
      compteur--;
      document.getElementById("compteur").innerHTML = compteur;
      if (compteur == 0) {{
        clearInterval(interval);
      }}
    }}
    var interval = setInterval(decrement, 1000);
  </script>
</head>
<body>
  <div style="display: flex; justify-content: center; align-items: center; padding: 1em;">
    <img src="../img/LTSS_logo_reduit.png" style="width: 50%; height: auto; max-width: 400px;">
  </div>
  <br/><br/>
  <body>
    <h1>Alerte lancee ! Fin dans <span id="compteur">{}</span> secondes.</h1>
</body>
</body>

""".format(temps_alerte, temps_alerte, temps_alerte)
print(html)

