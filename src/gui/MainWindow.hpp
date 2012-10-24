#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <vector>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include "gui/TaskThread.hpp"

using namespace std;

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
    // task thread
    TaskThread task;
    // defines
    map<string,string> defines;

  private:

    // fill widget defines
    void setDefines();
    // check dialog status
    void check(bool clear);

  public:

    // constructor
    explicit MainWindow(QWidget *parent = 0);
    // destructor
    ~MainWindow();

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
