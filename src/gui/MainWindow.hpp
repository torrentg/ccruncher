#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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

  private:

    // delete montecarlo
    void deletemc();

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
    // print message to log
    void print(const QString);
    // refresh progress bar
    void refresh();

};

#endif
