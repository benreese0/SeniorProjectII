#!/usr/bin/python
import serial
import socket
import select

#constants
baudrate = 9600
port = 7777
comport = '/dev/ttyAMA0'

#static sockets
arduino = serial.Serial( comport, baudrate, timeout=0)
ctrl_srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ctrl_srv.bind( ('',port) )
ctrl_srv.listen(1)

sources_static = [ctrl_srv, arduino]
sources_net = []
while True:
 try:
  inrdy, outrdy, errrdy = select.select(sources_static+sources_net,[],[])
  for src in errdy:#handle errors first
   src.close()
   sources_net.remove(src)
  for src in inrdy:
   if src == ctrl_srv: #new connection made
    new_client, addr = ctrl_srv.accept()
    sources_net.append(client)
    print("new client added: " + str(addr))    
   elif src == arduino:#data from Arduino
    data = arduino.read(1024)
    for con in sources_net:#send it to all net sockets
     con.send(data)
   else: #send any net data to Arduino only
    data = src.recv(1024)
    arduino.write(data)
 except KeyboardInterrupt:
  break
 except socket.error, msg:
  print('Error: '+ str(msg))
for src in sources_static:
 src.close()
for src in sources_net:
 src.close()
print('Exiting')

