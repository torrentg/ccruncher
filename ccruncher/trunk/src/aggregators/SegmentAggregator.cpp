
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// SegmentAggregator.cpp - SegmentAggregator code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <unistd.h>
#include <algorithm>
#include "SegmentAggregator.hpp"

//===========================================================================
// void constructor
//===========================================================================
ccruncher::SegmentAggregator::SegmentAggregator()
{
  reset();
}

//===========================================================================
// define
//===========================================================================
void ccruncher::SegmentAggregator::define(int isegs, int iseg, char comps, bool bv, bool bf)
{
  isegmentation = isegs;
  isegment = iseg;
  components = comps;
  bvalues = bv;
  bfull = bf;
}  

//===========================================================================
// reset
//===========================================================================
void ccruncher::SegmentAggregator::reset()
{
  iclients = NULL;
  cvalues = NULL;
  cratings = NULL;
  vertexes = NULL;

  isegmentation = -1;
  isegment = -1;
  components = COMPONENTS_ASSET;
  bvalues = true;
  bfull = false;

  path = "UNASSIGNED";
  bforce = false;
  buffersize = MAXSIZE;
  existheader = false;

  N = 0L;
  M = 0;
  K = 0L;
  nclients = 0L;
  nassets = 0L;
  cont = 0L;
  icont = 0L;
  idefault = -1;

  name0 = "";
  name1 = "";
  name2 = "";
  ratings = NULL;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::SegmentAggregator::~SegmentAggregator()
{
  release();
}

//===========================================================================
// release
//===========================================================================
void ccruncher::SegmentAggregator::release()
{
  // deleting clients array
  if (iclients != NULL) 
  {
    delete [] iclients;
    iclients = NULL;
  }

  // deleting vertexes
  if (vertexes != NULL)
  {
    for(int i=0;i<nclients;i++)
    {
      if (vertexes[i] != NULL)
      {
        delete [] vertexes[i];
        vertexes[i] = NULL;
      }
    }
    delete [] vertexes;
    vertexes = NULL;
  }
  
  // deleting cvalues
  if (cvalues != NULL)
  {
    for(int i=0;i<K;i++)
    {
      if (cvalues[i] != NULL)
      {
        delete [] cvalues[i];
        cvalues[i] = NULL;
      }
    }
    delete [] cvalues;
    cvalues = NULL;
  }

  // deleting cratings
  if (cratings != NULL)
  {
    for(int i=0;i<K;i++)
    {
      if (cratings[i] != NULL)
      {
        delete [] cratings[i];
        cratings[i] = NULL;
      }
    }
    delete [] cratings;
    cratings = NULL;
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::SegmentAggregator::initialize(Date *dates, int m, vector<Client *> *clients, 
  long n, int indexdefault, Ratings *ratings_) throw(Exception)
{
  bool *clientflag = NULL;

  // initial validations
  if (isegmentation == -1)
  {
    throw Exception("SegmentAggregator::initialize(): main variables not fixed");
  }

  // fixing ratings
  ratings = ratings_;

  // setting vars values
  N = n;
  M = m;
  K = bfull?buffersize:1L;
  idefault = indexdefault;

  // allocating clientflag
  clientflag = allocClientFlag(N);

  try
  {
    // counting clients + filling clientflag
    if (components == COMPONENTS_ASSET)
    {
      nclients = getANumClients(clients, N, clientflag);
    }
    else
    {
      nclients = getCNumClients(clients, N, clientflag);
    }

    // counting assets
    if (components == COMPONENTS_ASSET)
    {
      nassets = getANumAssets(clients, N, clientflag);
    }
    else
    {
      nassets = getCNumAssets(clients, N, clientflag);
    }

    // allocating & fixing clients
    iclients = allocIClients(clientflag, N);

    if (bvalues == true)
    {
      // allocating & filling vertexes
      vertexes = allocVertexes(dates, M, clients, N);

      // allocating cvalues
      cvalues = allocCValues(dates, M, K);
    }
    else 
    {
      // allocating cratings
      cratings = allocCRatings(dates, M, K, ratings->getRatings()->size());
    }

    // see MonteCarlo::init() for vertex computation
  }
  catch(std::exception &e)
  {
    if (clientflag != NULL) { delete [] clientflag; clientflag = NULL;}
    throw Exception(e, "SegmentAggregator::initialize()");
  }
  
  delete [] clientflag;
}

//===========================================================================
// getCNumClients
//===========================================================================
long ccruncher::SegmentAggregator::getCNumClients(vector<Client *> *clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    if ((*clients)[i]->belongsTo(isegmentation, isegment))
    {
      flags[i] = true;
      ret++;
    }
  }

  return ret;
}

//===========================================================================
// getANumClients
//===========================================================================
long ccruncher::SegmentAggregator::getANumClients(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset *> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;
    
    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j]->belongsTo(isegmentation, isegment))
      {
        flags[i] = true;
        ret++;
        break;
      }
    }
  }

  return ret;
}

//===========================================================================
// getANumAssets
//===========================================================================
long ccruncher::SegmentAggregator::getANumAssets(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset*> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j]->belongsTo(isegmentation, isegment))
      {
        ret++;
      }
    }
  }

  return ret;
}

//===========================================================================
// getCNumAssets
//===========================================================================
long ccruncher::SegmentAggregator::getCNumAssets(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset*> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    if (flags[i] == true)
    {
      assets = (*clients)[i]->getAssets();
      ret += assets->size();
    }
  }

  return ret;
}

//===========================================================================
// allocClientFlag
//===========================================================================
bool* ccruncher::SegmentAggregator::allocClientFlag(long n) throw(Exception)
{
  bool *ret = NULL;
  
  try
  {
    ret = new bool[n];
    for(long i=0;i<n;i++)
    {
      ret[i] = false;
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "SegmentAggregator::allocClientFlag(): not enougth space");
  }
  
  return ret;
}

//===========================================================================
// allocIClients
//===========================================================================
long* ccruncher::SegmentAggregator::allocIClients(bool *flags, long n) throw(Exception)
{
  long *ret = NULL;
  long num = 0L;
  long aux = 0L;

  for(long i=0;i<n;i++) if(flags[i]) num++;

  try
  {
    ret = new long[num];

    for(long i=0;i<n;i++)
    {
      if(flags[i])
      {
        ret[aux] = i;
        aux++;
      }
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "SegmentAggregator::allocIClients(): not enougth space");
  }

  // assertion
  if (aux != num)
  {
    throw Exception("SegmentAggregator::allocIClients(): assertion failed");
  }
  
  return ret;
}
    
//===========================================================================
// allocCValues
//===========================================================================
DateValues** ccruncher::SegmentAggregator::allocCValues(Date *dates, int m, long n) throw(Exception)
{
  DateValues **ret = NULL;

  try
  {
    ret = new DateValues*[n];
    for(long i=0;i<n;i++)
    {
      ret[i] = NULL;
    }    
  }
  catch(std::exception &e)
  {
    throw Exception(e, "SegmentAggregator::allocCValues(): not enougth space");
  }
  
  try
  {
    for(long i=0;i<n;i++)
    {
      ret[i] = new DateValues[m];

      for(int j=0;j<m;j++)
      {
        ret[i][j].date = dates[j];
        ret[i][j].cashflow = 0.0;
        ret[i][j].exposure = 0.0;
      }
    }
  }
  catch(std::exception &e)
  {
    for(int i=0;i<n;i++) if(ret[i] != NULL) {delete [] ret[i]; ret[i] = NULL; }
    delete [] ret;
    throw Exception(e, "SegmentAggregator::allocCValues(): not enougth space");
  }
  
  return ret;
}

//===========================================================================
// allocCRatings
//===========================================================================
DateRatings** ccruncher::SegmentAggregator::allocCRatings(Date *dates, int m, long n, int numratings) throw(Exception)
{
  DateRatings **ret = NULL;

  try
  {
    ret = new DateRatings*[n];
    for(long i=0;i<n;i++)
    {
      ret[i] = NULL;
    }    
  }
  catch(std::exception &e)
  {
    throw Exception(e, "SegmentAggregator::allocCRatings(): not enougth space");
  }

  try
  {
    for(long i=0;i<n;i++)
    {
      ret[i] = new DateRatings[m];

      for(int j=0;j<m;j++)
      {
        ret[i][j].initialize(dates[j], numratings);
      }
    }
  }
  catch(std::exception &e)
  {
    for(long i=0;i<n;i++) if(ret[i] != NULL) {delete [] ret[i]; ret[i] = NULL; }
    delete [] ret;
    throw Exception(e, "SegmentAggregator::allocCRatings(): not enougth space");
  }

  return ret;
}

//===========================================================================
// resetCValue
//===========================================================================
void ccruncher::SegmentAggregator::resetCValue(int pos)
{
  for(int i=0;i<M;i++)
  {
    cvalues[pos][i].cashflow = 0.0;
    cvalues[pos][i].exposure = 0.0;
  }
}

//===========================================================================
// resetCRating
//===========================================================================
void ccruncher::SegmentAggregator::resetCRating(int pos)
{
  for(int i=0;i<M;i++)
  {
    cratings[pos][i].reset();
  }
}

//===========================================================================
// append
//===========================================================================
void ccruncher::SegmentAggregator::append(int **rpaths, int m, long n, vector<Client *> *clients) throw(Exception)
{
  if (m != M || n != N)
  {
    throw Exception("SegmentAggregator::append(): diferents dimensions");
  }
  if (components == COMPONENTS_ASSET && bvalues == true)
  {
    appendA(rpaths);
  }
  else if (components == COMPONENTS_ASSET && bvalues == false)
  {
    throw Exception("SegmentAggregator::append(): panic1");
  }
  else if (components == COMPONENTS_CLIENT && bvalues == true)
  {
    appendA(rpaths);
  }
  else if (components == COMPONENTS_CLIENT && bvalues == false)
  {
    appendR(rpaths);
  }
  else
  {
    throw Exception("SegmentAggregator::append(): panic2");
  }
}

//===========================================================================
// appendA
//===========================================================================
void ccruncher::SegmentAggregator::appendA(int **rpaths)
{
  int cpos;

  // setting values to 0
  if (bfull == true)
  {
    resetCValue(icont);
  }

  // filling values
  for(long i=0;i<nclients;i++)
  {
    cpos = iclients[i];

    for(int k=0;k<M;k++)
    {
      if (rpaths[cpos][k] != idefault)
      {
        cvalues[icont][k].cashflow += vertexes[i][k].cashflow;
        cvalues[icont][k].exposure += 0.0;
      }
      else
      {
        cvalues[icont][k].cashflow += 0.0;
        cvalues[icont][k].exposure += vertexes[i][k].exposure;
      }
    }
  }
}

//===========================================================================
// appendR
//===========================================================================
void ccruncher::SegmentAggregator::appendR(int **rpaths)
{
  long cpos;
  
  // setting values to 0
  if (bfull == true)
  {
    resetCRating(icont);
  }

  // filling values
  for(long i=0;i<nclients;i++)
  {
    cpos = iclients[i];
    
    for(int j=0;j<M;j++)
    {
      cratings[icont][j].append(rpaths[cpos][j]);
    }
  }
}

//===========================================================================
// next
//===========================================================================
void ccruncher::SegmentAggregator::next()
{
  // incrementing counters
  icont = (bfull?icont+1:0);
  cont++;

  // flushing if buffer is full
  if (icont >= buffersize-1)
  {
    flush(false);
    icont = 0;
  }

  //TODO: flush if more than x sec. without make it (and full=true)
}

//===========================================================================
// allocVertexes
//===========================================================================
DateValues** ccruncher::SegmentAggregator::allocVertexes(Date *dates, int m, vector<Client *> *clients, long n) throw(Exception)
{
  DateValues **ret = NULL;
  DateValues *aux = NULL;
  vector<Asset*> *assets;

  try
  {
    aux = new DateValues[m];
    ret = new DateValues*[nclients];
    for(long i=0;i<nclients;i++)
    {
      ret[i] = NULL;
    }    
  }
  catch(std::exception &e)
  {
    if (aux != NULL) { delete [] aux; aux = NULL; }
    throw Exception(e, "SegmentAggregator::allocVertexes(): not enougth space");
  }
  
  try
  {
    for(long i=0;i<nclients;i++)
    {
      // allocating row
      ret[i] = new DateValues[m];

      // initializing row
      for(int k=0;k<m;k++)
      {
        ret[i][k].date = dates[k];
        ret[i][k].cashflow = 0.0;
        ret[i][k].exposure = 0.0;
      }

      // finding client info
      long cpos = iclients[i];
      assets = (*clients)[cpos]->getAssets();

      // filling row
      for(unsigned int j=0;j<assets->size();j++)
      {
        if (components==COMPONENTS_CLIENT || (components==COMPONENTS_ASSET && (*assets)[j]->belongsTo(isegmentation, isegment)))
        {
          (*assets)[j]->getVertexes(dates, m, aux);
          
          for(int k=0;k<m;k++)
          {
            ret[i][k].cashflow += aux[k].cashflow;
            ret[i][k].exposure += aux[k].exposure;
          }
        }
      }
    }
  }
  catch(std::exception &e)
  {
    if (aux != NULL) { delete [] aux; aux = NULL; }
    for(int i=0;i<n;i++) if(ret[i] != NULL) { delete [] ret[i]; ret[i] = NULL; }
    delete [] ret;
    throw Exception(e, "SegmentAggregator::allocVertexes(): not enougth space");
  }

  delete [] aux;
  return ret;
}

//===========================================================================
// print
//===========================================================================
void ccruncher::SegmentAggregator::flush(bool bfinalize) throw(Exception)
{
  // opening output stream
  ofsopen();

  try
  {
    // printing xml header
    if (!existheader)
    {
      printXMLHeader();
      existheader = true;
    }

    // printing XML content
    printXMLItems(bfinalize);
//TODO: pendent actualitzar info si bfull=false (fer trunc al obrir file)    
    // printing XML footer
    if (bfinalize == true)
    {
      printXMLFooter();
    }
  }
  catch(Exception e)
  {
    ofsclose();
    throw Exception(e, "SegmentAggregator::flush()");
  }

  // closing output stream
  ofsclose();
}

//===========================================================================
// printXMLDoc
//===========================================================================
void ccruncher::SegmentAggregator::printXMLItems(bool bfinalize) throw(Exception)
{
  // setting items tags
  if (nclients != 0 || nassets != 0)
  {
    if (bfull == true)
    {
      for(long i=0;i<icont;i++)
      {
        printXMLItem(i);
      }

      // reseting icont
      icont = 0;
    }
    else
    {
      if (bfinalize == true)
      {
        printXMLItem(0);
      }
    }
  }
}

//===========================================================================
// printXMLHeader
//===========================================================================
void ccruncher::SegmentAggregator::printXMLHeader() throw(Exception)
{
  // setting xml header
  fout << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";

  // oppening segmentaggregator tag
  fout << "<segmentaggregator ";
  fout << "name='" << name0 << "' ";
  fout << "segmentation='" << name1 << "' ";
  fout << "segment='" << name2 << "' ";
  fout << "components='" << (components==COMPONENTS_ASSET?"asset":"client") << "' ";
  fout << "type='" << (bvalues?"values":"ratings") << "' ";
  fout << "full='" << (bfull?"true":"false") << "' ";
  fout << "nclients='" << nclients << "' ";
  fout << "nassets='" << nassets << "' ";
  fout << "numratings='" << ratings->getRatings()->size() << "' ";
  fout << "ntranches='" << M << "' >\n";
}

//===========================================================================
// printXMLFooter
//===========================================================================
void ccruncher::SegmentAggregator::printXMLFooter() throw(Exception)
{
  // closing segmentaggregator tag
  fout << "</segmentaggregator>\n";
}

//===========================================================================
// printXMLItem
//===========================================================================
void ccruncher::SegmentAggregator::printXMLItem(int index) throw(Exception)
{
  // setting item tag
  fout << INDENTER1;
  fout << "<item num='" << (cont-icont+index+1) << "' numsims='" << (bfull?1:cont) << "'>\n";

  // setting tranch tags
  for (int i=0;i<M;i++)
  {
    printXMLTranch(index, i);
  }

  // closing item tag
  fout << INDENTER1;
  fout << "</item>\n";
}

//===========================================================================
// printXMLTranch
//===========================================================================
void ccruncher::SegmentAggregator::printXMLTranch(int index, int itranch) throw(Exception)
{
  // setting tranch tag
  Date tmp = (bvalues?cvalues[0][itranch].date:cratings[0][itranch].date);
  fout << INDENTER2;
  fout << "<tranch num='" << itranch <<"' date='" << tmp << "'>\n";

  // setting tranch info
  if (bvalues == true)
  {
    fout << INDENTER3;
    fout << "<value name='cashflow'>";
    fout << cvalues[index][itranch].cashflow;
    fout << "</value>\n";

    fout << INDENTER3;
    fout << "<value name='exposure'>";
    fout << cvalues[index][itranch].exposure;
    fout << "</value>\n";
  }
  else
  {
    int k = ratings->getRatings()->size();
    
    for (unsigned int i=0;i<k;i++)
    {
      fout << INDENTER3;
      fout << "<value name='" << ratings->getName(i) << "'>";
      fout << cratings[index][itranch].ocurrences[i];
      fout << "</value>\n";
    }
  }

  // closing tranch tag
  fout << INDENTER2;
  fout << "</tranch>\n";
}

//===========================================================================
// setOutputProperties
//===========================================================================
void ccruncher::SegmentAggregator::setOutputProperties(string spath, bool force, int ibs) throw(Exception)
{
  path = File::normalizePath(spath);
  bforce = force;
  buffersize = ibs;

  if (buffersize == 0)
  {
    buffersize = MAXSIZE;
  }
  if (buffersize < 0)
  {
    throw Exception("SegmentAggregator::setOutputProperties(): invalid buffer size");
  }
}

//===========================================================================
// ofsopen
//===========================================================================
void ccruncher::SegmentAggregator::ofsopen() throw(Exception)
{
  try
  {
    fout.open(getFilePath().c_str(), ios::out|ios::app);
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::open(): error opening file " + getFilePath());
  }
}

//===========================================================================
// close
//===========================================================================
void ccruncher::SegmentAggregator::ofsclose() throw(Exception)
{
  try
  {
    fout.close();
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::open(): error closing file" + getFilePath());
  }
}

//===========================================================================
// touch. test output file correctnes
//===========================================================================
void ccruncher::SegmentAggregator::touch() throw(Exception)
{
  if (bforce == false && access(getFilePath().c_str(), W_OK) == 0)
  {
    throw Exception("SegmentAggregator::touch(): file " + getFilePath() + " already exist");
  }

  try
  {
    fout.open(getFilePath().c_str(), ios::out|ios::trunc);
    fout.close();
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::touch(): error initializing file " + getFilePath());
  }
}

//===========================================================================
// setNames
//===========================================================================
void ccruncher::SegmentAggregator::setNames(string n0, string n1, string n2) throw(Exception)
{
  name0 = n0;
  name1 = n1;
  name2 = n2;
}

//===========================================================================
// getFilePath
//===========================================================================
string ccruncher::SegmentAggregator::getFilePath() throw(Exception)
{
  if (name0 == "" || name1 == "" || name2 == "")
  {
    throw Exception("SegmentAggregator::getFilePath(): panic, setNames() not called");
  }
  
  return path + name0 + "-" + name1 + "-" + name2 + ".xml";
}
