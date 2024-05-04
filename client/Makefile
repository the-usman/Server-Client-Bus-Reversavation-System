CFLAGS = -Wno-implicit-function-declaration

all: client

client: utils.o client.o
	$(TAB)gcc utils.o client.o -o client

utils.o: ../HeaderFiles/utils.c ../HeaderFiles/utils.h
	$(TAB)gcc $(CFLAGS) -c ../HeaderFiles/utils.c -o utils.o

client.o: client.c ../HeaderFiles/utils.h
	$(TAB)gcc $(CFLAGS) -c client.c -o client.o

clean:
	$(TAB)rm -f client utils.o client.o
