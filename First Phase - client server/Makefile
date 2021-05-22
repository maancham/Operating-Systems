all: server.o client.o

server.o: server.c
	gcc -w server.c -o server

client.o: client.c
	gcc -w client.c -o client

clean:
	rm -f *.o server client