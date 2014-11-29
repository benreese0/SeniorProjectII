#!/usr/bin/python
import io
import socket
import struct
import time
import picamera

# Connect a client socket to my_server:8000 (change my_server to the
# hostname of your server)
client_socket = socket.socket()
client_socket.connect(('192.168.1.6', 7779))
i=0
ptime = 0;
# Make a file-like object out of the connection
connection = client_socket.makefile('wb')
try:
    with picamera.PiCamera() as camera:
        camera.resolution = (640, 480)
        camera.rotation = 270
        # Start a preview and let the camera warm up for 2 seconds
        camera.start_preview()
        time.sleep(2)

        # Note the start time and construct a stream to hold image data
        # temporarily (we could write it directly to connection but in this
        # case we want to find out the size of each capture first to keep
        # our protocol simple)
        start = time.time()
        stream = io.BytesIO()
        for foo in camera.capture_continuous(stream, 'jpeg',use_video_port=True):
            # Write the length of the capture to the stream and flush to
            # ensure it actually gets sent
            imgSize = stream.tell()
            i=i+1
            print("Img# " +str (i) + " size:"+str( imgSize))
            print("process time:" + str(time.time() - ptime))
            ptime = time.time()
            connection.write(struct.pack('<L', stream.tell()))
            connection.flush()
            # Rewind the stream and send the image data over the wire
            stream.seek(0)
            connection.write(stream.read())
            # If we've been capturing for more than 30 seconds, quit
            #if time.time() - start > 30:
            #    break
            # Reset the stream for the next capture
            stream.seek(0)
            f = open('jpeg.jpeg','wb')
            f.write(stream.read())
            f.close()
            stream.seek(0)
            stream.truncate()
    #        print(client_socket.recv(1024))
    # Write a length of zero to the stream to signal we're done
    connection.write(struct.pack('<L', 0))
finally:
    connection.close()
    client_socket.close()
