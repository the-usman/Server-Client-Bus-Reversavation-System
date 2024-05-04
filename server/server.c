#include "../HeaderFiles/utils.h"



int main()
{
    int serverFD = createSocktIpv4();
    struct sockaddr_in *main_address = createAddresForSocketIpv4("", 8881);
    if (bind(serverFD, (struct sockaddr *)main_address, sizeof(*main_address)) == -1)
    {
        printf("Error binding to port");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFD, 10) == -1)
    {
        printf("Error listening on port");
        exit(EXIT_FAILURE);
    }
    printf("Server is ON......\n");
    init();
    while (true)
    {
        struct RequestedClients *client = AcceptClients(serverFD);
        ClientConnectionAddress(client->clientSocketFD);
        pthread_t threadID;
        pthread_create(&threadID, NULL, *HandleClients, (void *)&client->clientSocketFD);
    }

    return 0;
}