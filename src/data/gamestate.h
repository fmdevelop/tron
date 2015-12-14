#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "data.h"
#include "snake.h"
#include "field.h"
#include <QMap>
#include <QSet>


namespace tron {
namespace data {

/**
 * Data structure that holds all the necessary information to know how a game
 * has to be renderend in a certain instant.
 * @brief The GameState struct
 */
struct GameState {
    // id -> snake
    QMap<uint, Snake> snakes;
    QSet<Point> explodedPoints;
    Status status;
};


}
}

#endif // GAMESTATE_H
