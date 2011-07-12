#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <QString>
#include "ConfigFile.hpp"
#include "utils/Exception.hpp"
#include "Database.hpp"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;
namespace ccruncher_gui {

class Configuration
{

private:

		// configuration manager
		ConfigFile config;

private:

		// parse a string value
		QString getString(const string &key) throw(Exception);
		// parse a int value
		int getInt(const string &key) throw(Exception);

public:

		// configuration filename
		QString config_filename;
		// contents saved to disk
		bool config_saved;

		// database driver
		QString db_driver;
		// database host
		QString db_host;
		// database port
		int db_port;
		// daabase name
		QString db_name;
		// database user
		QString db_user;
		// database password
		QString db_password;

public:

		// default constructor
		Configuration();
		// constructor
		Configuration(const QString &filename) throw(Exception);
		// serialize configuration
		string serialize() const;

};

}

#endif
