#include <iostream>
#include <QApplication>
#include <QTextCodec>
#include "gui/MainWindow.hpp"
#include "gui/QDebugStream.hpp"
#include "utils/Utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    setlocale(LC_ALL, "C"); // sets decimal point to sprintf
    QDebugStream qout(cout);
    QDebugStream qerr(cerr);
    MainWindow w;
    qApp->connect(&qout, SIGNAL(print(QString)), &w, SLOT(print(QString)), Qt::QueuedConnection);
    qApp->connect(&qerr, SIGNAL(print(QString)), &w, SLOT(print(QString)), Qt::QueuedConnection);
    w.show();
    cout << Utils::copyright() << endl;
    return a.exec();
}
