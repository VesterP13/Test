// measuresetup.h
#ifndef MEASURESETUP_H
#define MEASURESETUP_H

#include <QObject>
#include <QThread>
#include "measureloop.h"

class measureSetup : public QObject
{
    Q_OBJECT

public:
    explicit measureSetup(QObject *parent = nullptr);
    ~measureSetup();  // Destructor for cleanup

    void setup();

signals:
    void messageReceived(int message1, int message2, int message3, int message4);

public slots:
    void updateReceivedMessage(int message1, int message2, int message3, int message4);
    int getMessage1();

private:
    QThread measureThread;
    measureLoop *measureObject;
    int rpm1;
};

#endif // MEASURESETUP_H
