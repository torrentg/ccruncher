#include <unistd.h>
#include <iostream>
#include <gsl/gsl_errno.h>
#include <QApplication>
#include <QTextCodec>
#include "gui/MainWindow.hpp"
#include "utils/Format.hpp"
#include "utils/CsvFile.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// gsl error handler
//===========================================================================
void gsl_handler(const char * reason, const char *file, int line, int gsl_errno)
{
  UNUSED(file);
  UNUSED(line);
  UNUSED(gsl_errno);
  string msg = reason;
  msg += " (file=" + string(file);
  msg += ", line=" + Format::toString(line);
  msg += ", errno=" + Format::toString(errno) + ")";
  Exception gsl_exception(msg);
  throw Exception(gsl_exception, "gsl exception");
}

//===========================================================================
// main function
//===========================================================================
int main(int argc, char *argv[])
{
/*
try {
  CsvFile csv("C:/Users/gtorrent/Projects/ccruncher/data/offices.csv", ",");
  vector<string> headers = csv.getHeaders();
  for(size_t i=0; i<headers.size(); i++) {
    cout << "header[" << i+1 << "] = " << headers[i] << endl;
    vector<double> values;
    csv.getValues(i, values);
    for(size_t j=0; j<values.size(); j++) {
        cout << "  " << values[j] << endl;
    }
  }
}
catch(std::exception &e) {
  cerr << e.what() << endl;
}
  return 0;
*/
  chdir("..");
  QApplication a(argc, argv);
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  setlocale(LC_ALL, "C"); // sets decimal point to sprintf

  gsl_set_error_handler(gsl_handler);

  MainWindow w;
  w.show();
  return a.exec();
}
