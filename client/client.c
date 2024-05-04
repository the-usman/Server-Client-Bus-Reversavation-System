#include "../HeaderFiles/utils.h"

int ServerConnection(char *ip, int port)
{
    int socketFD = createSocktIpv4();

    struct sockaddr_in *address = createAddresForSocketIpv4(ip, port);
    if (connect(socketFD, (struct sockaddr *)address, sizeof(*address)) == -1)
    {
        printf("Connection failed with server");
        exit(EXIT_FAILURE);
    }
    return socketFD;
}

void getBusNames(int socketFD)
{
    while (1)
    {
        char buffer[100];
        int recvBytes = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (recvBytes <= 0)
        {

            printf("Error receiving data");
            break;
        }
        buffer[recvBytes] = '\0';
        if (strcmp(buffer, "Complete") == 0)
        {
            break;
        }

        printf("%s\n", buffer);
        break;
    }
}

void deserializeAllBuses(char *serializedData)
{
    for (int i = 0; i < 10; i++)
    {
        char name[101];
        char destination[101];
        strncpy(name, serializedData, 100);
        name[100] = '\0'; 
        serializedData += 100; 
        strncpy(destination, serializedData, 100);
        destination[100] = '\0'; 
        serializedData += 100; 

        printf("%s\t\t %s\n", name, destination);
    }
}

void populateSeats(char seats[])
{
    for (int i = 0; i < strlen(seats); i++)
    {
        if (seats[i] == '0')
            printf("%d\t\t%s\n", i + 1, "Not Allocated");

        if (seats[i] == 'F' || seats[i] == 'f')
            printf("%d\t\t%s\n", i + 1, "Female Allocated");

        if (seats[i] == 'M' || seats[i] == 'm')
            printf("%d\t\t%s\n", i + 1, "Male Allocated");
    }
}
int main()
{
    int socketFD = ServerConnection("127.0.0.1", 8881);
    printf("Name\t\t\t Destination\n");
    char serializedData[10 * (100 + 100)]; 
    recv(socketFD, serializedData, sizeof(serializedData), 0);
    deserializeAllBuses(serializedData);
    char buffer[100];
    printf("Enter the destination\n");
    scanf("%s", buffer);
    printf("Hello, world!\n");
    send(socketFD, buffer, strlen(buffer) + 1, 0);
    getBusNames(socketFD);
    memset(buffer, 0, sizeof(buffer));

    printf("Enter the Bus name\n");
    scanf("%s", buffer);
    send(socketFD, buffer, strlen(buffer) + 1, 0);
    char seats[33];
    int recvBytes = recv(socketFD, seats, sizeof(seats), 0);
    if (recvBytes <= 0)
    {
        perror("recv");
        return 1;
    }
    seats[recvBytes] = '\0';
    printf("Received seats: %s\n", seats);
    printf("Seats No.\tStatus\n");
    populateSeats(seats);
    int seatNumber;
    printf("Enter the seats you want to book:   ");
    scanf("%d", &seatNumber);
    getchar();
    char gender;
    printf("Enter the Gender:    ");
    scanf("%c", &gender);
    if (gender != 'F' && gender != 'f')
    {
        gender = 'M';
    }

    send(socketFD, &seatNumber, sizeof(int), 0);
    send(socketFD, &gender, sizeof(char), 0);
    recvBytes = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (recvBytes <= 0)
    {

        printf("Error receiving data");
    }
    buffer[recvBytes] = '\0';
    printf("%s\n", buffer);
    return 0;
}
