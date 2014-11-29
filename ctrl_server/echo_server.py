#!/usr/bin/python

import socket
import select

#constants
cmd_port = 7777
img_port = 7780
pi_addr = '192.168.1.7'
img_addr = '192.168.1.6'
ctrl_addr = '192.168.1.6'
fourmb = 1024*1024*4

#static sockets
pi_img = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_img.connect( (pi_addr, 7778) )
print("Pi img connection made")
pi_cmd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_cmd.connect( (pi_addr, 7777) )
print("Pi cmd connection made")
ctrl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ctrl_sock.connect( (ctrl_addr, 7777) )
print("Ctrl connection made")
img_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
img_sock.connect( (img_addr, 7779) )
print("Img connection made")

sources = [pi_img, pi_cmd, img_sock, ctrl_sock]

while True:
 try:
  inrdy, outrdy, errdy = select.select(sources,[],[])
  for src in inrdy:
   if src == pi_img: #new image from pi
    data = pi_img.recv(fourmb)
    #while data.length() >0: 
     #   print("lenn" + str(data.len()))
    img_sock.sendall(data)
      #  data = pi_img.recv(fourmb)
   elif src == pi_cmd:#data from pi
    data = pi_cmd.recv(1024)
    print("Arduino:" + str(data))
   elif src == img_sock:#cmd from img_proc
    data = img_sock.recv(1024)
    print("IMGCMD:"+ str(data))
    #pi_cmd.sendall(data)
   elif src == ctrl_sock:#cmd from ctrl
    data = ctrl_sock.recv(1024)
    pi_cmd.sendall(data)
   else:
    print("got unexpected data:")
    print(src.recv(1024))
 except KeyboardInterrupt:
  break
 except ConnectionRefusedError:
  break
print('Exiting')

