#!/usr/bin/env python

import http.server

port = 8000
address = ("172.23.153.119", port)

server = http.server.HTTPServer

handler = http.server.CGIHTTPRequestHandler
handler.cgi_directories = ["/cgi-bin"]

httpd = server(address, handler)

print(f"Serveur démarré sur le PORT {port}")

httpd.serve_forever()