#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "params/Params.hpp"
#include <QFont>
#include <QMessageBox>
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
	currentId = 1;
	ui->tabs->setCurrentIndex(0);
}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
	delete ui;
}

//===========================================================================
// combo alignment utility function
//===========================================================================
void MainWindow::setTextAlignment(QComboBox *mComboBox, Qt::Alignment alignment)
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
// change to tab x
//===========================================================================
void MainWindow::changeTab(int num)
{
	switch(num)
	{
		case -1:
			break;
		case 0:
			initTabParameters(currentId);
			break;
		case 1:
			initTabInterests(currentId);
			break;
		case 2:
			initTabRatings(currentId);
			break;
		case 3:
			initTabSectors(currentId);
			break;
		case 4:
			initTabSegmentations(currentId);
			break;
		case 5:
			initTabPortfolio(currentId);
			break;
		case 6:
			initTabSimulation(currentId);
			break;
		default:
			assert(false);
	}
}

//===========================================================================
// initialize parameters tab
//===========================================================================
void MainWindow::initTabParameters(int id)
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
	QString type;
	QList<pair<QString,QString> > ratings;

	database.getRatings(id, ratings, type);

	setTextAlignment(ui->pd_type, Qt::AlignCenter);

	ui->ratings->clearContents();
	ui->ratings->setRowCount(ratings.size());
	for(int i=0; i<ratings.size(); i++)
	{
		QTableWidgetItem *col0 = new QTableWidgetItem(ratings[i].first);
		col0->setTextAlignment(Qt::AlignCenter);
		ui->ratings->setItem(i, 0, col0);

		QTableWidgetItem *col1 = new QTableWidgetItem(ratings[i].second);
		col1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		ui->ratings->setItem(i, 1, col1);
	}
	ui->ratings->setColumnWidth(0, DEFAULT_COLUMN_WIDTH);

	if (type == "transition")
	{
		ui->pd_type->setCurrentIndex(0);
		ui->pd_label->setTitle(tr("Transition matrix"));
		ui->survival_values->setVisible(false);
		ui->transitions_values->setVisible(true);

		ui->transitions_values->clearContents();
		ui->transitions_values->setRowCount(ratings.size());
		ui->transitions_values->setColumnCount(ratings.size()+1);
		ui->transitions_values->setHorizontalHeaderItem(ratings.size(), new QTableWidgetItem("Sum"));
		ui->transitions_values->setColumnWidth(ratings.size(), DEFAULT_COLUMN_WIDTH);

		for(int i=0; i<ratings.size(); i++)
		{
			double sum = 0.0;

			QTableWidgetItem *vheader = new QTableWidgetItem(ratings[i].first);
			ui->transitions_values->setVerticalHeaderItem(i, vheader);
			QTableWidgetItem *hheader = new QTableWidgetItem(ratings[i].first);
			ui->transitions_values->setHorizontalHeaderItem(i, hheader);
			ui->transitions_values->setColumnWidth(i, DEFAULT_COLUMN_WIDTH);

			for(int j=0; j<ratings.size(); j++)
			{
				double val = database.getTransition(id, ratings[i].first, ratings[j].first);
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
			ui->transitions_values->setItem(i, ratings.size(), item);
		}
	}
	else if (type == "survival")
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
	void getSurvivalCurve(int id, const QString &rating, QList<pair<int,double> > &ratings) const;
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
	QList<pair<QString,QString> > sectors;

	database.getSectors(id, sectors);

	ui->sectors->clearContents();
	ui->sectors->setRowCount(sectors.size());
	for(int i=0; i<sectors.size(); i++)
	{
		QTableWidgetItem *col0 = new QTableWidgetItem(sectors[i].first);
		col0->setTextAlignment(Qt::AlignCenter);
		ui->sectors->setItem(i, 0, col0);

		QTableWidgetItem *col1 = new QTableWidgetItem(sectors[i].second);
		col1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		ui->sectors->setItem(i, 1, col1);
	}

	ui->correlations_values->clearContents();
	ui->correlations_values->setRowCount(sectors.size());
	ui->correlations_values->setColumnCount(sectors.size());

	for(int i=0; i<sectors.size(); i++)
	{
		QTableWidgetItem *vheader = new QTableWidgetItem(sectors[i].first);
		ui->correlations_values->setVerticalHeaderItem(i, vheader);
		QTableWidgetItem *hheader = new QTableWidgetItem(sectors[i].first);
		ui->correlations_values->setHorizontalHeaderItem(i, hheader);
		ui->correlations_values->setColumnWidth(i, DEFAULT_COLUMN_WIDTH);

		for(int j=i; j<sectors.size(); j++)
		{
			double val = database.getCorrelation(id, sectors[i].first, sectors[j].first);

			QTableWidgetItem *item = new QTableWidgetItem(QString::number(100*val,'f',2)+" %");
			item->setTextAlignment(Qt::AlignCenter);
			ui->correlations_values->setItem(i, j, item);
		}
	}
}

//===========================================================================
// initialize segmentations tab
//===========================================================================
void MainWindow::initTabSegmentations(int id)
{
	QList<pair<QString,QString> > segmentations;

	ui->segmentations->clear();
	ui->segmentations->setColumnCount(3);
	ui->segmentations->setColumnWidth(0, 200);
	ui->segmentations->setColumnWidth(1, 100);

	database.getSegmentations(id, segmentations);

	for(int i=0; i<segmentations.size(); i++)
	{
		QStringList contents;

		QList<QString> segments;
		database.getSegments(id, segmentations[i].first, segments);

		contents << segmentations[i].first << segmentations[i].second;
		QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, contents);
		ui->segmentations->insertTopLevelItem(i, item);

		QComboBox *type = new QComboBox(ui->segmentations);
		type->addItem("obligors");
		type->addItem("assets");
		type->setCurrentIndex((segmentations[i].second=="asset"?1:0));
		setTextAlignment(type, Qt::AlignCenter);
		ui->segmentations->setItemWidget(item, 1, type);

		for(int j=0; j<segments.size(); j++)
		{
			contents.clear();
			contents << segments[j] << "";
			QTreeWidgetItem *subitem = new QTreeWidgetItem(item, contents);
			item->addChild(subitem);
		}

	}

}

//===========================================================================
// initialize portfolio tab
//===========================================================================
void MainWindow::initTabPortfolio(int id)
{

	QList<QString> obligors;

	ui->obligors->clear();
	ui->obligors->setColumnCount(1);

	database.getObligors(id, obligors);

	for(int i=0; i<obligors.size(); i++)
	{
		QList<QString> assets;

		database.getAssets(id, obligors[i], assets);

		QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(obligors[i]));
		ui->obligors->insertTopLevelItem(i, item);

		for(int j=0; j<assets.size(); j++)
		{
			QTreeWidgetItem *subitem = new QTreeWidgetItem(item, QStringList(assets[j]));
			item->addChild(subitem);
		}
	}

	setObligor(id, obligors[0]);
}

//===========================================================================
// auxiliar function
//===========================================================================
void MainWindow::setCurrentIndex(QComboBox *combo, const QString &str)
{
	for(int i=0; i<combo->count(); i++)
	{
		if (combo->itemText(i) == str) {
			combo->setCurrentIndex(i);
			return;
		}
	}

	assert(false);
}

//===========================================================================
// initialize portfolio tab
//===========================================================================
void MainWindow::setObligor(int id, const QString &oid)
{
	QString type;
	QList<pair<QString,QString> > ratings;
	QString rating, sector, recovery;
	QList<pair<QString,QString> > segments;
	QList<pair<QString,QString> > sectors;

	database.getRatings(id, ratings, type);
	database.getSectors(id, sectors);
	database.getObligorProperties(id, oid, rating, sector, recovery, segments);

	ui->obligor_rating->clear();
	for(int i=0; i<ratings.size(); i++) {
		ui->obligor_rating->addItem(ratings[i].first);
	}
	ui->obligor_sector->clear();
	for(int i=0; i<sectors.size(); i++) {
		ui->obligor_sector->addItem(sectors[i].first);
	}

	ui->obligor_id->setText(oid);
	setCurrentIndex(ui->obligor_rating, rating);
	setCurrentIndex(ui->obligor_sector, sector);
	ui->obligor_recovery->setText(recovery);
	ui->obligor_segments->clearContents();
	ui->obligor_segments->setRowCount(segments.size());
	for(int i=0; i<segments.size(); i++)
	{

		QTableWidgetItem *col0 = new QTableWidgetItem(segments[i].first);
		col0->setTextAlignment(Qt::AlignCenter);
		ui->obligor_segments->setItem(i, 0, col0);

		QTableWidgetItem *col1 = new QTableWidgetItem(segments[i].second);
		col1->setTextAlignment(Qt::AlignCenter);
		ui->obligor_segments->setItem(i, 1, col1);
	}

// assets
//void getAssets(int id, const QString &o, QList<QString> &a) const;

}

//===========================================================================
// initialize simulation tab
//===========================================================================
void MainWindow::initTabSimulation(int id)
{
	ui->maxIterations->setValue(database.getProperty(id,Database::MaxIterations).toInt());
	ui->maxSeconds->setValue(database.getProperty(id,Database::MaxSeconds).toInt());
	ui->seed->setValue(database.getProperty(id,Database::Seed).toInt());
	ui->antithetic->setChecked(database.getProperty(id,Database::Antithetic).toBool());
	ui->onlyActive->setChecked(database.getProperty(id,Database::OnlyActive).toBool());
}

//===========================================================================
// update name
//===========================================================================
void MainWindow::updateName()
{
	QString str = ui->name->text();
	database.updateName(currentId, str);
}

//===========================================================================
// update time0
//===========================================================================
void MainWindow::updateTime0()
{
	try {
		database.updateProperty(currentId, Database::Time0, ui->time0->date());
	}
	catch(Exception &e) {
		QMessageBox::warning(this, "invalid value", e.what());
		QVariant val = database.getProperty(currentId, Database::Time0);
		ui->time0->setDate(val.toDate());
		ui->time0->setFocus();
	}
}

//===========================================================================
// update timeT
//===========================================================================
void MainWindow::updateTimeT()
{
	try {
		database.updateProperty(currentId, Database::TimeT, ui->timeT->date());
	}
	catch(Exception &e) {
		QMessageBox::warning(this, "invalid value", e.what());
		QVariant val = database.getProperty(currentId, Database::TimeT);
		ui->timeT->setDate(val.toDate());
		ui->timeT->setFocus();
	}
}
