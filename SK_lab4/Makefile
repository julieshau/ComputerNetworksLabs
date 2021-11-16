CC = gcc
CFLAGS = -Wall
TARGETS = poll-server-telnet client

all: $(TARGETS) 

err.o: err.c err.h

client.o: client.c err.h

client: client.o err.o

poll-server-telnet.o: poll-server-telnet.c err.h

poll-server-telnet: poll-server-telnet.o err.o

clean:
	rm -f *.o $(TARGETS) 
