#include <bits/stdc++.h>
#include <sys/socket.h> //needed for socket funcs
#include <netinet/in.h> //needed for constants

using namespace std;

//handle a client
void handle_client(int client_socket)
{
    char buffer[1024] = {0};

    while (true)
    {
        // Clear buffer for new data
        memset(buffer, 0, sizeof(buffer));
    
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        // If bytes_received is <= 0, the client disconnected or error occurred
        if (bytes_received <= 0)
        {
            cout << "Client disconnected." << endl;
            break;
        }

        cout << "Received: " << buffer << endl;

        send(client_socket, buffer, bytes_received, 0);
    }

    close(client_socket);
}

int main()
{
    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serveraddress;
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(8080);
    serveraddress.sin_addr.s_addr = INADDR_ANY;

    bind(serversocket, (struct sockaddr *)&serveraddress, sizeof(serveraddress));

    listen(serversocket, 5);
    while (true)
    {
        int clientsocket = accept(serversocket, nullptr, nullptr);

        thread client_thread(handle_client, clientsocket);
        client_thread.detach();
    }
    close(serversocket);

    return 0;
}
