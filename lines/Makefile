CFLAGS= `pkg-config --cflags opencv` -Wall -Wextra 
LIBS= `pkg-config --libs opencv`
CC= g++

all: contours

contours: contours.cpp
		$(CC) $(CFLAGS) $(LIBS) $? -o $@
			
.PHONY:clean

clean:
	rm main

.PHONY:test

test:all
	./contours
