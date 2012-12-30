#ifndef _ProgressWidget_
#define _ProgressWidget_

#include <QTimer>
#include <QWidget>
#include <QMutex>
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
    // fade direction
    bool isin;
    // mutex
    QMutex mutex;

  public:

    // constructor
    explicit ProgressWidget(QWidget *parent = 0);
    // destructor
    ~ProgressWidget();
    // fadein (0->1)
    void fadein(size_t millis=0);
    // fadeout (1->0)
    void fadeout(size_t millis=0);
    // set opacity
    void setOpacity(double);

  public slots:

    // change opacity
    void changeOpacity();

};

#endif
