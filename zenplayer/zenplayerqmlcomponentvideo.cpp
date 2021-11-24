#include "zenplayerqmlcomponentvideo.h"
#include "zenplayerpresenter.h"
#include "zenplayerengine.h"

#include <QPainter>

ZenPlayerQmlComponentVideo::ZenPlayerQmlComponentVideo(QQuickItem *parent)
    :QQuickPaintedItem(parent)
{
    _engine = QSharedPointer<ZenPlayerEngine>::create();
    _presenter = QSharedPointer<ZenPlayerPresenter>::create(_engine.data());

    connect(_presenter.data(), &ZenPlayerPresenter::dataReady, this, &ZenPlayerQmlComponentVideo::update);
    connect(_engine.data(), &ZenPlayerEngine::error, this, &ZenPlayerQmlComponentVideo::error);
}

ZenPlayerQmlComponentVideo::~ZenPlayerQmlComponentVideo()
{
    _engine->stop();
}

void ZenPlayerQmlComponentVideo::start()
{
    if (_source.isEmpty()) {
        Q_EMIT error("Source is empty");
        return;
    }
    _engine->start(_source);
}

void ZenPlayerQmlComponentVideo::stop()
{
    _engine->stop();
}

void ZenPlayerQmlComponentVideo::paint(QPainter *painter)
{
    painter->setBrush(Qt::black);
    painter->drawRect(0,0, width(), height());

    _presenter->paint(painter, width(), height());
}

void ZenPlayerQmlComponentVideo::update()
{
    QQuickPaintedItem::update();
}

void ZenPlayerQmlComponentVideo::setSource(const QString &source)
{
    if (_source != source) {
        _source = source;
        Q_EMIT sourceChanged(_source);
    }
}

QString ZenPlayerQmlComponentVideo::source()
{
    return _source;
}
