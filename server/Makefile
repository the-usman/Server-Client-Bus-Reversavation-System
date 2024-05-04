CFLAGS = -Wno-implicit-function-declaration

all: server

server: utils.o server.o
	$(TAB)gcc utils.o server.o -o server

utils.o: ../HeaderFiles/utils.c ../HeaderFiles/utils.h
	$(TAB)gcc $(CFLAGS) -c ../HeaderFiles/utils.c -o utils.o

server.o: server.c ../HeaderFiles/utils.h
	$(TAB)gcc $(CFLAGS) -c server.c -o server.o

clean:
	$(TAB)rm -f server utils.o server.o
