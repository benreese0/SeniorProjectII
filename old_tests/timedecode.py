#!/usr/bin/python2

infile = open('network_time (1).txt', 'r')
cfile = open('c_file.txt', 'w')
v1file = open('v1_file.txt', 'w')
v2file = open('v2_file.txt', 'w')
sfile = open('s_file.txt', 'w')
yfile = open('y_file.txt', 'w')

for line in infile:
	if line[0] == 'C':
		cfile.write(line[2:])
	elif line[0] == 'V':
	 if line[1] == '1':
		v1file.write(line[3:])
	 elif line[1] == '2':
		v2file.write(line[2:])
	 else:
	  print('Idunno what to do with V:' + repr(line))
	elif line[0] == 'S':
		sfile.write(line[2:])
	elif line[0] == 'Y':
		yfile.write(line[2:])
	else:
	 print('Idunno what to do with:' + repr(line))


infile.close()
cfile.close()
v1file.close()
v2file.close()
sfile.close()
