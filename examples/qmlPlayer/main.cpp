#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QApplication>
#include <zenplayer.h>

#include <QDebug>

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    ZenPlayer::qmlRegister();

    QQmlApplicationEngine engine("qrc:/main.qml");
    return app.exec();

}
