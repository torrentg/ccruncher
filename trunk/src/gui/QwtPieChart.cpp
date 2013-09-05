#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "QwtPieChart.hpp"

using namespace std;

//===========================================================================
// constructor
//===========================================================================
QwtPieChart::QwtPieChart(const QString &title) : QwtPlotSeriesItem(title),
    items(0), palette(256)
{
  sum = 0.0;
  center = QPoint(-1,-1);
  radius = -1;
  setZ( 1000 );
  setRenderHint( QwtPlotItem::RenderAntialiased, true );

  seed = 10;
  for(size_t i=0; i<palette.size(); i++) {
    palette[i] = getRandomColor();
  }
}

//===========================================================================
// destructor
//===========================================================================
QwtPieChart::~QwtPieChart()
{
  // nothing to do
}

//===========================================================================
// Draw a subset of the samples
// param painter Painter
// param xMap Maps x-values into pixel coordinates.
// param yMap Maps y-values into pixel coordinates.
// param canvasRect Contents rectangle of the canvas
// param from Index of the first point to be painted
// param to Index of the last point to be painted.
//          If to < 0 the curve will be painted to its last point.
//===========================================================================
void QwtPieChart::drawSeries( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &rect, int from, int to ) const
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
  for (size_t i=0; i<items.size(); i++ )
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

//===========================================================================
// simple congruential RNG
// extracted from Numerical Recipes in C
//===========================================================================
int QwtPieChart::rand() const
{
  seed = seed*1103515245 + 12345;
  return (unsigned int)(seed/65536) % 32768;
}

//===========================================================================
// getRandomColor
//===========================================================================
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

//===========================================================================
// getInfo
//===========================================================================
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
    vector<item>::const_iterator pos = lower_bound(items.begin(), items.end(), angle);
    name = pos->name;
    val = pos->value;
    pct = pos->value/sum;
    return true;
  }
}

//===========================================================================
// setSamples
//===========================================================================
void QwtPieChart::setSamples(const vector<double> &values, const vector<string> &names)
{
  //TODO: check that values and names size are equals

  sum = 0.0;
  items.resize(values.size());

  for (size_t i=0; i<items.size(); i++)
  {
    items[i].value = values[i];
    items[i].name = names[i];
    items[i].angle = 0;
    sum += values[i];
  }

  double angle = 0.0;
  for (size_t i=0; i<items.size(); i++)
  {
    angle += 5760.0*(items[i].value/sum);
    items[i].angle = int(angle+0.5); // rounding to nearest
  }

  if (!items.empty()) {
    items.back().angle = 5760;
  }

  dataChanged();
}

