#include "initialgamestategenerator.h"
#include "../constants.h"

using namespace tron;
using namespace tron::gameplay;

InitialGameStateGenerator::InitialGameStateGenerator()
{

}

data::Field InitialGameStateGenerator::createGameField(const QSize &fieldSize)
{
    data::Field field;
    field.width = fieldSize.width();
    field.height = fieldSize.height();
    field.backgroundColor = tron::BACKGROUND_COLOR;

    data::Border border1, border2;
    border1.color = border2.color = tron::BORDER_COLOR;
    border1.offset = 3;
    border2.offset = 6;

    field.borders << border1 << border2;
    return field;
}

data::GameState *InitialGameStateGenerator::createInitialGameState(const data::InitObject &initObject)
{
    data::GameState* gameState = new data::GameState();


    Q_ASSERT_X(initObject.snakeNames.count() <= 8, "createInitialGameState", "Too many snakes, no more colors defined");

    uint nextSnakeId = 0;

    QSet<QString>::const_iterator iterator = initObject.snakeNames.constBegin();
    while (iterator != initObject.snakeNames.constEnd()) {
        uint snakeId = nextSnakeId++;
        gameState->snakes[snakeId].id = snakeId;
        gameState->snakes[snakeId].name = *iterator;
        gameState->snakes[snakeId].color = tron::STANDARD_SNAKE_COLORS[snakeId];
        gameState->snakes[snakeId].score = 0;
        ++iterator;
    }

    return gameState;
}

void InitialGameStateGenerator::cleanupGameStateForNewRound(data::GameState *gameState, const data::Field &field)
{
    gameState->explosions.clear();
    gameState->status = data::ROUND_ABOUT_TO_START;

    const int n = gameState->snakes.count();

    QList<int> startPositionIds;

    for (int i = 0; i < n; ++i)
        startPositionIds << i;

    QMap<uint, data::Snake>::iterator iterator = gameState->snakes.begin();

    while (iterator != gameState->snakes.end()) {

        iterator->isAlive = true;

        int randomStartPositionId = startPositionIds.takeAt((uint) qrand() / (((uint) RAND_MAX + 1) / (uint)startPositionIds.length()));

        int numVertical = (n+1)/2;


        data::Point first, second;

        if (randomStartPositionId < numVertical) {
            // left -> right
            iterator->direction = data::Direction::RIGHT;

            first.x = field.width/6;
            second.x = field.width/3;

            first.y = second.y = field.height/(numVertical+1)*(randomStartPositionId+1);

        } else {
            // right -> left
            iterator->direction = data::Direction::LEFT;

            first.x = field.width/6*5;
            second.x = field.width/3*2;

            int verticalPositionId = randomStartPositionId - numVertical;


            first.y = second.y = field.height/(numVertical+1)*(verticalPositionId+1);
        }

        iterator->points.clear();

        iterator->points << first << second;

        ++iterator;
    }
}
