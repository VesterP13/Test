// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QThread>
#include <QQuickView>

#include "autogen/environment.h"
#include "App/connection.h"
#include "App/measuresetup.h"

measureSetup *globalMeasureSetup = nullptr;

void SetupMeasuringLoop()
{
    globalMeasureSetup = new measureSetup();
    globalMeasureSetup->setup();
}


int main(int argc, char *argv[])
{
    SetupMeasuringLoop();
    set_qt_environment();
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;



    qmlRegisterType<Connection>("Connections",1,0,"Connection");
    qmlRegisterType<measureSetup>("Measurment", 1, 0, "ReceiveMessages");

    const QUrl url(mainQmlFile);
    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;
    SetupMeasuringLoop();
    return app.exec();
}
