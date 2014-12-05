#!/usr/bin/python2

import socket
import select
import struct
import time


#constants
cmd_port = 7777
img_port = 7778
signs_port = 7779
lines_port = 7780
pi_addr = '192.168.2.130'
img_addr = '192.168.2.108'
ctrl_addr = 'localhost'
fourmb = 1024*1024*4
batt_logfile_name = 'batterylog.txt'
batt_thresh = 6.18


currangle = -7 #start left by eight
currSpeed = 1480
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
img_sock.connect( (img_addr, signs_port) )
print("Img connection made")
img2_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
img2_sock.connect( (img_addr, lines_port) )
print("Img2 connection made")
lastSpeed = 0;

#startup with centered wheels
pi_cmd.sendall('L7\n')
batt_logfile = open(batt_logfile_name,'a')
batt_logfile.write("New Session\n")
networkTime_logfile = open("network_time.txt",'a')
networkTime_logfile.write("new Session \n")
line_logfile = open("line_time.txt",'a')
line_logfile.write("new session \n");


prevTime = time.time()
prevTime2 = time.time()
sources = [pi_img, pi_cmd, img_sock, img2_sock, ctrl_sock]
stopFlag = False
restartFlag = False
yieldFlag = False
repeatCnt = 0
restartCnt = 0
yieldCnt = 0
lastCmd = 'A'
stopTime = time.time()
resumeTime = time.time()
irTime = time.time()
irResume = time.time()

while True:
 try:
  inrdy, outrdy, errdy = select.select(sources,[],[])
  for src in inrdy:
   if src == pi_img: #new image from pi
    data = pi_img.recv(fourmb)
    img_sock.sendall(data)
    img2_sock.sendall(data)
   elif src == pi_cmd:#data from pi
    data = pi_cmd.recv(1024)
    data = data.strip('\x00')
    print data
    if data[0] == 'P':
     data = str(data).strip('\x00')
     data = data.split('\x00')
     for i in data:
      batt_logfile.write(str(i)[1:])
      if i < batt_thresh:
       print('POWER DANGEROUSLY LOW!!! REPLACE BATTERY NOW!!!!')

    #if data[0] == 'P':
    # voltage = float(data[1:].strip())
    # if voltage < 5.80:
    #  print("Voltage low:" + str(voltage))
    #else:
    elif data[0]=='G':
     cmd = 'F'+str(lastSpeed)+'\n'
     pi_cmd.sendall(bytes(cmd))
    elif data[0] == 'O':
     lastSpeed = currSpeed
     currSpeed = 1480
    print("Arduino:" + str(data))
   elif src == img_sock:#cmd from img_proc
    data = img_sock.recv(1024)
    data = str(data).strip('\x00')
    data = data.strip()
    data = data.split('\n')
    data = data[0]
    data = bytes(data)
    #print (repr(data))
    presTime = time.time()
    networkTime_logfile.write(str(data[0:2]) + ' ')
    networkTime_logfile.write(str(float(presTime-prevTime))+ '\n')
    prevTime = presTime
    print data
    if data[0] == 'S':
     repeatCnt = 0
     print('stopping once gone')
     if currSpeed>1480 and lastCmd!='S':
      pi_cmd.sendall(bytes('F1550\n'))
      lastSpeed = currSpeed
      currSpeed = 1550
      stopFlag = True
     #time.sleep(
     lastCmd = 'S'
     #pi_cmd.sendall(bytes('S\n'))
    elif data[0] == 'Y':
     print('yield')
     repeatCnt = 0
     if currSpeed>1550 and lastCmd!='Y': 
      lastSpeed = currSpeed
      yieldFlag = True
      diff = currSpeed-1520
      diff = diff/3
      cmd = 'F'+str(1520+diff)+'\n'
      pi_cmd.sendall(bytes(cmd))
      currSpeed = 1520+diff
      lastCmd = 'Y'
    elif data[0:2] == 'V1':
     print('V1')
     repeatCnt = 0
     lastCmd = 'V'
     pi_cmd.sendall(bytes('F1560\n'))
     currSpeed = 1560
    elif data[0:2] == 'V2':
     print('V2')
     repeatCnt = 0
     lastCmd = 'V'
     pi_cmd.sendall(bytes('F1575\n'))
     currSpeed = 1575
        
    elif data[0] == 'C':
     if lastCmd == 'C' and stopFlag==True:
      repeatCnt+=1
      restartCnt = 0
      yieldCnt = 0
     elif lastCmd == 'C' and restartFlag==True:
      repeatCnt=0
      restartCnt+=1
      yieldCnt = 0
     elif lastCmd == 'C' and yieldFlag==True:
      yieldCnt+=1 
      restartCnt = 0
      repeatCnt =0

     lastCmd = 'C'
     if stopFlag==True and repeatCnt>=2:
      repeatCnt = 0
      pi_cmd.sendall(bytes('S\n'))
      stopFlag = False
      restartFlag = True
      stopTime = time.time()
     elif restartFlag==True and restartCnt>=3:
      if time.time()-stopTime>=3:
       print 'restarting'
       restartFlag=False
       restartCnt=0
       cmd = 'F'+str(lastSpeed)+'\n'
       currSpeed = lastSpeed
       pi_cmd.sendall(bytes(cmd))
     elif yieldFlag ==True and yieldCnt>=3:  
      yieldFlag = False
      yieldCnt = 0
      cmd = 'F'+str(lastSpeed)+'\n'
      currSpeed = lastSpeed
      pi_cmd.sendall(bytes(cmd))
      

    else:
     print('strange data:'+repr(data))
   elif src == img2_sock:   
    #pass
    data = img2_sock.recv(1024)
#    data = str(data).strip('\x00')
#    data = data.strip()
#    data = data.split('\n')
#    data = data[0]
#    data = str(data).strip('\x00')
#    data = bytes(data)
#    print str(data) + "HEHE"
#    newangle = int(float(str(data)))
#    if newangle>30:
#     newangle=25
#    if newangle<-30:
#     newangle=-25
#    newangle = newangle-7
    presTime2 = time.time()
    line_logfile.write(str(float(presTime2-prevTime2))+ '\n')
    prevTime2 = presTime2
#
#    #print("newangle"+ str(newangle))
#    #if currangle > 30:
#    #    currangle = 25
#    #elif currangle < -30:
#    #    currangle = -25
#    #if abs(newangle) > 6:
#    # currangle = newangle/6 + currangle
#    # currangle = int(currangle)
#     #print("new currangle:" + str(currangle))
#    if newangle >0:
#     cmd = 'R' + str(newangle) + '\n'
#    else:
#     cmd = 'L' + str(-1*newangle) +'\n'
#     #print("command" + repr(cmd) + '\t newangle:' + str(newangle))
#    pi_cmd.sendall(cmd)
   elif src == ctrl_sock:#cmd from ctrl
    data = ctrl_sock.recv(1024)
    pi_cmd.sendall(data)
   else:
    print("got unexpected data:")
    print(src.recv(1024))
 except KeyboardInterrupt:
  break
 except IOError,e:
  pass

print('Exiting')

batt_logfile.close()
networkTime_logfile.close()
line_logfile.close()
