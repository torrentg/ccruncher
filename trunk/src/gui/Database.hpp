#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QList>
#include <QVariant>
#include <QString>
#include <QDate>
#include <QSqlDatabase>
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;
namespace ccruncher_gui {

class Database
{

	private:

		// database driver
		QString dbdriver;
		// database host
		QString dbhost;
		// database port
		int dbport;
		// database name
		QString dbname;
		// database user
		QString dbuser;
		// database password
		QString dbpassword;
		// database connection
		QSqlDatabase db;

	public:

		enum Property
		{
			Time0,
			TimeT,
			MaxIterations,
			MaxSeconds,
			CopulaType,
			Seed,
			Antithetic,
			OnlyActive
		};

	public:

		// initialize database connection
		void init(const QString &driver, const QString &host, const int port, const QString &dbname, const QString &dbuser, const QString &dbpasswd) throw(Exception);

		// return simulation identifiers
		void getIds(QList<int> &l) const;
		// get name
		QString getName(int id) const;
		// get description
		QString getDescription(int id) const;
		// get simulation date
		QVariant getProperty(int id, Property p) const;
		// interest curve
		void getInterest(int id, QList<pair<int,double> > &values, QString &type) const;
		// ratings
		void getRatings(int id, QList<pair<QString,QString> > &values, QString &type) const;
		// survival curve
		void getSurvivalCurve(int id, const QString &rating, QList<pair<int,double> > &values) const;
		// transition period
		int getTransitionPeriod(int id);
		// transition matrix
		double getTransition(int id, const QString &r1, const QString &r2) const;
		// sectors
		void getSectors(int id, QList<pair<QString,QString> > &values) const;
		// correlation
		double getCorrelation(int id, const QString &s1, const QString &s2) const;
		// segmentations
		void getSegmentations(int id, QList<pair<QString,QString> > &s) const;
		// segments
		void getSegments(int id, const QString &segmentation, QList<QString> &values) const;
		// num obligors
		int getNumObligors(int id) const;
		// obligors
		void getObligors(int id, QList<QString> &o, int offset=0, int limit=100) const;
		// obligor properties
		void getObligorProperties(int id, const QString &o, QString &rating, QString &sector, QString &recovery, QList<pair<QString,QString> > &segments) const;
		// assets
		void getAssets(int id, const QString &o, QList<QString> &a) const;
		// asset properties
		void getAssetProperties(int id, const QString &a, QDate &d, QString &r, QList<pair<QString,QString> > &segments) const;
		// asset data
		void getAssetData(int id, const QString &a, QList<QDate> &dates, QList<QString> &exposures, QList<QString> &recoveries) const;

		// update name
		void updateName(int id, const QString &str);
		// update description
		void updateDescripition(int id, const QString &str);
		// update property
		void updateProperty(int id, Property p, const QVariant &val) throw(Exception);
		// update interest type
		void updateInterestType(int id, const QString &typer);
		// insert or update update interest rate
		void updateInterestRate(int id, int month, double val);
		// remove interest rate
		void removeInterestRate(int id, int month);

};

}

#endif
