
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

#ifndef _PieChart_
#define _PieChart_

#include <vector>
#include <string>
#include <qwt_plot_seriesitem.h>

#if QWT_VERSION < 0x060100
  #error "Qwt library version needs to be higher or equal to 6.1.0"
#endif

class QwtPieChart : public QwtPlotSeriesItem
{

  private:

    // internal struct
    struct item {
        // item absolute value
        double value;
        // item name
        std::string name;
        // ending angle in drawPie() units (2PI = 5760)
        int angle;
        // less-than operator
        bool operator <(int val) const { return (angle < val); }
    };

  private:

    // sum values
    double sum;
    // data to plot
    std::vector<item> items;
    // color palette
    std::vector<QColor> palette;
    // radius (in pixels)
    mutable int radius;
    // center (in pixels)
    mutable QPoint center;
    // internal rng seed
    mutable unsigned long seed;

  private:

    // generate random color
    QColor getRandomColor() const;
    // basic congruential RNG
    int rand() const;

  public:

    // constructor
    QwtPieChart(const QString &title=QString::null);
    // destructor
    virtual ~QwtPieChart();
    // extended from QwtAbstractSeriesStore
    virtual void setRectOfInterest( const QRectF & ) {}
    // extended from QwtAbstractSeriesStore
    virtual QRectF dataRect() const { return QRectF(); }
    // extended from QwtAbstractSeriesStore
    virtual size_t dataSize() const { return items.size(); }
    // extended from QwtPlotSeriesItem
    virtual void drawSeries( QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;
    // extended from QwtPlotItem
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }
    // set data to be ploted
    void setSamples(const std::vector<double> &, const std::vector<std::string> &);
    // return name and value under point
    bool getInfo(QPoint point, std::string &name, double &val, double &pct) const;

};

#endif

