#ifndef ZENPLAYER_H
#define ZENPLAYER_H

#include <QObject>

class ZenPlayer : public QObject
{
    Q_OBJECT

    public:
        explicit ZenPlayer(QObject *parent = nullptr);

        static void qmlRegister();
};

#endif // ZENPLAYER_H
