
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#ifndef _Configuration_
#define _Configuration_

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include "utils/Exception.hpp"

namespace ccruncher_inf {

/**************************************************************************//**
 * @brief Configuration file.
 *
 * @details Configuration file allowing comments, key = value parameters,
 *          parameters can be value, vectors or matrices, multi-line, etc.
 */
class Configuration
{

  private:

    // parameters list
    std::vector<std::pair<std::string,std::vector<std::vector<std::string>>>> params;
    // auxiliar variable
    bool openbrace = false;
    // auxiliar variable
    bool mcomment = false;
    // auxiliar variable
    bool haskey = false;

  private:

    // trims a std::string
    static std::string trim(const std::string &s);
    // T as std::string
    template<class T> static std::string T_as_string( const T& t);
    // std::string as T
    template<class T> static T string_as_T( const std::string& s);
    // find an entry by key
    int find(const std::string &key) const;
    // parser procedure
    void parse1(const std::string &str) throw(ccruncher::Exception);
    // parser procedure
    void parse2(const std::string &str) throw(ccruncher::Exception);
    // parser procedure
    void parse3(const std::string &str) throw(ccruncher::Exception);
    // remove comments
    void removeComments(std::string &);

  public:

    // default constructor
    Configuration() {}
    // constructor
    Configuration(const std::string &filename) throw(ccruncher::Exception);

    // read a configuration value (throw exception if key is not found)
    template<class T> T read( const std::string& key ) const throw(ccruncher::Exception);  // call as read<T>
    // read a configuration value (returns default value if key not found)
    template<class T> T read( const std::string& key, const T& value ) const throw(ccruncher::Exception);
    // read a configuration value (throw exception if key is not found)
    template<class T> std::vector<T> readv( const std::string& key ) const throw(ccruncher::Exception);  // call as readv<T>
    // read a configuration value (returns default value if key not found)
    template<class T> std::vector<T> readv( const std::string& key, const std::vector<T> &value ) const throw(ccruncher::Exception);
    // read a configuration value (throw exception if key is not found)
    template<class T> std::vector<std::vector<T> > readm( const std::string& key ) const throw(ccruncher::Exception);  // call as readm<T>
    // read a configuration value (returns default value if key not found)
    template<class T> std::vector<std::vector<T> > readm( const std::string& key, const std::vector<std::vector<T> > &value ) const throw(ccruncher::Exception);

    // load configuration file
    void load(const std::string &filename) throw(ccruncher::Exception);
    // save configuration file
    void save(const std::string &filename);
    // check if a key exist
    bool exists(const std::string &key) const;
    // set content
    template<class T> void write( const std::string& key, const T &value );
    // set content
    template<class T> void write( const std::string& key, const std::vector<T> &value );
    // set content
    template<class T> void write( const std::string& key, const std::vector<std::vector<T> > &value );

    // check if entry is a value (eg. x1, or [x1])
    bool isValue(const std::string &key) const;
    // check if entry is an array (eg. [x1, x2, x3], or [])
    bool isArray(const std::string &key) const;
    // check if entry is a matrix (eg. [x11, x12, x13; x21, x22, x23], or x11, or [x11], or [])
    bool isMatrix(const std::string &key) const;
    // check type
    template<class T> bool istype(const std::string &key);
};

// -------------------------------------------------------

template<class T>
std::string Configuration::T_as_string( const T& t )
{
    // Convert from a T to a std::string
    // Type T must support << operator
    std::ostringstream ost;
    ost << t;
    return ost.str();
}

template<class T>
T Configuration::string_as_T( const std::string& s )
{
    // Convert from a std::string to a T
    // Type T must support >> operator
    T t;
    std::istringstream ist(s);
    ist >> t;
    if (!ist) throw ccruncher::Exception("error parsing value");
    ist.get(); // remaining chars
    if (ist.good()) throw ccruncher::Exception("error parsing value");
    return t;
}

template<>
inline std::string Configuration::string_as_T<std::string>( const std::string& s )
{
    return s;
}

template<>
inline bool Configuration::string_as_T<bool>( const std::string& s )
{
    // Convert from a std::string to a bool
    // Interpret "false", "F", "no", "n", "0" as false
    // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
    bool b = true;
    std::string sup = s;
    for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
            *p = toupper(*p);  // make std::string all caps
    if( sup==std::string("FALSE") || sup==std::string("F") ||
        sup==std::string("NO") || sup==std::string("N") ||
        sup==std::string("0") || sup==std::string("NONE") )
            b = false;
    return b;
}

// -------------------------------------------------------

template<class T>
T Configuration::read( const std::string& key ) const throw(ccruncher::Exception)
{
    int pos = find(key);
    if (pos < 0) throw ccruncher::Exception("key '" + key + "' not found");
    if (params[pos].second.size() != 1 || params[pos].second[0].size() != 1) throw ccruncher::Exception("key '" + key + "' has dim distinct than 1x1");
    return string_as_T<T>(params[pos].second[0][0]);
}

template<class T>
T Configuration::read( const std::string& key, const T& value ) const throw(ccruncher::Exception)
{
    if (!exists(key)) return value;
    else return read<T>(key);
}

// -------------------------------------------------------

template<class T>
std::vector<T> Configuration::readv( const std::string& key ) const throw(ccruncher::Exception)
{
    size_t i, j;
    int pos = find(key);
    if (pos < 0) throw ccruncher::Exception("'" + key + "' key not found");
    if (params[pos].second.size() == 0) return std::vector<T>();
    std::vector<T> ret;
    try {
        for(i=0; i<params[pos].second.size(); i++) {
            for(j=0; j<params[pos].second[i].size(); j++) {
                T val = string_as_T<T>(params[pos].second[i][j]);
                ret.push_back(val);
            }
        }
    }
    catch(ccruncher::Exception &e) {
        throw ccruncher::Exception("error parsing value " + key + "[" + T_as_string(i+1) + "," + T_as_string(j+1) + "]");
    }
    return ret;
}

template<class T>
std::vector<T> Configuration::readv( const std::string& key, const std::vector<T>& value ) const throw(ccruncher::Exception)
{
    if (!exists(key)) return value;
    else return readv<T>(key);
}

// -------------------------------------------------------

template<class T>
std::vector<std::vector<T> > Configuration::readm( const std::string& key ) const throw(ccruncher::Exception)
{
    size_t i, j=0;
    int pos = find(key);
    if (pos < 0) throw ccruncher::Exception("'" + key + "' key not found");
    std::vector<std::vector<T> > ret;
    try {
        for(i=0; i<params[pos].second.size(); i++) {
            if (params[pos].second[i].size() > 0) {
                ret.push_back(std::vector<T>(params[pos].second[i].size()));
            }
            for(j=0; j<params[pos].second[i].size(); j++) {
                T val = string_as_T<T>(params[pos].second[i][j]);
                ret.back()[j] = val;
            }
        }
    }
    catch(ccruncher::Exception &e) {
        throw ccruncher::Exception("error parsing value " + key + "[" + T_as_string(i+1) + "," + T_as_string(j+1) + "]");
    }
    return ret;
}

template<class T>
std::vector<std::vector<T> > Configuration::readm( const std::string& key, const std::vector<std::vector<T> >& value ) const throw(ccruncher::Exception)
{
    if (!exists(key)) return value;
    else return readm<T>(key);
}

// -------------------------------------------------------

template<class T>
void Configuration::write( const std::string& key, const T &value )
{
    int pos = find(key);
    if (pos < 0) {
        params.push_back(std::pair<std::string,std::vector<std::vector<std::string> > >(key, std::vector<std::vector<std::string> >()));
        pos = params.size()-1;
    }
    params[pos].second.clear();
    params[pos].second.push_back(std::vector<std::string>(1));
    params[pos].second[0][0] = T_as_string(value);
}

template<class T>
void Configuration::write( const std::string& key, const std::vector<T> &value )
{
    int pos = find(key);
    if (pos < 0) {
        params.push_back(std::pair<std::string,std::vector<std::vector<std::string> > >(key, std::vector<std::vector<std::string> >()));
        pos = params.size()-1;
    }
    params[pos].second.clear();
    params[pos].second.push_back(std::vector<std::string>(value.size()));
    for(size_t i=0; i<value.size(); i++) {
      params[pos].second[0][i] = T_as_string(value[i]);
    }
}

template<class T>
void Configuration::write( const std::string& key, const std::vector<std::vector<T> > &value )
{
    int pos = find(key);
    if (pos < 0) {
        params.push_back(std::pair<std::string,std::vector<std::vector<std::string> > >(key, std::vector<std::vector<std::string> >()));
        pos = params.size()-1;
    }
    params[pos].second.clear();
    for(size_t i=0; i<value.size(); i++) {
        params[pos].second.push_back(std::vector<std::string>(value[i].size()));
        for(size_t j=0; j<value[i].size(); j++) {
            params[pos].second[i][j] = T_as_string(value[i][j]);
        }
    }
}

// -------------------------------------------------------

template<class T> bool Configuration::istype(const std::string &key)
{
    int pos = find(key);
    if (pos < 0) return false;

    try
    {
        for(size_t i=0; i<params[pos].second.size(); i++) {
            for(size_t j=0; j<params[pos].second[i].size(); j++) {
                string_as_T<T>(params[pos].second[i][j]);
            }
        }
        return true;
    }
    catch(...)
    {
        return false;
    }
}

} // namespace

#endif
