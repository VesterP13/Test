#ifndef CONNECTION_H
#define CONNECTION_H

#include <array>
#include <string>
#include <QObject>

class Connection: public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    void changeMotorControlIPandPort(int motorControlPort, const char* motorControlIP);
    void receiveUdpMessage();
    int motor1_rpm = 0;

public slots:
    void motorSignalChangeTo(int sliderValue, int motorNumber);

private:
    std::array<int, 4> motorSignals = {1000, 1000, 1000, 1000};
    void sendUdpMessage();
    int motorControlPort = 12345;
    int measuringPort = 2390;
    const char* motorControlIP = "192.168.1.201";
};

#endif // CONNECTION_H
