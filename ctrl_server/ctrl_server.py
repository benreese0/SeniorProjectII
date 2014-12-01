#!/usr/bin/python

import socket
import select



#constants
cmd_port = 7777
img_port = 7778
sign_port = 7779
line_port = 7780
pi_addr = '192.168.1.7'
sign_addr = '192.168.1.4'
line_addr = '192.168.1.4'
ctrl_addr = '192.168.1.4'
fourmb = 1024*1024*4
v1 = 1566
v2 = 1580
vstopped = 1480
angle_tolerance = 10
batt_logfile_name = 'batterylog.txt'
maxturn = 30


#global variables

currentVeloctiy = vstoppped
currentStatus = str()
#values: 'Driving' 'Stopped' 'Obstacle' 'Yeild'
angle = -8
last_img = bytes()
line_img = bytes()
sign_img = bytes()





#static sockets
pi_img = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_img.connect( (pi_addr, img_port) )
print("Pi image connection made:" + str(pi_img.getpeername()))
pi_cmd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_cmd.connect( (pi_addr, cmd_port) )
print("Pi command connection made:" + str(pi_cmd.getpeername()))
sign_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sign_sock.connect( (img_addr, img_port) )
print("Sign connection made:" + str(sign_sock.getppername()))
line_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
line_sock.connect( (line_addr, line_port) )
print("Line connection made:" + str(line_sock.getpeername()))
ctrl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ctrl_sock.connect( (ctrl_addr, cmd_port) )
print("Local control connection made:" + str(ctrl_sock.getpeername()))

#Init
pi_cmd.sendall('\nL7\n')
batt_logfile =  open(batt_logfile_name, 'a')
batt_logfile.write('#New session started\n')

sources = [pi_img, pi_cmd, ctrl_sock, sign_sock, line_sock]
destinations = [sign_sock, line_sock]
#sources = [pi_img, pi_cmd, ctrl_sock, line_sock]

while True:
 try:
  inrdy, outrdy, errdy = select.select(sources, destinations,sources)
  #Error sockets
  for src in errdy:
   print ("Error with socket:" + src.getsockname())
   for sock in sources:
    sock.close()

  #Writing sockets
  for src in outrdy:
   if src == sign_sock and len(sign_img) >0:
    sign_img  = sign_img[sign_sock.send(sign_img):]
   elif src == line_sock and len(line_img) > 0:
    line_img = line_img[line_sock.send(line_img):]


  #Reading sockets
  for src in inrdy:

   if src == pi_img: #new image from pi
    last_img = pi_img.recv(fourmb)

   elif src == pi_cmd:#data from arduino
    data = pi_cmd.recv(1024)
    data = data.strip('\x00')
    if data[0] == 'A':#Ack some command
     if data[1] == 'L':#ack left
      pass #Don't really care
     elif data[1] == 'R':#ack right
      pass #Don't really cares
     elif data[1] == 'S':#Ack stopped
      currentStatus = 'Stopped'
     elif data[1] == 'F':#ack forward
      if currentVelocity == v1:
       currentStatus = 'Yield'
      else:
       currentStatus = 'Driving'
     else:
         print("Arduino unknown Ack:" + str(data))
    elif data[0] = 'P': #battery power
     batt_logfile.write(str(data))
    elif data[0] = 'G': #Go -> obstacle clear
     currentStatus = 'Driving'
     currentVelocity = v1
     pi_cmd.write('F' + str(currentVelocity) + '\n')
    elif data[0] = 'E': #Error
     print("Arduino error found:" + str(data))
    elif data[0] = 'O': #Obstacle
     currentStatus = 'Obstacle'
    else:
     print("Unknown arduino reply:" + str(data))

   elif src == line_sock:#cmd from lines
    data = line_sock.recv(1024)
    if currentStatus =='Yeild' or currentStatus == 'Driving':
     data = str(data).strip('\x00') 
     newangle = int(float(data))
     #Deal set maximum angle
     if angle > maxangle:
      angle = maxangle -6
     if angle < maxangle*-1:
      angle = -1*maxangle +6
     #only respond to large enough angle
     if abs(newangle) >6:
      angle = newangle/6 +angle
      angle = int(angle)
      print("New angle:" + str(angle))
      if angle >0 :
       cmd = 'R' + str(currangle) + '\n'
      else:
       cmd = 'L' + str(-1*currangle) + '\n'
      pi_cmd.sendall(cmd)
    line_img = last_img
   elif src == sign_sock:#cmd from signs
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

