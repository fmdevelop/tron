#ifndef DATA_H
#define DATA_H

#include <QtGlobal>

namespace tron {
namespace data {

struct Point {
    int x;
    int y;
};


uint qHash(const Point &point);

inline bool operator==(const Point& lhs, const Point& rhs){ return lhs.x == rhs.x && lhs.y == lhs.y; }
inline bool operator!=(const Point& lhs, const Point& rhs){return !(lhs == rhs);}

namespace Direction {
enum Direction {
    RIGHT = 0,
    UP = 1,
    LEFT = 2,
    DOWN = 3,
};
}

namespace DirectionChange {
enum DirectionChange {
    LEFT = 1,
    RIGHT = -1,
};
}

enum Status {
    ROUND_ABOUT_TO_START, // 0
    RUNNING, // 1
    ROUND_FINISHING, // 2
    ROUND_FINISHED, // 3
    SHOWING_RESULTS // 4
};

}
}

#endif // DATA_H
