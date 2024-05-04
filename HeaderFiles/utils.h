#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>



struct RequestedClients
{
    bool accepted;
    int clientSocketFD;
    struct sockaddr_in clientAddress;
};

struct Bus
{
    int totalSeats;
    char seats[33];
    char Customername[32][100];
    char name[100];
    char destination[100];
    char host[100];
};

int createSocktIpv4();
struct sockaddr_in *createAddresForSocketIpv4(char *ip, int port);

struct RequestedClients *AcceptClients(int serverFD);

void *HandleClients(void *arg);

struct Bus* InilizeBus(char *name);
struct Bus** InilizeBusArray(int count);
struct Bus *recvNameSendSeats(int socketFD);
int *getDestination(int socketFD);
void AllocateSeats(int socketFD, struct Bus *bus);
int CheckSeat(int seatNumber, struct Bus *bus);
void init();
