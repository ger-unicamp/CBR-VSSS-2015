#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    rob = 1;
    velR = 300;
    velL = 300;

    LS.Open(DEVICE_PORT,9600);

}

Widget::~Widget()
{
    delete ui;
}


void Widget::keyPressEvent(QKeyEvent *event)
{
    qDebug()<< "Widget::keyPressEvent()";
    switch (event->key()) {
    case Qt::Key_W:
        ui->textBrowser->append("W pressed!");
        sprintf(order, "S%d#1#%d#1#%dE", rob, velR, velL);
        LS.WriteString(order);
        break;
    case Qt::Key_A:
        ui->textBrowser->append("A pressed!");
        sprintf(order, "S%d#1#%d#-1#%dE", rob, velR, velL);
        LS.WriteString(order);
        break;
    case Qt::Key_D:
        ui->textBrowser->append("S pressed!");
        sprintf(order, "S%d#-1#%d#1#%dE", rob, velR, velL);
        LS.WriteString(order);
        break;
    case Qt::Key_S:
        ui->textBrowser->append("D pressed!");
        sprintf(order, "S%d#-1#%d#-1#%dE", rob, velR, velL);
        LS.WriteString(order);
        break;
    default:
        sprintf(order, "S%d#0#0#0#0E", rob);
        LS.WriteString(order);
        break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    qDebug()<< "Widget::keyReleaseEvent()";
    switch (event->key()) {
    case Qt::Key_W:
       // ui->textBrowser->append("W released!");
      //  serial->envoi_chaine("S1#0#0#0#0E$");
        break;
    case Qt::Key_A:
      //  ui->textBrowser->append("A released!");
    //    serial->envoi_chaine("S1#0#0#0#0E$");
        break;
    case Qt::Key_S:
      //  ui->textBrowser->append("S released!");
      //  serial->envoi_chaine("S1#0#0#0#0E$");
        break;
    case Qt::Key_D:
       // ui->textBrowser->append("D released!");
      //  serial->envoi_chaine("S1#0#0#0#0E$");
        break;
    }
}

/** Slots**/

void Widget::on_initButton_clicked()
{
    qDebug()<< "Widget::on_initButton_clicked()";
}

void Widget::on_resetButton_clicked()
{
    qDebug()<< "Widget::on_resetButton_clicked()";

}

void Widget::on_radioButton_1_clicked()
{
    rob=1;
}

void Widget::on_radioButton_2_clicked()
{
    rob=2;
}

void Widget::on_radioButton_3_clicked()
{
    rob=3;
}

void Widget::on_sliderR_sliderReleased()
{
    velR = ui->sliderR->value();
    qDebug() << velR;
}

void Widget::on_sliderL_sliderReleased()
{
    velL = ui->sliderL->value();
    qDebug() << velL;
}


