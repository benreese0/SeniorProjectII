#!/usr/bin/python

import socket
import select

#constants
cmd_port = 7777
img_port = 7778
sign_port = 7779
line_port = 7780
pi_addr = '192.168.1.7'
sign_addr = '192.168.1.6'
line_addr = '192.168.1.6'
ctrl_addr = '192.168.1.3'
fourmb = 1024*1024*4

#static sockets
pi_img = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_img.connect( (pi_addr, img_port) )
print("Pi img connection made")
pi_cmd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_cmd.connect( (pi_addr, cmd_port) )
print("Pi cmd connection made")
ctrl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ctrl_sock.connect( (ctrl_addr, cmd_port) )
print("Ctrl connection made")
sign_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sign_sock.connect( (img_addr, img_port) )
print("Sign connection made")
line_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
line_sock.connect( (img_addr, img_port) )
print("Line connection made")

sources = [pi_img, pi_cmd, ctrl_sock, sign_sock, line_sock]

while True:
 try:
  inrdy, outrdy, errdy = select.select(sources,[],sources)
  for src in errdy:
   for sock in sources:
    sock.close()

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
    pi_cmd.sendall(data)
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

