#ifndef _FindDefines_
#define _FindDefines_

#include <map>
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

class FindDefines : public ccruncher::ExpatHandlers
{

  private:

    // defines found
    std::map<std::string,std::string> defines;

  protected:

    // ExpatHandlers method
    void epstart(ccruncher::ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ccruncher::ExpatUserData &, const char *);
    // ExpatHandlers method
    void epdata(ccruncher::ExpatUserData &, const char *, const char *, int);

  public:

    // constructor
    FindDefines(const std::string &);
    // return defines
    const std::map<std::string,std::string> &getDefines() const;

};

#endif

