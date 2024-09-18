#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring> // For ZeroMemory

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Function to log messages to a file
void logMessage(const string &message)
{
    ofstream logFile("C:\\Users\\laudi\\Documents\\tcp_server\\src\\server.log", ios_base::app);
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
    // Initialize Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        logMessage("Can't initialize winsock!");
        return 1; // Return a non-zero value to indicate failure
    }

    // Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        logMessage("Can't create a socket");
        WSACleanup();
        return 1;
    }

    // Bind the IP address and port to a socket
    sockaddr_in hint;
    ZeroMemory(&hint, sizeof(hint)); // Clear memory
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == SOCKET_ERROR)
    {
        logMessage("Bind failed");
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    // Tell Winsock the socket is for listening
    if (listen(listening, SOMAXCONN) == SOCKET_ERROR)
    {
        logMessage("Listen failed");
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    // Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET)
    {
        logMessage("Accept failed");
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    char host[NI_MAXHOST];
    ZeroMemory(host, NI_MAXHOST);

    string clientInfo = string(inet_ntoa(client.sin_addr)) + " connected on port " + to_string(ntohs(client.sin_port));
    logMessage(clientInfo);
    cout << clientInfo << endl;

    // Close listening socket
    closesocket(listening);

    // While loop: accept and echo message back to client
    char buf[4096];
    while (true)
    {
        ZeroMemory(buf, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
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
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();
    return 0; // Return 0 to indicate success
}
