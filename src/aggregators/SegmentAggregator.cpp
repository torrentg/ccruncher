
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
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
//===========================================================================

#include <cmath>
#include <cassert>
#include <unistd.h>
#include <algorithm>
#include "aggregators/SegmentAggregator.hpp"

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
void ccruncher::SegmentAggregator::define(int isegs, int iseg, components_t comps, bool bv, bool bf)
{
  assert(isegs >= 0);
  assert(iseg >= 0);

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
  components = asset;
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
  long n, int indexdefault, Ratings *ratings_, Interests *interests_) throw(Exception)
{
  bool *clientflag = NULL;

  assert(dates != NULL);
  assert(m >= 0);
  assert(clients != NULL);
  assert(n >= 0);
  assert(indexdefault >= 0);
  assert(ratings_ != NULL);

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
    if (components == asset)
    {
      nclients = getANumClients(clients, N, clientflag);
    }
    else
    {
      nclients = getCNumClients(clients, N, clientflag);
    }

    // counting assets
    if (components == asset)
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
      vertexes = allocVertexes(dates, M, clients, N, interests_);

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
  vector<Asset> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j].belongsTo(isegmentation, isegment))
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
  vector<Asset> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j].belongsTo(isegmentation, isegment))
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
  vector<Asset> *assets;
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
  assert(aux == num);

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
        ret[i][j].recovery = 0.0;
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
    cvalues[pos][i].recovery = 0.0;
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
  assert(rpaths != NULL);
  assert(m >= 0);
  assert(n >= 0);
  assert(clients != NULL);

  if (m != M || n != N)
  {
    throw Exception("SegmentAggregator::append(): diferents dimensions");
  }
  if (components == asset && bvalues == true)
  {
    appendA(rpaths);
  }
  else if (components == asset && bvalues == false)
  {
    throw Exception("SegmentAggregator::append(): panic1");
  }
  else if (components == client && bvalues == true)
  {
    appendA(rpaths);
  }
  else if (components == client && bvalues == false)
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
        // TODO: add recovery value
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

  //TODO: flush if more than x sec. without flushing and bfull=true
}

//===========================================================================
// allocVertexes
//===========================================================================
DateValues** ccruncher::SegmentAggregator::allocVertexes(Date *dates, int m, vector<Client *> *clients, 
                long n, Interests *interests_) throw(Exception)
{
  DateValues **ret = NULL;
  DateValues *aux = NULL;
  vector<Asset> *assets;

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
        ret[i][k].recovery = 0.0;
      }

      // finding client info
      long cpos = iclients[i];
      assets = (*clients)[cpos]->getAssets();

      // filling row
      for(unsigned int j=0;j<assets->size();j++)
      {
        if (components==client || (components==asset && (*assets)[j].belongsTo(isegmentation, isegment)))
        {
          (*assets)[j].getVertexes(dates, m, interests_, aux);

          for(int k=0;k<m;k++)
          {
            ret[i][k].cashflow += aux[k].cashflow;
            ret[i][k].exposure += aux[k].exposure;
            ret[i][k].recovery += aux[k].recovery;
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
    // printing header
    if (!existheader)
    {
      printHeader();
      existheader = true;
    }

    // printing content
    printItems(bfinalize);
//TODO: pendent actualitzar info si bfull=false (fer trunc al obrir file)
    // printing footer
    if (bfinalize == true)
    {
      printFooter();
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
// printItems
//===========================================================================
void ccruncher::SegmentAggregator::printItems(bool bfinalize) throw(Exception)
{
  // setting items tags
  if (nclients != 0 || nassets != 0)
  {
    if (bfull == true)
    {
      for(long i=0;i<icont;i++)
      {
        printItem(i);
      }

      // reseting icont
      icont = 0;
    }
    else
    {
      if (bfinalize == true)
      {
        printItem(0);
      }
    }
  }
}

//===========================================================================
// printHeader
//===========================================================================
void ccruncher::SegmentAggregator::printHeader() throw(Exception)
{
  // setting header
  fout << "# ===========================================================\n";
  fout << "# FILE CONTENT:\n";
  fout << "#   CreditCruncher output file\n";
  fout << "#   content.version = " << VERSION << "\n";
  fout << "#   content.date = " << Date() << "\n";
  fout << "# -----------------------------------------------------------\n";  
  fout << "# FILE FORMAT:\n";
  fout << "#   tabulated data\n";
  fout << "#   comments begins with #\n";
  fout << "#   column separator is tab char (\\t)\n";
  fout << "# -----------------------------------------------------------\n";
  fout << "# PARAMETERS:\n";
  fout << "#   param.aggregator = " << name0 << "\n";
  fout << "#   param.segmentation = " << name1 << "\n";
  fout << "#   param.segment = " << name2 << "\n";
  fout << "#   param.components = " << (components==asset?"asset":"client") << "\n";
  fout << "#   param.type = " << (bvalues?"values":"ratings") << "\n";
  fout << "#   param.full = " << (bfull?"true":"false") << "\n";
  fout << "#   param.nclients = " << nclients << "\n";
  fout << "#   param.nassets = " << nassets << "\n";
  fout << "#   param.numratings = " << ratings->getRatings()->size() << "\n";
  fout << "#   param.ntranches = " << M << "\n";
  fout << "# -----------------------------------------------------------\n";
  fout << "# COLUMNS DESCRIPTIONS:\n";
  fout << "#   column.1.content = " << (bfull?"simulation counter":"number of simulations") << "\n";
  fout << "#   column.1.date = N/A\n";
  
  for(int numcol=2,i=0;i<M;i++)
  {
    if (bvalues == true)
    {
      Date tmp = cvalues[0][i].date;
      fout << "#   column." << numcol << ".content = value\n";
      fout << "#   column." << numcol << ".date = " << tmp << "\n";
      numcol++;
    }
    else
    {
      Date tmp = cratings[0][i].date;
      int numratings = ratings->getRatings()->size();
      for (int j=0;j<numratings;j++)
      {
        fout << "#   column." << numcol << ".content = number of ocurrences\n";
        fout << "#   column." << numcol << ".date = " << tmp << "\n";
        fout << "#   column." << numcol << ".rating = " << ratings->getName(j) << "\n";
        numcol++;
      }
    }
  }
  
  fout << "# ===========================================================\n";
}

//===========================================================================
// printFooter
//===========================================================================
void ccruncher::SegmentAggregator::printFooter() throw(Exception)
{
  // nothing to do
}

//===========================================================================
// printItem
//===========================================================================
void ccruncher::SegmentAggregator::printItem(int index) throw(Exception)
{
  // setting simulation counter
  fout << (cont-(bfull?icont:1)+index+1) << "\t";

  // each time tranch in a column
  for (int i=0;i<M;i++)
  {
    if (bvalues == true)
    {
      fout << cvalues[index][i].cashflow << "\t";
      //fout << cvalues[index][i].exposure;
    }
    else
    {
      int k = ratings->getRatings()->size();

      for (int j=0;j<k;j++)
      {
        fout << cratings[index][i].ocurrences[j] << "\t";
      }
    }
  }

  // new simulation -> new line
  fout << "\n";
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
//   n0 = aggregator name
//   n1 = segmentation name
//   n2 = segment name
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

  return path + name0 + "-" + name1 + "-" + name2 + ".out";
}
