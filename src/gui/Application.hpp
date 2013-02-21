#ifndef _Application_
#define _Application_

#include <QApplication>
#include <exception>
#include <iostream>

using namespace std;

//===========================================================================
// Application class (used to catch exceptions)
//===========================================================================
class Application : public QApplication
{

  Q_OBJECT

  public:

    // constructor
    explicit Application(int &argc, char** argv) : QApplication(argc, argv) {}

    // notify method
    bool notify(QObject *rec, QEvent *ev)
    {
      try
      {
        return QApplication::notify(rec, ev);
      }
      catch (std::exception &e)
      {
        cerr << "error: " << e.what() << endl;
        this->exit(1);
      }
      catch(...)
      {
        cerr <<
          "panic: unexpected error. Please report this bug sending input\n"
          "file, ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
        this->exit(1);
      }
      return false;
    }

};

#endif

