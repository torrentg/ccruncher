
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _IData_
#define _IData_

#include <map>
#include <string>
#include <streambuf>
#include <zlib.h>
#include <pthread.h>
#include "portfolio/Portfolio.hpp"
#include "params/Params.hpp"
#include "params/Interest.hpp"
#include "params/Ratings.hpp"
#include "params/Factors.hpp"
#include "params/Transitions.hpp"
#include "params/DefaultProbabilities.hpp"
#include "params/Correlations.hpp"
#include "params/Segmentations.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "utils/Logger.hpp"

#define STDIN_FILENAME "<stdin>"

namespace ccruncher {

/**************************************************************************//**
 * @brief CCruncher input file.
 *
 * @details Does the following things:
 *          - input file open/close/stdin/gzip
 *          - manages log trace
 *          - defines management
 *          - parse xml using an ExpatParser
 *          - manages sub-files (see attribute include in portfolio)
 *          - provides mechanism to stop parsing
 *
 * @see http://ccruncher.net/ifileref.html
 */
class IData : public ExpatHandlers
{

  private:

    //! Logger
    Logger log;
    //! Input filename
    std::string filename;
    //! Simulation title
    std::string title;
    //! Simulation description
    std::string description;
    //! Simulation parameters
    Params params;
    //! Simulation yield curve
    Interest interest;
    //! Simulation ratings
    Ratings ratings;
    //! Simulation transition matrix
    Transitions transitions;
    //! Default probabilities functions
    DefaultProbabilities dprobs;
    //! Simulation factors
    Factors factors;
    //! Simulation correlations
    Correlations correlations;
    //! Simulation segmentations
    Segmentations segmentations;
    //! Simulation portfolio
    Portfolio portfolio;
    //! Root tag existence (parsing aux)
    bool hasmaintag;
    //! Defines status (0=none, 1=current, 2=done) [parsing aux]
    int hasdefinestag;
    //! Variable to stop parser
    bool *stop;
    //! Ensures data consistence
    mutable pthread_mutex_t mutex;
    //! Input file
    gzFile curfile;
    //! Input file size
    size_t cursize;
    //! Parse portfolio flag
    bool parse_portfolio;

  private:
  
    //! Validate simulation data
    void validate() throw(Exception);
    //! Parse main input file
    void parse(gzFile file, const std::map<std::string,std::string> &m) throw(Exception);
    //! Parse portfolio
    void parsePortfolio(ExpatUserData &, const char *, const char **) throw(Exception);
    //! Check define
    void checkDefine(const std::string &key, const std::string &value) const throw(Exception);

  protected:

    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);
    //! Directives to process xml data element
    void epdata(ExpatUserData &, const char *, const char *, int);

  public:

    //! Default constructor
    IData(std::streambuf *s=NULL);
    //! Destructor
    ~IData();
    //! Initialize content
    void init(const std::string &f, const std::map<std::string,std::string> &m=(std::map<std::string,std::string>()), bool *stop_=NULL, bool parse_portfolio_=true) throw(Exception);
    //! Returns simulation title
    const std::string &getTitle() const;
    //! Returns simulation description
    const std::string & getDescription() const;
    //! Returns simulation params
    Params & getParams();
    //! Returns simulation yield curve
    Interest & getInterest();
    //! Returns simulation ratings
    Ratings & getRatings();
    //! Returns simulation transition matrix
    Transitions & getTransitions();
    //! Returns default probabilities functions
    DefaultProbabilities & getDefaultProbabilities();
    //! Returns simulation factors
    Factors & getFactors();
    //! Returns simulation correlation matrix
    Correlations & getCorrelations();
    //! Returns simulation correlations
    Segmentations & getSegmentations();
    //! Returns simulation portfolio
    Portfolio & getPortfolio();
    //! Indicates if dprobs tag is defined
    bool hasDefaultProbabilities() const;
    //! Returns input file size (in bytes)
    size_t getFileSize() const;
    //! Returns readed bytes
    size_t getReadedSize() const;

};

} // namespace

#endif

