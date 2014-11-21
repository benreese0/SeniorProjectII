#!/usr/bin/python2
import io
import socket
import struct
import time
import picamera


#constants
imgFormat = 'jpeg'
#imgformat = 'h264'
#camres = (1280,1024)
camRes = (1024, 768)
#camres = (720, 480)
camFramerate = 60
camIso = 800
sockPort = 12345

address = '192.168.1.3'
#place to store constants
camGain = tuple()


#get cam perameters
    print("Calibrating camera:")
try:
  with picamera.PiCamera() as camera:
    #FIXME: get the values we want exposure/awb
    camera.resolution = camRes
    camera.framerate = camFramerate
    camera.iso = camIso
    camera.start_preview()
    time.sleep(2)
    camera.shutter_speed = camera.exposure_speed
    camera.exposure_mode = 'off'
    camGain = camera.awb_gains
finally:
  connection.close()
  client_socket.close()

#prepare socket
main_sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
main_sock.bind( ( '', sockPort) )
main_sock.listen(1)
connection ,addr = main_sock.accept()
print("connection made: " + str(addr))
connection_file = client_socket.makefile('wb')
try:
  with picamera.PiCamera() as camera:
    #FIXME: set the values we want
    camera.resolution = camRes
    camera.framerate = camFramerate
    camera.iso = camIso
    camera.exposure_mode = 'off'
    camera.awb_mode = 'off'
    camera.awb_gains = camGain
    start = time.time()
    stream = io.BytesIO()
    for foo in camera.capture_continuous(stream, format= 'jpeg',use_video_port=True):
     connection_file.write(struct.pack('<L', stream.tell()))
     connection_file.flush()
      
      stream.seek(0)
      connection_file.write(stream.read())

      if time.time() - start > 30:
        break

      stream.seek(0)
      stream.truncate()
      print "end " + str(time.time())
  connection_file.write(struct.pack('<L', 0))
finally:
  connection_file.close()
  connection.close()
  main_sock.close()
