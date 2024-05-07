#include "./utils.h"
struct Bus **buses;

int createSocktIpv4()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in *createAddresForSocketIpv4(char *ip, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY;
    else
    {
        if (inet_pton(AF_INET, ip, &(address->sin_addr)) <= 0)
        {
            printf("Invalid address or address not supported");
            exit(EXIT_FAILURE);
        }
    }
    return address;
}

struct RequestedClients *AcceptClients(int serverFD)
{
    struct sockaddr_in client_addr;
    int clientAddressSize = sizeof(client_addr);
    int clientSocketFD = accept(serverFD, (struct sockaddr *)&client_addr, &clientAddressSize);

    struct RequestedClients *client = malloc(sizeof(struct RequestedClients));
    client->clientSocketFD = clientSocketFD;
    client->clientAddress = client_addr;
    client->accepted = true;
    return client;
}

void ClientConnectionAddress(int socketFD)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(socketFD, (struct sockaddr *)&client_addr, &addr_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
}

void serializeAllBuses(struct Bus **buses, char *serializedData)
{
    for (int i = 0; i < 10; i++)
    {
        // Serialize name
        strncpy(serializedData, buses[i]->name, 100);
        serializedData += 100; // Move the pointer to the next 100 bytes

        // Serialize destination
        strncpy(serializedData, buses[i]->destination, 100);
        serializedData += 100; // Move the pointer to the next 100 bytes
    }
}

void sendSerializedData(int socketFD)
{
    char serializedData[10 * (100 + 100)];
    serializeAllBuses(buses, serializedData);
    send(socketFD, serializedData, sizeof(serializedData), 0);
}

void *HandleClients(void *arg)
{
    int socketFD = *(int *)arg;
    printf("socketFD=%d\n", socketFD);
    sendSerializedData(socketFD);

    int *arr = getDestination(socketFD);
    int i = 0;
    printf("arr=%d\n", arr[0]);
    while (arr[i] >= 0)
    {
        struct Bus *bus = buses[arr[i]];
        printf("%s", bus->name);
        send(socketFD, bus->name, strlen(bus->name) + 1, 0);
        i++;
    }
    send(socketFD, "Complete", sizeof("Complete"), 0);
    struct Bus *bus = recvNameSendSeats(socketFD);
    if (bus == NULL)
    {
        return NULL;
    }
    AllocateSeats(socketFD, bus);
    pthread_exit(NULL);
}

struct Bus *InilizeBus(char *name)
{

    struct Bus *bus = malloc(sizeof(struct Bus));
    for (int i = 0; i < 32; i++)
    {
        bus->seats[i] = -1;
    }

    strcpy(bus->name, name);
    for (int i = 0; i < 32; i++)
    {
        bus->seats[i] = '0';
    }
    bus->seats[32] = '\0';
    return bus;
}

struct Bus **InilizeBusArray(int count)
{
    char busesNames[10][100];

    strcpy(busesNames[0], "CoastalCruiser");
    strcpy(busesNames[1], "KonkanKing");
    strcpy(busesNames[2], "GajapatiExpress");
    strcpy(busesNames[3], "DolphinDash");
    strcpy(busesNames[4], "BlueHorizonExpress");
    strcpy(busesNames[5], "ParadiseTravels");
    strcpy(busesNames[6], "CoastalVoyager");
    strcpy(busesNames[7], "MalabarMagic");
    strcpy(busesNames[8], "CoastalConnect");
    strcpy(busesNames[9], "GoldenSandExpress");

    char destinations[10][100] = {
        "Mangalore",
        "Karwar",
        "Bangalore",
        "Goa",
        "Kochi",
        "Chennai",
        "Mumbai",
        "Hyderabad",
        "Pune",
        "Coimbatore"};
    struct Bus **buses1 = malloc(count * sizeof(struct Bus *));
    for (int i = 0; i < count; i++)
    {
        buses1[i] = InilizeBus(busesNames[i]);
        strcpy(buses1[i]->destination, destinations[i]);
    }

    return buses1;
}

void init()
{
    buses = InilizeBusArray(10);
}

int *getDestination(int socketFD) {
    char buffer[101]; 
    int *arr = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        arr[i] = -1;
    }
    printf("Hello\n");
    int recvBytes = recv(socketFD, buffer, 100, 0);
    if (recvBytes <= 0) {
        printf("Error receiving data\n");
        return arr; 
    }
    buffer[recvBytes] = '\0';
    printf("Received data: %s\n", buffer);
    int index = 0;
    for (int i = 0; i < 10; i++) {
        if (buses[i] != NULL && strcmp(buses[i]->destination, buffer) == 0) {
            arr[index] = i;
            printf("Match found: %s\n", buses[i]->destination);
            index++;
            printf("Destination found\n");
        }
    }
    return arr;
}


struct Bus *recvNameSendSeats(int socketFD)
{
    printf("Hello\n");
    char buffer[100];
    int recvBytes = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (recvBytes <= 0)
    {
        perror("Error receiving data");
        return NULL;
    }
    buffer[recvBytes] = '\0';
    printf("Received bus name: %s\n", buffer);
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(buses[i]->name, buffer) == 0)
        {
            printf("Matched bus name: %s\n", buses[i]->name);
            printf("Seats: %s\n", buses[i]->seats);
            send(socketFD, buses[i]->seats, strlen(buses[i]->seats) + 1, 0);
            return buses[i];
        }
    }
    return NULL;
}

void AllocateSeats(int socketFD, struct Bus *bus)
{
    int seatNumber;
    recv(socketFD, &seatNumber, sizeof(int), 0);
    printf("%d\n", seatNumber);
    char gender;
    int recvBytes = recv(socketFD, &gender, sizeof(char), 0);
    if (recvBytes <= 0)
    {
        perror("Error receiving data");
        return;
    }
    printf("%c\n", gender);

    if (!CheckSeat(seatNumber, bus))
    {
        send(socketFD, "Seats is Already Allocated", strlen("Seats is Already Allocated") + 3, 0);
        return;
    }
    bus->seats[seatNumber - 1] = gender;
    send(socketFD, "Seats is Allocated", strlen("Seats is Allocated") + 1, 0);
}

int CheckSeat(int seatNumber, struct Bus *bus)
{
    return bus->seats[seatNumber - 1] == '0';
}