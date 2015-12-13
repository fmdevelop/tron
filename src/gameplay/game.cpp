#include "game.h"
#include "../constants.h"
#include <QtGlobal>
#include <QMutexLocker>

using namespace tron::gameplay;


Game::Game(InitialGameStateGenerator *initialGameStateGenerator, QObject *parent) : QObject(parent)
  , m_startGameTimer(new QTimer(this))
  , m_finishingGameTimer(new QTimer(this))
  , m_gameIntervalTimer(new QTimer(this))
  , m_initialGameStateGenerator(initialGameStateGenerator)
{
    m_gameIntervalTimer->setInterval(tron::MSEC_GAME_INTERVAL);
    connect(m_startGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(start()));
    connect(m_gameIntervalTimer, SIGNAL(timeout()), this, SLOT(doStep()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), m_gameIntervalTimer, SLOT(stop()));
    connect(m_finishingGameTimer, SIGNAL(timeout()), this, SLOT(roundFinished()));
}

void Game::changeDirection(uint id, data::DirectionChange::DirectionChange directionChange)
{
    Q_ASSERT_X(m_gameState != NULL, "changeDirection", "private game state NULL");
    Q_ASSERT_X(m_gameState->snakes.contains(id), "changeDirection", "unknown snake name");

    data::Snake& snake = m_gameState->snakes[id];

    // TODO proper locking... QMutexLocker locker(&snake.mutex());
    snake.direction = static_cast<data::Direction::Direction>((snake.direction + directionChange) % 4);
}

void Game::handleSnakeStep(data::Snake& snake)
{
    data::Point nextPos = snake.getNextPosition();

    if (nextPos.x < 0 || nextPos.y < 0
            || nextPos.x >= m_field.width
            || nextPos.y >= m_field.height) {
        // ESCAPE
        snake.isAlive = false;
        snake.score += SCORE_ESCAPE;
        return;
    }

    int obstacle = (*m_gameMatrix)[nextPos.x][nextPos.y];

    if (obstacle == EMPTY) {
        if (snake.directionHasChanged()) {
            snake.points << nextPos;
        } else {
            snake.points.last() = nextPos;
        }
        return;
    }

    // COLLISSION
    snake.isAlive = false;

    if (obstacle == BORDER)
        return; // no points

    // COLLISSION WITH OTHER SNAKE
    uint collissionSnakeId = static_cast<uint>(obstacle);
    if (collissionSnakeId == snake.id) {
       // no points for user --> OR -1 ?
       return;
    }
    data::Snake& collissionSnake = m_gameState->snakes[collissionSnakeId];
    bool isFrontalCollision = collissionSnake.points.last() != nextPos;

    if (isFrontalCollision) {
        collissionSnake.isAlive = false;
        collissionSnake.score += SCORE_COLLISSION;
        snake.score += SCORE_COLLISSION;
    } else {
        collissionSnake.score += SCORE_KILL;
    }
}

void Game::doStep()
{
    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    int snakesAlive = 0;

    while (iterator != m_gameState->snakes.constEnd()) {
        if (!iterator->isAlive)
            continue;

        ++snakesAlive;

        handleSnakeStep(*iterator);
    }

    if (m_gameState->status == data::RUNNING && snakesAlive <= 1) {
        m_gameState->status = data::ROUND_FINISHING;

        uint msecToProceed = MSEC_TO_FINISH_ONE_PLAYER_ALIVE;

        if (snakesAlive == 0) {
            msecToProceed = MSEC_TO_FINISH_ALL_PLAYERS_DEAD;
            m_gameIntervalTimer->stop();
        }

        m_finishingGameTimer->start(msecToProceed);
    }

    fireGameStateChanged();
}

void Game::roundFinished()
{
    QMap<uint, data::Snake>::iterator iterator = m_gameState->snakes.begin();

    while (iterator != m_gameState->snakes.constEnd()) {
        if (iterator->isAlive) {
            iterator->score += SCORE_SURVIVAL;
            // there should be only one
            break;
        }
    }

    m_gameState->status = data::SHOWING_RESULTS;
    fireGameStateChanged();
}

void Game::startRound()
{
    m_initialGameStateGenerator->cleanupGameStateForNewRound(m_gameState.take(), m_field);

    fillGameMatrix();

    fireGameStateChanged();

    m_startGameTimer->start(tron::MSEC_TO_START);
}

void Game::fireGameStateChanged()
{
    emit gamestateChanged(m_gameState.take());
}

void Game::fillGameMatrix()
{
    data::Field f = m_field;

    for (int i = 0; i < f.width; ++i)
        for (int j = 0; j < f.height; ++j)
            (*m_gameMatrix)[i][j] = EMPTY;

    QMap<uint, data::Snake>::const_iterator iterator = m_gameState->snakes.constBegin();

    while (iterator != m_gameState->snakes.constEnd()) {
        uint id = iterator.key();
        data::Snake snake = iterator.value();

        Q_ASSERT_X(snake.points.count() >= 2, "fillGameMatrix", "Invalid game state to start a game");

        data::Point start = snake.points[snake.points.count() - 2];
        data::Point end = snake.points.last();

        Q_ASSERT_X(start.x == end.x || start.y == end.y, "fillGameMatrix", "Invalid start points for a snake");

        if (start.x == end.x) {
            fillGameMatrixRow(start.x, start.y, end.y, id);
        } else {
            fillGameMatrixColumn(start.y, start.x, end.x, id);
        }
    }

    foreach(const data::Border& b, f.borders) {
        fillGameMatrixRow(b.offset, b.offset, f.width - 1 - b.offset, BORDER);
        fillGameMatrixRow(f.height - 1 - b.offset, b.offset, f.width - 1 - b.offset, BORDER);
        fillGameMatrixColumn(b.offset, b.offset, f.height -1 - b.offset, BORDER);
        fillGameMatrixColumn(f.width - 1 - b.offset, b.offset, f.height -1 - b.offset, BORDER);
    }
}

void Game::fillGameMatrixRow(uint row, uint from, uint to, int value)
{
    for (uint j = from; j <= to; ++j)
        (*m_gameMatrix)[row][j] = value;
}

void Game::fillGameMatrixColumn(uint column, uint from, uint to, int value)
{
    for (uint i = from; i <= to; ++i)
        (*m_gameMatrix)[i][column] = value;
}


void Game::startGame(const data::InitObject &initObjectgameState) {
    Q_ASSERT_X(initObjectgameState.snakeNames.count() > 0, "startGame", "No snakes to start a game");
    Q_ASSERT_X(initObjectgameState.field.width > 0, "startGame", "Invalid field width");
    Q_ASSERT_X(initObjectgameState.field.height > 0, "startGame", "Invalid field height");

    m_field = initObjectgameState.field;
    data::GameState* gameState = m_initialGameStateGenerator->createInitialGameState(initObjectgameState);
    m_gameState.reset(gameState);

    // initialize GameMatrix
    QVector<QVector<int> >* vec = new QVector<QVector<int> >(m_field.height);
    for (int i = 0; i < vec->length(); ++i)
        (*vec)[i].resize(initObjectgameState.field.width);
    m_gameMatrix.reset(vec);

    emit gameFieldInitialized(&m_field);

    startRound();
}
