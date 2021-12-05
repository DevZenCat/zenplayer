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
    video->open("./sample-mp4-file.mp4");
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
