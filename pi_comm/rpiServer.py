#!/usr/bin/python2
import io
import socket
import struct
import time
import picamera


address = '192.168.1.3'

client_socket = socket.socket()
client_socket.connect((address, 12345))
connection = client_socket.makefile('wb')
print connection
try:
  with picamera.PiCamera() as camera:
    #camera.resolution = (1280, 1024)
    camera.resolution = (1024, 768)
    #camera.resolution = (720, 480)
    camera.framerate = 60
    camera.iso = 400
    camera.start_preview()
    time.sleep(2)
    camera.shutter_speed = camera.exposure_speed
    camera.exposure_mode = 'off'
    g = camera.awb_gains
    camera.awb_mode = 'off'
    camera.awb_gains = g
    start = time.time()
    stream = io.BytesIO()
    for foo in camera.capture_continuous(stream, format= 'jpeg',use_video_port=True):
#    for foo in camera.start_recording(stream, format= 'h264',use_video_port=True):
#    print time.time()
#    while (time.time()-start > 45):
#      print "hello"
#      foo = camera.capture_continuous(stream, 'jpeg');
      print time.time()
      connection.write(struct.pack('<L', stream.tell()))
      connection.flush()
      
      stream.seek(0)
      connection.write(stream.read())

      if time.time() - start > 30:
        break

      stream.seek(0)
      stream.truncate()
      print "end " + str(time.time())
  connection.write(struct.pack('<L', 0))
finally:
  connection.close()
  client_socket.close()
