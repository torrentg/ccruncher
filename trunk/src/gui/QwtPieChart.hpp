
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

#ifndef _PieChart_
#define _PieChart_

#include <vector>
#include <string>
#include <qwt_plot_seriesitem.h>

#if QWT_VERSION < 0x060100
  #error "Qwt library version needs to be higher or equal to 6.1.0"
#endif

/**************************************************************************//**
 * @brief Qwt pie chart.
 */
class QwtPieChart : public QwtPlotSeriesItem
{

  private:

    //! Internal struct
    struct item {
        //! Item absolute value
        double value;
        //! Item name
        std::string name;
        //! Ending angle in drawPie() units (2PI = 5760)
        int angle;
        //! Less-than operator
        bool operator <(int val) const { return (angle < val); }
    };

  private:

    //! Sum values
    double sum;
    //! Data to plot
    std::vector<item> items;
    //! Color palette
    std::vector<QColor> palette;
    //! Radius (in pixels)
    mutable int radius;
    //! Center (in pixels)
    mutable QPoint center;
    //! Internal rng seed
    mutable ulong seed;

  private:

    //! Generate random color
    QColor getRandomColor() const;
    //! Basic congruential RNG
    int rand() const;

  public:

    //! Constructor
    QwtPieChart(const QString &title=QString::null);
    //! Destructor
    virtual ~QwtPieChart() override {}
    //! Extended from QwtAbstractSeriesStore
    virtual void setRectOfInterest( const QRectF & ) override {}
    //! Extended from QwtAbstractSeriesStore
    virtual QRectF dataRect() const override { return QRectF(); }
    //! Extended from QwtAbstractSeriesStore
    virtual size_t dataSize() const override { return items.size(); }
    //! Extended from QwtPlotSeriesItem
    virtual void drawSeries( QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const override;
    //! Extended from QwtPlotItem
    virtual int rtti() const override { return QwtPlotItem::Rtti_PlotUserItem; }
    //! Set data to be ploted
    void setSamples(const std::vector<double> &, const std::vector<std::string> &);
    //! Return name and value under point
    bool getInfo(QPoint point, std::string &name, double &val, double &pct) const;

};

#endif

