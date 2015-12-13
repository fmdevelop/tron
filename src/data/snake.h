#ifndef PATH_H
#define PATH_H

#include "data.h"
#include <QColor>
#include <QList>
#include <QString>
#include <QMutex>

namespace tron {
namespace data {

struct Snake
{
    uint id;
    QString name;
    QColor color;
    QList<Point> points;
    Direction::Direction direction;
    uint score;
    bool isAlive;

    /**
     * Determinates wheter or not the last two points coincide with the current
     * direction
     * @brief directionHasChanged
     * @return
     */
    inline bool directionHasChanged()
    {
        // for UP, DOWN direction the x coordinate has to be the same,
        // for LEFT, RIGHT direction the y coordinate has to be the same

        if (direction == Direction::UP || direction == Direction::DOWN)
            return points.last().x != points[points.count() - 2].x;

        // LEFT, RIGHT
        return points.last().y != points[points.count() - 2].y;

    }

    inline Point getNextPosition() {
        Point nextPos(points.last());

        switch (direction) {
        case Direction::RIGHT:
            ++nextPos.x;
            break;
        case Direction::UP:
            ++nextPos.y;
            break;
        case Direction::LEFT:
            --nextPos.x;
            break;
        case Direction::DOWN:
            --nextPos.y;
            break;
        }
        return nextPos;
    }
};

}
}

#endif // PATH_H
