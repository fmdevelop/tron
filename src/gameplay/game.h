#ifndef GAME_H
#define GAME_H

#include "../data/data.h"
#include "../data/gamestate.h"
#include "../data/snake.h"
#include "../data/initobject.h"
#include "initialgamestategenerator.h"
#include "explosiongenerator.h"
#include <QObject>
#include <QTimer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QVector>
#include <QMutex>
#include <QThread>
#include <QTime>

namespace tron {
namespace gameplay {

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(InitialGameStateGenerator* initialGameStateGenerator, QObject *parent = 0);

    ~Game();

signals:
    void gameFieldInitialized(data::Field const * field);
    void gamestateChanged(QSharedPointer<data::GameState> gameState);
public slots:

    void startGame(const tron::data::InitObject& initObject);
    void changeDirection(uint id, data::DirectionChange::DirectionChange directionChange);
    void continueWithNextRound();

private slots:
    void doStep();
    void roundFinished();
    void startRound();
    void showingScoreTimerFinished();

private:

    QMutex m_mutexes[8];
    QThread m_thread;


    enum GameMatrix {
        EMPTY = -1,
        BORDER = -2,
    };


    Q_DISABLE_COPY(Game)

    QSharedPointer<data::GameState> m_gameState;
    QScopedPointer<QVector<QVector<int> > > m_gameMatrix;
    data::Field m_field;
    QTimer* m_startGameTimer;
    QTimer* m_finishingGameTimer;
    QTimer* m_gameIntervalTimer;
    QTimer* m_showingScoreTimer;
    QTime m_roundStarted;

    bool m_waitAfterShowingScore;

    InitialGameStateGenerator* m_initialGameStateGenerator;
    QScopedPointer<ExplosionGenerator> m_explosionGenerator;

    void fireGameStateChanged();
    void fillGameMatrix();
    void fillGameMatrixRow(uint row, uint from, uint to, int value);
    void fillGameMatrixColumn(uint column, uint from, uint to, int value);

    void handleSnakeStep(data::Snake &snake);
};

}
}


#endif // GAME_H
