#!/usr/bin/env python
import os, time

s2fName = '/tmp/s2f_LA'
if not os.path.exists(s2fName):
   os.mkfifo(s2fName)
s2f = open(s2fName,'w+')

f2sName = '/tmp/f2s_LA'
if not os.path.exists(f2sName):
   os.mkfifo(f2sName)
f2s = open(f2sName,'r')

while str != "stop\n" : # Tant que "stop" n'est entre le serveur ne s'arrete pas
   str = f2s.readline() # On lit la ligne dans la fifo f2s
   print '%s' % str,    # On affiche la variable de lecture de la fifo


f2s.close()
s2f.close()
