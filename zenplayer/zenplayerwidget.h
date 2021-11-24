#ifndef ZENPLAYERWIDGET_H
#define ZENPLAYERWIDGET_H

#include <QWidget>
class ZenPlayerPresenter;
class ZenPlayerEngine;

class ZenPlayerWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit ZenPlayerWidget(QWidget *parent = nullptr);

        void start(const QString& fileName);
        void stop();

    protected:
        void paintEvent(QPaintEvent *event);

    private:
        void update();

    private:
        QSharedPointer<ZenPlayerPresenter> _presenter;
        QSharedPointer<ZenPlayerEngine> _engine;
};

#endif // ZENPLAYERWIDGET_H
