#include "Database.hpp"
#include <cassert>

using namespace ccruncher_gui;

//===========================================================================
// initialize database connection
//===========================================================================
void Database::init(const QString &driver, const QString &host, const int port, const QString &name, const QString &user, const QString &passwd) throw(Exception)
{
	dbdriver = driver;
	dbhost = host;
	dbport = port;
	dbname = name;
	dbuser = user;
	dbpassword = passwd;

	db = QSqlDatabase::addDatabase(dbdriver);
	if (!db.isValid()) {
		throw string("invalid driver");
	}

	db.setHostName(dbhost);
	db.setPort(dbport);
	db.setDatabaseName(dbname);
	db.setUserName(dbuser);
	db.setPassword(dbpassword);
	if (!db.open())
	{
		throw Exception("unable to open database " + name.toStdString());
	}
}

//===========================================================================
// return simulation identifiers
//===========================================================================
void Database::getIds(QList<int> &ret) const
{
	ret.clear();
	ret.append(1);
}

//===========================================================================
// return simulation identifiers
//===========================================================================
QString Database::getName(int id) const
{
	Q_UNUSED(id);
	return "sample1";
}

//===========================================================================
// get description
//===========================================================================
QString Database::getDescription(int id) const
{
	Q_UNUSED(id);
	return "this is a sample input file";
}

//===========================================================================
// get simulation date
//===========================================================================
QVariant Database::getProperty(int id, Property p) const
{
	Q_UNUSED(id);

	switch(p)
	{
		case Database::Time0:
			return QDate(2009, 10, 18);
			break;
		case Database::TimeT:
			return QDate(2015, 7, 15);
			break;
		case Database::MaxIterations:
			return 1000000;
			break;
		case Database::MaxSeconds:
			return 3600;
			break;
		case Database::CopulaType:
			return "t(5)";
			break;
		case Database::Seed:
			return 0;
			break;
		case Database::Antithetic:
			return true;
			break;
		case Database::OnlyActive:
			return true;
			break;
		default:
			assert(false);
	}
}

//===========================================================================
// interest curve
//===========================================================================
void Database::getInterest(int id, QList<pair<int,double> > &values, QString &type) const
{
	Q_UNUSED(id);
	values.clear();
	values.append(pair<int,double>(0, 0.0));
	values.append(pair<int,double>(1, 0.04));
	values.append(pair<int,double>(2, 0.041));
	values.append(pair<int,double>(3, 0.045));
	values.append(pair<int,double>(6, 0.0455));
	values.append(pair<int,double>(12, 0.048));
	values.append(pair<int,double>(24, 0.049));
	values.append(pair<int,double>(60, 0.05));
	values.append(pair<int,double>(120, 0.052));
	type = "compound";
}

//===========================================================================
// ratings
//===========================================================================
void Database::getRatings(int id, QList<pair<QString,QString> > &values, QString &type) const
{
	Q_UNUSED(id);
	values.clear();
	values.append(pair<QString,QString>("AAA","prime"));
	values.append(pair<QString,QString>("AA","high grade"));
	values.append(pair<QString,QString>("A","upper medium grade"));
	values.append(pair<QString,QString>("BBB","lower medium grade"));
	values.append(pair<QString,QString>("BB","non investement grade especulative"));
	values.append(pair<QString,QString>("B","highly speculative"));
	values.append(pair<QString,QString>("CCC","extremely speculative"));
	values.append(pair<QString,QString>("D","in default"));
	type = "transition";
}

//===========================================================================
// survival curve
//===========================================================================
void Database::getSurvivalCurve(int id, const QString &rating, QList<pair<int,double> > &values) const
{
	Q_UNUSED(id);
	values.clear();
	if (rating != "D")
	{
		values.append(pair<int,double>(0,1.0));
		values.append(pair<int,double>(12,0.95));
		values.append(pair<int,double>(60,0.90));
		values.append(pair<int,double>(120,0.85));
		values.append(pair<int,double>(240,0.80));
	}
	else
	{
		values.append(pair<int,double>(0,0.0));
		values.append(pair<int,double>(120,0.0));
	}
}

//===========================================================================
// transition period
//===========================================================================
int Database::getTransitionPeriod(int id)
{
	Q_UNUSED(id);
	return 12;
}

//===========================================================================
// transition matrix
//===========================================================================
double Database::getTransition(int id, const QString &r1, const QString &r2) const
{
	Q_UNUSED(id);

	if (r1 == "AAA")
	{
		if (r2 == "AAA") return 0.9283;
		else if (r2 == "AA") return 0.065;
		else if (r2 == "A") return 0.0056;
		else if (r2 == "BBB") return 0.0006;
		else if (r2 == "BB") return 0.0;
		else if (r2 == "B") return 0.0;
		else if (r2 == "CCC") return 0.0;
		else if (r2 == "D") return 0.0;
		else assert(false);
	}
	else if (r1 == "AA")
	{
		if (r2 == "AAA") return 0.063;
		else if (r2 == "AA") return 0.9187;
		else if (r2 == "A") return 0.0664;
		else if (r2 == "BBB") return 0.0065;
		else if (r2 == "BB") return 0.0006;
		else if (r2 == "B") return 0.0011;
		else if (r2 == "CCC") return 0.0004;
		else if (r2 == "D") return 0.0;
		else assert(false);
	}
	else if (r1 == "A")
	{
		if (r2 == "AAA") return 0.0008;
		else if (r2 == "AA") return 0.0226;
		else if (r2 == "A") return 0.9165;
		else if (r2 == "BBB") return 0.0511;
		else if (r2 == "BB") return 0.0061;
		else if (r2 == "B") return 0.0023;
		else if (r2 == "CCC") return 0.0002;
		else if (r2 == "D") return 0.0004;
		else assert(false);
	}
	else if (r1 == "BBB")
	{
		if (r2 == "AAA") return 0.0005;
		else if (r2 == "AA") return 0.0027;
		else if (r2 == "A") return 0.0584;
		else if (r2 == "BBB") return 0.8776;
		else if (r2 == "BB") return 0.0474;
		else if (r2 == "B") return 0.0098;
		else if (r2 == "CCC") return 0.0015;
		else if (r2 == "D") return 0.0021;
		else assert(false);
	}
	else if (r1 == "BB")
	{
		if (r2 == "AAA") return 0.0004;
		else if (r2 == "AA") return 0.0011;
		else if (r2 == "A") return 0.0064;
		else if (r2 == "BBB") return 0.0785;
		else if (r2 == "BB") return 0.8113;
		else if (r2 == "B") return 0.0827;
		else if (r2 == "CCC") return 0.0089;
		else if (r2 == "D") return 0.0107;
		else assert(false);
	}
	else if (r1 == "B")
	{
		if (r2 == "AAA") return 0.0;
		else if (r2 == "AA") return 0.0011;
		else if (r2 == "A") return 0.0030;
		else if (r2 == "BBB") return 0.0042;
		else if (r2 == "BB") return 0.0675;
		else if (r2 == "B") return 0.8308;
		else if (r2 == "CCC") return 0.0386;
		else if (r2 == "D") return 0.0548;
		else assert(false);
	}
	else if (r1 == "CCC")
	{
		if (r2 == "AAA") return 0.19;
		else if (r2 == "AA") return 0.0;
		else if (r2 == "A") return 0.0038;
		else if (r2 == "BBB") return 0.0075;
		else if (r2 == "BB") return 0.0244;
		else if (r2 == "B") return 0.1203;
		else if (r2 == "CCC") return 0.6071;
		else if (r2 == "D") return 0.2350;
		else assert(false);
	}
	else if (r1 == "D")
	{
		if (r2 == "AAA") return 0.0;
		else if (r2 == "AA") return 0.0;
		else if (r2 == "A") return 0.0;
		else if (r2 == "BBB") return 0.0;
		else if (r2 == "BB") return 0.0;
		else if (r2 == "B") return 0.0;
		else if (r2 == "CCC") return 0.0;
		else if (r2 == "D") return 1.0;
		else assert(false);
	}
	else assert(false);
}

//===========================================================================
// sectors
//===========================================================================
void Database::getSectors(int id, QList<pair<QString,QString> > &values) const
{
	Q_UNUSED(id);
	values.clear();
	values.append(pair<QString,QString>("S1","Consumer/service sector"));
	values.append(pair<QString,QString>("S2","Forest/building products"));
	values.append(pair<QString,QString>("S3","High technology"));
	values.append(pair<QString,QString>("S4","Utilities"));
	values.append(pair<QString,QString>("S5","Other sectors"));
}

//===========================================================================
// correlation
//===========================================================================
double Database::getCorrelation(int id, const QString &s1, const QString &s2) const
{
	Q_UNUSED(id);
	if (s1 == "S1" && s2 == "S1") return 0.1510;
	else if (s1 == "S1" && s2 == "S2") return 0.0620;
	else if (s1 == "S1" && s2 == "S3") return 0.0860;
	else if (s1 == "S1" && s2 == "S4") return 0.0390;
	else if (s1 == "S1" && s2 == "S5") return 0.0910;
	else if (s1 == "S2" && s2 == "S2") return 0.1790;
	else if (s1 == "S2" && s2 == "S3") return 0.0630;
	else if (s1 == "S2" && s2 == "S4") return 0.1020;
	else if (s1 == "S2" && s2 == "S5") return 0.0880;
	else if (s1 == "S3" && s2 == "S3") return 0.1750;
	else if (s1 == "S3" && s2 == "S4") return 0.0730;
	else if (s1 == "S3" && s2 == "S5") return 0.0710;
	else if (s1 == "S4" && s2 == "S4") return 0.1280;
	else if (s1 == "S4" && s2 == "S5") return 0.0640;
	else if (s1 == "S5" && s2 == "S5") return 0.1500;
	else assert(false);
}

//===========================================================================
// segmentations
//===========================================================================
void Database::getSegmentations(int id, QList<pair<QString,QString> > &s) const
{
	Q_UNUSED(id);
	s.clear();
	s.append(pair<QString,QString>("portfolio","asset"));
	s.append(pair<QString,QString>("obligors","obligor"));
	s.append(pair<QString,QString>("assets","asset"));
	s.append(pair<QString,QString>("sectors","obligor"));
	s.append(pair<QString,QString>("size","obligor"));
	s.append(pair<QString,QString>("products","asset"));
	s.append(pair<QString,QString>("offices","asset"));
}

//===========================================================================
// segments
//===========================================================================
void Database::getSegments(int id, const QString &segmentation, QList<QString> &values) const
{
	Q_UNUSED(id);
	values.clear();

	if (segmentation == "portfolio") {
		// none
	}
	else if (segmentation == "obligors") {
		values.append("*");
	}
	else if (segmentation == "assets") {
		values.append("*");
	}
	else if (segmentation == "sectors") {
		values.append("S1");
		values.append("S2");
		values.append("S3");
		values.append("S4");
		values.append("S5");
	}
	else if (segmentation == "size") {
		values.append("large");
		values.append("medium");
		values.append("large");
	}
	else if (segmentation == "products") {
		values.append("bond");
		values.append("loan");
		values.append("lease");
		values.append("mortgage");
		values.append("line of credit");
		values.append("bank guarantee");
	}
	else if (segmentation == "offices") {
		values.append("0001");
		values.append("0002");
		values.append("0003");
		values.append("0004");
	}
	else assert(false);
}

//===========================================================================
// num obligors
//===========================================================================
int Database::getNumObligors(int id) const
{
	Q_UNUSED(id);
	return 3;
}

//===========================================================================
// obligors
//===========================================================================
void Database::getObligors(int id, QList<QString> &o, int offset, int limit) const
{
	Q_UNUSED(id);
	Q_UNUSED(offset);
	Q_UNUSED(limit);
	o.append("000001");
	o.append("000002");
	o.append("000003");
}

//===========================================================================
// obligor properties
//===========================================================================
void Database::getObligorProperties(int id, const QString &o, QString &rating, QString &sector, QString &recovery, QList<pair<QString,QString> > &segments) const
{
	Q_UNUSED(id);
	segments.clear();

	if (o == "000001") {
		rating = "A";
		sector = "S1";
		recovery= "";
		segments.append(pair<QString,QString>("sectors","S1"));
		segments.append(pair<QString,QString>("size","medium"));
	}
	else if (o == "000002") {
		rating = "B";
		sector = "S2";
		recovery= "20%";
		segments.append(pair<QString,QString>("sectors","S2"));
		segments.append(pair<QString,QString>("size","large"));
	}
	else if (o == "000003") {
		rating = "CCC";
		sector = "S2";
		recovery= "";
		segments.append(pair<QString,QString>("sectors","S2"));
		segments.append(pair<QString,QString>("size","medium"));
	}
	else assert(false);
}

//===========================================================================
// assets
//===========================================================================
void Database::getAssets(int id, const QString &o, QList<QString> &a) const
{
	Q_UNUSED(id);
	Q_UNUSED(o);
	a.clear();
	a.append("op1");
	a.append("op2");
}

//===========================================================================
// asset properties
//===========================================================================
void Database::getAssetProperties(int id, const QString &a, QDate &d, QString &r, QList<pair<QString,QString> > &segments) const
{
	Q_UNUSED(id);
	Q_UNUSED(a);
	d = QDate(2005,1,1);
	r = "0.8";
	segments.clear();
	segments.append(pair<QString,QString>("products","bond"));
	segments.append(pair<QString,QString>("office","0001"));
}

//===========================================================================
// asset data
//===========================================================================
void Database::getAssetData(int id, const QString &a, QList<QDate> &dates, QList<QString> &exposures, QList<QString> &recoveries) const
{
	Q_UNUSED(id);
	Q_UNUSED(a);
	dates.clear();
	exposures.clear();
	recoveries.clear();

	dates.append(QDate(2009,7,1));
	dates.append(QDate(2011,7,1));
	dates.append(QDate(2013,7,1));
	dates.append(QDate(2015,7,1));
	dates.append(QDate(2017,7,1));

	exposures.append("4400.0");
	exposures.append("3920.0");
	exposures.append("3340.0");
	exposures.append("2960.0");
	exposures.append("2480.0");

	recoveries.append("75%");
	recoveries.append("70%");
	recoveries.append("65%");
	recoveries.append("60%");
	recoveries.append("55%");
}

//===========================================================================
// update name
//===========================================================================
void Database::updateName(int id, const QString &str)
{
	Q_UNUSED(id);
	Q_UNUSED(str);
}

//===========================================================================
// update description
//===========================================================================
void Database::updateDescripition(int id, const QString &str)
{
	Q_UNUSED(id);
	Q_UNUSED(str);
}

//===========================================================================
// update property
//===========================================================================
void Database::updateProperty(int id, Property p, const QVariant &val) throw(Exception)
{
	Q_UNUSED(id);
	Q_UNUSED(p);
	Q_UNUSED(val);
	//throw Exception();
	// Ending date can't be lower than starting date
	// Starting date can't be upper than ending date
}

//===========================================================================
// update interest type
//===========================================================================
void Database::updateInterestType(int id, const QString &type)
{
	Q_UNUSED(id);
	Q_UNUSED(type);
}

//===========================================================================
// insert or update update interest rate
//===========================================================================
void Database::updateInterestRate(int id, int month, double val)
{
	Q_UNUSED(id);
	Q_UNUSED(month);
	Q_UNUSED(val);
}

//===========================================================================
// remove interest rate
//===========================================================================
void Database::removeInterestRate(int id, int month)
{
	Q_UNUSED(id);
	Q_UNUSED(month);
}
