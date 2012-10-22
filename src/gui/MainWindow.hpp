#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <vector>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include "kernel/MonteCarlo.hpp"

using namespace std;
using namespace ccruncher;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  private:

    // interface widget
    Ui::MainWindow *ui;
    // internal timer
    QTimer timer;
    // Monte Carlo simulator
    MonteCarlo *montecarlo;
    // defines
    map<string,string> defines;

  private:

    // delete montecarlo
    void deletemc();
    // fill widget defines
    void setDefines();

  public:

    // constructor
    explicit MainWindow(QWidget *parent = 0);
    // destructor
    ~MainWindow();

  public slots:

    // selects input file
    void selectFile();
    // selects output directory
    void selectDir();
    // check dialog status
    void check();
    // run ccruncher
    void run();
    // stop ccruncher
    void stop();
    // print message to log
    void print(const QString);
    // refresh progress bar
    void refresh();
    // defines dialog
    void showDefines();

};

#endif
