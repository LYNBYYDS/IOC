#!/usr/bin/env python
import cgi, os, time, sys

form = cgi.FieldStorage()
val1 = form.getvalue('val1')
val2 = form.getvalue('val2')

#s2fName = '/tmp/s2f_fw'
#f2sName = '/tmp/f2s_fw'
#s2f = open(s2fName,'w+')
#f2s = open(f2sName,'r',0)

#s2f.write("w %s %s\n" % (val1, val2))
#s2f.flush()
#res = f2s.readline()
#f2s.close()
#s2f.close()

html = """
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT="1; URL=/cgi-bin/main.py">
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val1" cols="20" value="%s"></input>
  <input name="val2" cols="20" value="%s"></input>
  <input type="submit" value="Entrer">
  set %s %s
</form>
</body>
""" % (val1, val2, val1, val2)

print(html)
