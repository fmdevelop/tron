#include "data.h"
#include <QPair>
#include <QHash>

using namespace tron::data;

uint tron::data::qHash(const Point &point) {
   return qHash(qMakePair(point.x, point.y));
}
