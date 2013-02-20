#ifndef _DefinesDialog_
#define _DefinesDialog_

#include <map>
#include <string>
#include <QDialog>

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
    std::map<std::string,std::string> defines;

  public:

    // constructor
    explicit DefinesDialog(QWidget *parent, const std::map<std::string, std::string> &);
    // destructor
    ~DefinesDialog();
    // return content
    const std::map<std::string,std::string> & getDefines() const;

  public slots:

    // cell content changed
    void cellChanged(int, int);
    // submit dialog content
    void submit();

};

#endif

