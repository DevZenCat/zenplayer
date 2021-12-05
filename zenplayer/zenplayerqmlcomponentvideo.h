#ifndef ZENPLAYERQMLCOMPONENTVIDEO_H
#define ZENPLAYERQMLCOMPONENTVIDEO_H

#include <QQuickItem>
#include <QQuickPaintedItem>

class ZenPlayerPresenter;
class ZenPlayerEngine;

class ZenPlayerQmlComponentVideo : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY (QString source READ source WRITE setSource NOTIFY sourceChanged)

    public:
        ZenPlayerQmlComponentVideo(QQuickItem *parent = 0);
        ~ZenPlayerQmlComponentVideo();

        void setSource(const QString &source);
        QString source();

        Q_INVOKABLE void open(const QString &fileName);
        Q_INVOKABLE void play();
        Q_INVOKABLE void stop();
        Q_INVOKABLE void pause();

    protected:
        void paint(QPainter *painter) override;

    private Q_SLOTS:
        void update();

    private:
        QSharedPointer<ZenPlayerPresenter> _presenter;
        QSharedPointer<ZenPlayerEngine> _engine;

        QString _source;

    Q_SIGNALS:
        void sourceChanged(const QString &source);
        void error(const QString &error);
};

#endif // ZENPLAYERQMLCOMPONENTVIDEO_H
