#ifndef _SimulationWidget_
#define _SimulationWidget_

#include <map>
#include <vector>
#include <QWidget>
#include <QString>
#include <QTimer>
#include "gui/QStreamBuf.hpp"
#include "gui/SimulationTask.hpp"
#include "gui/ProgressWidget.hpp"

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
    SimulationTask task;
    // defines
    map<string,string> defines;
    // stdout redirect
    QStreamBuf qstream;
    // progress widget
    ProgressWidget *progress;

  private:

    // set input file
    void setFile();
    // fill widget defines
    void setDefines();
    // update widgets status
    void updateControls();
    // clear log area
    void clearLog();

  public:

    // constructor
    explicit SimulationWidget(const QString &filename, QWidget *parent = 0);
    // destructor
    ~SimulationWidget();

  public slots:

    // selects output directory
    void selectDir();
    // set ooutput directory
    void setDir();
    // submit task
    void submit();
    // print message to log
    void log(const QString);
    // draw widget
    void draw();
    // defines dialog
    void showDefines();
    // set status
    void setStatus(int);

};

#endif
