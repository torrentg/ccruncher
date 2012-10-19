#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "gui/QDebugStream.hpp"
#include "kernel/MonteCarlo.hpp"

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
    // redirects cout to textedit
    QDebugStream *qout;
    // redirects cerr to textedit
    QDebugStream *qerr;
    // Monte Carlo simulator
    MonteCarlo *montecarlo;

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

};

#endif
