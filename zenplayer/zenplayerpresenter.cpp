#include "zenplayerpresenter.h"
#include "zenplayerengine.h"

#include <QPainter>
#include <QDebug>

ZenPlayerPresenter::ZenPlayerPresenter(ZenPlayerEngine *engine, QObject *parent)
    : QObject{parent}
    , _engine(engine)
    , _currentFrame(nullptr)
{
    connect(_engine, &ZenPlayerEngine::frameComplete, this, &ZenPlayerPresenter::updateFrame, Qt::DirectConnection);
}

void ZenPlayerPresenter::updateFrame(QImage *frame)
{
    QMutexLocker locker(&_mutex);

    if (_currentFrame != nullptr) {
        delete _currentFrame;
        _currentFrame = nullptr;
    }

    _currentFrame = frame;

    Q_EMIT dataReady();
}

void ZenPlayerPresenter::paint(QPainter *painter, int width, int height)
{
    QMutexLocker locker(&_mutex);

    if (_currentFrame == nullptr) {
        return;
    }

    painter->drawImage(QRect(0,0,width, height), *_currentFrame, QRect(0,0,_currentFrame->width(), _currentFrame->height()));
}
