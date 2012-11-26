#ifndef _MainWindow_
#define _MainWindow_

#include <QMainWindow>
#include <QString>
#include <QMdiArea>
#include <QUrl>
#include <QCloseEvent>

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
    QMdiSubWindow *findMdiChild(const QString &filename);

  protected:

    // close event
    void closeEvent(QCloseEvent *event);

  public:

    // constructor
    explicit MainWindow(QWidget *parent = 0);
    // destructor
    ~MainWindow();

  public slots:

    // about dialog
    void about();
    // select file
    void selectFile();
    // open file
    void openFile(const QUrl &url);

};

#endif
