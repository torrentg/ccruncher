#include "Configuration.hpp"
#include <cassert>

// configuration keys
#define CFGKEY_DB_DRIVER "db.driver"
#define CFGKEY_DB_HOST "db.host"
#define CFGKEY_DB_PORT "db.port"
#define CFGKEY_DB_NAME "db.name"
#define CFGKEY_DB_USER "db.user"
#define CFGKEY_DB_PASSWORD "db.password"
// other stuff
#define UNASSIGNED_STRING "UNASSIGNED"
#define UNASSIGNED_INT -1

//=============================================================
// constructor
//=============================================================
Configuration::Configuration(const QString &filename) throw(Exception)
{
	// reading configuration file
	try {
		config = ConfigFile(filename.toStdString().c_str());
		config_filename = filename;
		config_saved = true;
		// parse parameters
		db_driver = getString(CFGKEY_DB_DRIVER);
		db_host = getString(CFGKEY_DB_HOST);
		db_port = getInt(CFGKEY_DB_PORT);
		db_name = getString(CFGKEY_DB_NAME);
		db_user = getString(CFGKEY_DB_USER);
		db_password = getString(CFGKEY_DB_PASSWORD);
	}
	catch(ConfigFile::file_not_found &e) {
			throw Exception("error: configuration file '" + filename.toStdString() + "' not found");
	}
	catch(Exception &e) {
			throw Exception(e, "error: parsing configuration file '" + filename.toStdString() + "'");
	}
	catch(...) {
			throw Exception("error: parsing configuration file '" + filename.toStdString() + "'");
	}
}

//=============================================================
// getString
//=============================================================
QString Configuration::getString(const string &key) throw(Exception)
{
	string val = config.read<string>(key, UNASSIGNED_STRING);
	if (val == UNASSIGNED_STRING)
	{
		throw Exception("error: entry '" + key + "' has an invalid value");
	}
	return QString(val.c_str());
}

//=============================================================
// getInt
//=============================================================
int Configuration::getInt(const string &key) throw(Exception)
{
	int val = config.read<int>(key, UNASSIGNED_INT);
	if (val == UNASSIGNED_INT)
	{
		throw Exception("error: entry '" + key + "' has an invalid value");
	}
	return val;
}
