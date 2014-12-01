#!/usr/bin/python2

import socket
import select
import struct
import time

#constants
cmd_port = 7777
img_port = 7778
lines_port = 7780
pi_addr = '192.168.1.7'
img_addr = '192.168.1.4'
ctrl_addr = '192.168.1.4'
fourmb = 1024*1024*4


currangle = -7 #start left by eight

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
img_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
img_sock.connect( (img_addr, lines_port) )
print("Img connection made")

time.sleep(10);
#startup with centered wheels
print "sending stuff"
pi_cmd.sendall('L7\n')
print "made center"

sources = [pi_img, pi_cmd, img_sock, ctrl_sock]
print "sending more stuff"
pi_cmd.sendall('L30\n')
pi_cmd.sendall('R30\n')
pi_cmd.sendall('L7\n')
time.sleep(4);
print "sent more turns"
while True:
 try:
  inrdy, outrdy, errdy = select.select(sources,[],[])
  for src in inrdy:
   if src == pi_img: #new image from pi
    data = pi_img.recv(fourmb)
    img_sock.sendall(data)
   elif src == pi_cmd:#data from pi
    data = pi_cmd.recv(1024)
    print("Arduino:" + str(data))
   elif src == img_sock:#cmd from img_proc
    data = img_sock.recv(1024)
    data = str(data).strip('\x00')
    newangle = int(float(data))
    print("newangle"+ str(newangle))
    if currangle > 30:
        currangle = 25
    elif currangle < -30:
        currangle = -25
    if abs(newangle-currangle) > 4:
     currangle = newangle
     currangle = int(currangle)
     print("new currangle:" + str(currangle))
     if currangle >0:
      cmd = 'R' + str(currangle) + '\n'
     else:
      cmd = 'L' + str(-1*currangle) +'\n'
     print("command" + repr(cmd) + '\t newangle:' + str(newangle))
     pi_cmd.sendall(cmd)
   elif src == ctrl_sock:#cmd from ctrl
    data = ctrl_sock.recv(1024)
    pi_cmd.sendall(data)
   else:
    print("got unexpected data:")
    print(src.recv(1024))
 except KeyboardInterrupt:
  break
print('Exiting')

