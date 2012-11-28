#ifndef _SimulationWidget_
#define _SimulationWidget_

#include <map>
#include <vector>
#include <QUrl>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QMutex>
#include <QTextCursor>
#include <QToolBar>
#include <QAction>
#include "gui/MdiChildWidget.hpp"
#include "gui/QStreamBuf.hpp"
#include "gui/SimulationTask.hpp"
#include "gui/ProgressWidget.hpp"

using namespace std;

namespace Ui {
class SimulationWidget;
}

class SimulationWidget : public MdiChildWidget
{
    Q_OBJECT

  private:

    // interface widget
    Ui::SimulationWidget *ui;
    // internal timer
    QTimer timer;
    // task thread
    SimulationTask task;
    // defines
    map<string,string> defines;
    // stdout redirect
    QStreamBuf qstream;
    // progress widget
    ProgressWidget *progress;
    // mutex
    QMutex mutex;
    // log line
    QString logline;
    // log cursor
    QTextCursor logcursor;
    // toolbar
    QToolBar *toolbar;
    // actions
    QAction *actionEdit;
    QAction *actionDefines;
    QAction *actionRun;
    QAction *actionStop;
    QAction *actionAnal;

  private:

    // set input file
    void setFile();
    // fill widget defines
    void setDefines();
    // update widgets status
    void updateControls();
    // clear log area
    void clearLog();
    // linkify a token
    void linkify(QString &);

  protected:

    // override close
    void closeEvent(QCloseEvent *event);

  public:

    // constructor
    explicit SimulationWidget(const QString &filename, QWidget *parent = 0);
    // destructor
    ~SimulationWidget();
    // virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    // edit current file
    void editFile();
    // selects output directory
    void selectDir();
    // set ooutput directory
    void setDir();
    // submit task
    void submit();
    // print message to log
    void log(const QString &);
    // draw widget
    void draw();
    // defines dialog
    void showDefines();
    // set status
    void setStatus(int);
    // open csv files
    void openData();

  signals:

    // anchor clicked
    void anchorClicked(const QUrl &);

};

#endif
