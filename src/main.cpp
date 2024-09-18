#include <iostream>
#include <fstream>
#include <cstring> // For memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_ntop
#include <unistd.h>    // For close()

using namespace std;

// Function to log messages to a file
void logMessage(const string &message)
{
    ofstream logFile("/home/backend/Documentos/tcp-server/src/server.log", ios_base::app);
    if (logFile.is_open())
    {
        logFile << message << endl;
        logFile.close();
    }
    else
    {
        cerr << "Unable to open log file" << endl;
    }
}

int main()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        logMessage("Can't create a socket");
        return 1;
    }

    // Bind the IP address and port to a socket
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // Clear memory
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        logMessage("Bind failed");
        close(serverSocket);
        return 1;
    }

    // Tell the socket to listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) < 0)
    {
        logMessage("Listen failed");
        close(serverSocket);
        return 1;
    }

    // Wait for a connection
    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);
    if (clientSocket < 0)
    {
        logMessage("Accept failed");
        close(serverSocket);
        return 1;
    }

    char host[INET_ADDRSTRLEN];
    memset(host, 0, INET_ADDRSTRLEN);

    // Convert IP address to string
    inet_ntop(AF_INET, &clientAddr.sin_addr, host, INET_ADDRSTRLEN);

    string clientInfo = string(host) + " connected on port " + to_string(ntohs(clientAddr.sin_port));
    logMessage(clientInfo);
    cout << clientInfo << endl;

    // Close listening socket
    close(serverSocket);

    // While loop: accept and echo message back to client
    char buf[4096];
    while (true)
    {
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived < 0)
        {
            logMessage("Error in recv().");
            break;
        }

        if (bytesReceived == 0)
        {
            logMessage("Client disconnected");
            cout << "Client disconnected" << endl;
            break;
        }

        // Echo message back to client
        send(clientSocket, buf, bytesReceived, 0);
    }

    // Close the socket
    close(clientSocket);

    return 0; // Return 0 to indicate success
}
