#include "zenplayerwidget.h"

#include <QPainter>
#include "zenplayerpresenter.h"
#include "zenplayerengine.h"

ZenPlayerWidget::ZenPlayerWidget(QWidget *parent)
    : QWidget{parent}
{
    _engine = QSharedPointer<ZenPlayerEngine>::create();
    _presenter = QSharedPointer<ZenPlayerPresenter>::create(_engine.data());
    connect(_presenter.data(), &ZenPlayerPresenter::dataReady, this, &ZenPlayerWidget::update);
}

void ZenPlayerWidget::start(const QString& fileName)
{
    _engine->start(fileName);
}

void ZenPlayerWidget::stop()
{
    _engine->stop();
}

void ZenPlayerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0, width(), height());

    _presenter->paint(&painter, width(), height());
    painter.end();
}

void ZenPlayerWidget::update()
{
    QWidget::update();
}
