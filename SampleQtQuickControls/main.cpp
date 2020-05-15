#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include"camera_utils.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<camera_utils>("com.camera_utils", 1, 0, "cameraUtils");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

