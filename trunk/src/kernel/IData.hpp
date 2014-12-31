
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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
#include <mutex>
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
    Logger logger;
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
    mutable std::mutex mMutex;
    //! Input file
    gzFile curfile;
    //! Input file size
    size_t cursize;
    //! Parse portfolio flag
    bool parse_portfolio;

  private:
  
    //! Validate simulation data
    void validate();
    //! Parse main input file
    void parse(gzFile file, const std::map<std::string,std::string> &m);
    //! Parse portfolio
    void parsePortfolio(ExpatUserData &, const char *, const char **);
    //! Check define
    void checkDefine(const std::string &key, const std::string &value) const;

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
    //! Directives to process xml data element
    virtual void epdata(ExpatUserData &, const char *, const char *, int) override;

  public:

    //! Default constructor
    IData(std::streambuf *s=nullptr);
    //! Destructor
    virtual ~IData() override;
    //! Initialize content
    void init(const std::string &f, const std::map<std::string,std::string> &m=(std::map<std::string,std::string>()), bool *stop_=nullptr, bool parse_portfolio_=true);

    //! Returns simulation title
    const std::string & getTitle() const { return title; }
    //! Returns simulation description
    const std::string & getDescription() const { return description; }
    //! Returns simulation params
    const Params & getParams() const { return params; }
    //! Returns simulation yield curve
    const Interest & getInterest() const { return interest; }
    //! Returns simulation ratings
    const Ratings & getRatings() const { return ratings; }
    //! Returns simulation transition matrix
    const Transitions & getTransitions() const { return transitions; }
    //! Returns default probabilities functions
    const DefaultProbabilities & getDefaultProbabilities() const { return dprobs; }
    //! Returns simulation factors
    const Factors & getFactors() const { return factors; }
    //! Returns simulation correlation matrix
    const Correlations & getCorrelations() const { return correlations; }
    //! Returns simulation correlations
    const Segmentations & getSegmentations() const { return segmentations; }
    //! Returns simulation portfolio
    const Portfolio & getPortfolio() const { return portfolio; }
    Portfolio & getPortfolio() { return portfolio; }
    
    //! Indicates if dprobs tag is defined
    bool hasDefaultProbabilities() const;
    //! Return input file name
    const std::string & getFilename() const { return filename; }
    //! Returns input file size (in bytes)
    size_t getFileSize() const;
    //! Returns readed bytes
    size_t getReadedSize() const;

};

} // namespace

#endif

