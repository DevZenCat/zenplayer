#ifndef ZENPLAYERPRESENTER_H
#define ZENPLAYERPRESENTER_H

#include <QObject>
#include <QMutex>

class ZenPlayerEngine;
class QPainter;

class ZenPlayerPresenter : public QObject
{
    Q_OBJECT

    public:
        explicit ZenPlayerPresenter(ZenPlayerEngine *engine, QObject *parent = nullptr);

        void paint(QPainter *painter, int width, int height);

    Q_SIGNALS:
        void dataReady();

    private Q_SLOTS:
        void updateFrame(QImage *image);

    private:
        ZenPlayerEngine *_engine;
        QImage *_currentFrame;
        QMutex _mutex;
};

#endif // ZENPLAYERPRESENTER_H
