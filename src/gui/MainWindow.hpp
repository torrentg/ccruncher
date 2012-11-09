#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QString>
#include <QMdiArea>

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
    // mdi area
    QMdiArea *mdiArea;

  private:

    // find mdi child
    QMdiSubWindow *findMdiChild(const QString &fileName);

  public:

    // constructor
    explicit MainWindow(QWidget *parent = 0);
    // destructor
    ~MainWindow();

  public slots:

    // about dialog
    void about();
    // open file
    void openFile();
    // exit app
    void exit();

};

#endif
