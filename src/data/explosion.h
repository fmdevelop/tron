#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "data.h"
#include <QSet>

namespace tron {
namespace data {

struct Explosion {
    QSet<Point> points;
};

}
}

#endif // EXPLOSION_H
