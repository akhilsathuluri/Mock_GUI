#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include"camera_utils.h"
#include"torch_utils.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<cameraUtils>("io.qt.camerautils", 1, 0, "CamUtils");
    qmlRegisterType<torchUtils>("io.qt.torchutils", 1, 0, "TorUtils");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

