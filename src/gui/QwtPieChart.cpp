
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <QColor>
#include <QPainter>
#include "QwtPieChart.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] title Chart title.
 */
QwtPieChart::QwtPieChart(const QString &title) : QwtPlotSeriesItem(title),
    items(0), palette(256)
{
  sum = 0.0;
  center = QPoint(-1,-1);
  radius = -1;
  setZ( 1000 );
  setRenderHint( QwtPlotItem::RenderAntialiased, true );

  seed = 10;
  for(QColor &color : palette) {
    color = getRandomColor();
  }
}

/**************************************************************************//**
 * @details Draw a subset of the samples.
 * @param[in] painter Painter
 * @param[in] xMap Maps x-values into pixel coordinates.
 * @param[in] yMap Maps y-values into pixel coordinates.
 * @param[in] rect Contents rectangle of the canvas
 * @param[in] from Index of the first point to be painted
 * @param[in] to Index of the last point to be painted.
 *            If to < 0 the curve will be painted to its last point.
 */
void QwtPieChart::drawSeries( QPainter *painter,
    const QwtScaleMap & /*xMap*/, const QwtScaleMap &/*yMap*/,
    const QRectF &rect, int /*from*/, int /*to*/ ) const
{
  int margin = 5;
  qreal side = std::min(rect.width(), rect.height()) - 2*margin;
  QRectF pieRect;
  pieRect.setX(rect.x() + (rect.width() - side)/2.0);
  pieRect.setY(rect.y() + (rect.height() - side)/2.0);
  pieRect.setWidth(side);
  pieRect.setHeight(side);

  radius = side/2;
  center = QPoint(pieRect.x()+pieRect.width()/2, pieRect.y()+pieRect.height()/2);

  painter->save();
  QPen pen(QColor(255,255,255));
  pen.setWidth(1);
  painter->setPen(pen);
  for(size_t i=0; i<items.size(); i++ )
  {
    int angle1 = (i==0 ? 0:items[i-1].angle);
    int angle2 = items[i].angle;
    if (angle2-angle1 != 0)
    {
      painter->setBrush(QBrush(palette[i%palette.size()], Qt::SolidPattern));
      painter->drawPie(pieRect, angle1, angle2-angle1);
    }
  }
  painter->restore();
}

/**************************************************************************//**
 * @details Extracted from 'Numerical Recipes in C'.
 * @return Random number.
 */
int QwtPieChart::rand() const
{
  seed = seed*1103515245 + 12345;
  return (unsigned int)(seed/65536) % 32768;
}

/**************************************************************************//**
 * @return Random color.
 */
QColor QwtPieChart::getRandomColor() const
{
  QColor mix(255, 255, 255);

  int red = rand() % 256;
  int green = rand() % 256;
  int blue = rand() % 256;

  // mix the color
  red = (red + mix.red()) / 2;
  green = (green + mix.green()) / 2;
  blue = (blue + mix.blue()) / 2;

  return QColor(red, green, blue);
}

/**************************************************************************//**
 * @param[in] point Position in the canvas.
 * @param[out] name Name of the sector.
 * @param[out] val Value of the sector.
 * @param[out] pct Value of the sector (in percentage).
 * @return true = there is a sector, false = otherwise.
 */
bool QwtPieChart::getInfo(QPoint point, string &name, double &val, double &pct) const
{
  QPoint v = point-center;
  double len = sqrt(pow(v.x(),2) + pow(v.y(),2));
  if (len > radius) {
    return false;
  }
  else if (len == 0) {
    name = items[0].name;
    val = items[0].value;
    pct = items[0].value/sum;
    return true;
  }
  else {
    int angle = acos(v.x()/len) * (5760.0/(2.0*M_PI));
    if (v.y() < 0) angle = 5760 - angle;
    angle = 5760 - angle;
    auto pos = lower_bound(items.begin(), items.end(), angle);
    name = pos->name;
    val = pos->value;
    pct = pos->value/sum;
    return true;
  }
}

/**************************************************************************//**
 * @param[in] values Values to plot.
 * @param[in] names Sector names.
 */
void QwtPieChart::setSamples(const std::vector<double> &values,
    const std::vector<std::string> &names)
{
  //TODO: check that values and names size are equals

  sum = 0.0;
  items.resize(values.size());

  for(size_t i=0; i<items.size(); i++)
  {
    items[i].value = values[i];
    items[i].name = names[i];
    items[i].angle = 0;
    sum += values[i];
  }

  double angle = 0.0;
  for(size_t i=0; i<items.size(); i++)
  {
    angle += 5760.0*(items[i].value/sum);
    items[i].angle = int(angle+0.5); // rounding to nearest
  }

  if (!items.empty()) {
    items.back().angle = 5760;
  }

  dataChanged();
}

