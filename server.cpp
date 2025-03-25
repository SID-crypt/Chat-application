#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // For InetPton
#include<tchar.h>
#include<thread>
#include<vector>
#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

using namespace std;

// Function to initialize Winsock
bool initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void InteractWithClient(SOCKET clientsocket, vector <SOCKET>& clients) {
    cout << "Client connected!" << endl;

    // Receive data from the client
    char buffer[4096];
    while (1) {
        int bytercvd = recv(clientsocket, buffer, sizeof(buffer) - 1, 0);  // Reserve space for null-terminator
        string message(buffer, bytercvd);
        if (bytercvd == SOCKET_ERROR) {
            cout << "Receive failed. Error: " << WSAGetLastError() << endl;
            break;
        }
        else if (bytercvd <= 0) {
            cout << "Connection closed by client." << endl;
            break;
        }
        else {
            buffer[bytercvd] = '\0';  // Null-terminate the received string
            cout << "Message from client: " << message << endl;
        }
        for (auto client : clients) {
            if (client != clientsocket) {
                send(client, message.c_str(), message.length(), 0);

            }
        }
    }
    auto it = find(clients.begin(), clients.end(), clientsocket);
    if (it != clients.end()) {
        clients.erase(it);
    }
    // Close the client socket
    closesocket(clientsocket);
}

int main() {
    // Initialize Winsock
    if (!initialize()) {
        cout << "Winsock initialization failed" << endl;
        return 1;
    }

    // Create a socket
    SOCKET listensocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listensocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address (sockaddr_in)
    int port = 12345;
    sockaddr_in serveraddr = {};  // Zero-initialize the structure
    serveraddr.sin_family = AF_INET;  // Address family: IPv4
    serveraddr.sin_port = htons(port);  // Port number, converted to network byte order

    // Use InetPton to convert the IP address string to binary form
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
        cout << "IP address conversion failed" << endl;
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }

    // Bind the socket to the address and port
    if (bind(listensocket, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "Binding socket failed. Error: " << WSAGetLastError() << endl;
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed. Error: " << WSAGetLastError() << endl;
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }

    cout << "Server has started listening on port " << port << endl;
    vector <SOCKET> clients;

    while (1) {
        // Accept a client connection
        SOCKET clientsocket = accept(listensocket, nullptr, nullptr);
        if (clientsocket == INVALID_SOCKET) {
            cout << "Accept failed. Error: " << WSAGetLastError() << endl;
            closesocket(listensocket);
            WSACleanup();
            return 1;
        }
        clients.push_back(clientsocket);
        thread t1(InteractWithClient, clientsocket, std::ref(clients));
        t1.detach();
    }




    // Cleanup and close the listening socket
    closesocket(listensocket);
    WSACleanup();

    return 0;
}
