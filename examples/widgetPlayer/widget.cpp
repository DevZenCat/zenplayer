#include "widget.h"
#include "ui_widget.h"

#include <QHBoxLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    _layout = new QHBoxLayout(this);


    video = new ZenPlayerWidget(this);
//    video->open("C:/Users/ecspertiza/AppData/Local/Joxi/tmp/video/1638730115226.mp4");
    video->open("C:/Users/ecspertiza/Documents/2021-12-06 10-06-11.mp4");
    _layout->addWidget(video);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    video->stop();
    QWidget::closeEvent(event);
}

bool pause = true;

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (pause) {
        video->play();
        pause = false;
    }else {
        video->pause();
        pause = true;
    }
}
