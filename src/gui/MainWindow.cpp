#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "params/Params.hpp"
#include <cassert>

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
	QString str;

	// parameters tab
	ui->name->setText(database.getName(id));
	ui->description->setPlainText(database.getDescription(id));
	ui->time0->setDate(database.getProperty(id,Database::Time0).toDate());
	ui->timeT->setDate(database.getProperty(id,Database::TimeT).toDate());
	str = database.getProperty(id,Database::CopulaType).toString();
	if (Params::getCopulaType(str.toStdString()) == "gaussian") {
		ui->copula->setCurrentIndex(0);
		ui->degrees_of_freedom->setEnabled(false);
	}
	else if (Params::getCopulaType(str.toStdString()) == "t") {
		ui->copula->setCurrentIndex(1);
		ui->degrees_of_freedom->setEnabled(true);
		double val = Params::getCopulaParam(str.toStdString());
		ui->degrees_of_freedom->setValue(val);
	}
	else {
		assert(false);
	}

	// interests tab
	QList<pair<int,double> > values;
	database.getInterest(id, values, str);
	if (str == "simple") {
		ui->interest_type->setCurrentIndex(0);
	}
	else if (str == "compound") {
		ui->interest_type->setCurrentIndex(1);
	}
	else if (str == "continuous") {
		ui->interest_type->setCurrentIndex(2);
	}
	else {
		assert(false);
	}
	ui->interest_values->clearContents();
	ui->interest_values->setRowCount(values.size());
	for(int i=0; i<values.size(); i++) {
		ui->interest_values->setItem(i, 0, new QTableWidgetItem(QString::number(values[i].first)));
		ui->interest_values->setItem(i, 1, new QTableWidgetItem(QString::number(values[i].second)));
	}

	// simulation tab
	ui->maxIterations->setValue(database.getProperty(id,Database::MaxIterations).toInt());
	ui->maxSeconds->setValue(database.getProperty(id,Database::MaxSeconds).toInt());
	ui->seed->setValue(database.getProperty(id,Database::Seed).toInt());
	ui->antithetic->setChecked(database.getProperty(id,Database::Antithetic).toBool());
	ui->onlyActive->setChecked(database.getProperty(id,Database::OnlyActive).toBool());
}
