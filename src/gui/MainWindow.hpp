#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "Configuration.hpp"
#include "Database.hpp"

namespace Ui {
    class MainWindow;
}

using namespace ccruncher;
using namespace ccruncher_gui;
namespace ccruncher_gui {

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

	// widgets
	Ui::MainWindow *ui;
	// paramaters
	Configuration config;
	// Database
	Database database;


private:

	// set data
	void setData(int id);
	// initialize paramaters tab
	void initTabParamaters(int id);
	// initialize interests tab
	void initTabInterests(int id);
	// initialize ratings tab
	void initTabRatings(int id);
	// initialize sectors tab
	void initTabSectors(int id);

public:

	// constructor
	explicit MainWindow(const Configuration &c, QWidget *parent = 0);
	// destructor
    ~MainWindow();

};

}

#endif
