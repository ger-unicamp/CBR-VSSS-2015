#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include "Serial.h"

#define DEVICE_PORT "COM5"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    
private slots:
  
    void on_initButton_clicked();
    void on_resetButton_clicked();
    void on_radioButton_1_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_sliderR_sliderReleased();
    void on_sliderL_sliderReleased();


private:
    Ui::Widget *ui;

    serialib LS;

    int rob;
    char order[20];
    int velR;
    int velL;

};

#endif // WIDGET_H
