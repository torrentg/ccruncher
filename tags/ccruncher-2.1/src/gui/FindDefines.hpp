#ifndef _FindDefines_
#define _FindDefines_

#include <map>
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

class FindDefines : public ExpatHandlers
{

  private:

    // defines found
    map<string,string> defines;

  protected:

    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
    // ExpatHandlers method
    void epdata(ExpatUserData &, const char *, const char *, int);

  public:

    // constructor
    FindDefines(const string &);
    // return defines
    const map<string,string> &getDefines() const;

};

#endif

