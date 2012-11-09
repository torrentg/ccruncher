#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include <map>
#include <vector>
#include <QWidget>
#include <QString>
#include <QTimer>
#include "gui/QDebugStream.hpp"
#include "gui/TaskThread.hpp"

using namespace std;

namespace Ui {
class SimulationWidget;
}

class SimulationWidget : public QWidget
{
    Q_OBJECT

  private:

    // interface widget
    Ui::SimulationWidget *ui;
    // internal timer
    QTimer timer;
    // task thread
    TaskThread task;
    // defines
    map<string,string> defines;
    // stdout redirect
    QDebugStream qout;
    // stderr redirect
    QDebugStream qerr;

  private:

    // fill widget defines
    void setDefines();
    // check dialog status
    void check(bool clear);

  public:

    // constructor
    explicit SimulationWidget(QWidget *parent = 0);
    // destructor
    ~SimulationWidget();

  public slots:

    // selects input file
    void selectFile();
    // set input file
    void setFile();
    // selects output directory
    void selectDir();
    // set ooutput directory
    void setDir();
    // run ccruncher
    void run();
    // print message to log
    void print(const QString);
    // refresh progress bar
    void refresh();
    // defines dialog
    void showDefines();
    // set status
    void setStatus(int);

};

#endif
