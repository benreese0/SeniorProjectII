#!/usr/bin/python2
import asyncore
import socket
import picamera
import picamera.array

# RPI
class EchoHandler(asyncore.dispatcher_with_send):
  def handle_read(self):
    data = self.recv(8192)
    if data == 'img':
      with picamera.PiCamera() as camera:
        with picamera.array.PiRGBArray(camera) as stream:
          camera.capture(stream, format='bgr')
          # At this point the image is available as stream.array
          image = stream.array  
        self.send(image)
        print 'i got an i'
    else :
      print data

# rpi needs to listen, when control server is ready, it needs to contact 
# needs to be a client to the PI
# control server
class EchoServer(asyncore.dispatcher):
  def __init__(self, host, port):
    asyncore.dispatcher.__init__(self)
  # gets all the settings together- what you want
    self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
    self.set_reuse_addr()
  # I can do this
    self.bind((host, port))
    # i'm ready for talk      
    self.listen(5)

  def handle_accept(self):
    pair = self.accept()
    if pair is not None:
      socket, address = pair
      print 'Incoming connection from %s' % repr(address)
      handler = EchoHandler(socket)

server = EchoServer('',12345)
asyncore.loop()

#request and response
#chmod +x
#create_socket
#connect(host, 8080)
#buffer = i
# image = self.recv(4000000)
#recv(image)
