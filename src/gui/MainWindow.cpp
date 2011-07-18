#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "params/Params.hpp"
#include <QFont>
#include <cassert>

#define DEFAULT_COLUMN_WIDTH 70


//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(const Configuration &c, QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow), config(c)
{
	ui->setupUi(this);
	ui->mainToolBar->setVisible(false);

	// enabling database
	database.init(config.db_driver, config.db_host,
				  config.db_port, config.db_name,
				  config.db_user, config.db_password);

	// linking signals and slots
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

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

void setTextAlignment(QComboBox *mComboBox, Qt::Alignment alignment)
{
	// First : Set the combobox the editable (this allows us to use the lineEdit)
	mComboBox->setEditable(true);
	// Second : Put the lineEdit in read-only mode
	mComboBox->lineEdit()->setReadOnly(true);
	// Third  : Align the lineEdit to right
	mComboBox->lineEdit()->setAlignment(alignment);
	// Fourth : Align each item in the combo to the right
	for(int i = 0; i < mComboBox->count(); i++)	{
		mComboBox->setItemData(i, QVariant(alignment), Qt::TextAlignmentRole);
	}
}

//===========================================================================
// destructor
//===========================================================================
void MainWindow::setData(int id)
{
	initTabParamaters(id);
	initTabInterests(id);
	initTabRatings(id);
	initTabSectors(id);

	// simulation tab
	ui->maxIterations->setValue(database.getProperty(id,Database::MaxIterations).toInt());
	ui->maxSeconds->setValue(database.getProperty(id,Database::MaxSeconds).toInt());
	ui->seed->setValue(database.getProperty(id,Database::Seed).toInt());
	ui->antithetic->setChecked(database.getProperty(id,Database::Antithetic).toBool());
	ui->onlyActive->setChecked(database.getProperty(id,Database::OnlyActive).toBool());
}


//===========================================================================
// initialize parameters tab
//===========================================================================
void MainWindow::initTabParamaters(int id)
{
	QString str;

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
}

//===========================================================================
// interests tab
//===========================================================================
void MainWindow::initTabInterests(int id)
{
	QString str;
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
	setTextAlignment(ui->interest_type, Qt::AlignCenter);

	ui->interest_values->clearContents();
	ui->interest_values->setRowCount(values.size());
	ui->interest_values->setColumnWidth(0, DEFAULT_COLUMN_WIDTH);
	ui->interest_values->setColumnWidth(1, DEFAULT_COLUMN_WIDTH);

	for(int i=0; i<values.size(); i++)
	{
		delete ui->interest_values->verticalHeaderItem(i);

		QTableWidgetItem *col0 = new QTableWidgetItem(QString::number(values[i].first));
		col0->setTextAlignment(Qt::AlignCenter);
		ui->interest_values->setItem(i, 0, col0);


		QTableWidgetItem *col1 = new QTableWidgetItem(QString::number(100*values[i].second, 'f', 2) + " %");
		col1->setTextAlignment(Qt::AlignCenter);
		ui->interest_values->setItem(i, 1, col1);
	}
}

//===========================================================================
// ratings tab
//===========================================================================
void MainWindow::initTabRatings(int id)
{
	QString str;
	QList<pair<QString,QString> > values;

	database.getRatings(id, values, str);

	setTextAlignment(ui->pd_type, Qt::AlignCenter);

	ui->ratings->clearContents();
	ui->ratings->setRowCount(values.size());
	for(int i=0; i<values.size(); i++)
	{
		delete ui->ratings->verticalHeaderItem(i);

		QTableWidgetItem *col0 = new QTableWidgetItem(values[i].first);
		col0->setTextAlignment(Qt::AlignCenter);
		ui->ratings->setItem(i, 0, col0);

		QTableWidgetItem *col1 = new QTableWidgetItem(values[i].second);
		col1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		ui->ratings->setItem(i, 1, col1);
	}

	if (str == "transition")
	{
		ui->pd_type->setCurrentIndex(0);
		ui->pd_label->setTitle(tr("Transition matrix"));
		ui->survival_values->setVisible(false);
		ui->transitions_values->setVisible(true);

		ui->transitions_values->clearContents();
		ui->transitions_values->setRowCount(values.size());
		ui->transitions_values->setColumnCount(values.size()+1);
		ui->transitions_values->setHorizontalHeaderItem(values.size(), new QTableWidgetItem("Sum"));
		ui->transitions_values->setColumnWidth(values.size(), DEFAULT_COLUMN_WIDTH);

		for(int i=0; i<values.size(); i++)
		{
			double sum = 0.0;

			QTableWidgetItem *vheader = new QTableWidgetItem(values[i].first);
			ui->transitions_values->setVerticalHeaderItem(i, vheader);
			QTableWidgetItem *hheader = new QTableWidgetItem(values[i].first);
			ui->transitions_values->setHorizontalHeaderItem(i, hheader);
			ui->transitions_values->setColumnWidth(i, DEFAULT_COLUMN_WIDTH);

			for(int j=0; j<values.size(); j++)
			{
				double val = database.getTransition(id, values[i].first, values[j].first);
				sum += val;

				QTableWidgetItem *item = new QTableWidgetItem(QString::number(100*val,'f',2)+" %");
				item->setTextAlignment(Qt::AlignCenter);
				ui->transitions_values->setItem(i, j, item);
			}

			QTableWidgetItem *item = new QTableWidgetItem(QString::number(100*sum,'f',2)+" %");
			item->setTextAlignment(Qt::AlignCenter);
			QFont font = item->font();
			font.setBold(true);
			item->setFont(font);
			ui->transitions_values->setItem(i, values.size(), item);
		}
	}
	else if (str == "survival")
	{
		ui->pd_type->setCurrentIndex(1);
		ui->pd_label->setTitle(tr("Survival curve"));
		ui->survival_values->setVisible(true);
		ui->transitions_values->setVisible(false);
	}
	else
	{
		assert(false);
	}

/*
	// survival curve
	void getSurvivalCurve(int id, const QString &rating, QList<pair<int,double> > &values) const;
	// transition period
	int getTransitionPeriod(int id);
	// transition matrix

	*/
}

//===========================================================================
// sectors tab
//===========================================================================
void MainWindow::initTabSectors(int id)
{
	QList<pair<QString,QString> > values;

	database.getSectors(id, values);

	ui->sectors->clearContents();
	ui->sectors->setRowCount(values.size());
	for(int i=0; i<values.size(); i++)
	{
		delete ui->sectors->verticalHeaderItem(i);

		QTableWidgetItem *col0 = new QTableWidgetItem(values[i].first);
		col0->setTextAlignment(Qt::AlignCenter);
		ui->sectors->setItem(i, 0, col0);

		QTableWidgetItem *col1 = new QTableWidgetItem(values[i].second);
		col1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		ui->sectors->setItem(i, 1, col1);
	}

	ui->correlations_values->clearContents();
	ui->correlations_values->setRowCount(values.size());
	ui->correlations_values->setColumnCount(values.size());

	for(int i=0; i<values.size(); i++)
	{
		QTableWidgetItem *vheader = new QTableWidgetItem(values[i].first);
		ui->correlations_values->setVerticalHeaderItem(i, vheader);
		QTableWidgetItem *hheader = new QTableWidgetItem(values[i].first);
		ui->correlations_values->setHorizontalHeaderItem(i, hheader);
		ui->correlations_values->setColumnWidth(i, DEFAULT_COLUMN_WIDTH);

		for(int j=i; j<values.size(); j++)
		{
			double val = database.getCorrelation(id, values[i].first, values[j].first);

			QTableWidgetItem *item = new QTableWidgetItem(QString::number(100*val,'f',2)+" %");
			item->setTextAlignment(Qt::AlignCenter);
			ui->correlations_values->setItem(i, j, item);
		}
	}
}
