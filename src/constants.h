#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>
#include <QColor>

namespace tron{

// BEHAVIOR
const uint MSEC_TO_START = 2500;
const uint MSEC_TO_FINISH_ONE_PLAYER_ALIVE = 2500;
const uint MSEC_TO_FINISH_ALL_PLAYERS_DEAD = 1500;
const uint MSEC_GAME_INTERVAL = 10;
const int EXPLOSION_RADIUS_MIN = 5;
const int EXPLOSION_RADIUS_MAX = 8;
const int EXPLOSION_RADIUS_FRONTAL_COLLISION_MAX = 13;

// SCORE
const uint SCORE_ESCAPE = 5;
const uint SCORE_SURVIVAL = 3;
const uint SCORE_KILL = 2;
const uint SCORE_COLLISSION = 1;


// LOOK
extern const QColor STANDARD_SNAKE_COLORS[];
const QColor BORDER_COLOR(Qt::darkGray);
const QColor BACKGROUND_COLOR(Qt::black);
const QColor EXPLOSION_COLOR(Qt::darkMagenta);


}



#endif // CONSTANTS_H
