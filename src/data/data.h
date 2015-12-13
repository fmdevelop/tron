#ifndef DATA_H
#define DATA_H

#include <QtGlobal>

namespace tron {
namespace data {

struct Point {
    int x;
    int y;
};

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
    ROUND_ABOUT_TO_START,
    RUNNING,
    ROUND_FINISHING,
    ROUND_FINISHED,
    SHOWING_RESULTS
};

}
}

#endif // DATA_H
