#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#define CONFIG_UNASSIGNED_STRING "UNASSIGNED"
#define CONFIG_UNASSIGNED_INT "UNASSIGNED"

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(const Configuration &c, QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow), config(c)
{
	ui->setupUi(this);
	ui->mainToolBar->setVisible(false);

	// enabling database
	database.init(config.db_driver, config.db_host, config.db_port, config.db_name, config.db_user, config.db_password);

	// filling forms
	setData(1);
}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
	delete ui;
}

//===========================================================================
// destructor
//===========================================================================
void MainWindow::setData(int id)
{
	// parameters tab
	ui->name->setText(database.getName(id));
	ui->description->setPlainText(database.getDescription(id));
	ui->time0->setDate(database.getProperty(id,Database::Time0).toDate());
	ui->timeT->setDate(database.getProperty(id,Database::TimeT).toDate());


	// simulation tab
	ui->maxIterations->setValue(database.getProperty(id,Database::MaxIterations).toInt());
	ui->maxSeconds->setValue(database.getProperty(id,Database::MaxSeconds).toInt());
	ui->seed->setValue(database.getProperty(id,Database::Seed).toInt());
	ui->antithetic->setChecked(database.getProperty(id,Database::Antithetic).toBool());
	ui->onlyActive->setChecked(database.getProperty(id,Database::OnlyActive).toBool());
}
