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
ctrl_addr = '192.168.1.6'
fourmb = 1024*1024*4
v1 = 1560
v2 = 1600
angle_tolerance = 10


#global variables

currentVeloctiy = 1480 # 1480 = stopped
currentStatus = str()
angle = 8






#static sockets
pi_img = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_img.connect( (pi_addr, img_port) )
print("Pi image connection made:" + str(pi_img.getpeername()))
pi_cmd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_cmd.connect( (pi_addr, cmd_port) )
print("Pi command connection made:" + str(pi_cmd.getpeername()))
#sign_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#sign_sock.connect( (img_addr, img_port) )
#print("Sign connection made:" + str(sign_sock.getppername()))
line_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
line_sock.connect( (line_addr, line_port) )
print("Line connection made:" + str(line_sock.getpeername()))
ctrl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ctrl_sock.connect( (ctrl_addr, cmd_port) )
print("Local control connection made:" + str(ctrl_sock.getpeername()))

#sources = [pi_img, pi_cmd, ctrl_sock, sign_sock, line_sock]
sources = [pi_img, pi_cmd, ctrl_sock, line_sock]

while True:
 try:
  inrdy, outrdy, errdy = select.select(sources,[],sources)
  for src in errdy:
   print ("Error with socket:" + src.getsockname())
   for sock in sources:
    sock.close()

  for src in inrdy:
   if src == pi_img: #new image from pi
    data = pi_img.recv(fourmb)
    #while data.length() >0: 
     #   print("lenn" + str(data.len()))
    line_sock.sendall(data)
      #  data = pi_img.recv(fourmb)
   elif src == pi_cmd:#data from pi
    data = pi_cmd.recv(1024)
    print("Arduino:" + str(data))
   elif src == line_sock:#cmd from img_proc
    data = line_sock.recv(1024)
    print("IMGCMD:"+ str(data))
    angle = angle + float(data[0:4])
    if angle >0:
     pi_cmd.sendall('L' + str(int(angle)))   
    else:
     pi_cmd.sendall('R' + str(int(-1.0*angle)))
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

