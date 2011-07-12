#include <exception>
#include <iostream>
#include <exception>
#include <QApplication>
#include <QTextCodec>
#include <QString>
#include "Exception.hpp"
#include "MainWindow.hpp"
#include "Configuration.hpp"

#define DEFAULT_CONFIG_FILENAME "ccruncher.conf"

using namespace std;

int main(int argc, char *argv[])
{
	QString filename;

	// program initialization
	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	// checking arguments
	if (argc == 1) {
		filename = DEFAULT_CONFIG_FILENAME;
	}
	else {
		filename  = QString(argv[1]);
	}

	try
	{
		Configuration config(filename);
		MainWindow w(config);
		w.show();
		return a.exec();
	}
	catch(Exception &e)
	{
		cout << "error: " << e << endl;
		return 1;
	}
	catch(exception &e)
	{
		cout << "error: " << e.what() << endl;
		return 1;
	}
	catch(...)
	{
		cout << "panic: unexpected error" << endl;
		return 1;
	}
}
