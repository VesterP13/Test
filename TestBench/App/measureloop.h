#ifndef MEASURELOOP_H
#define MEASURELOOP_H

#include <QObject>
#include <thread>

class measureLoop : public QObject
{
    Q_OBJECT

public:
    explicit measureLoop(QObject *parent = nullptr);
    void run();
    void stop();
    int frequency = 50;
    int receivedMessage1;
    int receivedMessage2;
    int receivedMessage3;
    int receivedMessage4;

signals:
    void operationFinished();
    void messageReceived(int message1, int message2, int message3, int message4);

private:
    void receiveUdpMessage();
    int measuringPort = 2391;
};

#endif
