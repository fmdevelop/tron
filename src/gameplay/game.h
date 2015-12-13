#ifndef GAME_H
#define GAME_H

#include "../data/data.h"
#include "../data/gamestate.h"
#include "../data/snake.h"
#include "../data/initobject.h"
#include "initialgamestategenerator.h"
#include <QObject>
#include <QTimer>
#include <QScopedPointer>
#include <QVector>

namespace tron {
namespace gameplay {

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(InitialGameStateGenerator* initialGameStateGenerator, QObject *parent = 0);

signals:
    void gameFieldInitialized(data::Field const * field);
    void gamestateChanged(data::GameState const * gameState);
public slots:

    void startGame(const data::InitObject& initObject);
    void changeDirection(uint id, data::DirectionChange::DirectionChange directionChange);

private slots:
    void doStep();
    void roundFinished();
    void startRound();

private:

    enum GameMatrix {
        EMPTY = -1,
        BORDER = -2,
    };


    Q_DISABLE_COPY(Game)

    QScopedPointer<data::GameState> m_gameState;
    QScopedPointer<QVector<QVector<int> > > m_gameMatrix;
    data::Field m_field;
    QTimer* m_startGameTimer;
    QTimer* m_finishingGameTimer;
    QTimer* m_gameIntervalTimer;

    InitialGameStateGenerator* m_initialGameStateGenerator;

    void fireGameStateChanged();
    void fillGameMatrix();
    void fillGameMatrixRow(uint row, uint from, uint to, int value);
    void fillGameMatrixColumn(uint column, uint from, uint to, int value);

    void handleSnakeStep(data::Snake &snake);
};

}
}


#endif // GAME_H
