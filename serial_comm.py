#!/usr/bin/python2


import serial

#create a function that will do serial communication from the arduino to rpi 
def arduinoToRpi();
	ser = serial.Serial('[pwd of USB]',9600)
	ser.write('r')
	return ser.read(1)

print arduinoToRpi()
