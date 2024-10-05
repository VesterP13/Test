#include "connection.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <QString>
#include <QDebug>
#pragma comment(lib, "Ws2_32.lib") // Łączenie z biblioteką Winsock

Connection::Connection(QObject *parent) : QObject(parent){}

void Connection::motorSignalChangeTo(int sliderValue,int motorNumber){
    qDebug("elo");
    int value = 1000 + ((2000 - 1000) * sliderValue) / 100;
    motorSignals[motorNumber]=value;
    sendUdpMessage();
}

void Connection::sendUdpMessage() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        WSACleanup();
        return;
    }

    sockaddr_in recipientAddr;
    memset(&recipientAddr, 0, sizeof(recipientAddr));
    recipientAddr.sin_family = AF_INET;
    recipientAddr.sin_port = htons(motorControlPort); // PORT
    inet_pton(AF_INET, motorControlIP, &recipientAddr.sin_addr); // IP

    std::string message = std::to_string(motorSignals[0]) + ";" + std::to_string(motorSignals[1]) + ";" + std::to_string(motorSignals[2]) + ";" + std::to_string(motorSignals[3]);

    int bytesSent = sendto(sockfd, message.c_str(), message.size(), 0, (sockaddr*)&recipientAddr, sizeof(recipientAddr));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Błąd podczas wysyłania danych: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Wysłano " << bytesSent << " bajtów danych\n";
    }

    closesocket(sockfd);
    WSACleanup();
}

void Connection::changeMotorControlIPandPort(int newPort, const char* newIP){
    motorControlPort = newPort;
    motorControlIP = newIP;
}

void Connection::receiveUdpMessage() {
    std::cout<<"ELOELOEXDDD";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(measuringPort); // PORT
    localAddr.sin_addr.s_addr = INADDR_ANY; // Listen on any interface

    if (bind(sockfd, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // Set socket to non-blocking mode
    u_long mode = 1;
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        std::cerr << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    char buffer[512]; // Buffer to store incoming messages

    int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrSize);
    if (bytesReceived == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK) {
            std::cerr << "Receive failed: " << error << std::endl;
        } else {
            std::cerr << "No data available to read" << std::endl;
        }
    } else {
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        int receivedNumber = atoi(buffer); // Convert received data to an integer
        motor1_rpm = receivedNumber;
    }

    closesocket(sockfd);
    WSACleanup();
}

