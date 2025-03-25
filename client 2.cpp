#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>  
#include <tchar.h>      
#include <winsock.h>
#include<thread>
#include<vector>
#include<string>
#pragma comment(lib, "ws2_32.lib")  

using namespace std;
bool initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void SendMessages(SOCKET s) {
    cout << "Enter your chat name:" << endl;
    string name;
    getline(cin, name);
    string message;
    while (1) {
        getline(cin, message);
        string msg = name + ":" + message;
        int bytesent = send(s, msg.c_str(), msg.length(), 0);
        if (bytesent == SOCKET_ERROR) {
            cout << "ERROR SENDING MESSAGE" << endl;
            break;
        }
        if (message == "quit") {
            cout << "Stopping The Application" << endl;
            break;
        }

    }
    closesocket(s);
    WSACleanup();
}
void ReceiveMessage(SOCKET s) {
    char buffer[4096];
    int recvlength;
    string mssg = "";
    while (1) {
        recvlength = recv(s, buffer, sizeof(buffer), 0);
        if (recvlength <= 0) {
            cout << "Disconnected From The server" << endl;
            break;
        }
        else {
            mssg = string(buffer, recvlength);
            cout << mssg << endl;

        }
    }
    closesocket(s);
    WSACleanup();
}

int main() {
    if (!initialize()) {
        cout << "Winsock initialization failed" << endl;
        return 1;
    }
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }
    string serveraddress = "127.0.0.1";
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));
    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "NOT ABLE TO CANNECT TO SERVER" << endl;
        cout << ":" << WSAGetLastError();
        closesocket(s);
        WSACleanup();
        return 1;

    }
    cout << "Succesfully cannected to server" << endl;
    thread senderthread(SendMessages, s);
    thread receiver(ReceiveMessage, s);
    senderthread.join();
    receiver.join();

    return 0;

}