#ifndef EXPLOSIONGENERATOR_H
#define EXPLOSIONGENERATOR_H

#include <QSet>
#include <QQueue>
#include "../data/data.h"
#include "../data/field.h"


namespace tron {
namespace gameplay {

class ExplosionGenerator
{
public:
    ExplosionGenerator();

    void explode(const data::Field& field, const data::Point& point, bool wasFrontalCollision);
    QSet<data::Point> getExplodedPoints();

private:
    QQueue<QSet<data::Point> > m_explodingPoints;

    void addPoint(int step, const data::Point& point);
};

}
}

#endif // EXPLOSIONGENERATOR_H
