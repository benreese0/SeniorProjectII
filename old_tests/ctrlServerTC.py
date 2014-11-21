#!/usr/bin/python2
import io
import socket
import struct
import time
from PIL import Image

server_socket = socket.socket()    
server_socket.bind(('0.0.0.0', 12345))
server_socket.listen(0)

connection = server_socket.accept()[0].makefile('rb')
try:
  while True:
    t0 = time.time()
    image_len = struct.unpack('<L', connection.read(struct.calcsize('<L')))[0]
    if not image_len:
      break

    image_stream = io.BytesIO()
    image_stream.write(connection.read(image_len))

    image_stream.seek(0)
    image = Image.open(image_stream)
    #image.rotate(180).show()
    print('Image is %dx%d' % image.size)     
    image.verify() 
    print('Image is verified')

    rate = time.time() - t0
    print('Time used:' + str(rate) + '\t' + str(1/rate))
finally:
   connection.close()
   server_socket.close()
