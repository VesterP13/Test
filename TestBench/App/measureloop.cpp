#include "measureLoop.h"
//#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>

//For Windows
#include <Winsock2.h>
#include <Ws2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")

measureLoop::measureLoop(QObject *parent)
    : QObject(parent)
{}

void measureLoop::run() {
    while (true){
        receiveUdpMessage();
        qDebug() << receivedMessage1;
        emit messageReceived(receivedMessage1, receivedMessage2, receivedMessage3, receivedMessage4);
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
}

void measureLoop::receiveUdpMessage() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        WSACleanup();
        return;
    }

    sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(measuringPort);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    u_long mode = 1;
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    char buffer[512];
    int bytesReceived;

    for (int attempts = 0; attempts < 5; ++attempts) {
        bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                break;
            } else {
                Sleep(10);
            }
        } else {
            buffer[bytesReceived] = '\0';

            // Parsing the received message
            char* token = strtok(buffer, ";");
            int values[4] = {0};

            for (int i = 0; i < 4 && token != NULL; ++i) {
                values[i] = atoi(token);
                token = strtok(NULL, ";");
            }

            // Assigning the values to the respective variables
            receivedMessage1 = values[0];
            receivedMessage2 = values[1];
            receivedMessage3 = values[2];
            receivedMessage4 = values[3];
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();
}

