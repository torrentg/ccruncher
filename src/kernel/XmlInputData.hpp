
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include <cstdio>
#include <map>
#include <vector>
#include <string>
#include <streambuf>
#include <zlib.h>
#include <mutex>
#include "kernel/InputData.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Exception.hpp"
#include "utils/Logger.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief CCruncher xml input file.
 *
 * @details Does the following things:
 *          - manages input file (open/close/stdin/gzip)
 *          - manages log trace
 *          - manages defines
 *          - parses xml content
 *          - manages sub-files (see attribute 'include' in portfolio)
 *          - provides mechanism to stop parsing
 *
 * @see http://ccruncher.net/ifileref.html
 */
class XmlInputData : public InputData, public ExpatHandlers
{

  private:

    //! Supported xml tags
    enum class XmlTag : unsigned char
    {
      CCRUNCHER, TITLE, DESCRIPTION, DEFINES, DEFINE, PARAMETERS,
      PARAMETER, INTEREST, RATE, RATINGS, RATING, TRANSITIONS,
      TRANSITION,FACTORS, FACTOR, CORRELATIONS, CORRELATION,
      SEGMENTATIONS, SEGMENTATION, SEGMENT, PORTFOLIO, OBLIGOR,
      BELONGSTO, ASSET, DATA, VALUES, DPROBS, DPROB
    };

  private:

    //! Logger
    Logger logger;
    //! Input filename
    std::string filename;
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

    //! Map used to check id obligor oneness
    std::map<std::string,size_t> mIdObligors;
    //! Map used to check id asset oneness
    std::map<std::string,size_t> mIdAssets;
    //! number of enabled segmentations
    unsigned short mNumEnabledSegmentations = 0;
    //! Current obligor segments
    std::vector<unsigned short> mObligorSegments;
    //! current asset lgd
    LGD mAssetLGD;

    //! Current tags stack
    std::vector<XmlTag> currentTags;
    //! Tags used by user
    bool mHasTag[32] = {false};

  private:

    //! Read content from a file
    void read(FILE *f);
    //! Push a tag to stack
    void pushTag(XmlTag tag);
    //! Pop a tag from stack
    void popTag();
    //! Check if tag was previously defined
    bool hasTag(XmlTag tag);
    //! Returns the included file name
    std::string getIncludedFileName(const std::string &name);
    //! Check macro
    void checkMacro(const std::string &key, const std::string &value) const;
    //! Parse portfolio
    void parseIncludedPortfolio(const std::string &include);
    //! Parse main input file
    void parse(gzFile file);
    //! Validate simulation data
    void validate();
    //! Prepare data to be used
    void prepare();
    //! Print a summary
    void summary();
    //! Returns the number of enabled segmentations
    unsigned short getNumEnabledSegmentations() const;

    //! process tags of level 0
    void epstart0(const char *tag, const char **attributes);
    //! process tags of level 1
    void epstart1(const char *tag, const char **attributes);
    //! process tags of level 2
    void epstart2(const char *tag, const char **attributes);
    //! process tags of level 3
    void epstart3(const char *tag, const char **attributes);
    //! process tags of level 4 or bigger
    void epstart4(const char *tag, const char **attributes);

    //! Process tag define
    void processTagDefine(const std::string &key, const std::string &value);
    //! Process tag interest
    void processTagInterest(const std::string &type, const std::string &spline);
    //! Process tag interest
    void processTagRate(const char *str, double r);
    //! Process tag transition
    void processTagTransition(const std::string &from, const std::string &to, double value);
    //! Process tag dprob
    void processTagDprob(const std::string &srating, const char *str, double value);
    //! Process tag correlation
    void processTagCorrelation(const std::string &factor1, const std::string &factor2, double value);
    //! Process tag segmentation
    void processTagSegmentation(const std::string &name, bool enabled);
    //! Process tag obligor
    void processTagObligor(const std::string &id, const char *sfactor, const char *srating, const char *slgd);
    //! Process tag belongs-to
    void processTagBelongsTo(XmlTag parent, const char *ssegmentation, const char *ssegment);
    //! Process tag asset
    void processTagAsset(const std::string &id, const Date &date,  const char *lgd);
    //! Process tag values
    void processTagValues(const char *str, const char *ead, const char *lgd);
    //! Process last obligor
    void processLastObligor();
    //! Process last asset
    void processLastAsset();

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(const char *) override;
    //! Directives to process xml data element
    virtual void epdata(const char *, int) override;

  public:

    //! Default constructor
    XmlInputData(std::streambuf *s=nullptr);
    //! Destructor
    virtual ~XmlInputData() override;
    //! Read content from file
    void readFile(const std::string &f, const std::map<std::string,std::string> &m=(std::map<std::string,std::string>()), bool *s=nullptr, bool p=true);
    //! Read content from stdin
    void readStdin(const std::map<std::string,std::string> &m=(std::map<std::string,std::string>()), bool *s=nullptr, bool p=true);
    //! Read content from string
    void readString(const std::string &str, const std::map<std::string,std::string> &m=(std::map<std::string,std::string>()), bool *s=nullptr, bool p=true);

    //! Return input file name
    const std::string & getFilename() const { return filename; }
    //! Returns input file size (in bytes)
    size_t getFileSize() const;
    //! Returns readed bytes
    size_t getReadedSize() const;

};

} // namespace
