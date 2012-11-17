#ifndef _ProgressWidget_
#define _ProgressWidget_

#include <QTimer>
#include <QWidget>
#include <QPaintEvent>
#include "ui_ProgressWidget.h"

using namespace std;

class ProgressWidget : public QWidget
{
    Q_OBJECT

  public:

    // interface widget
    Ui::ProgressWidget *ui;
    // fade timer
    QTimer timer;
    // fade alpha
    double opacity;
    // fade duration
    int duration;

  public:

    // constructor
    explicit ProgressWidget(QWidget *parent = 0);
    // destructor
    ~ProgressWidget();
    // fade
    void fade(int millis=0);
    // set opacity
    void setOpacity(double);

  public slots:

    // overwrite
    void show();
    // decrease
    void decrease();

};

#endif
