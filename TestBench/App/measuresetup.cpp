// measuresetup.cpp

#include "measuresetup.h"
#include <QDebug>

measureSetup::measureSetup(QObject *parent)
    : QObject{parent}, measureObject(nullptr)
{}

measureSetup::~measureSetup()
{
    // Ensure proper cleanup
    measureThread.quit();
    measureThread.wait();
    delete measureObject;
}

void measureSetup::setup()
{
    // Ensure measureObject is not null
    if (!measureObject) {
        measureObject = new measureLoop;

        // Move to thread and connect signals
        measureObject->moveToThread(&measureThread);

        // Start the thread and handle received messages
        QObject::connect(&measureThread, &QThread::started, measureObject, &measureLoop::run);
        QObject::connect(measureObject, &measureLoop::messageReceived, this, &measureSetup::updateReceivedMessage);
        measureThread.start();
    }
}

void measureSetup::updateReceivedMessage(int message1, int message2, int message3, int message4)
{
    qDebug() << "Received messages:" << message1 << message2 << message3 << message4;
    rpm1 = message1;
}

int measureSetup::getMessage1()
{
    return rpm1; // Return the stored message1 value
}
