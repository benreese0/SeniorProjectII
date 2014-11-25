CFLAGS= `pkg-config --cflags opencv` -Wall -Wextra -gstabs
LIBS= `pkg-config --libs opencv`
CC= g++

all: lines

lines: lines.cpp
		$(CC) $(CFLAGS) $? $(LIBS) -o $@
			
contours: contours.cpp
		$(CC) $(CFLAGS) $? $(LIBS) -o $@
			
.PHONY:clean

clean:
	-rm -f main
	-rm -f contours
	-rm -f lines

.PHONY:test

test:all
	./lines
