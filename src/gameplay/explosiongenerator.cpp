#include "explosiongenerator.h"
#include "../constants.h"
#include "../utilities.h"
#include <QtGlobal>
#include <QtMath>

using namespace tron::gameplay;

ExplosionGenerator::ExplosionGenerator()
{

}

void ExplosionGenerator::explode(const data::Field& field, const tron::data::Point &origin, bool wasFrontalCollision)
{
    const int minRadius = tron::EXPLOSION_RADIUS_MIN;
    const int maxRadius = wasFrontalCollision
                                ? tron::EXPLOSION_RADIUS_FRONTAL_COLLISION_MAX
                                : tron::EXPLOSION_RADIUS_MAX;

    const int radius = generateRandomIntInRange(minRadius, maxRadius);

    int leftBound = qMax(0, static_cast<int>(origin.x-radius));
    int rightBound = qMin(static_cast<int>(field.width-1), static_cast<int>(origin.x+radius));
    // relative to the TOP LEFT corner
    int upperBound = qMax(0, static_cast<int>(origin.y-radius));
    int lowerBound = qMin(static_cast<int>(field.height-1), static_cast<int>(origin.y+radius));

    for (int x = leftBound; x <= rightBound; ++x) {
        for (int y = upperBound; y <= lowerBound; ++y) {
            if (x == origin.x && y == origin.y) {
                addPoint(0, origin);
                continue;
            }
            int delta_x = x - origin.x;
            int delta_y = y - origin.y;
            qreal d = qSqrt(delta_x*delta_x + delta_y*delta_y);

            int step = qFloor(d);

            if (step > radius)
                continue;

            if (generateRandomIntInRange(0, step*step/4) < 3) {
                data::Point p;
                p.x = x;
                p.y = y;
                addPoint(step, p);
            }
        }
    }
}

QSet<tron::data::Point> ExplosionGenerator::getExplodedPoints()
{
    return m_explodingPoints.isEmpty()
              ? QSet<tron::data::Point>()
              : m_explodingPoints.dequeue();
}

void ExplosionGenerator::addPoint(int step, const tron::data::Point &point)
{
    if (step < m_explodingPoints.size()) {
        m_explodingPoints[step] << point;
    } else {
        for (int i = m_explodingPoints.size(); i <= step; ++i) {
            QSet<data::Point> set;
            if (i == step)
                set << point;
            m_explodingPoints.enqueue(set);
        }
    }
}
