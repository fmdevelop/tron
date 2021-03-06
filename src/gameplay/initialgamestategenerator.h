#ifndef INITIALGAMESTATEGENERATOR_H
#define INITIALGAMESTATEGENERATOR_H

#include "../data/initobject.h"
#include "../data/gamestate.h"
#include "../data/field.h"
#include "../data/data.h"
#include <QList>
#include <QPair>
#include <QSize>

namespace tron {
namespace gameplay {

class InitialGameStateGenerator
{
public:
    InitialGameStateGenerator();


    data::Field createGameField(const QSize& fieldSize);
    data::GameState* createInitialGameState(const data::InitObject& initObject);
    void cleanupGameStateForNewRound(data::GameState* gameState, const data::Field& field);

};


}
}

#endif // INITIALGAMESTATEGENERATOR_H
