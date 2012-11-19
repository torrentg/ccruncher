#ifndef _DefinesDialog_
#define _DefinesDialog_

#include <map>
#include <QDialog>

using namespace std;

namespace Ui {
class DefinesDialog;
}

class DefinesDialog : public QDialog
{
    Q_OBJECT

  private:

    // dialog widgets
    Ui::DefinesDialog *ui;
    // defines
    map<string,string> defines;

  public:

    // constructor
    explicit DefinesDialog(QWidget *parent, const map<string, string> &);
    // destructor
    ~DefinesDialog();
    // return content
    const map<string,string> & getDefines() const;

  public slots:

    // cell content changed
    void cellChanged(int, int);
    // submit dialog content
    void submit();

};

#endif
