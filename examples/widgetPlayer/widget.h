#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <zenplayerwidget.h>

class QHBoxLayout;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    public:
        Widget(QWidget *parent = nullptr);
        ~Widget();

        void closeEvent(QCloseEvent *event);
        void mousePressEvent(QMouseEvent *event);

    private:
        Ui::Widget *ui;

        ZenPlayerWidget *video;
        bool pause = true;

        QHBoxLayout *_layout;
};
#endif // WIDGET_H
