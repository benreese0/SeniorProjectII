CFLAGS= `pkg-config --cflags opencv` -Wall -Wextra -gstabs
LIBS= `pkg-config --libs opencv`
CC= g++

all: signs

signs: signslinux.cpp
		$(CC) $(CFLAGS) $? $(LIBS) -o $@
			
.PHONY:clean

clean:
	-rm -f signs

.PHONY:test

test:all
	./signs
