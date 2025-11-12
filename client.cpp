#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main()
{
    int clientsocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serveraddress;
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(8080);
    serveraddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientsocket, (struct sockaddr *)&serveraddress, sizeof(serveraddress));

    const char *message = "echooooo!!!\n";

    send(clientsocket, message, strlen(message), 0);

    char buffer[1024] = {0};
    int valread = recv(clientsocket, buffer, 1024, 0);
    if (valread > 0)
    {
        cout << "Server echoed back: " << buffer << endl;
    }

    sleep(60);

    close(clientsocket);

    return 0;
}
